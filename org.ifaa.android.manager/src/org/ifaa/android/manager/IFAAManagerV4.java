package org.ifaa.android.manager;

import android.annotation.UnsupportedAppUsage;

public abstract class IFAAManagerV4 extends IFAAManagerV3 {
    @UnsupportedAppUsage
    public abstract int getEnabled(int bioType);

    @UnsupportedAppUsage
    public abstract int[] getIDList(int bioType);

}
