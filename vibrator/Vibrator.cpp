/*
 * Copyright (C) 2017 The LineageOS Project
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

#define LOG_TAG "VibratorService"

#include <log/log.h>

#include <cutils/properties.h>

#include "Vibrator.h"

#include <cmath>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

namespace {
static constexpr int32_t MIN_VTG_INPUT = 120;
static constexpr int32_t MAX_VTG_INPUT = 2750;

static constexpr char MODE_DIRECT[] = "direct";
static constexpr char MODE_BUFFER[] = "buffer";

static constexpr int32_t WAVEFORM_CLICK_EFFECT_MS = 16;
static constexpr uint8_t WAVEFORM_CLICK_EFFECT_SEQ[] = {
    18, 24, 26, 40, 62, 0, 0, 0
};

static constexpr int32_t WAVEFORM_TICK_EFFECT_MS = 8;
static constexpr uint8_t WAVEFORM_TICK_EFFECT_SEQ[] = {
    18, 40, 62, 40, 16, 4, 0, 0
};

static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_MS = 128;
static constexpr uint8_t WAVEFORM_DOUBLE_CLICK_EFFECT_SEQ[] = {
    62, 48, 24, 0, 0, 62, 48, 24
};

/*
 * Vibrator buffer mode
 *
 * 8 byte buffer, corresponding to a whole buffer played at a time
 * Each byte is a voltage value, corresponding to the following
 * formula:
 * Voltage = ((0.116mV * (VALUE >> 1))
 * Voltage = Voltage * 2 if (0x40 & VALUE)
 * Each byte plays for period set in DTSI, qcom,play-rate-us, in microseconds
 *
 * Valid values of VALUE without overdrive (x2 voltage) range from
 * 0 to 31, for valid voltages of 0-3596
 *
 * Valid values of VALUE with overdrive (x2 voltage) range from
 * 32 to 62, for valid voltages of 3712-7192
 */
static int convertVoltageLevel(uint8_t voltageLevel) {
    uint8_t val = 0;

    if (voltageLevel > 62) {
        return -EINVAL;
    }

    val = voltageLevel;

    if (voltageLevel > 31) {
        /* Divide value by 2, since setting bit 6 doubles value */
        val = val >> 1;
    }

    /* Shift one bit, since bit 0 is unused */
    val = val << 1;

    /* Set bit 6 for overdrive */
    if (voltageLevel > 31) {
        val |= 0x40;
    }

    return val;
}

static int convertEffectStrength(
        android::hardware::vibrator::V1_0::EffectStrength strength,
        int voltageLevel) {
    switch (strength) {
    case android::hardware::vibrator::V1_0::EffectStrength::LIGHT:
        /* Cut strength in half */
        return voltageLevel >> 1;
        break;
    case android::hardware::vibrator::V1_0::EffectStrength::MEDIUM:
    case android::hardware::vibrator::V1_0::EffectStrength::STRONG:
    default:
        return voltageLevel;
        break;
    }
}
} // anonymous namespace

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_1 {
namespace implementation {

using Status = ::android::hardware::vibrator::V1_0::Status;

Vibrator::Vibrator(std::ofstream&& duration, std::ofstream&& vtgInput,
        std::ofstream&& mode, std::ofstream&& bufferUpdate,
        std::vector<std::ofstream>&& buffers) :
    mDuration(std::move(duration)),
    mVtgInput(std::move(vtgInput)),
    mMode(std::move(mode)),
    mBufferUpdate(std::move(bufferUpdate)),
    mBuffers(std::move(buffers)) {

    mClickDuration = property_get_int32("ro.vibrator.hal.click.duration",
            WAVEFORM_CLICK_EFFECT_MS);
    mTickDuration = property_get_int32("ro.vibrator.hal.tick.duration",
            WAVEFORM_TICK_EFFECT_MS);
}

Return<Status> Vibrator::on(uint32_t timeoutMs, bool isWaveform) {
    if (isWaveform) {
        mMode << MODE_BUFFER << std::endl;
    } else {
        mMode << MODE_DIRECT << std::endl;
    }

    mDuration << timeoutMs << std::endl;
    if (!mDuration) {
        ALOGE("Failed to activate (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }

   return Status::OK;
}

Return<Status> Vibrator::on(uint32_t timeoutMs) {
    return on(timeoutMs, false /* isWaveform */);
}

Return<Status> Vibrator::off()  {
    mDuration << 0 << std::endl;
    if (!mDuration) {
        ALOGE("Failed to turn vibrator off (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }

    return Status::OK;
}

Return<bool> Vibrator::supportsAmplitudeControl()  {
    return (mVtgInput ? true : false);
}

Return<Status> Vibrator::setAmplitude(uint8_t amplitude) {

    if (amplitude == 0) {
        return Status::BAD_VALUE;
    }

    int32_t voltage =
            std::lround((amplitude - 1) / 254.0 * (MAX_VTG_INPUT - MIN_VTG_INPUT) +
            MIN_VTG_INPUT);

    mVtgInput << voltage << std::endl;
    if (!mVtgInput) {
        ALOGE("Failed to set amplitude (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }

    return Status::OK;
}

Return<void> Vibrator::perform(Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    Status status = Status::OK;
    uint32_t timeMS;

    if (effect == Effect::CLICK) {
        for (uint32_t i = 0; i < ARRAY_SIZE(WAVEFORM_CLICK_EFFECT_SEQ); i++) {
            mBuffers[i] << std::hex << convertEffectStrength(strength,
                    convertVoltageLevel(WAVEFORM_CLICK_EFFECT_SEQ[i])) <<
                std::endl;
        }
        mBufferUpdate << 1 << std::endl;
        timeMS = mClickDuration;
    } else if (effect == Effect::DOUBLE_CLICK) {
        for (uint32_t i = 0; i < ARRAY_SIZE(WAVEFORM_DOUBLE_CLICK_EFFECT_SEQ);
                i++) {
            mBuffers[i] << std::hex << convertEffectStrength(strength,
                    convertVoltageLevel(WAVEFORM_DOUBLE_CLICK_EFFECT_SEQ[i])) <<
                std::endl;
        }
        mBufferUpdate << 1 << std::endl;
        timeMS = WAVEFORM_DOUBLE_CLICK_EFFECT_MS;
    } else {
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }

    on(timeMS, true /* isWaveform */);

    _hidl_cb(status, timeMS);
    return Void();
}

Return<void> Vibrator::perform_1_1(Effect_1_1 effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    Status status = Status::OK;
    uint32_t timeMS;

    if (effect == Effect_1_1::TICK) {
        for (uint32_t i = 0; i < ARRAY_SIZE(WAVEFORM_TICK_EFFECT_SEQ); i++) {
            mBuffers[i] << std::hex << convertEffectStrength(strength,
                    convertVoltageLevel(WAVEFORM_TICK_EFFECT_SEQ[i])) <<
                std::endl;
        }
        mBufferUpdate << 1 << std::endl;
        timeMS = mTickDuration;
    } else if (effect < Effect_1_1::TICK) {
        return perform(static_cast<Effect>(effect), strength, _hidl_cb);
    } else {
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }

    on(timeMS, true /* isWaveform */);

    _hidl_cb(status, timeMS);
    return Void();
}

} // namespace implementation
} // namespace V1_1
} // namespace vibrator
} // namespace hardware
} // namespace android
