/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2020 The LineageOS Project
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

#define LOG_TAG "android.hardware.light@2.0-service.xiaomi_sdm660"

#include "Light.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <unistd.h>

#include <iomanip>

namespace {

#define LEDS "/sys/class/leds/"
#define LCD_LED LEDS "lcd-backlight/"
#define WHITE LEDS "white/"
#define RED LEDS "red/"
#define BUTTON LEDS "button-backlight/"
#define BUTTON1 LEDS "button-backlight1/"
#define BRIGHTNESS "brightness"
#define MAX_BRIGHTNESS "max_brightness"
#define BLINK "blink"
#define DUTY_PCTS "duty_pcts"
#define PAUSE_HI "pause_hi"
#define PAUSE_LO "pause_lo"
#define RAMP_STEP_MS "ramp_step_ms"
#define START_IDX "start_idx"

using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;

// Default max brightness
constexpr auto kDefaultMaxLedBrightness = 255;
constexpr auto kDefaultMaxScreenBrightness = 4095;

// Each step will stay on for 150ms by default.
constexpr auto kRampStepDuration = 150;

// Each value represents a duty percent (0 - 100) for the led pwm.
constexpr std::array kBrightnessRamp = {0, 12, 25, 37, 50, 72, 85, 100};

// Write value to path and close file.
bool WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

bool WriteToFile(const std::string& path, const std::string& content) {
    return WriteStringToFile(content, path);
}

uint32_t RgbaToBrightness(uint32_t color) {
    // Extract brightness from AARRGGBB.
    uint32_t alpha = (color >> 24) & 0xFF;

    // Retrieve each of the RGB colors
    uint32_t red = (color >> 16) & 0xFF;
    uint32_t green = (color >> 8) & 0xFF;
    uint32_t blue = color & 0xFF;

    // Scale RGB colors if a brightness has been applied by the user
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

inline uint32_t RgbaToBrightness(uint32_t color, uint32_t max_brightness) {
    return RgbaToBrightness(color) * max_brightness / 0xFF;
}

/*
 * Scale each value of the brightness ramp according to the
 * brightness of the color.
 */
std::string GetScaledDutyPcts(uint32_t brightness) {
    std::stringstream ramp;

    for (size_t i = 0; i < kBrightnessRamp.size(); i++) {
        if (i > 0) ramp << ",";
        ramp << kBrightnessRamp[i] * brightness / 0xFF;
    }

    return ramp.str();
}

inline bool IsLit(uint32_t color) {
    return color & 0x00ffffff;
}

}  // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Light::Light() {
    std::string buf;

    if (ReadFileToString(LCD_LED MAX_BRIGHTNESS, &buf)) {
        max_screen_brightness_ = std::stoi(buf);
    } else {
        max_screen_brightness_ = kDefaultMaxScreenBrightness;
        LOG(ERROR) << "Failed to read max screen brightness, fallback to "
                   << kDefaultMaxLedBrightness;
    }

    if (ReadFileToString(WHITE MAX_BRIGHTNESS, &buf)) {
        max_led_brightness_ = std::stoi(buf);
    } else {
        max_led_brightness_ = kDefaultMaxLedBrightness;
        LOG(ERROR) << "Failed to read max white LED brightness, fallback to " << kDefaultMaxLedBrightness;
    }

    if (ReadFileToString(RED MAX_BRIGHTNESS, &buf)) {
        max_red_led_brightness_= std::stoi(buf);
    } else {
        max_red_led_brightness_ = kDefaultMaxLedBrightness;
        LOG(ERROR) << "Failed to read max red LED brightness, fallback to " << kDefaultMaxLedBrightness;
    }

    if (!access(BUTTON BRIGHTNESS, W_OK)) {
        lights_.emplace(std::make_pair(Type::BUTTONS,
                                       [this](auto&&... args) { setLightButtons(args...); }));
        buttons_.emplace_back(BUTTON BRIGHTNESS);

        if (!access(BUTTON1 BRIGHTNESS, W_OK)) {
            buttons_.emplace_back(BUTTON1 BRIGHTNESS);
        }

        if (ReadFileToString(BUTTON MAX_BRIGHTNESS, &buf)) {
            max_button_brightness_ = std::stoi(buf);
        } else {
            max_button_brightness_ = kDefaultMaxLedBrightness;
            LOG(ERROR) << "Failed to read max button brightness, fallback to "
                       << kDefaultMaxLedBrightness;
        }
    }
}

Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = lights_.find(type);

    if (it == lights_.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    it->second(type, state);

    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (auto&& light : lights_) types.emplace_back(light.first);

    _hidl_cb(types);

    return Void();
}

void Light::setLightBacklight(Type /*type*/, const LightState& state) {
    uint32_t brightness = RgbaToBrightness(state.color, max_screen_brightness_);
    WriteToFile(LCD_LED BRIGHTNESS, brightness);
}

void Light::setLightButtons(Type /*type*/, const LightState& state) {
    uint32_t brightness = RgbaToBrightness(state.color, max_button_brightness_);
    for (auto&& button : buttons_) {
        WriteToFile(button, brightness);
    }
}

void Light::setLightNotification(Type type, const LightState& state) {
    bool found = false;
    for (auto&& [cur_type, cur_state] : notif_states_) {
        if (cur_type == type) {
            cur_state = state;
        }

        // Fallback to battery light
        if (!found && (cur_type == Type::BATTERY || IsLit(state.color))) {
            found = true;
            LOG(DEBUG) << __func__ << ": type=" << toString(cur_type);
            applyNotificationState(state);
        }
    }
}

void Light::applyNotificationState(const LightState& state) {
    uint32_t white_brightness = RgbaToBrightness(state.color, max_led_brightness_);
    uint32_t red_brightness = RgbaToBrightness(state.color, max_red_led_brightness_);

    // Turn off the leds (initially)
    WriteToFile(WHITE BLINK, 0);
    WriteToFile(RED BLINK, 0);

    if (state.flashMode == Flash::TIMED && state.flashOnMs > 0 && state.flashOffMs > 0) {
        /*
         * If the flashOnMs duration is not long enough to fit ramping up
         * and down at the default step duration, step duration is modified
         * to fit.
         */
        int32_t step_duration = kRampStepDuration;
        int32_t pause_hi = state.flashOnMs - (step_duration * kBrightnessRamp.size() * 2);
        if (pause_hi < 0) {
            step_duration = state.flashOnMs / (kBrightnessRamp.size() * 2);
            pause_hi = 0;
        }

        LOG(DEBUG) << __func__ << ": color=" << std::hex << state.color << std::dec
                   << " onMs=" << state.flashOnMs << " offMs=" << state.flashOffMs;

        // White
        WriteToFile(WHITE START_IDX, 0);
        WriteToFile(WHITE DUTY_PCTS, GetScaledDutyPcts(white_brightness));
        WriteToFile(WHITE PAUSE_LO, static_cast<uint32_t>(state.flashOffMs));
        WriteToFile(WHITE PAUSE_HI, static_cast<uint32_t>(pause_hi));
        WriteToFile(WHITE RAMP_STEP_MS, static_cast<uint32_t>(step_duration));
        WriteToFile(WHITE BLINK, 1);

        // Red
        WriteToFile(RED START_IDX, 0);
        WriteToFile(RED DUTY_PCTS, GetScaledDutyPcts(red_brightness));
        WriteToFile(RED PAUSE_LO, static_cast<uint32_t>(state.flashOffMs));
        WriteToFile(RED PAUSE_HI, static_cast<uint32_t>(pause_hi));
        WriteToFile(RED RAMP_STEP_MS, static_cast<uint32_t>(step_duration));
        WriteToFile(RED BLINK, 1);
    } else {
        WriteToFile(WHITE BRIGHTNESS, white_brightness);
        WriteToFile(RED BRIGHTNESS, red_brightness);
    }
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
