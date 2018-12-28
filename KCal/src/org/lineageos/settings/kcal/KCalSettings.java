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

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v14.preference.PreferenceFragment;
import android.support.v14.preference.SwitchPreference;
import android.support.v7.preference.Preference;

public class KCalSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener, Utils {

    SwitchPreference mSetOnBoot;
    SwitchPreference mEnabled;

    CustomSeekBarPreference mRed;
    CustomSeekBarPreference mGreen;
    CustomSeekBarPreference mBlue;

    CustomSeekBarPreference mSaturation;
    CustomSeekBarPreference mValue;
    CustomSeekBarPreference mContrast;
    CustomSeekBarPreference mHue;
    CustomSeekBarPreference mMin;

    private FileUtils mFileUtils = new FileUtils();

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.main, rootKey);

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(KCalSettingsActivity.getContext());

        boolean enabled = sharedPrefs.getBoolean(PREF_ENABLED, false);
        mSetOnBoot = (SwitchPreference) findPreference(PREF_SETONBOOT);
        mEnabled = (SwitchPreference) findPreference(PREF_ENABLED);
        mEnabled.setTitle(enabled ? R.string.kcal_enabled : R.string.kcal_disabled);
        mMin = (CustomSeekBarPreference) findPreference(PREF_MINIMUM);
        mRed = (CustomSeekBarPreference) findPreference(PREF_RED);
        mGreen = (CustomSeekBarPreference) findPreference(PREF_GREEN);
        mBlue = (CustomSeekBarPreference) findPreference(PREF_BLUE);
        mSaturation = (CustomSeekBarPreference) findPreference(PREF_SATURATION);
        mValue = (CustomSeekBarPreference) findPreference(PREF_VALUE);
        mContrast = (CustomSeekBarPreference) findPreference(PREF_CONTRAST);
        mHue = (CustomSeekBarPreference) findPreference(PREF_HUE);

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
            case PREF_SETONBOOT:
                mSetOnBoot.setChecked((boolean) value);
                break;

            case PREF_ENABLED:
                setmEnabled((boolean) value);
                mEnabled.setChecked((boolean) value);
                break;

            case PREF_MINIMUM:
                setmMinimum((int) value);
                mMin.setValue((int) value);
                break;

            case PREF_RED:
                setmRed((int) value);
                mRed.setValue((int) value);
                break;

            case PREF_GREEN:
                setmGreen((int) value);
                mGreen.setValue((int) value);
                break;

            case PREF_BLUE:
                setmBlue((int) value);
                mBlue.setValue((int) value);
                break;

            case PREF_SATURATION:
                setmSaturation((int) value);
                mSaturation.setValue((int) value);
                break;

            case PREF_VALUE:
                setmValue((int) value);
                mValue.setValue((int) value);
                break;

            case PREF_CONTRAST:
                setmContrast((int) value);
                mContrast.setValue((int) value);
                break;

            case PREF_HUE:
                setmHue((int) value);
                mHue.setValue((int) value);
                break;

            default:
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

    private void setmEnabled(boolean value) {
        if (mFileUtils.isSupported(Utils.KCAL_ENABLE)) {
            mFileUtils.setValue(Utils.KCAL_ENABLE, value);
        }

        mEnabled.setTitle(value ? R.string.kcal_enabled : R.string.kcal_disabled);
        prefState(value);
    }

    private void setmMinimum(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_MIN)) {
            mFileUtils.setValue(Utils.KCAL_MIN, value);
        }
    }

    private void setmRed(int value) {
        String rgbString = value + " " + mGreen.getValue() + " " + mBlue.getValue();
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setmGreen(int value) {
        String rgbString = mRed.getValue() + " " + value + " " + mBlue.getValue();
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setmBlue(int value) {
        String rgbString = mRed.getValue() + " " + mGreen.getValue() + " " + value;
        if (mFileUtils.isSupported(Utils.KCAL_RGB)) {
            mFileUtils.setValue(Utils.KCAL_RGB, rgbString);
        }
    }

    private void setmSaturation(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_SAT)) {
            mFileUtils.setValue(Utils.KCAL_SAT, value + SATURATION_OFFSET);
        }
    }

    private void setmValue(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_VAL)) {
            mFileUtils.setValue(Utils.KCAL_VAL, value + VALUE_OFFSET);
        }
    }

    private void setmContrast(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_CONT)) {
            mFileUtils.setValue(Utils.KCAL_CONT, value + CONTRAST_OFFSET);
        }
    }

    private void setmHue(int value) {
        if (mFileUtils.isSupported(Utils.KCAL_HUE)) {
            mFileUtils.setValue(Utils.KCAL_HUE, value);
        }
    }
}

