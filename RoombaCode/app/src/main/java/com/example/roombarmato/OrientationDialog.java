package com.example.roombarmato;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Spinner;
import android.widget.Switch;

import androidx.fragment.app.DialogFragment;

public class OrientationDialog extends DialogFragment {

    static final String TAG = "ORIENTATION_DIALOG";

    public OrientationDialog() {
    }

    @Override
    public Dialog onCreateDialog(Bundle _savedInstanceState) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        builder.setTitle(getString(R.string.orientationDialogTitle));
        final LayoutInflater inflater = requireActivity().getLayoutInflater();
        View _view = inflater.inflate(R.layout.orientation_dialog, null);
        builder.setView(_view);

        SharedPreferences spFile = getContext().getSharedPreferences(getString(R.string.shared_preferences_file), Context.MODE_PRIVATE);
        final SharedPreferences.Editor spFileEditor = spFile.edit();
        final Spinner selectOrientationSpinner = _view.findViewById(R.id.selectOrientationSpinner);
        final Switch lockOrientationSwitch = _view.findViewById(R.id.lockOrientationSwitch);

        if(spFile.getString(getString(R.string.spFile_defaultOrientation), "Verticale").equals("Verticale")) selectOrientationSpinner.setSelection(0);
        else selectOrientationSpinner.setSelection(1);
        lockOrientationSwitch.setChecked(spFile.getBoolean(getString(R.string.spFile_orientationLocked), false));

        builder.setPositiveButton("SALVA", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface thisDialog, int which) {
                spFileEditor.putBoolean(getString(R.string.spFile_orientationLocked), lockOrientationSwitch.isChecked());
                spFileEditor.putString(getString(R.string.spFile_defaultOrientation), selectOrientationSpinner.getSelectedItem().toString());
                spFileEditor.apply();
            }
        });

        builder.setNegativeButton("IGNORA", null);

        setCancelable(true);
        return builder.create();
    }
}
