package org.lineageos.settings.kcal;

import android.content.Context;
import android.util.AttributeSet;

public class SecureCustomSeekBarPreference extends CustomSeekBarPreference {

    public SecureCustomSeekBarPreference(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        setPreferenceDataStore(new SecureSettingsStore(context.getContentResolver()));
    }

    public SecureCustomSeekBarPreference(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setPreferenceDataStore(new SecureSettingsStore(context.getContentResolver()));
    }

    public SecureCustomSeekBarPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        setPreferenceDataStore(new SecureSettingsStore(context.getContentResolver()));
    }

    public SecureCustomSeekBarPreference(Context context) {
        super(context);
        setPreferenceDataStore(new SecureSettingsStore(context.getContentResolver()));
    }
}
