/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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

#define LOG_TAG "LocSvc_AGnssInterface"

#include <log_util.h>
#include "Gnss.h"
#include "AGnss.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V2_0 {
namespace implementation {

static AGnss* spAGnss = nullptr;

AGnss::AGnss(Gnss* gnss) : mGnss(gnss) {
    spAGnss = this;
}

AGnss::~AGnss() {
    spAGnss = nullptr;
}

void AGnss::agnssStatusIpV4Cb(AGnssExtStatusIpV4 status){
}

void AGnss::statusIpV4Cb(AGnssExtStatusIpV4 status) {
}

Return<void> AGnss::setCallback(const sp<V2_0::IAGnssCallback>& callback) {
    return Void();
}

Return<bool> AGnss::dataConnClosed() {
    return true;
}

Return<bool> AGnss::dataConnFailed() {
    return true;
}

Return<bool> AGnss::dataConnOpen(uint64_t networkHandle, const hidl_string& apn,
        V2_0::IAGnss::ApnIpType apnIpType) {
    return true;
}

Return<bool> AGnss::setServer(V2_0::IAGnssCallback::AGnssType type,
                              const hidl_string& hostname,
                              int32_t port) {
    return true;
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
