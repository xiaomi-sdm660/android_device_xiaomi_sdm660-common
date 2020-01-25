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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.power@1.3-service.xiaomi_sdm660-libperfmgr"

#include "Power.h"

#include <mutex>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#include <utils/Log.h>
#include <utils/Trace.h>

#include "power-helper.h"

/* RPM runs at 19.2Mhz. Divide by 19200 for msec */
#define RPM_CLK 19200

#ifndef TAP_TO_WAKE_NODE
#define TAP_TO_WAKE_NODE "/sys/touchpanel/double_tap"
#endif

extern struct stat_pair rpm_stat_map[];

namespace android {
namespace hardware {
namespace power {
namespace V1_3 {
namespace implementation {

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::power::V1_0::Feature;
using ::android::hardware::power::V1_0::PowerStatePlatformSleepState;
using ::android::hardware::power::V1_0::Status;
using ::android::hardware::power::V1_1::PowerStateSubsystem;
using ::android::hardware::power::V1_1::PowerStateSubsystemSleepState;

constexpr char kPowerHalStateProp[] = "vendor.powerhal.state";
constexpr char kPowerHalAudioProp[] = "vendor.powerhal.audio";
constexpr char kPowerHalInitProp[] = "vendor.powerhal.init";
constexpr char kPowerHalRenderingProp[] = "vendor.powerhal.rendering";
constexpr char kPowerHalConfigPath[] = "/vendor/etc/powerhint.json";

Power::Power()
    : mHintManager(nullptr),
      mInteractionHandler(nullptr),
      mSustainedPerfModeOn(false),
      mReady(false) {
    mInitThread = std::thread([this]() {
        android::base::WaitForProperty(kPowerHalInitProp, "1");
        mHintManager = HintManager::GetFromJSON(kPowerHalConfigPath);
        if (!mHintManager) {
            LOG(FATAL) << "Invalid config: " << kPowerHalConfigPath;
        }
        mInteractionHandler = std::make_unique<InteractionHandler>(mHintManager);
        mInteractionHandler->Init();
        std::string state = android::base::GetProperty(kPowerHalStateProp, "");
        if (state == "SUSTAINED_PERFORMANCE") {
            ALOGI("Initialize with SUSTAINED_PERFORMANCE on");
            mHintManager->DoHint("SUSTAINED_PERFORMANCE");
            mSustainedPerfModeOn = true;
        } else {
            ALOGI("Initialize PowerHAL");
        }

        state = android::base::GetProperty(kPowerHalAudioProp, "");
        if (state == "AUDIO_LOW_LATENCY") {
            ALOGI("Initialize with AUDIO_LOW_LATENCY on");
            mHintManager->DoHint("AUDIO_LOW_LATENCY");
        }

        state = android::base::GetProperty(kPowerHalRenderingProp, "");
        if (state == "EXPENSIVE_RENDERING") {
            ALOGI("Initialize with EXPENSIVE_RENDERING on");
            mHintManager->DoHint("EXPENSIVE_RENDERING");
        }
        // Now start to take powerhint
        mReady.store(true);
        ALOGI("PowerHAL ready to process hints");
    });
    mInitThread.detach();
}

// Methods from ::android::hardware::power::V1_0::IPower follow.
Return<void> Power::setInteractive(bool /* interactive */) {
    return Void();
}

Return<void> Power::powerHint(PowerHint_1_0 hint, int32_t data) {
    if (!mReady) {
        return Void();
    }
    ALOGD_IF(hint != PowerHint_1_0::INTERACTION, "%s: %d",
             android::hardware::power::V1_0::toString(hint).c_str(), static_cast<int>(data));
    switch (hint) {
        case PowerHint_1_0::INTERACTION:
            if (mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                mInteractionHandler->Acquire(data);
            }
            break;
        case PowerHint_1_0::SUSTAINED_PERFORMANCE:
            if (data && !mSustainedPerfModeOn) {
                mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                mSustainedPerfModeOn = true;
            } else if (!data && mSustainedPerfModeOn) {
                mHintManager->EndHint("SUSTAINED_PERFORMANCE");
                mSustainedPerfModeOn = false;
            }
            break;
        case PowerHint_1_0::LAUNCH:
            if (mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    // Hint until canceled
                    mHintManager->DoHint("LAUNCH");
                } else {
                    mHintManager->EndHint("LAUNCH");
                }
            }
            break;
        case PowerHint_1_0::LOW_POWER:
            break;
        default:
            break;
    }
    return Void();
}

Return<void> Power::setFeature(Feature feature, bool activate)  {
    switch (feature) {
#ifdef TAP_TO_WAKE_NODE
        case Feature::POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
            ::android::base::WriteStringToFile(activate ? "1" : "0", TAP_TO_WAKE_NODE, true);
            break;
#endif
        default:
            break;
    }
    return Void();
}

Return<void> Power::getPlatformLowPowerStats(getPlatformLowPowerStats_cb _hidl_cb) {

    hidl_vec<PowerStatePlatformSleepState> states;
    uint64_t stats[MAX_PLATFORM_STATS * MAX_RPM_PARAMS] = {0};
    uint64_t *values;
    struct PowerStatePlatformSleepState *state;
    int ret;

    states.resize(PLATFORM_SLEEP_MODES_COUNT);

    ret = extract_platform_stats(stats);
    if (ret != 0) {
        states.resize(0);
        goto done;
    }

    /* Update statistics for XO_shutdown */
    state = &states[RPM_MODE_XO];
    state->name = "XO_shutdown";
    values = stats + (RPM_MODE_XO * MAX_RPM_PARAMS);

    state->residencyInMsecSinceBoot = values[1];
    state->totalTransitions = values[0];
    state->supportedOnlyInSuspend = false;
    state->voters.resize(XO_VOTERS);
    for(size_t i = 0; i < XO_VOTERS; i++) {
        int voter = static_cast<int>(i + XO_VOTERS_START);
        state->voters[i].name = rpm_stat_map[voter].label;
        values = stats + (voter * MAX_RPM_PARAMS);
        state->voters[i].totalTimeInMsecVotedForSinceBoot = values[0] / RPM_CLK;
        state->voters[i].totalNumberOfTimesVotedSinceBoot = values[1];
    }

    /* Update statistics for VMIN state */
    state = &states[RPM_MODE_VMIN];
    state->name = "VMIN";
    values = stats + (RPM_MODE_VMIN * MAX_RPM_PARAMS);

    state->residencyInMsecSinceBoot = values[1];
    state->totalTransitions = values[0];
    state->supportedOnlyInSuspend = false;
    state->voters.resize(VMIN_VOTERS);
    //Note: No filling of state voters since VMIN_VOTERS = 0

done:
    _hidl_cb(states, Status::SUCCESS);
    return Void();
}

// Methods from ::android::hardware::power::V1_1::IPower follow.
Return<void> Power::getSubsystemLowPowerStats(getSubsystemLowPowerStats_cb _hidl_cb) {
    hidl_vec<PowerStateSubsystem> subsystems;
    subsystems.resize(0);
    _hidl_cb(subsystems, Status::SUCCESS);
    return Void();
}

Return<void> Power::powerHintAsync(PowerHint_1_0 hint, int32_t data) {
    // just call the normal power hint in this oneway function
    return powerHint(hint, data);
}

// Methods from ::android::hardware::power::V1_2::IPower follow.
Return<void> Power::powerHintAsync_1_2(PowerHint_1_2 hint, int32_t data) {
    if (!mReady) {
        return Void();
    }

    ALOGD_IF(hint >= PowerHint_1_2::AUDIO_STREAMING, "%s: %d",
             android::hardware::power::V1_2::toString(hint).c_str(), static_cast<int>(data));

    switch (hint) {
        case PowerHint_1_2::AUDIO_LOW_LATENCY:
            if (data) {
                // Hint until canceled
                mHintManager->DoHint("AUDIO_LOW_LATENCY");
            } else {
                mHintManager->EndHint("AUDIO_LOW_LATENCY");
            }
            break;
        case PowerHint_1_2::AUDIO_STREAMING:
            if (mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    mHintManager->DoHint("AUDIO_STREAMING");
                } else {
                    mHintManager->EndHint("AUDIO_STREAMING");
                }
            }
            break;
        default:
            return powerHint(static_cast<PowerHint_1_0>(hint), data);
    }
    return Void();
}

// Methods from ::android::hardware::power::V1_3::IPower follow.
Return<void> Power::powerHintAsync_1_3(PowerHint_1_3 hint, int32_t data) {
    if (!mReady) {
        return Void();
    }

    if (hint == PowerHint_1_3::EXPENSIVE_RENDERING) {
        if (mSustainedPerfModeOn) {
            ALOGV("%s: ignoring due to other active perf hints", __func__);
        } else {
            if (data > 0) {
                mHintManager->DoHint("EXPENSIVE_RENDERING");
            } else {
                mHintManager->EndHint("EXPENSIVE_RENDERING");
            }
        }
    } else {
        return powerHintAsync_1_2(static_cast<PowerHint_1_2>(hint), data);
    }
    return Void();
}

}  // namespace implementation
}  // namespace V1_3
}  // namespace power
}  // namespace hardware
}  // namespace android
