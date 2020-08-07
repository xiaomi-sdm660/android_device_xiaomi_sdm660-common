/*
* Copyright (C) 2019-20 Evolution X Project
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
package org.lineageos.settings.doze;

import android.annotation.TargetApi;
import android.content.Intent;
import android.service.quicksettings.Tile;
import android.service.quicksettings.TileService;

import org.lineageos.settings.doze.R;

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
        Intent XiaomiDoze = new Intent(this, DozeSettingsActivity.class);
        XiaomiDoze.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivityAndCollapse(XiaomiDoze);
    }
}
