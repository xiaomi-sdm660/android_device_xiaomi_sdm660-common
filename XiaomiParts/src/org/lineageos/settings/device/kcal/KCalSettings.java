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

package org.lineageos.settings.device.kcal;

import android.os.Bundle;
import android.provider.Settings;
import android.support.v14.preference.PreferenceFragment;
import android.support.v7.preference.Preference;

import org.lineageos.settings.device.R;
import org.lineageos.settings.device.preferences.SecureSettingCustomSeekBarPreference;
import org.lineageos.settings.device.preferences.SecureSettingSwitchPreference;

public class KCalSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener, Utils {

    private final FileUtils mFileUtils = new FileUtils();

    private SecureSettingSwitchPreference mEnabled;
    private SecureSettingSwitchPreference mSetOnBoot;
    private SecureSettingCustomSeekBarPreference mRed;
    private SecureSettingCustomSeekBarPreference mGreen;
    private SecureSettingCustomSeekBarPreference mBlue;
    private SecureSettingCustomSeekBarPreference mSaturation;
    private SecureSettingCustomSeekBarPreference mValue;
    private SecureSettingCustomSeekBarPreference mContrast;
    private SecureSettingCustomSeekBarPreference mHue;
    private SecureSettingCustomSeekBarPreference mMin;

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.preferences_kcal, rootKey);

        mEnabled = (SecureSettingSwitchPreference) findPreference(PREF_ENABLED);
        mEnabled.setOnPreferenceChangeListener(this);
        mEnabled.setTitle(Settings.Secure.getInt(getContext().getContentResolver(), PREF_ENABLED,
                0) == 1 ? R.string.kcal_enabled : R.string.kcal_disabled);

        mSetOnBoot = (SecureSettingSwitchPreference) findPreference(PREF_SETONBOOT);
        mSetOnBoot.setOnPreferenceChangeListener(this);

        mMin = (SecureSettingCustomSeekBarPreference) findPreference(PREF_MINIMUM);
        mMin.setOnPreferenceChangeListener(this);

        mRed = (SecureSettingCustomSeekBarPreference) findPreference(PREF_RED);
        mRed.setOnPreferenceChangeListener(this);

        mGreen = (SecureSettingCustomSeekBarPreference) findPreference(PREF_GREEN);
        mGreen.setOnPreferenceChangeListener(this);

        mBlue = (SecureSettingCustomSeekBarPreference) findPreference(PREF_BLUE);
        mBlue.setOnPreferenceChangeListener(this);

        mSaturation = (SecureSettingCustomSeekBarPreference) findPreference(PREF_SATURATION);
        mSaturation.setOnPreferenceChangeListener(this);

        mValue = (SecureSettingCustomSeekBarPreference) findPreference(PREF_VALUE);
        mValue.setOnPreferenceChangeListener(this);

        mContrast = (SecureSettingCustomSeekBarPreference) findPreference(PREF_CONTRAST);
        mContrast.setOnPreferenceChangeListener(this);

        mHue = (SecureSettingCustomSeekBarPreference) findPreference(PREF_HUE);
        mHue.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        final String key = preference.getKey();

        String rgbString;

        switch (key) {
            case PREF_ENABLED:
                if (mFileUtils.isSupported(KCAL_ENABLE)) {
                    mFileUtils.setValue(KCAL_ENABLE, (boolean) value);
                }
                mEnabled.setTitle((boolean) value ? R.string.kcal_enabled : R.string.kcal_disabled);
                break;

            case PREF_MINIMUM:
                if (mFileUtils.isSupported(KCAL_MIN)) {
                    mFileUtils.setValue(KCAL_MIN, (int) value);
                }
                break;

            case PREF_RED:
                rgbString = value + " " + mGreen.getValue() + " " + mBlue.getValue();
                if (mFileUtils.isSupported(KCAL_RGB)) {
                    mFileUtils.setValue(KCAL_RGB, rgbString);
                }
                break;

            case PREF_GREEN:
                rgbString = mRed.getValue() + " " + value + " " + mBlue.getValue();
                if (mFileUtils.isSupported(KCAL_RGB)) {
                    mFileUtils.setValue(KCAL_RGB, rgbString);
                }
                break;

            case PREF_BLUE:
                rgbString = mRed.getValue() + " " + mGreen.getValue() + " " + value;
                if (mFileUtils.isSupported(KCAL_RGB)) {
                    mFileUtils.setValue(KCAL_RGB, rgbString);
                }
                break;

            case PREF_SATURATION:
                if (mFileUtils.isSupported(KCAL_SAT)) {
                    mFileUtils.setValue(KCAL_SAT, (int) value + SATURATION_OFFSET);
                }
                break;

            case PREF_VALUE:
                if (mFileUtils.isSupported(KCAL_VAL)) {
                    mFileUtils.setValue(KCAL_VAL, (int) value + VALUE_OFFSET);
                }
                break;

            case PREF_CONTRAST:
                if (mFileUtils.isSupported(KCAL_CONT)) {
                    mFileUtils.setValue(KCAL_CONT, (int) value + CONTRAST_OFFSET);
                }
                break;

            case PREF_HUE:
                if (mFileUtils.isSupported(KCAL_HUE)) {
                    mFileUtils.setValue(KCAL_HUE, (int) value);
                }
                break;

            default:
                break;
        }
        return true;
    }

    void applyValues(String preset) {
        String[] values = preset.split(" ");
        int red = Integer.parseInt(values[0]);
        int green = Integer.parseInt(values[1]);
        int blue = Integer.parseInt(values[2]);
        int min = Integer.parseInt(values[3]);
        int sat = Integer.parseInt(values[4]);
        int value = Integer.parseInt(values[5]);
        int contrast = Integer.parseInt(values[6]);
        int hue = Integer.parseInt(values[7]);

        mRed.refresh(red);
        mGreen.refresh(green);
        mBlue.refresh(blue);
        mMin.refresh(min);
        mSaturation.refresh(sat);
        mValue.refresh(value);
        mContrast.refresh(contrast);
        mHue.refresh(hue);
    }

    void setmSetOnBoot(boolean checked) {
        mSetOnBoot.setChecked(checked);
    }
}

