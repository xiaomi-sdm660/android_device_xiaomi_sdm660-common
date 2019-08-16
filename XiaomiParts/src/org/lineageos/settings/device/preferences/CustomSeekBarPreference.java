/*
 * Copyright (C) 2016 The Dirty Unicorns Project
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

package org.lineageos.settings.device.preferences;

import android.app.AlertDialog;
import android.content.Context;
import android.content.res.TypedArray;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceViewHolder;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import org.lineageos.settings.device.R;

public class CustomSeekBarPreference extends Preference implements SeekBar.OnSeekBarChangeListener {
    private static final String APP_NS = "http://schemas.android.com/apk/res-auto";
    private static final String ANDROIDNS = "http://schemas.android.com/apk/res/android";
    private static final int DEFAULT_VALUE = 50;
    private final String TAG = getClass().getName();
    private final SeekBar mSeekBar;
    private int mMin;
    private int mInterval = 1;
    private int mCurrentValue;
    private int mDefaultValue;
    private int mMax;
    private String mUnits;
    private String mDefaultText;
    private TextView mTitle;
    private TextView mStatusText;
    private AlertDialog mDialog;
    private EditText mEdit;

    public CustomSeekBarPreference(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        final TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.CustomSeekBarPreference);

        mMax = attrs.getAttributeIntValue(ANDROIDNS, "max", 100);
        mMin = attrs.getAttributeIntValue(APP_NS, "min", 0);
        mDefaultValue = attrs.getAttributeIntValue(ANDROIDNS, "defaultValue", -1);
        if (mDefaultValue > mMax) {
            mDefaultValue = mMax;
        }
        mUnits = getAttributeStringValue(attrs, APP_NS, "units", "");
        mDefaultText = getAttributeStringValue(attrs, APP_NS, "defaultText",
                Integer.toString(mDefaultValue));

        int id = a.getResourceId(R.styleable.CustomSeekBarPreference_units, 0);
        if (id > 0) {
            mUnits = context.getResources().getString(id);
        }
        id = a.getResourceId(R.styleable.CustomSeekBarPreference_defaultText, 0);
        if (id > 0) {
            mDefaultText = context.getResources().getString(id);
        }

        try {
            String newInterval = attrs.getAttributeValue(APP_NS, "interval");
            if (newInterval != null)
                mInterval = Integer.parseInt(newInterval);
        } catch (Exception e) {
            Log.e(TAG, "Invalid interval value", e);
        }

        a.recycle();
        mSeekBar = new SeekBar(context, attrs);
        mSeekBar.setMax(mMax - mMin);
        mSeekBar.setOnSeekBarChangeListener(this);
        setLayoutResource(R.layout.preference_custom_seekbar);
    }

    public CustomSeekBarPreference(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public CustomSeekBarPreference(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CustomSeekBarPreference(Context context) {
        this(context, null);
    }

    private String getAttributeStringValue(AttributeSet attrs, String namespace, String name, String defaultValue) {
        String value = attrs.getAttributeValue(namespace, name);
        if (value == null)
            value = defaultValue;

        return value;
    }

    @Override
    public void onDependencyChanged(Preference dependency, boolean disableDependent) {
        super.onDependencyChanged(dependency, disableDependent);
        this.setShouldDisableView(true);
        if (mTitle != null)
            mTitle.setEnabled(!disableDependent);
        if (mSeekBar != null)
            mSeekBar.setEnabled(!disableDependent);
        if (mStatusText != null)
            mStatusText.setEnabled(!disableDependent);
    }

    @Override
    public void onBindViewHolder(PreferenceViewHolder view) {
        super.onBindViewHolder(view);
        try {
            // move our seekbar to the new view we've been given
            ViewParent oldContainer = mSeekBar.getParent();
            ViewGroup newContainer = (ViewGroup) view.findViewById(R.id.seekBarPrefBarContainer);

            if (oldContainer != newContainer) {
                // remove the seekbar from the old view
                if (oldContainer != null) {
                    ((ViewGroup) oldContainer).removeView(mSeekBar);
                }
                // remove the existing seekbar (there may not be one) and add ours
                newContainer.removeAllViews();
                newContainer.addView(mSeekBar, ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            }
        } catch (Exception ex) {
            Log.e(TAG, "Error binding view: " + ex.toString());
        }
        mStatusText = (TextView) view.findViewById(R.id.seekBarPrefValue);
        if (mCurrentValue == mDefaultValue) {
            mStatusText.setText(mDefaultText);
        } else {
            mStatusText.setText(mCurrentValue + mUnits);
        }
        mSeekBar.setProgress(mCurrentValue - mMin);
        LinearLayout statusTextContainer = (LinearLayout) view.findViewById(R.id.text_container);
        statusTextContainer.setClickable(true);
        statusTextContainer.setOnClickListener((View v) -> {
            mDialog = new AlertDialog.Builder(getContext())
                    .setTitle(getContext().getResources().getString(R.string.edit_value))
                    .setView(R.layout.custom_seekbar_preference_dialog)
                    .setNegativeButton(android.R.string.cancel, (dialog, which) -> mDialog = null)
                    .setNeutralButton(R.string.default_value, (dialog, which) -> {
                        refresh(mDefaultValue);
                        mDialog = null;
                    })
                    .setPositiveButton(android.R.string.ok, (dialog, which) -> {
                        if (mEdit != null) {
                            try {
                                if (Integer.parseInt(mEdit.getText().toString()) >= mMin && Integer.parseInt(mEdit.getText().toString()) <= mMax) {
                                    refresh(Integer.parseInt(mEdit.getText().toString()));
                                }
                            } catch (NumberFormatException e) {
                                // Prevent crashes and ignore invalid input
                            }
                        }
                        mDialog = null;
                    })
                    .show();
            mEdit = mDialog.findViewById(R.id.edit);
            mEdit.addTextChangedListener(new TextWatcher() {
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                }

                @Override
                public void onTextChanged(CharSequence s, int start, int before, int count) {
                }

                @Override
                public void afterTextChanged(Editable s) {
                    try {
                        if (Integer.parseInt(s.toString()) > mMax) {
                            s.replace(0, s.length(), String.valueOf(mMax));
                        } else if (Integer.parseInt(s.toString()) < mMin) {
                            s.replace(0, s.length(), String.valueOf(mMin));
                        }
                    } catch (NumberFormatException e) {
                        // Prevent crashes and ignore invalid input
                    }
                }
            });
            mEdit.setHint(getContext().getResources().getString(R.string.edit_hint, mMin, mMax));
            mEdit.setText(String.valueOf(mCurrentValue));
        });

        mTitle = (TextView) view.findViewById(android.R.id.title);
        view.setDividerAllowedAbove(false);

        mSeekBar.setEnabled(isEnabled());
    }

    public void setMax(int max) {
        mMax = max;
        mSeekBar.setMax(mMax - mMin);
    }

    public void setMin(int min) {
        mMin = min;
        mSeekBar.setMax(mMax - mMin);
    }

    public void setIntervalValue(int value) {
        mInterval = value;
    }

    public String getValue() {
        return Integer.toString(mCurrentValue);
    }

    public void setValue(int value) {
        mCurrentValue = value;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        int newValue = progress + mMin;
        if (newValue > mMax)
            newValue = mMax;
        else if (newValue < mMin)
            newValue = mMin;
        else if (mInterval != 1 && newValue % mInterval != 0)
            newValue = Math.round(((float) newValue) / mInterval) * mInterval;

        // change rejected, revert to the previous value
        if (!callChangeListener(newValue)) {
            seekBar.setProgress(mCurrentValue - mMin);
            return;
        }
        // change accepted, store it
        mCurrentValue = newValue;
        if (mStatusText != null) {
            if (newValue == mDefaultValue) {
                mStatusText.setText(mDefaultText);
            } else {
                mStatusText.setText(newValue + mUnits);
            }
        }
        persistInt(newValue);
    }

    public void refresh(int newValue) {
        // this will trigger onProgressChanged and refresh everything
        mSeekBar.setProgress(newValue - mMin);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        notifyChanged();
    }

    @Override
    protected Object onGetDefaultValue(TypedArray ta, int index) {
        return ta.getInt(index, DEFAULT_VALUE);
    }

    @Override
    protected void onSetInitialValue(boolean restoreValue, Object defaultValue) {
        if (restoreValue) {
            mCurrentValue = getPersistedInt(mDefaultValue);
        } else {
            int temp = 0;
            try {
                temp = (Integer) defaultValue;
            } catch (Exception ex) {
                Log.e(TAG, "Invalid default value: " + defaultValue.toString());
            }
            persistInt(temp);
            mCurrentValue = temp;
        }
    }

    public void setDefaultValue(int value) {
        mDefaultValue = value;
        if (mDefaultValue > mMax) {
            mDefaultValue = mMax;
        }
        if (mCurrentValue == mDefaultValue && mStatusText != null) {
            mStatusText.setText(mDefaultText);
        }
    }

    @Override
    public void setEnabled(boolean enabled) {
        if (mSeekBar != null && mStatusText != null && mTitle != null) {
            mSeekBar.setEnabled(enabled);
            mStatusText.setEnabled(enabled);
            mTitle.setEnabled(enabled);
        }
        super.setEnabled(enabled);
    }
}
