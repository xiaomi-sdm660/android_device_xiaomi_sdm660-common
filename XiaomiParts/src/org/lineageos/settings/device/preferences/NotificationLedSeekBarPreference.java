package org.lineageos.settings.device.preferences;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.SeekBar;
import org.lineageos.settings.device.FileUtils;

public class NotificationLedSeekBarPreference extends SecureSettingCustomSeekBarPreference {

    public static final String NOTIF_LED_PATH = "/sys/class/leds/white/brightness";

    public NotificationLedSeekBarPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    
    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        notifyChanged();
        FileUtils.setValue(NOTIF_LED_PATH, 64);
    }
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        notifyChanged();
        FileUtils.setValue(NOTIF_LED_PATH, 0);
    }
}