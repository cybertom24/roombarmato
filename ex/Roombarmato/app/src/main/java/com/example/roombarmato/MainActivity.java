package com.example.roombarmato;

import static com.example.roombarmato.Commands.makeCommand;

import android.annotation.SuppressLint;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.PopupMenu;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    // Objects
    Indicator rocketsIndicator;
    Joystick joystick;
    TextView terminalView;
    String terminalText = "";
    EditText terminalEdit;
    Button playButton, muteButton, nextButton, previousButton;
    Spinner songsSpinner;
    SeekBar volumeSeekBar;
    Lever angleLever;
    Button emergencyButton;
    Button menuButton;
    TextView queueTextView;

    Resources resources;
    // Control
    Commands commands;
    // Connection
    boolean connected = false;
    ArrayList<byte[]> sendBuffer;
    static final int COMMAND_DELAY = 75;
    static final int MAX_BUFFER_SIZE = 4;
    // Saving
    SharedPreferences spFile;
    SharedPreferences.Editor spFileEditor;
    // MP3
    JSONArray songs;
    boolean playing = false, muted = false;
    int volume;
    // Combat
    boolean[] status;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        resources = getResources();

        spFile = getSharedPreferences(resources.getString(R.string.shared_preferences_file), MODE_PRIVATE);
        spFileEditor = spFile.edit();

        sendBuffer = new ArrayList<>();

        basicConfiguration();

        connected = spFile.getBoolean(resources.getString(R.string.spFile_connected), false);
        if (connected) startUdp();

        commands = new Commands();
        commands.setCommandsListener(new Commands.CommandsListener() {
            @Override
            public void onCommand(byte[] command) {
            }

            @Override
            public void onConnected() {
                // When connected send the important messages
                send(makeCommand(Commands.REQUEST_STATUS_ROCKETS));
            }

            @Override
            public void onStatusRockets(byte data) {
                // Convert data in the array
                final boolean[] status = new boolean[rocketsIndicator.quantity];
                for (int i = 0; i < status.length; i++) {
                    status[i] = (data & ((byte) Math.pow(2, i))) > 0;
                }
                // Write it into the view
                if (rocketsIndicator != null) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                rocketsIndicator.setStatusArray(status);
                            } catch (Exception e) {
                                e.printStackTrace();
                                Toast.makeText(getApplicationContext(), "An error has occurred", Toast.LENGTH_SHORT).show();
                            }
                        }
                    });
                }
            }
        });
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    void basicConfiguration() {
        Configuration config = resources.getConfiguration();
        switch (config.orientation) {
            case Configuration.ORIENTATION_PORTRAIT:
                break;
            case Configuration.ORIENTATION_LANDSCAPE:
                joystick = findViewById(R.id.joystick);
                rocketsIndicator = findViewById(R.id.rockets_indicator);
                playButton = findViewById(R.id.play_button);
                muteButton = findViewById(R.id.mute_button);
                songsSpinner = findViewById(R.id.songs_spinner);
                volumeSeekBar = findViewById(R.id.volume_seekBar);
                angleLever = findViewById(R.id.angle_lever);
                emergencyButton = findViewById(R.id.emergency_button);
                nextButton = findViewById(R.id.next_button);
                previousButton = findViewById(R.id.previous_button);
                menuButton = findViewById(R.id.menu_button);
                queueTextView = findViewById(R.id.queue_textView);

                updateSpinner();

                menuButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        // Create the popup menu
                        PopupMenu popupMenu = new PopupMenu(getApplicationContext(), v);
                        MenuInflater inflater = popupMenu.getMenuInflater();
                        inflater.inflate(R.menu.main_menu, popupMenu.getMenu());

                        popupMenu.getMenu().findItem(R.id.menu_connected).setChecked(connected);

                        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                            @Override
                            public boolean onMenuItemClick(MenuItem item) {
                                switch (item.getItemId()) {
                                    case R.id.menu_connected:
                                        if (connected) {
                                            connected = false;
                                            spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected);
                                            spFileEditor.apply();
                                        } else startUdp();
                                        break;
                                    case R.id.menu_connectivity:
                                        new ConnectivityDialog().show(getSupportFragmentManager(), ConnectivityDialog.TAG);
                                        break;
                                    case R.id.menu_reload:
                                        send(makeCommand(Commands.REQUEST_STATUS_ROCKETS));
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                updateSpinner();
                                            }
                                        });
                                        break;
                                    case R.id.menu_orientation:
                                        new OrientationDialog().show(getSupportFragmentManager(), OrientationDialog.TAG);
                                        break;
                                    case R.id.menu_clear:
                                        clearTerminal();
                                        break;
                                    case R.id.menu_information:
                                        Log.i("TEST", "funziona?");
                                        break;
                                    case R.id.menu_download:
                                        // Create the txt file and save it
                                        saveFile("" + terminalView.getText());
                                        break;
                                    case R.id.menu_songs:
                                        new SongsDialog().show(getSupportFragmentManager(), SongsDialog.TAG);
                                        break;
                                }
                                return true;
                            }
                        });

                        popupMenu.show();
                    }
                });

                playButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        playing = !playing;
                        if (playing) {
                            int selection = songsSpinner.getSelectedItemPosition();
                            send(makeCommand(Commands.SET_VOLUME, volume));
                            if (selection == 0) send(makeCommand(Commands.PLAY));
                            else send(makeCommand(Commands.PLAY_INDEX, selection - 1));

                            playButton.setText("PAUSE");
                        } else {
                            send(makeCommand(Commands.PAUSE));
                            playButton.setText("PLAY");
                        }
                    }
                });

                muteButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        muted = !muted;
                        if (muted) {
                            send(makeCommand(Commands.SET_VOLUME, 0));
                            muteButton.setText("UNMUTE");
                        } else {
                            send(makeCommand(Commands.SET_VOLUME, volume));
                            muteButton.setText("MUTE");
                        }
                    }
                });

                nextButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        send(makeCommand(Commands.NEXT_SONG));
                    }
                });

                previousButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        send(makeCommand(Commands.PREVIOUS_SONG));
                    }
                });

                volumeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        int newVolume = progress / 5;
                        newVolume *= 5;
                        if (newVolume != volume) {
                            volume = newVolume;
                            send(makeCommand(Commands.SET_VOLUME, volume));
                        }
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {

                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {

                    }
                });
                volume = volumeSeekBar.getProgress() / 5;
                volume *= 5;

                angleLever.setLeverListener(new Lever.LeverListener() {
                    @Override
                    public void onValueChanged(float value, float oldValue) {
                        Log.i("angle", value + "");
                    }
                });

                joystick.setJoystickListener(new Joystick.JoystickListener() {
                    @Override
                    public void onPositionChanged(float[] position, float[] oldPosition) {
                        // .valX() restituisce la posizione come float che va da -100 a 100 dove 0 è centrale
                        // .value_p() invece la distanza in percentuale dal centro del joystick alla manopola
                        // ovvero non ha valore
                        float valD = 0;
                        float valS = 0;
                        float valX = joystick.valX(), valP = joystick.value_p();

                        if (joystick.valY() >= 0) {
                            if (valX < 0)
                                valD = 1;
                            else {
                                // valX va da 0 a 1 (dividendolo per 100) e lo devo fare andare da 1 a -1
                                valD = valX / 100f;
                                valD *= -2;     // Inverto il senso e aumento il range
                                valD += 1;      // Sommo l'offset
                            }

                            if (valX > 0)
                                valS = 1;
                            else {
                                valS = valX / 100f;
                                valS *= 2;      // Aumento il range
                                valS += 1;      // Sommo l'offset
                            }
                        }
                        // Nel caso in cui la manopola fosse sotto lo zero gestisco i due motori in modo inverso
                        else {
                            // Invertendo la potenza (e quindi la direzione)
                            valP *= -1;
                            if (valX < 0)
                                valS = 1;
                            else {
                                valS = valX / 100f;
                                valS *= -2;
                                valS += 1;
                            }

                            if (valX > 0)
                                valD = 1;
                            else {
                                valD = valX / 100f;
                                valD *= 2;
                                valD += 1;
                            }
                        }
                        // valD e valS vanno da -1 a 1 e li faccio andare da -100 a 100 in base a quanto dista
                        // la manopola dal centro del joystick
                        valD *= valP;
                        valS *= valP;

                        //Log.i("joystick", "\nValore:\n\tD: " + valD + "\n\tS: " + valS);

                        // Conversione dei dati
                        /* In data[0] è contenuta la potenza da dare al motore DX
                         * In data[1] quella per il motore SX
                         * Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
                         * mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
                         */
                        // Recupero la direzione
                        boolean dirD = valD >= 0;
                        boolean dirS = valS >= 0;
                        // Espando il range da 0 a 100 (togliendo il segno) a da 0 a 255
                        valD = (float) Math.abs(valD) * 2.55f;
                        if (valD > 255) valD = 255;
                        valS = (float) Math.abs(valS) * 2.55f;
                        if (valS > 255) valS = 255;
                        // Genero il dato in byte che devo inviare
                        byte potD = (byte) valD;
                        byte potS = (byte) valS;
                        // Sovrascrivo la direzione sul LSB del dato da inviare
                        if (dirD) potD = (byte) (potD | 0x01);
                        else potD = (byte) (potD & 0xFE);
                        if (dirS) potS = (byte) (potS | 0x01);
                        else potS = (byte) (potS & 0xFE);

                        // Se i motori si devono fermare (pot = 00000001, in caso di "0" la direzione è preimpostata in avanti)
                        // segna il messaggio come importante (qualsiasi altro messaggio in lista viene eliminato e viene inviato solo questo)
                        //Log.i("MOVE", String.format("0x%02x / 0x%02x", potS, potD));
                        send(makeCommand(Commands.MOVE, potD, potS), (potD == 1) && (potS == 1));
                    }
                });

                emergencyButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        send(makeCommand(Commands.MOVE, 1, 1), true);
                    }
                });
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @SuppressLint("SourceLockedOrientationActivity")
    @Override
    protected void onResume() {
        super.onResume();

        // Change the rotation
        if (spFile.getBoolean(resources.getString(R.string.spFile_orientationLocked), true)) {
            // If the orientation is locked
            String defaultOrientation = spFile.getString(resources.getString(R.string.spFile_defaultOrientation), "Orizzontale");
            if (defaultOrientation.equals(resources.getStringArray(R.array.spinner_orientations)[0]))
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
            else setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        } else setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_USER);

        basicConfiguration();
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            // Hide the navigation bar
            View decorView = getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN;
            decorView.setSystemUiVisibility(uiOptions);
        }
    }

    public void writeOnTerminal(final String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (terminalView != null && terminalEdit != null) {
                    Date currentTime = Calendar.getInstance().getTime();
                    SimpleDateFormat format = new SimpleDateFormat("dd/MM/yyyy - HH:mm:ss.SSS");
                    String time = "[" + format.format(currentTime) + "] ";
                    terminalText = terminalView.getText() + "\n" + time + text;
                    terminalView.setText(terminalText);
                }
            }
        });
    }

    public void clearTerminal() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (terminalView != null) {
                    terminalText = "";
                    terminalView.setText(terminalText);
                }
            }
        });
    }

    private void send(byte[] message, boolean important) {
        if (connected) {
            if(important) {
                sendBuffer.clear();
                sendBuffer.add(message);
            }
            else if(sendBuffer.size() < MAX_BUFFER_SIZE)
                sendBuffer.add(message);
            // Add the message to the terminal and clear the editor
            //writeOnTerminal(message);
        } else Toast.makeText(getApplicationContext(), "Not connected", Toast.LENGTH_SHORT).show();
    }

    private void send(byte[] message) {
        send(message, false);
    }

    private void startUdp() {
        /*// Ottieni il manager delle connessioni wifi
        WifiManager manager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        String ssid = manager.getConnectionInfo().getSSID();
        Log.i("SSID", ssid);
        // Se il wifi non è quello giusto
        if(ssid.compareTo(resources.getString(R.string.wifi_ssid)) != 0) {
            Toast.makeText(getApplicationContext(), "Connect to " + resources.getString(R.string.wifi_ssid), Toast.LENGTH_SHORT).show();
            return;
        }*/
        // Altrimenti crea il thread e fallo partire
        new Thread(new Runnable() {
            @RequiresApi(api = Build.VERSION_CODES.KITKAT)
            @Override
            public void run() {
                // Configurations
                String ipReceiver = spFile.getString(resources.getString(R.string.spFile_receiverIP), resources.getString(R.string.receiverIP_default));
                int portReceiver = spFile.getInt(resources.getString(R.string.spFile_receiverPORT), resources.getInteger(R.integer.receiverPORT_default));
                connected = false;

                final InetAddress addressReceiver;
                final DatagramSocket socket;

                try {
                    addressReceiver = InetAddress.getByName(ipReceiver);
                    socket = new DatagramSocket();

                    connected = true;
                    spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected)
                            .apply();

                    //writeOnTerminal("Successfully connected to " + addressReceiver + ":" + portReceiver);

                    // To receive messages
                    new Thread(new Runnable() {
                        @RequiresApi(api = Build.VERSION_CODES.KITKAT)
                        @Override
                        public void run() {
                            try {
                                while (connected && !socket.isClosed()) {
                                    // Receiving messages
                                    try {
                                        byte[] buffer = new byte[Commands.SIZE];
                                        DatagramPacket received = new DatagramPacket(buffer, buffer.length);
                                        socket.receive(received);
                                        commands.manageCommand(buffer);
                                        writeOnTerminal("Received: " + received);
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }
                                }
                                socket.close();
                            } catch (Exception e) {
                                e.printStackTrace();
                                connected = false;
                                spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected)
                                        .apply();
                            }
                        }
                    }).start();

                    while (connected && !socket.isClosed()) {
                        // Sending messages
                        // If there is any message to send
                        //queueTextView.setText("In lista: " + sendBuffer.size());
                        if (!sendBuffer.isEmpty()) {
                            byte[] message = sendBuffer.get(0);
                            if (message != null) {
                                DatagramPacket packet = new DatagramPacket(message, message.length, addressReceiver, portReceiver);
                                socket.send(packet);
                                sendBuffer.remove(0);

                                updateQueueTextView();
                                Thread.sleep(COMMAND_DELAY);
                            }
                        }
                    }

                    socket.close();
                    connected = false;
                    spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected)
                            .apply();
                } catch (Exception e) {
                    e.printStackTrace();
                    connected = false;
                    spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected)
                            .apply();
                }
            }
        }).start();
    }

    private void updateSpinner() {
        try {
            songs = new JSONArray(spFile.getString(resources.getString(R.string.spFile_songs), new JSONArray().toString()));
            ArrayList<String> songList = new ArrayList<>();
            songList.add("Qualsiasi...");
            for (int i = 0; i < songs.length(); i++) {
                JSONObject song = songs.getJSONObject(i);
                songList.add(song.getString(resources.getString(R.string.json_title)));
            }
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, songList);
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            songsSpinner.setAdapter(adapter);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void updateQueueTextView() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                int x = sendBuffer.size();
                if (x == 0)
                    queueTextView.setText("Lista vuota");
                else
                    queueTextView.setText("Lista: " + x);
            }
        });
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    private void saveFile(String text) {
        String fileName = "DEBUG-" + Calendar.getInstance().getTimeInMillis() + ".txt";
        File file = new File(getApplicationContext().getExternalFilesDir(resources.getString(R.string.fileDirectory)), fileName);

        try {
            InputStream is = new ByteArrayInputStream(text.getBytes(StandardCharsets.UTF_8));
            OutputStream os = new FileOutputStream(file);
            byte[] data = new byte[is.available()];
            is.read(data);
            os.write(data);
            is.close();
            os.close();
            Toast.makeText(getApplicationContext(), "Debug file saved", Toast.LENGTH_SHORT).show();
        } catch (IOException e) {
            e.printStackTrace();
            Toast.makeText(getApplicationContext(), "A problem has occurred, no file has been saved", Toast.LENGTH_SHORT).show();
        }
    }
}
