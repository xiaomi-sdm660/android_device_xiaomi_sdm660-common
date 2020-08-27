/*
 * Copyright (C) 2018-2020 The Xiaomi-SDM660 Project
 *
 *  https://github.com/xiaomi-sdm660
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

package com.advanced.settings.doze;

import android.annotation.TargetApi;
import android.content.Intent;
import android.service.quicksettings.Tile;
import android.service.quicksettings.TileService;

import com.advanced.settings.doze.R;

@TargetApi(24)
public class DozeTileService extends TileService {
    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onTileAdded() {
        super.onTileAdded();
    }

    @Override
    public void onTileRemoved() {
        super.onTileRemoved();
    }

    @Override
    public void onStartListening() {
        super.onStartListening();
    }

    @Override
    public void onStopListening() {
        super.onStopListening();
    }

    @Override
    public void onClick() {
        super.onClick();
        Intent DeviceDoze = new Intent(this, DozeSettingsActivity.class);
        DeviceDoze.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivityAndCollapse(DeviceDoze);
    }
}
