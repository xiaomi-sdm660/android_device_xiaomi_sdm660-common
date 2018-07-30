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
#define DUTY_PCTS                  "duty_pcts"
#define PAUSE_HI                   "pause_hi"
#define PAUSE_LO                   "pause_lo"
#define RAMP_STEP_MS               "ramp_step_ms"
#define START_IDX                  "start_idx"

/*
 * 8 duty percent steps.
 */
#define RAMP_STEPS 15
/*
 * Each step will stay on for 50ms by default.
 */
#define RAMP_STEP_DURATION 150
/*
 * Each value represents a duty percent (0 - 100) for the led pwm.
 */
static int32_t BRIGHTNESS_RAMP[RAMP_STEPS] = {0, 12, 25, 37, 50, 72, 85, 100, 85, 72, 50, 37, 25, 12, 0};

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

/*
 * Scale each value of the brightness ramp according to the
 * brightness of the color.
 */
static std::string getScaledRamp(uint32_t brightness) {
    std::string ramp, pad;

    for (auto const& step : BRIGHTNESS_RAMP) {
        ramp += pad + std::to_string(step * brightness / 0xFF);
        pad = ",";
    }

    return ramp;
}

static void handleWayneBacklight(Type /*type*/, const LightState& state) {
    uint32_t brightness = state.color & 0xFF;
    set(LCD_LED BRIGHTNESS, brightness);
}

static void setNotification(const LightState& state) {
    uint32_t redBrightness, brightness;

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
        /*
         * If the flashOnMs duration is not long enough to fit ramping up
         * and down at the default step duration, step duration is modified
         * to fit.
        */
        int32_t stepDuration = RAMP_STEP_DURATION;
        int32_t pauseHi = state.flashOnMs - (stepDuration * RAMP_STEPS * 2);
        int32_t pauseLo = state.flashOffMs;

        if (pauseHi < 0) {
            pauseHi = 0;
        }

        /* Red(Actually White) */
        set(RED BLINK, 1);
        set(RED START_IDX, 0 * RAMP_STEPS);
        set(RED DUTY_PCTS, getScaledRamp(redBrightness));
        set(RED PAUSE_LO, pauseLo);
        set(RED PAUSE_HI, pauseHi);
        set(RED RAMP_STEP_MS, stepDuration);
    } else {
        set(RED BRIGHTNESS, redBrightness);
    }
}

static inline bool isLit(const LightState& state) {
    return state.color & 0x00ffffff;
}

/*
 * Keep sorted in the order of importance.
 */
static const LightState offState = {};
static std::vector<std::pair<Type, LightState>> notificationStates = {
    { Type::ATTENTION, offState },
    { Type::NOTIFICATIONS, offState },
    { Type::BATTERY, offState },
};

static void handleWayneNotification(Type type, const LightState& state) {
    for(auto it : notificationStates) {
        if (it.first == type) {
            it.second = state;
        }

        if  (isLit(it.second)) {
            setNotification(it.second);
            return;
        }
    }

    setNotification(offState);
}

static std::map<Type, std::function<void(Type type, const LightState&)>> lights = {
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
    it->second(type, state);
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
