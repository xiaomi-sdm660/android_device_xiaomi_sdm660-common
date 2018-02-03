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
#pragma once

#include <android/hardware/vibrator/1.1/IVibrator.h>
#include <hidl/Status.h>

#include <fstream>
#include <vector>

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_1 {
namespace implementation {

class Vibrator : public IVibrator {
public:
    Vibrator(std::ofstream&& duration, std::ofstream&& vtgInput,
            std::ofstream&& mode, std::ofstream&& bufferUpdate,
            std::vector<std::ofstream>&& buffers);

    // Methods from ::android::hardware::vibrator::V1_0::IVibrator follow.
    using Status = ::android::hardware::vibrator::V1_0::Status;
    Return<Status> on(uint32_t timeoutMs) override;
    Return<Status> off() override;
    Return<bool> supportsAmplitudeControl() override;
    Return<Status> setAmplitude(uint8_t amplitude) override;

    using EffectStrength = ::android::hardware::vibrator::V1_0::EffectStrength;
    using Effect = ::android::hardware::vibrator::V1_0::Effect;
    Return<void> perform(Effect effect, EffectStrength strength,
            perform_cb _hidl_cb) override;
    Return<void> perform_1_1(Effect_1_1 effect, EffectStrength strength,
            perform_cb _hidl_cb) override;

private:
    Return<Status> on(uint32_t timeoutMs, bool isWaveform);
    std::ofstream mDuration;
    std::ofstream mVtgInput;
    std::ofstream mMode;
    std::ofstream mBufferUpdate;
    std::vector<std::ofstream> mBuffers;
    int32_t mClickDuration;
    int32_t mTickDuration;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
