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

import android.app.ActivityManager;
import android.app.IntentService;
import android.app.StatusBarManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.input.InputManager;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;

import java.util.Arrays;

public class StartAction extends IntentService {

    private static final String[] CAMERA_PACKAGES = new String[]{"com.android.camera",
            "com.android.camera2", "com.google.android.GoogleCamera"};

    private static StatusBarManager sStatusBarManager;

    public StartAction() {
        super("StartAction");
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        ActivityManager activityManager =
                (ActivityManager) this.getSystemService(Context.ACTIVITY_SERVICE);
        SharedPreferences sharedPreferences =
                PreferenceManager.getDefaultSharedPreferences(this);
        sStatusBarManager = (StatusBarManager) getSystemService(STATUS_BAR_SERVICE);

        ComponentName componentName = activityManager.getRunningTasks(1).get(0)
                .topActivity;

        boolean cameraActive = Arrays.asList(CAMERA_PACKAGES).contains(componentName.getPackageName());
        boolean fpActionEnabled = sharedPreferences.getBoolean(DeviceSettings.PREF_ENABLE_FPACTION,
                false);
        boolean fpShutterEnabled = sharedPreferences.getBoolean(DeviceSettings.PREF_FP_SHUTTER,
                false);
        String fpAction = sharedPreferences.getString(DeviceSettings.PREF_FPACTION, "4");

        if (fpShutterEnabled) {
            if (cameraActive) {
                sendKeyCode(KeyEvent.KEYCODE_CAMERA);
            } else if (fpActionEnabled) {
                fpAction(fpAction);
            }
        } else if (fpActionEnabled) {
            fpAction(fpAction);
        }
    }

    private void fpAction(String action) {
        switch (action) {
            case "expnp":
                sStatusBarManager.expandNotificationsPanel();
                break;
            case "expqs":
                sStatusBarManager.expandSettingsPanel();
                break;
            default:
                sendKeyCode(Integer.parseInt(action));
                break;
        }
    }

    private void sendKeyCode(int code) {
        InputManager inputManager = InputManager.getInstance();
        final KeyEvent downEvent = new KeyEvent(SystemClock.uptimeMillis(),
                SystemClock.uptimeMillis(), KeyEvent.ACTION_DOWN, code, 0, 0,
                KeyCharacterMap.VIRTUAL_KEYBOARD, 0, KeyEvent.FLAG_FROM_SYSTEM,
                InputDevice.SOURCE_KEYBOARD);
        final KeyEvent upEvent = KeyEvent.changeAction(downEvent, KeyEvent.ACTION_UP);
        inputManager.injectInputEvent(downEvent, InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
        inputManager.injectInputEvent(upEvent, InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
    }
}
