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

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v14.preference.PreferenceFragment;
import android.support.v14.preference.SwitchPreference;
import android.support.v7.preference.ListPreference;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceCategory;

public class DeviceSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener {

    private static final String PREF_ENABLE_HAL3 = "hal3";
    private static final String PREF_ENABLE_EIS = "eis";
    final static String PREF_ENABLE_FPACTION = "fpaction_enabled";
    final static String PREF_FP_SHUTTER = "fp_shutter";
    final static String PREF_FPACTION = "fpaction";
    final static String PREF_TORCH_BRIGHTNESS = "torch_brightness";
    final static String PREF_VIBRATION_STRENGTH = "vibration_strength";
    private static final String CATEGORY_DISPLAY = "display";
    private static final String PREF_DEVICE_DOZE = "device_doze";
    private static final String PREF_DEVICE_KCAL = "device_kcal";
    private static final String PREF_SPECTRUM = "spectrum";
    private static final String PREF_ENABLE_DIRAC = "dirac_enabled";
    private static final String PREF_HEADSET = "dirac_headset_pref";
    private static final String PREF_PRESET = "dirac_preset_pref";

    private static final String HAL3_SYSTEM_PROPERTY = "persist.camera.HAL3.enabled";
    private static final String EIS_SYSTEM_PROPERTY = "persist.camera.eis.enable";

    private final static String TORCH_1_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/" + "" +
            "spmi-0/spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_0/max_brightness";
    private final static String TORCH_2_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom,spmi/" + "" +
            "spmi-0/spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_1/max_brightness";
    private final static String VIBRATION_STRENGTH_PATH = "/sys/devices/virtual/timed_output/vibrator/vtg_level";

    private static final String DEVICE_DOZE_PACKAGE_NAME = "org.lineageos.settings.doze";
    private static final String DEVICE_KCAL_PACKAGE_NAME = "org.lineageos.settings.kcal";

    private static final String SPECTRUM_SYSTEM_PROPERTY = "persist.spectrum.profile";

    // value of vtg_min and vtg_max
    final static int MIN_VIBRATION = 116;
    final static int MAX_VIBRATION = 3596;

    private SwitchPreference mEnableHAL3;
    private SwitchPreference mEnableEIS;
    private SwitchPreference mEnableFpAction;
    private SwitchPreference mFpShutter;
    private ListPreference mFpAction;
    private TorchSeekBarPreference mTorchBrightness;
    private VibrationSeekBarPreference mVibrationStrength;
    private ListPreference mSPECTRUM;
    private SwitchPreference mEnableDirac;
    private ListPreference mHeadsetType;
    private ListPreference mPreset;

    private DiracUtils mDiracUtils;
    private Handler mHandler = new Handler();

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.main, rootKey);

        mDiracUtils = new DiracUtils(getContext());

        mEnableHAL3 = (SwitchPreference) findPreference(PREF_ENABLE_HAL3);
        mEnableHAL3.setChecked(FileUtils.getProp(HAL3_SYSTEM_PROPERTY, false));
        mEnableHAL3.setOnPreferenceChangeListener(this);

        mEnableEIS = (SwitchPreference) findPreference(PREF_ENABLE_EIS);
        mEnableEIS.setChecked(FileUtils.getProp(EIS_SYSTEM_PROPERTY, false));
        mEnableEIS.setOnPreferenceChangeListener(this);

        mEnableFpAction = (SwitchPreference) findPreference(PREF_ENABLE_FPACTION);
        mEnableFpAction.setOnPreferenceChangeListener(this);

        mFpShutter = (SwitchPreference) findPreference(PREF_FP_SHUTTER);
        mFpShutter.setOnPreferenceChangeListener(this);

        mFpAction = (ListPreference) findPreference(PREF_FPACTION);
        mFpAction.setSummary(mFpAction.getEntry());
        mFpAction.setOnPreferenceChangeListener(this);

        mTorchBrightness = (TorchSeekBarPreference) findPreference(PREF_TORCH_BRIGHTNESS);
        mTorchBrightness.setEnabled(FileUtils.fileWritable(TORCH_1_BRIGHTNESS_PATH) &&
                FileUtils.fileWritable(TORCH_2_BRIGHTNESS_PATH));
        mTorchBrightness.setOnPreferenceChangeListener(this);

        mVibrationStrength = (VibrationSeekBarPreference) findPreference(PREF_VIBRATION_STRENGTH);
        mVibrationStrength.setEnabled(FileUtils.fileWritable(VIBRATION_STRENGTH_PATH));
        mVibrationStrength.setOnPreferenceChangeListener(this);

        PreferenceCategory displayCategory = (PreferenceCategory) findPreference(CATEGORY_DISPLAY);
        if (isAppNotInstalled(DEVICE_DOZE_PACKAGE_NAME)) {
            displayCategory.removePreference(findPreference(PREF_DEVICE_DOZE));
        }

        if (isAppNotInstalled(DEVICE_KCAL_PACKAGE_NAME)) {
            displayCategory.removePreference(findPreference(PREF_DEVICE_KCAL));
        }

        mSPECTRUM = (ListPreference) findPreference(PREF_SPECTRUM);
        mSPECTRUM.setValue(FileUtils.getStringProp(SPECTRUM_SYSTEM_PROPERTY, "0"));
        mSPECTRUM.setSummary(mSPECTRUM.getEntry());
        mSPECTRUM.setOnPreferenceChangeListener(this);

        boolean enhancerEnabled = mDiracUtils.isDiracEnabled();

        mEnableDirac = (SwitchPreference) findPreference(PREF_ENABLE_DIRAC);
        mEnableDirac.setOnPreferenceChangeListener(this);
        mEnableDirac.setChecked(enhancerEnabled);

        mHeadsetType = (ListPreference) findPreference(PREF_HEADSET);
        mHeadsetType.setOnPreferenceChangeListener(this);
        mHeadsetType.setEnabled(enhancerEnabled);

        mPreset = (ListPreference) findPreference(PREF_PRESET);
        mPreset.setOnPreferenceChangeListener(this);
        mPreset.setEnabled(enhancerEnabled);
    }


    @Override
    public boolean onPreferenceTreeClick(Preference preference) {
        return super.onPreferenceTreeClick(preference);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        final String key = preference.getKey();
        switch (key) {
            case PREF_ENABLE_HAL3:
                FileUtils.setProp(HAL3_SYSTEM_PROPERTY, (Boolean) value);
                break;

            case PREF_ENABLE_EIS:
                FileUtils.setProp(EIS_SYSTEM_PROPERTY, (Boolean) value);
                break;

            case PREF_ENABLE_FPACTION:
                mFpAction.setEnabled((Boolean) value);
                break;

            case PREF_FPACTION:
                mFpAction.setValue((String) value);
                mFpAction.setSummary(mFpAction.getEntry());
                break;

            case PREF_TORCH_BRIGHTNESS:
                FileUtils.setValue(TORCH_1_BRIGHTNESS_PATH, (int) value);
                FileUtils.setValue(TORCH_2_BRIGHTNESS_PATH, (int) value);
                break;

            case PREF_VIBRATION_STRENGTH:
                double vibrationValue = (int) value / 100.0 * (MAX_VIBRATION - MIN_VIBRATION) + MIN_VIBRATION;
                FileUtils.setValue(VIBRATION_STRENGTH_PATH, vibrationValue);
                break;

            case PREF_SPECTRUM:
                mSPECTRUM.setValue((String) value);
                mSPECTRUM.setSummary(mSPECTRUM.getEntry());
                FileUtils.setStringProp(SPECTRUM_SYSTEM_PROPERTY, (String) value);
                break;

            case PREF_ENABLE_DIRAC:
                mDiracUtils.setEnabled((boolean) value);
                mHeadsetType.setEnabled((boolean) value);
                mPreset.setEnabled((boolean) value);
                break;

            case PREF_HEADSET:
                mDiracUtils.setHeadsetType(Integer.parseInt(value.toString()));
                break;

            case PREF_PRESET:
                mDiracUtils.setLevel(String.valueOf(value));
                break;

            default:
                break;
        }
        return true;
    }

    private boolean isAppNotInstalled(String uri) {
        PackageManager packageManager = getContext().getPackageManager();
        try {
            packageManager.getPackageInfo(uri, PackageManager.GET_ACTIVITIES);
            return false;
        } catch (PackageManager.NameNotFoundException e) {
            return true;
        }
    }
}
