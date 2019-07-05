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

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v14.preference.PreferenceFragment;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceCategory;

import org.lineageos.settings.device.kcal.KCalSettingsActivity;
import org.lineageos.settings.device.preferences.SecureSettingCustomSeekBarPreference;
import org.lineageos.settings.device.preferences.SecureSettingListPreference;
import org.lineageos.settings.device.preferences.SecureSettingSwitchPreference;
import org.lineageos.settings.device.preferences.VibrationSeekBarPreference;

public class DeviceSettings extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener {

    public static final String CATEGORY_FP = "fingerprint";
    public static final String PREF_ENABLE_FPACTION = "fpaction_enabled";
    public static final String PREF_FP_SHUTTER = "fp_shutter";
    public static final String PREF_FPACTION_UP = "fpaction_up";
    public static final String PREF_FPACTION_DOWN = "fpaction_down";
    public static final String PREF_FPACTION_LEFT = "fpaction_left";
    public static final String PREF_FPACTION_RIGHT = "fpaction_right";

    public static final String PREF_TORCH_BRIGHTNESS = "torch_brightness";
    public static final String TORCH_1_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom," +
            "spmi/spmi-0/spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_0/max_brightness";
    public static final String TORCH_2_BRIGHTNESS_PATH = "/sys/devices/soc/800f000.qcom," +
            "spmi/spmi-0/spmi0-03/800f000.qcom,spmi:qcom,pm660l@3:qcom,leds@d300/leds/led:torch_1/max_brightness";

    public static final String PREF_VIBRATION_STRENGTH = "vibration_strength";
    public static final String VIBRATION_STRENGTH_PATH = "/sys/devices/virtual/timed_output/vibrator/vtg_level";

    // value of vtg_min and vtg_max
    public static final int MIN_VIBRATION = 116;
    public static final int MAX_VIBRATION = 3596;

    public static final String PREF_ENABLE_HAL3 = "hal3";
    public static final String HAL3_SYSTEM_PROPERTY = "persist.camera.HAL3.enabled";

    public static final String PREF_ENABLE_EIS = "eis";
    public static final String EIS_SYSTEM_PROPERTY = "persist.camera.eis.enable";

    public static final String CATEGORY_DISPLAY = "display";
    public static final String PREF_DEVICE_DOZE = "device_doze";
    public static final String PREF_DEVICE_KCAL = "device_kcal";

    public static final String PREF_SPECTRUM = "spectrum";
    public static final String SPECTRUM_SYSTEM_PROPERTY = "persist.spectrum.profile";

    public static final String PREF_ENABLE_DIRAC = "dirac_enabled";
    public static final String PREF_HEADSET = "dirac_headset_pref";
    public static final String PREF_PRESET = "dirac_preset_pref";
    public static final String PREF_HEADPHONE_GAIN = "headphone_gain";
    public static final String HEADPHONE_GAIN_PATH = "/sys/kernel/sound_control/headphone_gain";
    public static final String PREF_MICROPHONE_GAIN = "microphone_gain";
    public static final String MICROPHONE_GAIN_PATH = "/sys/kernel/sound_control/mic_gain";

    public static final String CATEGORY_FASTCHARGE = "usb_fastcharge";
    public static final String PREF_USB_FASTCHARGE = "fastcharge";
    public static final String USB_FASTCHARGE_PATH = "/sys/kernel/fast_charge/force_fast_charge";
    public static final String CHARGE_PATH = "/sys/class/power_supply/battery/input_suspend";

    public static final String CATEGORY_HALL_WAKEUP = "hall_wakeup";
    public static final String PREF_HALL_WAKEUP = "hall";
    public static final String HALL_WAKEUP_PATH = "/sys/module/hall/parameters/hall_toggle";

    public static final String DEVICE_DOZE_PACKAGE_NAME = "org.lineageos.settings.doze";

    private SecureSettingSwitchPreference mEnableHAL3;
    private SecureSettingSwitchPreference mEnableEIS;
    private SecureSettingSwitchPreference mEnableFpAction;
    private SecureSettingSwitchPreference mFpShutter;
    private SecureSettingListPreference mFpActionUp;
    private SecureSettingListPreference mFpActionDown;
    private SecureSettingListPreference mFpActionLeft;
    private SecureSettingListPreference mFpActionRight;
    private SecureSettingCustomSeekBarPreference mTorchBrightness;
    private VibrationSeekBarPreference mVibrationStrength;
    private Preference mKcal;
    private SecureSettingListPreference mSPECTRUM;
    private SecureSettingSwitchPreference mEnableDirac;
    private SecureSettingListPreference mHeadsetType;
    private SecureSettingListPreference mPreset;
    private SecureSettingCustomSeekBarPreference mHeadphoneGain;
    private SecureSettingCustomSeekBarPreference mMicrophoneGain;
    private SecureSettingSwitchPreference mFastcharge;
    private SecureSettingSwitchPreference mHall;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.preferences_xiaomi_parts, rootKey);

        String device = FileUtils.getStringProp("ro.build.product", "unknown");

        mEnableHAL3 = (SecureSettingSwitchPreference) findPreference(PREF_ENABLE_HAL3);
        mEnableHAL3.setChecked(FileUtils.getProp(HAL3_SYSTEM_PROPERTY, false));
        mEnableHAL3.setOnPreferenceChangeListener(this);

        mEnableEIS = (SecureSettingSwitchPreference) findPreference(PREF_ENABLE_EIS);
        mEnableEIS.setChecked(FileUtils.getProp(EIS_SYSTEM_PROPERTY, false));
        mEnableEIS.setOnPreferenceChangeListener(this);

        mEnableFpAction = (SecureSettingSwitchPreference) findPreference(PREF_ENABLE_FPACTION);
        mEnableFpAction.setOnPreferenceChangeListener(this);

        mFpShutter = (SecureSettingSwitchPreference) findPreference(PREF_FP_SHUTTER);
        mFpShutter.setOnPreferenceChangeListener(this);

        mFpActionUp = (SecureSettingListPreference) findPreference(PREF_FPACTION_UP);
        mFpActionUp.setSummary(mFpActionUp.getEntry());
        mFpActionUp.setOnPreferenceChangeListener(this);

        mFpActionDown = (SecureSettingListPreference) findPreference(PREF_FPACTION_DOWN);
        mFpActionDown.setSummary(mFpActionDown.getEntry());
        mFpActionDown.setOnPreferenceChangeListener(this);

        mFpActionLeft = (SecureSettingListPreference) findPreference(PREF_FPACTION_LEFT);
        mFpActionLeft.setSummary(mFpActionLeft.getEntry());
        mFpActionLeft.setOnPreferenceChangeListener(this);

        mFpActionRight = (SecureSettingListPreference) findPreference(PREF_FPACTION_RIGHT);
        mFpActionRight.setSummary(mFpActionRight.getEntry());
        mFpActionRight.setOnPreferenceChangeListener(this);

        if (device.equals("clover")) {
            PreferenceCategory fpCategory = (PreferenceCategory) findPreference(CATEGORY_FP);
            fpCategory.removePreference(mFpActionUp);
            fpCategory.removePreference(mFpActionLeft);
            fpCategory.removePreference(mFpActionRight);
            mFpActionDown.setTitle(R.string.fpaction_title);
        }

        mTorchBrightness = (SecureSettingCustomSeekBarPreference) findPreference(PREF_TORCH_BRIGHTNESS);
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

        mKcal = findPreference(PREF_DEVICE_KCAL);

        mKcal.setOnPreferenceClickListener(preference -> {
            Intent intent = new Intent(getActivity().getApplicationContext(), KCalSettingsActivity.class);
            startActivity(intent);
            return true;
        });

        mSPECTRUM = (SecureSettingListPreference) findPreference(PREF_SPECTRUM);
        mSPECTRUM.setValue(FileUtils.getStringProp(SPECTRUM_SYSTEM_PROPERTY, "0"));
        mSPECTRUM.setSummary(mSPECTRUM.getEntry());
        mSPECTRUM.setOnPreferenceChangeListener(this);

        boolean enhancerEnabled;
        try {
            enhancerEnabled = DiracService.sDiracUtils.isDiracEnabled();
        } catch (java.lang.NullPointerException e) {
            getContext().startService(new Intent(getContext(), DiracService.class));
            enhancerEnabled = DiracService.sDiracUtils.isDiracEnabled();
        }

        mEnableDirac = (SecureSettingSwitchPreference) findPreference(PREF_ENABLE_DIRAC);
        mEnableDirac.setOnPreferenceChangeListener(this);
        mEnableDirac.setChecked(enhancerEnabled);

        mHeadsetType = (SecureSettingListPreference) findPreference(PREF_HEADSET);
        mHeadsetType.setOnPreferenceChangeListener(this);

        mPreset = (SecureSettingListPreference) findPreference(PREF_PRESET);
        mPreset.setOnPreferenceChangeListener(this);

        mHeadphoneGain = (SecureSettingCustomSeekBarPreference) findPreference(PREF_HEADPHONE_GAIN);
        mHeadphoneGain.setOnPreferenceChangeListener(this);

        mMicrophoneGain = (SecureSettingCustomSeekBarPreference) findPreference(PREF_MICROPHONE_GAIN);
        mMicrophoneGain.setOnPreferenceChangeListener(this);

        if (FileUtils.fileWritable(USB_FASTCHARGE_PATH)) {
            mFastcharge = (SecureSettingSwitchPreference) findPreference(PREF_USB_FASTCHARGE);
            mFastcharge.setChecked(FileUtils.getFileValueAsBoolean(USB_FASTCHARGE_PATH, false));
            mFastcharge.setOnPreferenceChangeListener(this);
        } else {
            getPreferenceScreen().removePreference(findPreference(CATEGORY_FASTCHARGE));
        }

        if (FileUtils.fileWritable(HALL_WAKEUP_PATH)) {
             mHall = (SecureSettingSwitchPreference) findPreference(PREF_HALL_WAKEUP);
             mHall.setChecked(FileUtils.getFileValueAsBoolean(HALL_WAKEUP_PATH, false));
             mHall.setOnPreferenceChangeListener(this);
         } else {
             getPreferenceScreen().removePreference(findPreference(CATEGORY_HALL_WAKEUP));
         }
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

            case PREF_FPACTION_UP:
                mFpActionUp.setValue((String) value);
                mFpActionUp.setSummary(mFpActionUp.getEntry());
                break;

            case PREF_FPACTION_DOWN:
                mFpActionDown.setValue((String) value);
                mFpActionDown.setSummary(mFpActionDown.getEntry());
                break;

            case PREF_FPACTION_LEFT:
                mFpActionLeft.setValue((String) value);
                mFpActionLeft.setSummary(mFpActionLeft.getEntry());
                break;

            case PREF_FPACTION_RIGHT:
                mFpActionRight.setValue((String) value);
                mFpActionRight.setSummary(mFpActionRight.getEntry());
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
                try {
                    DiracService.sDiracUtils.setEnabled((boolean) value);
                } catch (java.lang.NullPointerException e) {
                    getContext().startService(new Intent(getContext(), DiracService.class));
                    DiracService.sDiracUtils.setEnabled((boolean) value);
                }
                break;

            case PREF_HEADSET:
                try {
                    DiracService.sDiracUtils.setHeadsetType(Integer.parseInt(value.toString()));
                } catch (java.lang.NullPointerException e) {
                    getContext().startService(new Intent(getContext(), DiracService.class));
                    DiracService.sDiracUtils.setHeadsetType(Integer.parseInt(value.toString()));
                }
                break;

            case PREF_PRESET:
                try {
                    DiracService.sDiracUtils.setLevel(String.valueOf(value));
                } catch (java.lang.NullPointerException e) {
                    getContext().startService(new Intent(getContext(), DiracService.class));
                    DiracService.sDiracUtils.setLevel(String.valueOf(value));
                }
                break;

            case PREF_HEADPHONE_GAIN:
                FileUtils.setValue(HEADPHONE_GAIN_PATH, value + " " + value);
                break;

            case PREF_MICROPHONE_GAIN:
                FileUtils.setValue(MICROPHONE_GAIN_PATH, (int) value);
                break;

            case PREF_USB_FASTCHARGE:
                FileUtils.setValue(USB_FASTCHARGE_PATH, (boolean) value);
                FileUtils.setValue(CHARGE_PATH, (int) 1);
                FileUtils.setValue(CHARGE_PATH, (int) 0);
                break;

            case PREF_HALL_WAKEUP:
                FileUtils.setValue(HALL_WAKEUP_PATH, (boolean) value);
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
