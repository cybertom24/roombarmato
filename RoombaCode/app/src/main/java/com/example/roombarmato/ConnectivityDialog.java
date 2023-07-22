package com.example.roombarmato;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.EditText;
import android.widget.Toast;

import androidx.fragment.app.DialogFragment;

public class ConnectivityDialog extends DialogFragment {

    static final String TAG = "CONNECTIVITY_DIALOG";

    public ConnectivityDialog() {
    }

    @Override
    public Dialog onCreateDialog(Bundle _savedInstanceState) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        builder.setTitle(getString(R.string.connectivityDialogTitle));
        final LayoutInflater inflater = requireActivity().getLayoutInflater();
        builder.setView(inflater.inflate(R.layout.connectivity_dialog, null));

        builder.setPositiveButton("SALVA", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface thisDialog, int which) {
                Dialog dialog = Dialog.class.cast(thisDialog);

                EditText ip1EditText = dialog.findViewById(R.id.ip1EditText);
                EditText ip2EditText = dialog.findViewById(R.id.ip2EditText);
                EditText ip3EditText = dialog.findViewById(R.id.ip3EditText);
                EditText ip4EditText = dialog.findViewById(R.id.ip4EditText);
                EditText receiverPortEditText = dialog.findViewById(R.id.receiverPortEditText);

                if (ip1EditText.getText().toString().length() > 0 && ip2EditText.getText().toString().length() > 0 && ip3EditText.getText().toString().length() > 0 && ip4EditText.getText().toString().length() > 0 && receiverPortEditText.getText().toString().length() > 0) {
                    int ip1 = Integer.parseInt(ip1EditText.getText().toString());
                    int ip2 = Integer.parseInt(ip2EditText.getText().toString());
                    int ip3 = Integer.parseInt(ip3EditText.getText().toString());
                    int ip4 = Integer.parseInt(ip4EditText.getText().toString());
                    int receiverPort = Integer.parseInt(receiverPortEditText.getText().toString());

                    if (ip1 < 256 && ip2 < 256 && ip3 < 256 && ip4 < 256 && receiverPort >= 0 && receiverPort <= 65535) {
                        SharedPreferences spFile = getContext().getSharedPreferences(getString(R.string.shared_preferences_file), Context.MODE_PRIVATE);
                        SharedPreferences.Editor spFileEditor = spFile.edit();

                        String ip = ip1 + "." + ip2 + "." + ip3 + "." + ip4;
                        spFileEditor.putString(getString(R.string.spFile_receiverIP), ip);
                        spFileEditor.putInt(getString(R.string.spFile_receiverPORT), receiverPort);
                        spFileEditor.apply();
                    } else
                        Toast.makeText(getContext(), "Error: at least one number was wrong", Toast.LENGTH_LONG).show();
                } else
                    Toast.makeText(getContext(), "Error: at least one field was empty", Toast.LENGTH_LONG).show();
            }
        });

        builder.setNegativeButton("IGNORA", null);

        setCancelable(true);
        return builder.create();
    }
}
