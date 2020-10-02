/*
 * Copyright (C) 2017 The Android Open Source Project
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

#define ATRACE_TAG (ATRACE_TAG_POWER | ATRACE_TAG_HAL)
#define LOG_TAG "libperfmgr"

#include "perfmgr/FileNode.h"

#include <android-base/chrono_utils.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>
#include <utils/Trace.h>

namespace android {
namespace perfmgr {

FileNode::FileNode(std::string name, std::string node_path,
                   std::vector<RequestGroup> req_sorted,
                   std::size_t default_val_index, bool reset_on_init,
                   bool hold_fd)
    : Node(std::move(name), std::move(node_path), std::move(req_sorted),
           default_val_index, reset_on_init),
      hold_fd_(hold_fd),
      warn_timeout_(
          android::base::GetBoolProperty("ro.debuggable", false) ? 5ms : 50ms) {
}

std::chrono::milliseconds FileNode::Update(bool log_error) {
    std::size_t value_index = default_val_index_;
    std::chrono::milliseconds expire_time = std::chrono::milliseconds::max();

    // Find the highest outstanding request's expire time
    for (std::size_t i = 0; i < req_sorted_.size(); i++) {
        if (req_sorted_[i].GetExpireTime(&expire_time)) {
            value_index = i;
            break;
        }
    }

    // Update node only if request index changes
    if (value_index != current_val_index_ || reset_on_init_) {
        ATRACE_BEGIN(GetName().c_str());
        const std::string& req_value =
            req_sorted_[value_index].GetRequestValue();

        android::base::Timer t;
        fd_.reset(TEMP_FAILURE_RETRY(
            open(node_path_.c_str(), O_WRONLY | O_CLOEXEC | O_TRUNC)));

        if (fd_ == -1 || !android::base::WriteStringToFd(req_value, fd_)) {
            if (log_error) {
                LOG(WARNING) << "Failed to write to node: " << node_path_
                             << " with value: " << req_value << ", fd: " << fd_;
            }
            // Retry in 500ms or sooner
            expire_time = std::min(expire_time, std::chrono::milliseconds(500));
        } else {
            // For regular file system, we need fsync
            fsync(fd_);
            // Some dev node requires file to remain open during the entire hint
            // duration e.g. /dev/cpu_dma_latency, so fd_ is intentionally kept
            // open during any requested value other than default one. If
            // request a default value, node will write the value and then
            // release the fd.
            if ((!hold_fd_) || value_index == default_val_index_) {
                fd_.reset();
            }
            auto duration = t.duration();
            if (duration > warn_timeout_) {
                LOG(WARNING) << "Slow writing to file: '" << node_path_
                             << "' with value: '" << req_value
                             << "' took: " << duration.count() << " ms";
            }
            // Update current index only when succeed
            current_val_index_ = value_index;
            reset_on_init_ = false;
        }
        ATRACE_END();
    }
    return expire_time;
}

bool FileNode::GetHoldFd() const {
    return hold_fd_;
}

void FileNode::DumpToFd(int fd) const {
    std::string node_value;
    if (!android::base::ReadFileToString(node_path_, &node_value)) {
        LOG(ERROR) << "Failed to read node path: " << node_path_;
    }
    node_value = android::base::Trim(node_value);
    std::string buf(android::base::StringPrintf(
        "%s\t%s\t%zu\t%s\n", name_.c_str(), node_path_.c_str(),
        current_val_index_, node_value.c_str()));
    if (!android::base::WriteStringToFd(buf, fd)) {
        LOG(ERROR) << "Failed to dump fd: " << fd;
    }
    for (std::size_t i = 0; i < req_sorted_.size(); i++) {
        req_sorted_[i].DumpToFd(
            fd, android::base::StringPrintf("\t\tReq%zu:\t", i));
    }
}

}  // namespace perfmgr
}  // namespace android
