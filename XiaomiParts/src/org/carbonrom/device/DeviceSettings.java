/*
#
# Copyright (C) 2018 The Xiaomi-SDM660 Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#
*/
package org.carbonrom.settings.device;

import android.os.Bundle;
import android.os.SystemProperties;
import android.support.v14.preference.PreferenceFragment;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceCategory;
import android.support.v7.preference.PreferenceManager;
import android.support.v7.preference.PreferenceScreen;
import android.support.v14.preference.SwitchPreference;
import android.provider.Settings;
import android.view.View;
import android.util.Log;

public class DeviceSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener {

    private static final String KEY_CATEGORY_CAMERA = "camera";
    private static final String ENABLE_HAL3_KEY = "hal3";

    private static final String HAL3_SYSTEM_PROPERTY = "persist.camera.HAL3.enabled";

    private SwitchPreference mEnableHAL3;
    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.main, rootKey);
        mEnableHAL3 = (SwitchPreference) findPreference(ENABLE_HAL3_KEY);
        mEnableHAL3.setChecked(SystemProperties.getBoolean(HAL3_SYSTEM_PROPERTY, false));
        mEnableHAL3.setOnPreferenceChangeListener(this);
    }

   private void setEnableHAL3(boolean value) {
        if(value) {
            SystemProperties.set(HAL3_SYSTEM_PROPERTY, "1");
        } else {
            SystemProperties.set(HAL3_SYSTEM_PROPERTY, "0");
        }
    }

    @Override
    public boolean onPreferenceTreeClick(Preference preference) {
        return super.onPreferenceTreeClick(preference);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final String key = preference.getKey();
        boolean value;
        String strvalue;
        if (ENABLE_HAL3_KEY.equals(key)) {
            value = (Boolean) newValue;
            mEnableHAL3.setChecked(value);
            setEnableHAL3(value);
            return true;
        }
        return true;
    }

}
