package org.lineageos.settings.kcal;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

public class EditTextDialog extends DialogFragment {

    int mMin;
    int mMax;
    int mDefaultValue;
    DialogResult mResult;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        LayoutInflater inflater = getActivity().getLayoutInflater();
        View view = inflater.inflate(R.layout.dialog_edit_text, null);
        EditText editTex = view.findViewById(R.id.value);
        editTex.setHint(String.valueOf(mMin) + " - " + String.valueOf(mMax));
        builder.setView(view)
                .setMessage(R.string.edit_value)
                .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        if (mResult != null) {
                            String newValueString = editTex.getText().toString();
                            if (newValueString != null && !newValueString.isEmpty()) {
                                mResult.finish(Integer.parseInt(editTex.getText().toString()));
                            } else mResult.finish(-1);
                        }
                        EditTextDialog.this.dismiss();
                    }
                })
                .setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        EditTextDialog.this.getDialog().cancel();
                    }
                });
        return builder.create();
    }

    public void setDialogResult(DialogResult dialogResult) {
        mResult = dialogResult;
    }

    public void setMin(int min) {
        mMin = min;
    }

    public void setMax(int max) {
        mMax = max;
    }

    public void setDefaultValue(int defaultValue) {
        mDefaultValue = defaultValue;
    }

    public interface DialogResult {
        void finish(int result);
    }
}
