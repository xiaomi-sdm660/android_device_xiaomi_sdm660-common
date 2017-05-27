/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 * Not a Contribution
 */
/*
 * Copyright (C) 2016 The Android Open Source Project
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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LocSvc__AGnssRilInterface"

#include <log_util.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sstream>
#include <string>
#include "Gnss.h"
#include "AGnssRil.h"
typedef void* (getLocationInterface)();

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

static bool sendConnectionEvent(const bool connected, const uint8_t type);

AGnssRil::AGnssRil(Gnss* gnss) : mGnss(gnss) {
    ENTRY_LOG_CALLFLOW();
}

AGnssRil::~AGnssRil() {
    ENTRY_LOG_CALLFLOW();
}

Return<bool> AGnssRil::updateNetworkState(bool connected, NetworkType type, bool roaming) {
    ENTRY_LOG_CALLFLOW();

    // for XTRA
    sendConnectionEvent(connected, (uint8_t)type);

    return true;
}

// for XTRA
static inline int createSocket() {
    int socketFd = -1;

    if ((socketFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));

     } else {
        const char* socketPath = "/data/vendor/location/xtra/socket_hal_xtra";
        struct sockaddr_un addr = { .sun_family = AF_UNIX };
        snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", socketPath);

        if (::connect(socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("cannot connect to XTRA. reason:%s", strerror(errno));
            if (::close(socketFd)) {
                LOC_LOGe("close socket error. reason:%s", strerror(errno));
            }
            socketFd = -1;
        }
    }

    return socketFd;
}

static inline void closeSocket(const int socketFd) {
    if (socketFd >= 0) {
        if(::close(socketFd)) {
            LOC_LOGe("close socket error. reason:%s", strerror(errno));
        }
    }
}

static inline bool sendConnectionEvent(const bool connected, const uint8_t type) {
    int socketFd = createSocket();
    if (socketFd < 0) {
        LOC_LOGe("XTRA unreachable. sending failed.");
        return false;
    }

    std::stringstream ss;
    ss <<  "connection";
    ss << " " << (connected ? "1" : "0");
    ss << " " << (int)type;
    ss << "\n"; // append seperator

    const std::string& data = ss.str();
    int remain = data.length();
    ssize_t sent = 0;

    while (remain > 0 &&
          (sent = ::send(socketFd, data.c_str() + (data.length() - remain),
                       remain, MSG_NOSIGNAL)) > 0) {
        remain -= sent;
    }

    if (sent < 0) {
        LOC_LOGe("sending error. reason:%s", strerror(errno));
    }

    closeSocket(socketFd);

    return (remain == 0);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
