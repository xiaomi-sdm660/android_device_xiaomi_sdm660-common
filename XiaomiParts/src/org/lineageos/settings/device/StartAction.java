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
import android.hardware.input.InputManager;
import android.os.SystemClock;
import android.provider.Settings;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;

import java.util.Arrays;

public class StartAction extends IntentService {

    private static final String[] CAMERA_PACKAGES = new String[]{"com.android.camera",
            "com.android.camera2", "com.google.android.GoogleCamera"};

    private static final String FP_SWIPE_DIRECTION = "FP_SWIPE_DIRECTION";
    private static final int FP_SWIPE_UP = 0;
    private static final int FP_SWIPE_DOWN = 1;
    private static final int FP_SWIPE_LEFT = 2;
    private static final int FP_SWIPE_RIGHT = 3;

    private boolean mCameraActive = false;
    private boolean mFpActionEnabled = false;
    private boolean mFpShutterEnabled = false;

    private String fpActionUp;
    private String fpActionDown;
    private String fpActionLeft;
    private String fpActionRight;

    private static StatusBarManager sStatusBarManager;

    public StartAction() {
        super("StartAction");
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        ActivityManager activityManager =
                (ActivityManager) this.getSystemService(Context.ACTIVITY_SERVICE);
        sStatusBarManager = (StatusBarManager) getSystemService(STATUS_BAR_SERVICE);

        ComponentName componentName = activityManager.getRunningTasks(1).get(0)
                .topActivity;

        mCameraActive = Arrays.asList(CAMERA_PACKAGES).contains(componentName.getPackageName());
        mFpActionEnabled = Settings.Secure.getInt(this.getContentResolver(),
                DeviceSettings.PREF_ENABLE_FPACTION, 0) == 1;
        mFpShutterEnabled = Settings.Secure.getInt(this.getContentResolver(),
                DeviceSettings.PREF_FP_SHUTTER, 0) == 1;

        fpActionUp = Settings.Secure.getString(this.getContentResolver(),
                DeviceSettings.PREF_FPACTION_UP);
        fpActionDown = Settings.Secure.getString(this.getContentResolver(),
                DeviceSettings.PREF_FPACTION_DOWN);
        fpActionLeft = Settings.Secure.getString(this.getContentResolver(),
                DeviceSettings.PREF_FPACTION_LEFT);
        fpActionRight = Settings.Secure.getString(this.getContentResolver(),
                DeviceSettings.PREF_FPACTION_RIGHT);

        int swipeDirection = intent.getIntExtra(FP_SWIPE_DIRECTION, 4);
        if (mFpShutterEnabled) {
            if (mCameraActive) {
                sendKeyCode(KeyEvent.KEYCODE_CAMERA);
            } else {
                fpAction(swipeDirection);
            }
        } else {
            fpAction(swipeDirection);
        }
    }

    private void fpAction(int swipeDirection) {
        String action;
        int codeDef;
        switch (swipeDirection) {
            case FP_SWIPE_UP:
                action = fpActionUp;
                codeDef = 280;
                break;

            case FP_SWIPE_DOWN:
                action = fpActionDown;
                codeDef = 281;
                break;

            case FP_SWIPE_LEFT:
                action = fpActionLeft;
                codeDef = 282;
                break;

            case FP_SWIPE_RIGHT:
                action = fpActionRight;
                codeDef = 283;
                break;

            default:
                action = fpActionDown;
                codeDef = 281;
                break;
        }

        if (!mFpActionEnabled && (!mCameraActive || !mFpShutterEnabled)) {
            action = "def";
        }

        switch (action) {
            case "def":
                sendKeyCode(codeDef);
                break;
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
