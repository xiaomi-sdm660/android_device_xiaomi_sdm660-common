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

#define LOG_TAG "LocSvc_GnssConfigurationInterface"

#include <log_util.h>
#include "Gnss.h"
#include "GnssConfiguration.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

GnssConfiguration::GnssConfiguration(Gnss* gnss) : mGnss(gnss) {
}

// Methods from ::android::hardware::gps::V1_0::IGnssConfiguration follow.
Return<bool> GnssConfiguration::setSuplEs(bool enabled)  {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_SUPL_EM_SERVICES_BIT;
    config.suplEmergencyServices = static_cast<GnssConfigSuplEmergencyServices>(enabled);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setSuplVersion(uint32_t version)  {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_SUPL_VERSION_VALID_BIT;
    config.suplVersion = static_cast<GnssConfigSuplVersion>(version);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setSuplMode(uint8_t mode)  {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_SUPL_MODE_BIT;
    config.suplModeMask = static_cast<GnssConfigSuplModeMask>(mode);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setLppProfile(uint8_t lppProfile) {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_LPP_PROFILE_VALID_BIT;
    config.lppProfile = static_cast<GnssConfigLppProfile>(lppProfile);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setGlonassPositioningProtocol(uint8_t protocol) {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_AGLONASS_POSITION_PROTOCOL_VALID_BIT;
    config.aGlonassPositionProtocolMask =
        static_cast<GnssConfigAGlonassPositionProtocolMask>(protocol);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setGpsLock(uint8_t lock) {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_GPS_LOCK_VALID_BIT;
    config.gpsLock = static_cast<GnssConfigGpsLock>(lock);

    return mGnss->updateConfiguration(config);
}

Return<bool> GnssConfiguration::setEmergencySuplPdn(bool enabled) {
    if (mGnss == nullptr) {
        LOC_LOGE("%s]: mGnss is nullptr", __FUNCTION__);
        return false;
    }

    GnssConfig config;
    memset(&config, 0, sizeof(GnssConfig));
    config.size = sizeof(GnssConfig);
    config.flags = GNSS_CONFIG_FLAGS_EM_PDN_FOR_EM_SUPL_VALID_BIT;
    config.emergencyPdnForEmergencySupl =
        static_cast<GnssConfigEmergencyPdnForEmergencySupl>(enabled);

    return mGnss->updateConfiguration(config);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
