/*
* Copyright (C) 2020 ronaxdevil <pratabidya.007@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
package org.lineageos.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.provider.Settings;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceManager;

import org.lineageos.settings.device.DeviceSettings;

public class Fastcharge implements OnPreferenceChangeListener {

    private Context mContext;

    public Fastcharge(Context context) {
        mContext = context;
    }

    public static String getFile() {
        if (FileUtils.fileWritable(DeviceSettings.USB_FASTCHARGE_PATH)) {
            return DeviceSettings.USB_FASTCHARGE_PATH;
        }
        return null;
    }

    public static boolean isSupported() {
        return FileUtils.fileWritable(getFile());
    }

    public static boolean isCurrentlyEnabled(Context context) {
        return FileUtils.getFileValueAsBoolean(getFile(), false);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        final String key = preference.getKey();
        switch (key) {
            case DeviceSettings.PREF_USB_FASTCHARGE:
                FileUtils.setValue(DeviceSettings.USB_FASTCHARGE_PATH, (boolean) value);
                break;

            default:
                break;
        }
        return true;
    }
}
