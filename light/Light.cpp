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

//Author := dev_harsh1998, Isaac Chen

#define LOG_TAG "android.hardware.light@2.0-service.xiaomi_wayne"

#include <log/log.h>
#include <fstream>
#include "Light.h"

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

#define LEDS                       "/sys/class/leds/"
#define LCD_LED                    LEDS "lcd-backlight/"
#define BRIGHTNESS                 "brightness"
#define RED                        LEDS "red/"
#define BLINK                      "blink"

/*
 * Write value to path and close file.
 */
static void set(std::string path, std::string value) {
    std::ofstream file(path);
    /* Only write brightness value if stream is open, alive & well */
    if (file.is_open()) {
        file << value;
    } else {
        /* Fire a warning a bail out */
        ALOGE("failed to write %s to %s", value.c_str(), path.c_str());
        return;
    }
}

static void set(std::string path, int value) {
    set(path, std::to_string(value));
}

static void handleWayneBacklight(const LightState& state) {
    uint32_t brightness = state.color & 0xFF;
    set(LCD_LED BRIGHTNESS, brightness);
}

static void handleWayneNotification(const LightState& state) {
    uint32_t redBrightness, brightness, blink;

    /*
     * Extract brightness from RGB.
     */
    redBrightness   = (state.color >> 16) & 0xFF;
    brightness      = (state.color >> 24) & 0xFF;

    /*
     * Scale RGB brightness if the Alpha brightness is not 0xFF.
     */
    if (brightness != 0xFF) {
        redBrightness   = (redBrightness * brightness) / 0xFF;
    }

    /* Turn off the leds (initially) */
    set(RED BRIGHTNESS, 0);
    set(RED BLINK, 0);

    if (state.flashMode == Flash::TIMED) {
        uint32_t onMS  = state.flashOnMs;
        uint32_t offMS = state.flashOffMs;

        if (onMS > 0 && offMS > 0) {
            blink = 1;
        } else {
            blink = 0;
        }

        /* Red(Actually White) */
        set(RED BLINK, blink);
        set(RED BRIGHTNESS, 0);

    } else {
        set(RED BRIGHTNESS, redBrightness);
    }
}

static std::map<Type, std::function<void(const LightState&)>> lights = {
    {Type::BACKLIGHT, handleWayneBacklight},
    {Type::NOTIFICATIONS, handleWayneNotification},
    {Type::BATTERY, handleWayneNotification},
    {Type::ATTENTION, handleWayneNotification},
};

Light::Light() {}

Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = lights.find(type);

    if (it == lights.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    /*
     * Lock global mutex until light state is updated.
     */

    std::lock_guard<std::mutex> lock(globalLock);
    it->second(state);
    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (auto const& light : lights) types.push_back(light.first);

    _hidl_cb(types);

    return Void();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
