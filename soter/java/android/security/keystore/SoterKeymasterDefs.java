package android.security.keystore;

import android.security.keymaster.KeymasterDefs;

public class SoterKeymasterDefs {

    public static final int KM_TAG_SOTER_IS_FROM_SOTER = KeymasterDefs.KM_BOOL | 11000;
    public static final int KM_TAG_SOTER_IS_AUTO_SIGNED_WITH_ATTK_WHEN_GET_PUBLIC_KEY = KeymasterDefs.KM_BOOL | 11001;
    public static final int KM_TAG_SOTER_IS_AUTO_SIGNED_WITH_COMMON_KEY_WHEN_GET_PUBLIC_KEY = KeymasterDefs.KM_BOOL | 11002;
    public static final int KM_TAG_SOTER_AUTO_SIGNED_COMMON_KEY_WHEN_GET_PUBLIC_KEY = KeymasterDefs.KM_BYTES | 11003;
    public static final int KM_TAG_SOTER_AUTO_ADD_COUNTER_WHEN_GET_PUBLIC_KEY = KeymasterDefs.KM_BOOL | 11004;
    public static final int KM_TAG_SOTER_IS_SECMSG_FID_COUNTER_SIGNED_WHEN_SIGN = KeymasterDefs.KM_BOOL | 11005;
    public static final int KM_TAG_SOTER_USE_NEXT_ATTK = KeymasterDefs.KM_BOOL | 11006;
    public static final int KM_TAG_SOTER_UID = KeymasterDefs.KM_UINT | 11007;
    public static final int KM_TAG_SOTER_AUTO_SIGNED_COMMON_KEY_WHEN_GET_PUBLIC_KEY_BLOB = KeymasterDefs.KM_BYTES | 11008;

}
