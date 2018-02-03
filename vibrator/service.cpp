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
#define LOG_TAG "android.hardware.vibrator@1.1-service.xiaomi_sdm660"

#include <hidl/HidlTransportSupport.h>

#include "Vibrator.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using android::hardware::vibrator::V1_1::IVibrator;
using android::hardware::vibrator::V1_1::implementation::Vibrator;

using android::OK;
using android::sp;
using android::status_t;

static constexpr char DURATION_PATH[] = "/sys/devices/virtual/timed_output/vibrator/enable";
static constexpr char VTG_INPUT_PATH[] = "/sys/devices/virtual/timed_output/vibrator/vtg_level";
static constexpr char MODE_PATH[] = "/sys/devices/virtual/timed_output/vibrator/play_mode";
static constexpr char BUFFER_UPDATE_PATH[] = "/sys/devices/virtual/timed_output/vibrator/wf_update";
static constexpr char BUFFER_PATH_PATTERN[] = "/sys/devices/virtual/timed_output/vibrator/wf_s%d";

static constexpr int8_t NUM_BUFFERS = 8;

status_t registerVibratorService() {
    // ostreams below are required
    std::ofstream duration{DURATION_PATH};
    if (!duration) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", DURATION_PATH, error,
                strerror(error));
        return -error;
    }

    std::ofstream vtgInput{VTG_INPUT_PATH};
    if (!vtgInput) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", VTG_INPUT_PATH, error,
                strerror(error));
        return -error;
    }

    std::ofstream mode{MODE_PATH};
    if (!mode) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", MODE_PATH, error, strerror(error));
        return -error;
    }

    std::ofstream bufferUpdate{BUFFER_UPDATE_PATH};
    if (!mode) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", BUFFER_UPDATE_PATH, error,
                strerror(error));
        return -error;
    }

    std::vector<std::ofstream> buffers;
    for (int i = 0; i < NUM_BUFFERS; i++) {
      char path[sizeof(BUFFER_PATH_PATTERN) + 1];
      snprintf(path, sizeof(path), BUFFER_PATH_PATTERN, i);
      std::ofstream buf{path};
      if (!buf) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", path, error, strerror(error));
        return -error;
      }
      buffers.push_back(std::move(buf));
    }

    sp<IVibrator> vibrator = new Vibrator(std::move(duration),
            std::move(vtgInput),std::move(mode), std::move(bufferUpdate),
            std::move(buffers));

    vibrator->registerAsService();

    return OK;
}

int main() {
    configureRpcThreadpool(1, true);
    status_t status = registerVibratorService();

    if (status != OK) {
        return status;
    }

    ALOGI("Vibrator HAL service ready.");

    joinRpcThreadpool();

    ALOGI("Vibrator HAL service failed to join thread pool.");
    return 1;
}
