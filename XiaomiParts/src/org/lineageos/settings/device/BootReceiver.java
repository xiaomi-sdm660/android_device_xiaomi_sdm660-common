/*
 * Copyright (C) 2018 The Xiaomi-SDM660 Project
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
 * limitations under the License
 */

package org.lineageos.settings.device;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;

import org.lineageos.settings.device.kcal.Utils;

public class BootReceiver extends BroadcastReceiver implements Utils {

    private final String TORCH_1_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/spmi-0/" +
            "spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_0/" +
            "max_brightness";
    private final String TORCH_2_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/spmi-0/" +
            "spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_1/" +
            "max_brightness";
    private final String VIBRATION_STRENGTH_PATH = "/sys/devices/virtual/timed_output/vibrator" +
            "/vtg_level";
    private final String HEADPHONE_GAIN_PATH = "/sys/kernel/sound_control/headphone_gain";
    private final String MICROPHONE_GAIN_PATH = "/sys/kernel/sound_control/mic_gain";

    public void onReceive(Context context, Intent intent) {

        if (Settings.Secure.getInt(context.getContentResolver(), PREF_ENABLED, 0) == 1) {
            FileUtils.setValue(KCAL_ENABLE, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_ENABLED, 0));

            String rgbValue = Settings.Secure.getInt(context.getContentResolver(),
                    PREF_RED, RED_DEFAULT) + " " +
                    Settings.Secure.getInt(context.getContentResolver(), PREF_GREEN,
                            GREEN_DEFAULT) + " " +
                    Settings.Secure.getInt(context.getContentResolver(), PREF_BLUE,
                            BLUE_DEFAULT);

            FileUtils.setValue(KCAL_RGB, rgbValue);
            FileUtils.setValue(KCAL_MIN, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_MINIMUM, MINIMUM_DEFAULT));
            FileUtils.setValue(KCAL_SAT, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_GRAYSCALE, 0) == 1 ? 128 :
                    Settings.Secure.getInt(context.getContentResolver(),
                            PREF_SATURATION, SATURATION_DEFAULT) + SATURATION_OFFSET);
            FileUtils.setValue(KCAL_VAL, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_VALUE, VALUE_DEFAULT) + VALUE_OFFSET);
            FileUtils.setValue(KCAL_CONT, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_CONTRAST, CONTRAST_DEFAULT) + CONTRAST_OFFSET);
            FileUtils.setValue(KCAL_HUE, Settings.Secure.getInt(context.getContentResolver(),
                    PREF_HUE, HUE_DEFAULT));
        }

        FileUtils.setValue(TORCH_1_BRIGHTNESS_PATH,
                Settings.Secure.getInt(context.getContentResolver(),
                        DeviceSettings.PREF_TORCH_BRIGHTNESS, 100));
        FileUtils.setValue(TORCH_2_BRIGHTNESS_PATH,
                Settings.Secure.getInt(context.getContentResolver(),
                        DeviceSettings.PREF_TORCH_BRIGHTNESS, 100));
        FileUtils.setValue(VIBRATION_STRENGTH_PATH, Settings.Secure.getInt(
                context.getContentResolver(), DeviceSettings.PREF_VIBRATION_STRENGTH, 80) / 100.0 * (DeviceSettings.MAX_VIBRATION - DeviceSettings.MIN_VIBRATION) + DeviceSettings.MIN_VIBRATION);
        int gain = Settings.Secure.getInt(context.getContentResolver(),
                DeviceSettings.PREF_HEADPHONE_GAIN, 4);
        FileUtils.setValue(HEADPHONE_GAIN_PATH, gain + " " + gain);
        FileUtils.setValue(MICROPHONE_GAIN_PATH, Settings.Secure.getInt(context.getContentResolver(),
                DeviceSettings.PREF_MICROPHONE_GAIN, 0));
        context.startService(new Intent(context, DiracService.class));
    }
}
