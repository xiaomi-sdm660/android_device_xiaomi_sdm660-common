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

import android.os.Bundle;
import android.provider.Settings;
import android.support.v14.preference.PreferenceFragment;
import android.support.v7.preference.Preference;

public class KCalSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener, Utils {

    PresetDialog mPresets;

    SecureSwitchPreference mSetOnBoot;
    SecureSwitchPreference mEnabled;

    private SecureCustomSeekBarPreference mRed;
    private SecureCustomSeekBarPreference mGreen;
    private SecureCustomSeekBarPreference mBlue;

    private SecureCustomSeekBarPreference mSaturation;
    private SecureCustomSeekBarPreference mValue;
    private SecureCustomSeekBarPreference mContrast;
    private SecureCustomSeekBarPreference mHue;
    private SecureCustomSeekBarPreference mMin;

    private FileUtils mFileUtils = new FileUtils();

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.main, rootKey);

        mPresets = new PresetDialog();

        boolean enabled =
                Settings.Secure.getInt(getContext().getContentResolver(), PREF_ENABLED, 0) == 1;
        mSetOnBoot = (SecureSwitchPreference) findPreference(PREF_SETONBOOT);
        mEnabled = (SecureSwitchPreference) findPreference(PREF_ENABLED);
        mEnabled.setTitle(enabled ? R.string.kcal_enabled : R.string.kcal_disabled);
        mMin = (SecureCustomSeekBarPreference) findPreference(PREF_MINIMUM);
        mRed = (SecureCustomSeekBarPreference) findPreference(PREF_RED);
        mGreen = (SecureCustomSeekBarPreference) findPreference(PREF_GREEN);
        mBlue = (SecureCustomSeekBarPreference) findPreference(PREF_BLUE);
        mSaturation = (SecureCustomSeekBarPreference) findPreference(PREF_SATURATION);
        mValue = (SecureCustomSeekBarPreference) findPreference(PREF_VALUE);
        mContrast = (SecureCustomSeekBarPreference) findPreference(PREF_CONTRAST);
        mHue = (SecureCustomSeekBarPreference) findPreference(PREF_HUE);

        prefState(enabled);

        mSetOnBoot.setOnPreferenceChangeListener(this);
        mEnabled.setOnPreferenceChangeListener(this);
        mMin.setOnPreferenceChangeListener(this);
        mRed.setOnPreferenceChangeListener(this);
        mGreen.setOnPreferenceChangeListener(this);
        mBlue.setOnPreferenceChangeListener(this);
        mSaturation.setOnPreferenceChangeListener(this);
        mValue.setOnPreferenceChangeListener(this);
        mContrast.setOnPreferenceChangeListener(this);
        mHue.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        final String key = preference.getKey();

        switch (key) {
            case PREF_ENABLED:
                setEnabled((boolean) value);
                mEnabled.setChecked((boolean) value);
                break;

            case PREF_SETONBOOT:
                mSetOnBoot.setChecked((boolean) value);
                break;

            case PREF_MINIMUM:
                setMinimum((int) value);
                mMin.setValue((int) value);
                break;

            case PREF_RED:
                setRed((int) value);
                mRed.setValue((int) value);
                break;

            case PREF_GREEN:
                setGreen((int) value);
                mGreen.setValue((int) value);
                break;

            case PREF_BLUE:
                setBlue((int) value);
                mBlue.setValue((int) value);
                break;

            case PREF_SATURATION:
                setSaturation((int) value);
                mSaturation.setValue((int) value);
                break;

            case PREF_VALUE:
                setValue((int) value);
                mValue.setValue((int) value);
                break;

            case PREF_CONTRAST:
                setContrast((int) value);
                mContrast.setValue((int) value);
                break;

            case PREF_HUE:
                setHue((int) value);
                mHue.setValue((int) value);
                break;
        }
        return true;
    }


    void prefState(boolean state) {
        mSetOnBoot.setEnabled(state);
        mRed.setEnabled(state);
        mGreen.setEnabled(state);
        mBlue.setEnabled(state);
        mMin.setEnabled(state);
        mSaturation.setEnabled(state);
        mValue.setEnabled(state);
        mContrast.setEnabled(state);
        mHue.setEnabled(state);
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

    private void setEnabled(boolean value) {
        if (mFileUtils.isSupported(Utils.KCAL_ENABLE)) {
            mFileUtils.setValue(Utils.KCAL_ENABLE, value);
        }

        mEnabled.setTitle(value ? R.string.kcal_enabled : R.string.kcal_disabled);
        prefState(value);
    }

    private void setMinimum(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_MIN)) {
            mFileUtils.setValue(Utils.KCAL_MIN, value);
        }
    }

    private void setRed(int value) {
        String rgbString = value + " " + mGreen.getValue() + " " + mBlue.getValue();
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setGreen(int value) {
        String rgbString = mRed.getValue() + " " + value + " " + mBlue.getValue();
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setBlue(int value) {
        String rgbString = mRed.getValue() + " " + mGreen.getValue() + " " + value;
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setSaturation(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_SAT)) {
            mFileUtils.setValue(Utils.KCAL_SAT, value + SATURATION_OFFSET);
        }
    }

    private void setValue(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_VAL)) {
            mFileUtils.setValue(Utils.KCAL_VAL, value + VALUE_OFFSET);
        }
    }

    private void setContrast(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_CONT)) {
            mFileUtils.setValue(Utils.KCAL_CONT, value + CONTRAST_OFFSET);
        }
    }

    private void setHue(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_HUE)) {
            mFileUtils.setValue(Utils.KCAL_HUE, value);
        }
    }
}

