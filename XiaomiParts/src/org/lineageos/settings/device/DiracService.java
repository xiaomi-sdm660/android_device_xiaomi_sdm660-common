package org.lineageos.settings.device;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class DiracService extends Service {

    private final String TAG = this.getClass().getName();

    static DiracUtils sDiracUtils;

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        sDiracUtils = new DiracUtils();
        sDiracUtils.onBootCompleted();
        Log.d(TAG, "Service started");
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "Service destroyed");
    }
}
