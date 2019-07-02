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

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.view.KeyEvent;

import com.android.internal.os.DeviceKeyHandler;

public class KeyHandler implements DeviceKeyHandler {

    private static final int SCANCODE_JASMINE_UP = 103;
    private static final int SCANCODE_JASMINE_DOWN = 108;
    private static final int SCANCODE_JASMINE_LEFT = 105;
    private static final int SCANCODE_JASMINE_RIGHT = 106;

    private static final int SCANCODE_CLOVER_DOWN = 172;

    private static final String FP_SWIPE_DIRECTION = "FP_SWIPE_DIRECTION";
    private static final int FP_SWIPE_UP = 0;
    private static final int FP_SWIPE_DOWN = 1;
    private static final int FP_SWIPE_LEFT = 2;
    private static final int FP_SWIPE_RIGHT = 3;

    private static int sActionBefore = KeyEvent.ACTION_DOWN;

    private static PackageManager sPackageManager;
    private Context mContext;

    public KeyHandler(Context context) {
        sPackageManager = context.getPackageManager();
        this.mContext = context;
    }

    @Override
    public KeyEvent handleKeyEvent(KeyEvent event) {
        int scanCode = event.getScanCode();
        int action = event.getAction();

        int swipeDirection = -1;

        switch (scanCode) {
            case SCANCODE_JASMINE_UP:
                swipeDirection = FP_SWIPE_UP;
                break;

            case SCANCODE_CLOVER_DOWN:
            case SCANCODE_JASMINE_DOWN:
                swipeDirection = FP_SWIPE_DOWN;
                break;

            case SCANCODE_JASMINE_LEFT:
                swipeDirection = FP_SWIPE_LEFT;
                break;

            case SCANCODE_JASMINE_RIGHT:
                swipeDirection = FP_SWIPE_RIGHT;
                break;

            default:
                swipeDirection = -1;
                break;
        }

        if (swipeDirection >= 0) {
            if (sActionBefore != KeyEvent.ACTION_UP) {
                if (action == KeyEvent.ACTION_UP) {
                    Intent startAction = new Intent()
                            .setComponent(new ComponentName("org.lineageos.settings.device",
                                    "org.lineageos.settings.device.StartAction"))
                            .putExtra(FP_SWIPE_DIRECTION, swipeDirection);

                    if (startAction.resolveActivity(sPackageManager) != null) {
                        mContext.startService(startAction);
                    }
                }
            }
            sActionBefore = action;
        }
        return event;
    }
}
