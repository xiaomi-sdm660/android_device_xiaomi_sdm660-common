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
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.view.KeyEvent;

import com.android.internal.os.DeviceKeyHandler;

import java.util.Arrays;

import static android.content.ContentValues.TAG;

public class KeyHandler implements DeviceKeyHandler {

    private static final int SCANCODE_JASMINE = 96;
    private static final int SCANCODE_CLOVER = 172;

    private static int actionBefore = KeyEvent.ACTION_DOWN;

    private static final String[] CAMERA_PACKAGES = new String[]{"com.android.camera", "com.android.camera2"};

    private static PackageManager packageManager;
    ActivityManager activityManager;
    private Context context;

    public KeyHandler(Context context) {
        packageManager = context.getPackageManager();
        activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);

        this.context = context;
    }

    @Override
    public KeyEvent handleKeyEvent(KeyEvent event) {
        int scanCode = event.getScanCode();
        int action = event.getAction();

        if (scanCode == SCANCODE_JASMINE || scanCode == SCANCODE_CLOVER) {
            if (actionBefore != KeyEvent.ACTION_UP) {
                if (action == KeyEvent.ACTION_UP) {
                    ComponentName componentName = activityManager.getRunningTasks(1).get(0).topActivity;

                    Intent startActivity = new Intent()
                            .setComponent(new ComponentName("org.lineageos.settings.device",
                                    "org.lineageos.settings.device.StartActionActivity"))
                            .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                            .putExtra("rCamera", Arrays.asList(CAMERA_PACKAGES).contains(componentName.getPackageName()));

                    if (startActivity.resolveActivity(packageManager) != null) {
                        context.startActivity(startActivity);
                    }
                }
            }
            actionBefore = action;
        }
        return event;
    }
}
