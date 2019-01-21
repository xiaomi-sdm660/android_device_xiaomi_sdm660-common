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

import android.app.StatusBarManager;
import android.hardware.input.InputManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;

public class StartActionActivity extends PreferenceActivity {

    private static PreferenceManager preferenceManager;
    private static StatusBarManager statusBarManager;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        preferenceManager = getPreferenceManager();
        statusBarManager = (StatusBarManager) getSystemService(STATUS_BAR_SERVICE);


        boolean rCamera = getIntent().getExtras().getBoolean("rCamera");
        boolean fpActionEnabled = preferenceManager.getSharedPreferences().getBoolean(DeviceSettings.ENABLE_FPACTION_KEY, false);
        String fpAction = preferenceManager.getSharedPreferences().getString(DeviceSettings.FPACTION_KEY, "4");

        if (preferenceManager.getSharedPreferences().getBoolean(DeviceSettings.FP_SHUTTER_KEY, false)) {
            if (rCamera) {
                sendKeyCode(KeyEvent.KEYCODE_CAMERA);
            } else {
                if (fpActionEnabled) {
                    fpAction(fpAction);
                }
            }
        } else if (fpActionEnabled) {
            fpAction(fpAction);
        }
        finish();
    }

    private void fpAction(String action) {
        switch (action) {
            case "expnp":
                statusBarManager.expandNotificationsPanel();
                break;
            case "expqs":
                statusBarManager.expandSettingsPanel();
                break;
            case "187":
                // If you have a better idea to hide it completely from the recent apps then feel free to commit
                try {
                    Thread.sleep(2);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                sendKeyCode(KeyEvent.KEYCODE_APP_SWITCH);
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
