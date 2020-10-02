/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specic language governing permissions and
 * limitations under the License.
 */

#include <android-base/file.h>
#include <android-base/logging.h>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>

#include <thread>

#include "perfmgr/HintManager.h"

namespace android {
namespace perfmgr {

class NodeVerifier : public HintManager {
  public:
    static bool VerifyNodes(const std::string& config_path) {
        std::string json_doc;

        if (!android::base::ReadFileToString(config_path, &json_doc)) {
            LOG(ERROR) << "Failed to read JSON config from " << config_path;
            return false;
        }

        std::vector<std::unique_ptr<Node>> nodes = ParseNodes(json_doc);
        if (nodes.empty()) {
            LOG(ERROR) << "Failed to parse Nodes section from " << config_path;
            return false;
        }

        return true;
    }

  private:
    NodeVerifier(sp<NodeLooperThread> nm,
                 const std::map<std::string, std::vector<NodeAction>>& actions)
        : HintManager(std::move(nm), actions) {}
};

}  // namespace perfmgr
}  // namespace android

static void printUsage(const char* exec_name) {
    std::string usage = exec_name;
    usage =
        usage +
        " is a command-line tool to verify Nodes in Json config are writable.\n"
        "Usages:\n"
        "    [su system] " +
        exec_name +
        " [options]\n"
        "\n"
        "Options:\n"
        "   --config, -c  [PATH]\n"
        "       path to Json config file\n\n"
        "   --exec_hint, -e\n"
        "       do hints in Json config\n\n"
        "   --hint_name, -i\n"
        "       do only the specific hint\n\n"
        "   --hint_duration, -d  [duration]\n"
        "       duration in ms for each hint\n\n"
        "   --help, -h\n"
        "       print this message\n\n"
        "   --verbose, -v\n"
        "       print verbose log during execution\n\n";

    LOG(INFO) << usage;
}

static void execConfig(const std::string& json_file,
                       const std::string& hint_name, uint64_t hint_duration) {
    std::unique_ptr<android::perfmgr::HintManager> hm =
        android::perfmgr::HintManager::GetFromJSON(json_file);
    if (!hm.get() || !hm->IsRunning()) {
        LOG(ERROR) << "Failed to Parse JSON config";
    }
    std::vector<std::string> hints = hm->GetHints();
    for (const auto& hint : hints) {
        if (!hint_name.empty() && hint_name != hint) continue;
        LOG(INFO) << "Do hint: " << hint;
        hm->DoHint(hint, std::chrono::milliseconds(hint_duration));
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(hint_duration));
        LOG(INFO) << "End hint: " << hint;
        hm->EndHint(hint);
        std::this_thread::yield();
    }
}

int main(int argc, char* argv[]) {
    android::base::InitLogging(argv, android::base::StdioLogger);

    if (getuid() == 0) {
        LOG(WARNING) << "Running as root might mask node permission";
    }

    std::string config_path;
    std::string hint_name;
    bool exec_hint = false;
    uint64_t hint_duration = 100;

    while (true) {
        static struct option opts[] = {
            {"config", required_argument, nullptr, 'c'},
            {"exec_hint", no_argument, nullptr, 'e'},
            {"hint_name", required_argument, nullptr, 'i'},
            {"hint_duration", required_argument, nullptr, 'd'},
            {"help", no_argument, nullptr, 'h'},
            {"verbose", no_argument, nullptr, 'v'},
            {0, 0, 0, 0}  // termination of the option list
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "c:ei:d:hv", opts, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'c':
                config_path = optarg;
                break;
            case 'e':
                exec_hint = true;
                break;
            case 'i':
                hint_name = optarg;
                break;
            case 'd':
                hint_duration = strtoul(optarg, NULL, 10);
                break;
            case 'v':
                android::base::SetMinimumLogSeverity(android::base::VERBOSE);
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            default:
                // getopt already prints "invalid option -- %c" for us.
                return 1;
        }
    }

    if (config_path.empty()) {
        LOG(ERROR) << "Need specify JSON config";
        printUsage(argv[0]);
        return 1;
    }

    if (exec_hint) {
        execConfig(config_path, hint_name, hint_duration);
        return 0;
    }

    if (android::perfmgr::NodeVerifier::VerifyNodes(config_path)) {
        LOG(INFO) << "Verified writing to JSON config";
        return 0;
    } else {
        LOG(ERROR) << "Failed to verify nodes in JSON config";
        return 1;
    }
}
