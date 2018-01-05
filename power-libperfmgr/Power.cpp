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

#define LOG_TAG "android.hardware.power@1.2-service.sdm660-libperfmgr"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <utils/Log.h>

#include "Power.h"

#include "power-helper.h"

/* RPM runs at 19.2Mhz. Divide by 19200 for msec */
#define RPM_CLK 19200

extern struct stat_pair rpm_stat_map[];

namespace android {
namespace hardware {
namespace power {
namespace V1_2 {
namespace implementation {

using ::android::hardware::power::V1_0::Feature;
using ::android::hardware::power::V1_0::PowerStatePlatformSleepState;
using ::android::hardware::power::V1_0::Status;
using ::android::hardware::power::V1_1::PowerStateSubsystem;
using ::android::hardware::power::V1_1::PowerStateSubsystemSleepState;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

Power::Power() :
        mHintManager(HintManager::GetFromJSON("/vendor/etc/powerhint.json")),
        mInteractionHandler(mHintManager),
        mVRModeOn(false),
        mSustainedPerfModeOn(false),
        mEncoderModeOn(false) {
    mInteractionHandler.Init();
}

// Methods from ::android::hardware::power::V1_0::IPower follow.
Return<void> Power::setInteractive(bool /* interactive */)  {
    return Void();
}

Return<void> Power::powerHint(PowerHint_1_0 hint, int32_t data) {
    if (!isSupportedGovernor()) {
        return Void();
    }

    switch(hint) {
        case PowerHint_1_0::INTERACTION:
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                mInteractionHandler.Acquire(data);
            }
            break;
        case PowerHint_1_0::VIDEO_ENCODE:
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    // Hint until canceled
                    mHintManager->DoHint("VIDEO_ENCODE");
                    ALOGD("VIDEO_ENCODE ON");
                    mEncoderModeOn = true;
                } else {
                    mHintManager->EndHint("VIDEO_ENCODE");
                    ALOGD("VIDEO_ENCODE OFF");
                    mEncoderModeOn = false;
                }
            }
            break;
        case PowerHint_1_0::SUSTAINED_PERFORMANCE:
            if (data && !mSustainedPerfModeOn) {
                ALOGD("SUSTAINED_PERFORMANCE ON");
                if (!mVRModeOn) { // Sustained mode only.
                    mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                } else { // Sustained + VR mode.
                    mHintManager->EndHint("VR_MODE");
                    mHintManager->DoHint("VR_SUSTAINED_PERFORMANCE");
                }
                mSustainedPerfModeOn = true;
            } else if (!data && mSustainedPerfModeOn) {
                ALOGD("SUSTAINED_PERFORMANCE OFF");
                mHintManager->EndHint("VR_SUSTAINED_PERFORMANCE");
                mHintManager->EndHint("SUSTAINED_PERFORMANCE");
                if (mVRModeOn) { // Switch back to VR Mode.
                    mHintManager->DoHint("VR_MODE");
                }
                mSustainedPerfModeOn = false;
            }
            break;
        case PowerHint_1_0::VR_MODE:
            if (data && !mVRModeOn) {
                ALOGD("VR_MODE ON");
                if (!mSustainedPerfModeOn) { // VR mode only.
                    mHintManager->DoHint("VR_MODE");
                } else { // Sustained + VR mode.
                    mHintManager->EndHint("SUSTAINED_PERFORMANCE");
                    mHintManager->DoHint("VR_SUSTAINED_PERFORMANCE");
                }
                mVRModeOn = true;
            } else if (!data && mVRModeOn) {
                ALOGD("VR_MODE OFF");
                mHintManager->EndHint("VR_SUSTAINED_PERFORMANCE");
                mHintManager->EndHint("VR_MODE");
                if (mSustainedPerfModeOn) { // Switch back to sustained Mode.
                    mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                }
                mVRModeOn = false;
            }
            break;
        case PowerHint_1_0::LAUNCH:
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    // Hint until canceled
                    mHintManager->DoHint("LAUNCH");
                    ALOGD("LAUNCH ON");
                } else {
                    mHintManager->EndHint("LAUNCH");
                    ALOGD("LAUNCH OFF");
                }
            }
            break;
        default:
            break;

    }
    return Void();
}

Return<void> Power::setFeature(Feature /*feature*/, bool /*activate*/)  {
    //Nothing to do
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

static int get_wlan_low_power_stats(struct PowerStateSubsystem &subsystem) {

    uint64_t stats[WLAN_POWER_PARAMS_COUNT] = {0};
    struct PowerStateSubsystemSleepState *state;
    int ret;

    ret = extract_wlan_stats(stats);
    if (ret)
        return ret;

    subsystem.name = "wlan";
    subsystem.states.resize(WLAN_STATES_COUNT);

    /* Update statistics for Active State */
    state = &subsystem.states[WLAN_STATE_ACTIVE];
    state->name = "Active";
    state->residencyInMsecSinceBoot = stats[CUMULATIVE_TOTAL_ON_TIME_MS];
    state->totalTransitions = stats[DEEP_SLEEP_ENTER_COUNTER];
    state->lastEntryTimestampMs = 0; //FIXME need a new value from Qcom
    state->supportedOnlyInSuspend = false;

    /* Update statistics for Deep-Sleep state */
    state = &subsystem.states[WLAN_STATE_DEEP_SLEEP];
    state->name = "Deep-Sleep";
    state->residencyInMsecSinceBoot = stats[CUMULATIVE_SLEEP_TIME_MS];
    state->totalTransitions = stats[DEEP_SLEEP_ENTER_COUNTER];
    state->lastEntryTimestampMs = stats[LAST_DEEP_SLEEP_ENTER_TSTAMP_MS];
    state->supportedOnlyInSuspend = false;

    return 0;
}

// Methods from ::android::hardware::power::V1_1::IPower follow.
Return<void> Power::getSubsystemLowPowerStats(getSubsystemLowPowerStats_cb _hidl_cb) {

    hidl_vec<PowerStateSubsystem> subsystems;
    int ret;

    subsystems.resize(SUBSYSTEM_COUNT);

    //We currently have only one Subsystem for WLAN
    ret = get_wlan_low_power_stats(subsystems[SUBSYSTEM_WLAN]);
    if (ret != 0)
        goto done;

    //Add query for other subsystems here

done:
    _hidl_cb(subsystems, Status::SUCCESS);
    return Void();
}

bool Power::isSupportedGovernor() {
    std::string buf;
    if (android::base::ReadFileToString("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", &buf)) {
        buf = android::base::Trim(buf);
    }
    // Only support EAS 1.2, legacy EAS
    if (buf == "schedutil" || buf == "sched") {
        return true;
    } else {
        LOG(ERROR) << "Governor not supported by powerHAL, skipping";
        return false;
    }
}

Return<void> Power::powerHintAsync(PowerHint_1_0 hint, int32_t data) {
    // just call the normal power hint in this oneway function
    return powerHint(hint, data);
}

// Methods from ::android::hardware::power::V1_2::IPower follow.
Return<void> Power::powerHintAsync_1_2(PowerHint_1_2 hint, int32_t data) {
    if (!isSupportedGovernor()) {
        return Void();
    }

    switch(hint) {
        case PowerHint_1_2::AUDIO_LOW_LATENCY:
            if (data) {
                // Hint until canceled
                mHintManager->DoHint("AUDIO_LOW_LATENCY");
                ALOGD("AUDIO LOW LATENCY ON");
            } else {
                mHintManager->EndHint("AUDIO_LOW_LATENCY");
                ALOGD("AUDIO LOW LATENCY OFF");
            }
            break;
        case PowerHint_1_2::AUDIO_STREAMING:
            if (data) {
                // Hint until canceled
                mHintManager->DoHint("AUDIO_STREAMING");
                ALOGD("AUDIO LOW LATENCY ON");
            } else {
                mHintManager->EndHint("AUDIO_STREAMING");
                ALOGD("AUDIO LOW LATENCY OFF");
            }
            break;
        case PowerHint_1_2::CAMERA_LAUNCH:
            if (data > 0) {
                mHintManager->DoHint("CAMERA_LAUNCH", std::chrono::milliseconds(data));
                ALOGD("CAMERA LAUNCH ON: %d MS", data);
                // boosts 2.5s for launching
                mHintManager->DoHint("LAUNCH", std::chrono::milliseconds(2500));
            } else if (data == 0) {
                mHintManager->EndHint("CAMERA_LAUNCH");
                ALOGD("CAMERA LAUNCH OFF");
            } else {
                ALOGE("CAMERA LAUNCH INVALID DATA: %d", data);
            }
            break;
        case PowerHint_1_2::CAMERA_STREAMING:
            if (data > 0) {
                mHintManager->DoHint("CAMERA_STREAMING", std::chrono::milliseconds(data));
                ALOGD("CAMERA STREAMING ON: %d MS", data);
            } else if (data == 0) {
                mHintManager->EndHint("CAMERA_STREAMING");
                ALOGD("CAMERA STREAMING OFF");
            } else {
                ALOGE("CAMERA STREAMING INVALID DATA: %d", data);
            }
            break;
        case PowerHint_1_2::CAMERA_SHOT:
            if (data > 0) {
                mHintManager->DoHint("CAMERA_SHOT", std::chrono::milliseconds(data));
                ALOGD("CAMERA SHOT ON: %d MS", data);
            } else if (data == 0) {
                mHintManager->EndHint("CAMERA_SHOT");
                ALOGD("CAMERA SHOT OFF");
            } else {
                ALOGE("CAMERA SHOT INVALID DATA: %d", data);
            }
            break;
        default:
            return powerHint(static_cast<PowerHint_1_0>(hint), data);
    }
    return Void();
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace power
}  // namespace hardware
}  // namespace android
