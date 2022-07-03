package com.example.roombarmato;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.fragment.app.DialogFragment;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class SongsDialog extends DialogFragment {

    static final String TAG = "SONGS_DIALOG";

    public SongsDialog() {

    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        builder.setTitle("Lista canzoni");
        final LayoutInflater inflater = requireActivity().getLayoutInflater();
        View view = inflater.inflate(R.layout.songs_dialog, null);
        builder.setView(view);

        final SharedPreferences spFile = getContext().getSharedPreferences(getString(R.string.shared_preferences_file), Context.MODE_PRIVATE);
        final SharedPreferences.Editor spFileEditor = spFile.edit();

        try {
            final JSONArray songs = new JSONArray(spFile.getString(getString(R.string.spFile_songs), new JSONArray().toString()));

            final TextView songText = view.findViewById(R.id.songs_textView);
            final EditText titleEdit = view.findViewById(R.id.title_editText);
            final Button addButton = view.findViewById(R.id.add_button);
            final Button removeButton = view.findViewById(R.id.remove_button);

            String titlesString = "";
            for (int i = 0; i < songs.length(); i++) {
                JSONObject song = songs.getJSONObject(i);
                titlesString += song.getString(getString(R.string.json_title)) + " | " + song.getInt(getString(R.string.json_index)) + "\n";
            }
            songText.setText(titlesString);

            addButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    String title = "" + titleEdit.getText();
                    if (title.length() > 0 && !title.contains("\"") && !title.contains("\\") && !title.contains("\'")) {
                        JSONObject newSong = new JSONObject();
                        try {
                            newSong.put(getString(R.string.json_title), title);
                            newSong.put(getString(R.string.json_index), songs.length());
                            songs.put(newSong);

                            String _titlesString = "";
                            for (int i = 0; i < songs.length(); i++) {
                                try {
                                    JSONObject _song = songs.getJSONObject(i);
                                    _titlesString += _song.getString(getString(R.string.json_title)) + "\t| " + _song.getInt(getString(R.string.json_index)) + "\n";
                                } catch (JSONException e) {
                                    e.printStackTrace();
                                }
                            }
                            songText.setText(_titlesString);

                            titleEdit.setText("");
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                }
            });

            removeButton.setOnClickListener(new View.OnClickListener() {
                @RequiresApi(api = Build.VERSION_CODES.KITKAT)
                @Override
                public void onClick(View v) {
                    songs.remove(songs.length() - 1);

                    String _titlesString = "";
                    for (int i = 0; i < songs.length(); i++) {
                        try {
                            JSONObject _song = songs.getJSONObject(i);
                            _titlesString += _song.getString(getString(R.string.json_title)) + "\t| " + _song.getInt(getString(R.string.json_index)) + "\n";
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                    songText.setText(_titlesString);
                }
            });

            titleEdit.setOnEditorActionListener(new TextView.OnEditorActionListener() {
                @Override
                public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                    switch (actionId) {
                        case EditorInfo.IME_ACTION_DONE:
                        case EditorInfo.IME_ACTION_GO:
                        case EditorInfo.IME_ACTION_NEXT:
                        case EditorInfo.IME_ACTION_SEARCH:
                        case EditorInfo.IME_ACTION_SEND:
                            String title = "" + titleEdit.getText();
                            if (title.length() > 0 && !title.contains("\"") && !title.contains("\\") && !title.contains("\'")) {
                                JSONObject newSong = new JSONObject();
                                try {
                                    newSong.put(getString(R.string.json_title), title);
                                    newSong.put(getString(R.string.json_index), songs.length());
                                    songs.put(newSong);

                                    String _titlesString = "";
                                    for (int i = 0; i < songs.length(); i++) {
                                        try {
                                            JSONObject _song = songs.getJSONObject(i);
                                            _titlesString += _song.getString(getString(R.string.json_title)) + " | " + _song.getInt(getString(R.string.json_index)) + "\n";
                                        } catch (JSONException e) {
                                            e.printStackTrace();
                                        }
                                    }
                                    songText.setText(_titlesString);

                                    titleEdit.setText("");
                                } catch (JSONException e) {
                                    e.printStackTrace();
                                }
                            }
                            break;

                    }
                    return true;
                }
            });

            builder.setPositiveButton("SALVA", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    spFileEditor.putString(getString(R.string.spFile_songs), songs.toString());
                    spFileEditor.apply();
                }
            });
        } catch (JSONException e) {
            e.printStackTrace();
        }

        builder.setNegativeButton("ANNULLA", null);

        return builder.create();
    }
}