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

#define LOG_TAG "LocSvc_AGnssInterface"

#include <log_util.h>
#include "Gnss.h"
#include "AGnss.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

AGnss::AGnss(Gnss* gnss) : mGnss(gnss) {
}

Return<bool> AGnss::setServer(IAGnssCallback::AGnssType type,
                              const hidl_string& hostname,
                              int32_t port) {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_SET_ASSISTANCE_DATA_VALID_BIT;
    config.assistanceServer.size = sizeof(GnssConfigSetAssistanceServer);
    if (type == IAGnssCallback::AGnssType::TYPE_SUPL) {
        config.assistanceServer.type = GNSS_ASSISTANCE_TYPE_SUPL;
    } else if (type == IAGnssCallback::AGnssType::TYPE_C2K) {
        config.assistanceServer.type = GNSS_ASSISTANCE_TYPE_C2K;
    } else {
        LOC_LOGE("%s]: invalid AGnssType: %d", __FUNCTION__, static_cast<int>(type));
        return false;
    }
    config.assistanceServer.hostName = strdup(hostname.c_str());
    config.assistanceServer.port = port;
    return mGnss->updateConfiguration(config);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
