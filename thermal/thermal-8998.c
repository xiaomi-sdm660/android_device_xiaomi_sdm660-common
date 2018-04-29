/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 * Not a contribution
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

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "ThermalHAL-8998"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/thermal.h>

#define MAX_LENGTH                    50

#define TEMPERATURE_FILE_FORMAT       "/sys/class/thermal/thermal_zone%d/temp"

#define BATTERY_SENSOR_NUM            0
#define GPU_SENSOR_NUM                17
#define SKIN_SENSOR_NUM               5

const int CPU_SENSORS[] = {8, 9, 10, 11, 12, 13, 14,15};

#define CPU_NUM                       (sizeof(CPU_SENSORS) / sizeof(int))
// Sum of CPU_NUM + 3 for GPU, BATTERY, and SKIN.
#define TEMPERATURE_NUM               (CPU_NUM + 3)

//therm-reset-temp
#define CPU_SHUTDOWN_THRESHOLD        115
//limit-temp
#define CPU_THROTTLING_THRESHOLD      60
#define BATTERY_SHUTDOWN_THRESHOLD    60
//must match thermal-engine.conf
#define SKIN_THROTTLING_THRESHOLD     44
#define SKIN_SHUTDOWN_THRESHOLD       70
#define VR_THROTTLED_BELOW_MIN        58

#define GPU_LABEL                     "GPU"
#define BATTERY_LABEL                 "battery"
#define SKIN_LABEL                    "skin"

const char *CPU_LABEL[] = {"CPU0", "CPU1", "CPU2", "CPU3", "CPU4", "CPU5", "CPU6", "CPU7"};

const char *get_cpu_label(unsigned int cpu_num) {

    if(cpu_num >= CPU_NUM)
        return NULL;

    return CPU_LABEL[cpu_num];
}

size_t get_num_cpus() {
    return CPU_NUM;
}

/**
 * Reads device temperature.
 *
 * @param sensor_num Number of sensor file with temperature.
 * @param type Device temperature type.
 * @param name Device temperature name.
 * @param mult Multiplier used to translate temperature to Celsius.
 * @param throttling_threshold Throttling threshold for the temperature.
 * @param shutdown_threshold Shutdown threshold for the temperature.
 * @param out Pointer to temperature_t structure that will be filled with current
 *     values.
 *
 * @return 0 on success or negative value -errno on error.
 */
static ssize_t read_temperature(int sensor_num, int type, const char *name, float mult,
        float throttling_threshold, float shutdown_threshold, float vr_throttling_threshold,
        temperature_t *out) 
{
    ALOGD("Entering %s",__func__);
    FILE *file;
    char file_name[MAX_LENGTH];
    float temp;

    snprintf(file_name, sizeof(file_name), TEMPERATURE_FILE_FORMAT, sensor_num);
    file = fopen(file_name, "r");
    if (file == NULL) {
        ALOGE("%s: failed to open: %s", __func__, strerror(errno));
        return -errno;
    }
    if (1 != fscanf(file, "%f", &temp)) {
        fclose(file);
        ALOGE("%s: failed to read a float: %s", __func__, strerror(errno));
        return errno ? -errno : -EIO;
    }

    fclose(file);

    (*out) = (temperature_t) {
        .type = type,
        .name = name,
        .current_value = temp * mult,
        .throttling_threshold = throttling_threshold,
        .shutdown_threshold = shutdown_threshold,
        .vr_throttling_threshold = vr_throttling_threshold
    };

    return 0;
}

static ssize_t get_cpu_temperatures(temperature_t *list, size_t size) 
{
    ALOGD("Entering %s",__func__);
    size_t cpu;
    for (cpu = 0; cpu < CPU_NUM; cpu++) {
        if (cpu >= size) {
            break;
        }
        // tsens_tz_sensor[4,6,9,11]: temperature in decidegrees Celsius.
        ssize_t result = read_temperature(CPU_SENSORS[cpu], DEVICE_TEMPERATURE_CPU, CPU_LABEL[cpu],
                0.1, CPU_THROTTLING_THRESHOLD, CPU_SHUTDOWN_THRESHOLD, UNKNOWN_TEMPERATURE,
                &list[cpu]);
        if (result != 0) {
            return result;
        }
    }

    return cpu;
}

ssize_t get_temperatures(thermal_module_t *module, temperature_t *list, size_t size) {
    ALOGD("Entering %s",__func__);
    ssize_t result = 0;
    size_t current_index = 0;

    if (list == NULL) {
        return TEMPERATURE_NUM;
    }

    result = get_cpu_temperatures(list, size);
    if (result < 0) {
        return result;
    }
    current_index += result;

    // GPU temperature.
    if (current_index < size) {
        // tsens_tz_sensor14: temperature in decidegrees Celsius.
        result = read_temperature(GPU_SENSOR_NUM, DEVICE_TEMPERATURE_GPU, GPU_LABEL, 0.1,
                UNKNOWN_TEMPERATURE, UNKNOWN_TEMPERATURE, UNKNOWN_TEMPERATURE,
                &list[current_index]);
        if (result < 0) {
            return result;
        }
        current_index++;
    }

    // Battery temperature.
    if (current_index < size) {
        // tsens_tz_sensor29: battery: temperature in millidegrees Celsius.
        result = read_temperature(BATTERY_SENSOR_NUM, DEVICE_TEMPERATURE_BATTERY, BATTERY_LABEL,
                0.001, UNKNOWN_TEMPERATURE, BATTERY_SHUTDOWN_THRESHOLD, UNKNOWN_TEMPERATURE,
                &list[current_index]);
        if (result < 0) {
            return result;
        }
        current_index++;
    }

    // Skin temperature.
    if (current_index < size) {
        // tsens_tz_sensor24: temperature in Celsius.
        result = read_temperature(SKIN_SENSOR_NUM, DEVICE_TEMPERATURE_SKIN, SKIN_LABEL, 1.,
                SKIN_THROTTLING_THRESHOLD, SKIN_SHUTDOWN_THRESHOLD, VR_THROTTLED_BELOW_MIN,
                &list[current_index]);
        if (result < 0) {
            return result;
        }
        current_index++;
    }
    return TEMPERATURE_NUM;
}
