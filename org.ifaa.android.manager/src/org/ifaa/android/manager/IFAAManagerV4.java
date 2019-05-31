package org.ifaa.android.manager;

public abstract class IFAAManagerV4 extends IFAAManagerV3 {
    public abstract int getEnabled(int bioType);

    public abstract int[] getIDList(int bioType);
}
