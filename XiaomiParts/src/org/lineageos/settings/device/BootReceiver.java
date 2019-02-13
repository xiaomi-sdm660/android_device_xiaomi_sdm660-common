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
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class BootReceiver extends BroadcastReceiver {

    private final String TORCH_1_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/spmi-0/" +
            "spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_0/max_brightness";
    private final String TORCH_2_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/spmi-0/" +
            "spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_1/max_brightness";
    private final String VIBRATION_STRENGTH_PATH = "/sys/devices/virtual/timed_output/vibrator/vtg_level";

    public void onReceive(Context context, Intent intent) {
        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);

        FileUtils.setValue(TORCH_1_BRIGHTNESS_PATH, sharedPrefs.getInt(
                DeviceSettings.PREF_TORCH_BRIGHTNESS, 100));
        FileUtils.setValue(TORCH_2_BRIGHTNESS_PATH, sharedPrefs.getInt(
                DeviceSettings.PREF_TORCH_BRIGHTNESS, 100));
        FileUtils.setValue(VIBRATION_STRENGTH_PATH, sharedPrefs.getInt(
                DeviceSettings.PREF_VIBRATION_STRENGTH, 80) / 100.0 *
                (DeviceSettings.MAX_VIBRATION - DeviceSettings.MIN_VIBRATION) +
                DeviceSettings.MIN_VIBRATION);

        context.startService(new Intent(context, DiracService.class));
    }
}
