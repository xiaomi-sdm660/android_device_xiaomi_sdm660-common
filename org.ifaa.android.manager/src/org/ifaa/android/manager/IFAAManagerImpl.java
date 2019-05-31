package org.ifaa.android.manager;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.Build.VERSION;
import android.os.HwBinder;
import android.os.HwBlob;
import android.os.HwParcel;
import android.os.IBinder;
import android.os.IBinder.DeathRecipient;
import android.os.IHwBinder;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Slog;

import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;

public class IFAAManagerImpl extends IFAAManagerV4 {
    private static final boolean DEBUG = false;
    private static final String TAG = "IfaaManagerImpl";

    private static final int CODE_PROCESS_CMD = 1;
    private static final int CODE_GETIDLIST_CMD = 2;

    private static final int IFAA_TYPE_FINGER = 1;
    private static final int IFAA_TYPE_IRIS = (1 << 1);
    private static final int IFAA_TYPE_SENSOR_FOD = (1 << 4);
    private static final int IFAA_TYPE_2DFA = (1 << 5);

    private static volatile IFAAManagerImpl INSTANCE = null;

    private static final String INTERFACE_DESCRIPTOR = "vendor.xiaomi.hardware.mlipay@1.0::IMlipayService";
    private static final String SERVICE_NAME = "vendor.xiaomi.hardware.mlipay@1.0::IMlipayService";

    private static final int ACTIVITY_START_SUCCESS = 0;
    private static final int ACTIVITY_START_FAILED = -1;
    // private static final String mFingerActName = "com.android.settings.NewFingerprintActivity";
    // private static final String mFingerPackName = "com.android.settings";
    private static final String mIfaaActName = "org.ifaa.android.manager.IFAAService";
    private static final String mIfaaInterfaceDesc = "org.ifaa.android.manager.IIFAAService";
    private static final String mIfaaPackName = "com.tencent.soter.soterserver";

    private static IBinder mService = null;
    private static final String seperate = ",";
    private String mDevModel = null;

    private static Context mContext = null;

    private static ServiceConnection ifaaconn = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = service;
            try {
                mService.linkToDeath(mDeathRecipient, 0);
            } catch (RemoteException e) {
                Slog.e(TAG, "linkToDeath fail.", e);
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            if (mContext != null) {
                Slog.i(IFAAManagerImpl.TAG, "re-bind the service.");
                initService();
            }
        }
    };

    private static DeathRecipient mDeathRecipient = new DeathRecipient() {
        public void binderDied() {
            if (mService != null) {
                Slog.d(TAG, "binderDied, unlink the service.");
                mService.unlinkToDeath(mDeathRecipient, 0);
            }
        }
    };

    public static IFAAManagerV4 getInstance(Context context) {
        if (INSTANCE == null) {
            synchronized (IFAAManagerImpl.class) {
                if (INSTANCE == null) {
                    INSTANCE = new IFAAManagerImpl();
                    if (VERSION.SDK_INT >= 28) {
                        mContext = context;
                        initService();
                    }
                }
            }
        }
        return INSTANCE;
    }

    private static void initService() {
        Intent ifaaIntent = new Intent();
        ifaaIntent.setClassName(mIfaaPackName, mIfaaActName);
        if (!mContext.bindService(ifaaIntent, ifaaconn, 1)) {
            Slog.e(TAG, "cannot bind service org.ifaa.android.manager.IFAAService");
        }
    }

    public int getSupportBIOTypes(Context context) {
        int ifaaProp;
        int res;
        String fpVendor;
        int ifaa_2dfa_support = 0; // FeatureParser.getInteger("ifaa_2dfa_support", 0);

        if (VERSION.SDK_INT >= 28) {
            ifaaProp = SystemProperties.getInt("persist.vendor.sys.pay.ifaa", 0);
            fpVendor = SystemProperties.get(
                    "persist.vendor.sys.fp.vendor", "");
        } else {
            ifaaProp = SystemProperties.getInt("persist.sys.ifaa", 0);
            fpVendor = SystemProperties.get("persist.sys.fp.vendor", "");
        }

        if ("none".equalsIgnoreCase(fpVendor)) {
            res = ifaaProp & IFAA_TYPE_IRIS;
        } else {
            res = ifaaProp & (IFAA_TYPE_FINGER | IFAA_TYPE_IRIS);
        }

        if ((res & IFAA_TYPE_FINGER) == IFAA_TYPE_FINGER && sIsFod) {
            res |= IFAA_TYPE_SENSOR_FOD;
        }

        if (ifaa_2dfa_support == 1 && IFAAManagerV3.VLAUE_FINGERPRINT_ENABLE.equals(
                SystemProperties.get("ro.boot.hypvm", ""))) {
            res |= IFAA_TYPE_2DFA;
        }

        Slog.i(TAG, "getSupportBIOTypesV26:" + ifaaProp + " " + sIsFod + " " + fpVendor +
                " res:" + res);

        return res;
    }

    public int startBIOManager(Context context, int authType) {
        int res = ACTIVITY_START_FAILED;
        if (IFAA_TYPE_FINGER == authType) {
            // Intent intent = new Intent();
            // intent.setClassName("com.android.settings", mFingerActName);
            Intent intent = new Intent("android.settings.SECURITY_SETTINGS");
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
            res = ACTIVITY_START_SUCCESS;
        }
        if (DEBUG) Slog.i(TAG, "startBIOManager authType:" + authType + " res:" + res);
        return res;
    }

    public String getDeviceModel() {
        // String miuiFeature;
        if (mDevModel == null) {
            // miuiFeature = FeatureParser.getString("finger_alipay_ifaa_model");
            // if (miuiFeature != null) {
            //     if (!StringUtils.EMPTY.equalsIgnoreCase(miuiFeature)) {
            //         this.mDevModel = miuiFeature;
            //     }
            // }
            mDevModel = "xiaomi" + "-" + "wayne";
        }
        if (DEBUG) Slog.i(TAG, "getDeviceModel devcieModel:" + mDevModel);
        return mDevModel;
    }

    public int getVersion() {
        if (DEBUG) Slog.i(TAG, "getVersion sdk:" + VERSION.SDK_INT + " ifaaVer:" + sIfaaVer);
        return sIfaaVer;
    }

    public byte[] processCmdV2(Context context, byte[] param) {
        Slog.i(TAG, "processCmdV2 sdk:" + VERSION.SDK_INT);

        if (VERSION.SDK_INT >= 28) {
            int retry_count = 10;

            while (true) {
                int retry_count2 = retry_count - 1;
                if (retry_count <= 0) {
                    break;
                }
                if (mService == null || !mService.pingBinder()) {
                    Slog.i(TAG, "processCmdV2 waiting ifaaService, remain: " +
                            retry_count2 + " time(s)");
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException e) {
                        Slog.e(TAG, "processCmdV2 InterruptedException while waiting: " + e);
                    }
                } else {
                    Parcel data = Parcel.obtain();
                    Parcel reply = Parcel.obtain();
                    try {
                        data.writeInterfaceToken(mIfaaInterfaceDesc);
                        data.writeByteArray(param);
                        mService.transact(CODE_PROCESS_CMD, data, reply, 0);
                        reply.readException();
                        return reply.createByteArray();
                    } catch (RemoteException e) {
                        Slog.e(TAG, "processCmdV2 transact failed. " + e);
                        retry_count = retry_count2;
                    } finally {
                        data.recycle();
                        reply.recycle();
                    }
                }
                retry_count = retry_count2;
            }
        }
        HwParcel hidl_reply = new HwParcel();
        try {
            IHwBinder hwService = HwBinder.getService(SERVICE_NAME, "default");
            if (hwService != null) {
                HwParcel hidl_request = new HwParcel();
                hidl_request.writeInterfaceToken(INTERFACE_DESCRIPTOR);
                ArrayList<Byte> sbuf = new ArrayList(Arrays.asList(HwBlob.wrapArray(param)));
                hidl_request.writeInt8Vector(sbuf);
                hidl_request.writeInt32(sbuf.size());
                hwService.transact(CODE_PROCESS_CMD, hidl_request, hidl_reply, 0);
                hidl_reply.verifySuccess();
                hidl_request.releaseTemporaryStorage();
                ArrayList<Byte> val = hidl_reply.readInt8Vector();
                int n = val.size();
                byte[] array = new byte[n];
                for (int i = 0; i < n; i++) {
                    array[i] = ((Byte) val.get(i)).byteValue();
                }
                hidl_reply.release();
                return array;
            }
        } catch (RemoteException e) {
            Slog.e(TAG, "transact failed. " + e);
        } finally {
            hidl_reply.release();
        }
        Slog.e(TAG, "processCmdV2, return null");
        return null;
    }

    public String getExtInfo(int authType, String keyExtInfo) {
        return initExtString();
    }

    public void setExtInfo(int authType, String keyExtInfo, String valExtInfo) {
    }

    public int getEnabled(int bioType) {
        if (IFAA_TYPE_FINGER == bioType) {
            return 1000;
        }
        return 1003;
    }

    public int[] getIDList(int bioType) {
        int[] idList = new int[]{0};
        if (IFAA_TYPE_FINGER == bioType) {
            int retry_count = 10;
            while (true) {
                int retry_count2 = retry_count - 1;
                if (retry_count <= 0) {
                    break;
                }
                if (mService == null || !mService.pingBinder()) {
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException e) {
                        Slog.e(TAG, "getIDList InterruptedException while waiting: " + e);
                    }
                } else {
                    Parcel data = Parcel.obtain();
                    Parcel reply = Parcel.obtain();
                    try {
                        data.writeInterfaceToken(mIfaaInterfaceDesc);
                        data.writeInt(bioType);
                        mService.transact(CODE_GETIDLIST_CMD, data, reply, 0);
                        reply.readException();
                        idList = reply.createIntArray();
                    } catch (RemoteException e) {
                        Slog.e(TAG, "getIDList transact failed. " + e);
                    } finally {
                        data.recycle();
                        reply.recycle();
                    }
                }
                retry_count = retry_count2;
            }
        }
        return idList;
    }

    private String initExtString() {
        String extStr = "";
        JSONObject obj = new JSONObject();
        JSONObject keyInfo = new JSONObject();
        String xy = "";
        String wh = "";

        if (VERSION.SDK_INT >= 28) {
            xy = SystemProperties.get("persist.vendor.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.vendor.sys.fp.fod.size.width_height", "");
        } else {
            xy = SystemProperties.get("persist.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.sys.fp.fod.size.width_height", "");
        }

        try {
            if (validateVal(xy) && validateVal(wh)) {
                String[] splitXy = xy.split(seperate);
                String[] splitWh = wh.split(seperate);
                keyInfo.put("startX", Integer.parseInt(splitXy[0]));
                keyInfo.put("startY", Integer.parseInt(splitXy[1]));
                keyInfo.put("width", Integer.parseInt(splitWh[0]));
                keyInfo.put("height", Integer.parseInt(splitWh[1]));
                keyInfo.put("navConflict", true);
                obj.put("type", 0);
                obj.put("fullView", keyInfo);
                extStr = obj.toString();
            } else {
                Slog.e(TAG, "initExtString invalidate, xy:" + xy + " wh:" + wh);
            }
        } catch (Exception e) {
            Slog.e(TAG, "Exception , xy:" + xy + " wh:" + wh, e);
        }
        return extStr;
    }

    private boolean validateVal(String value) {
        if ("".equalsIgnoreCase(value) || !value.contains(seperate)) {
            return false;
        }
        return true;
    }
}
