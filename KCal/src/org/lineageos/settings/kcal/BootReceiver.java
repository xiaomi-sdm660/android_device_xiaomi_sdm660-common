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

package org.lineageos.settings.kcal;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class BootReceiver extends BroadcastReceiver implements Utils {
    public void onReceive(Context context, Intent intent) {

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);

        if (sharedPrefs.getBoolean(PREF_SETONBOOT, false)) {
            restoreValue(KCAL_ENABLE, sharedPrefs.getBoolean(PREF_ENABLED, false) ? "1" : "0");

            String rgbValue = sharedPrefs.getInt(PREF_RED, RED_DEFAULT) + " " +
                    sharedPrefs.getInt(PREF_GREEN, GREEN_DEFAULT) + " " +
                    sharedPrefs.getInt(PREF_BLUE, BLUE_DEFAULT);

            restoreValue(KCAL_RGB, rgbValue);
            restoreValue(KCAL_MIN, String.valueOf(sharedPrefs.getInt(PREF_MINIMUM, MINIMUM_DEFAULT)));
            restoreValue(KCAL_SAT, String.valueOf(sharedPrefs.getInt(PREF_SATURATION, SATURATION_DEFAULT) + SATURATION_OFFSET));
            restoreValue(KCAL_VAL, String.valueOf(sharedPrefs.getInt(PREF_VALUE, VALUE_DEFAULT) + VALUE_OFFSET));
            restoreValue(KCAL_CONT, String.valueOf(sharedPrefs.getInt(PREF_CONTRAST, CONTRAST_DEFAULT) + CONTRAST_OFFSET));
            restoreValue(KCAL_HUE, String.valueOf(sharedPrefs.getInt(PREF_HUE, HUE_DEFAULT)));
        }
    }

    private void restoreValue(String path, String value) {
        if (path == null) {
            return;
        }
        try {
            FileOutputStream fos = new FileOutputStream(new File(path));
            fos.write(value.getBytes());
            fos.flush();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
