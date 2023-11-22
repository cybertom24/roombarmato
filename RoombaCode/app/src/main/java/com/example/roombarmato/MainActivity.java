package com.example.roombarmato;

import static com.example.roombarmato.Commands.FIRE;
import static com.example.roombarmato.Commands.makeCommand;

import android.annotation.SuppressLint;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.PopupMenu;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.slider.Slider;

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
import java.util.Timer;
import java.util.TimerTask;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class MainActivity extends AppCompatActivity implements RiceviMessaggi.MessageCallback {
    int loopInt=100;
    final int delay = 500; // 1000 milliseconds == 1 second

    //CREO IL CLIENT DA FAR CONNETTERE AL ROOMBA
    TcpClient2 myTcpClient2 = new TcpClient2("192.168.4.1", 4000);
    // Objects
    Indicator rocketsIndicator;
    Button openButton;
    Button rocketBtn1;
    Button rocketBtn2;
    Button rocketBtn3;
    Button rocketBtn4;

    boolean isRck1=false;
    boolean isRck2=false;
    boolean isRck3=false;
    boolean isRck4=false;

    boolean ischargeRck1=true;
    boolean ischargeRck2=true;
    boolean ischargeRck3=true;
    boolean ischargeRck4=true;


    boolean isOpen = false;


    String msg = "Buongiorno";
    JoystickView joystick;
    TextView terminalView;
    String terminalText = "";

    int lastNotifiedProgress=0;
    EditText terminalEdit;
    Button playButton, muteButton, nextButton, previousButton, switchMainBtn, addButton, fireButton;

    Slider slider;
    Spinner songsSpinner;
    SeekBar volumeSeekBar;
    Lever angleLever;
    Button emergencyButton;
    Button menuButton;

    Button switchSongBtn;
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
        Log.d("test", "PODLWNPdvdfngkjernkg");

        spFile = getSharedPreferences(resources.getString(R.string.shared_preferences_file), MODE_PRIVATE);
        spFileEditor = spFile.edit();

        sendBuffer = new ArrayList<>();

        //riceviMessaggiTask = new RiceviMessaggi("192.168.4.1", 4000, this);



        basicConfiguration(); //qui imposto la configurazione base dei vari View e imposto i loro listener

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
                myTcpClient2.send(makeCommand(Commands.REQUEST_STATUS_ROCKETS));
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
        rocketsIndicator = findViewById(R.id.rockets_indicator);
        switchSongBtn = findViewById(R.id.btnSwitchSong);
        joystick = (JoystickView) findViewById(R.id.joystickView);
        emergencyButton = findViewById(R.id.emergency_button);
        menuButton = findViewById(R.id.menu_button);
        slider = findViewById(R.id.slider_angle);
        rocketBtn1 = findViewById(R.id.rocketButton4);
        rocketBtn2 = findViewById(R.id.rocketButton2);
        rocketBtn3 = findViewById(R.id.rocketButton1);
        rocketBtn4 = findViewById(R.id.rocketButton3);
        openButton = findViewById(R.id.open_button);
        fireButton= findViewById(R.id.fire_button);

        fireButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (isRck1 && ischargeRck1){
                    myTcpClient2.send(Commands.makeCommand(Commands.FIRE,0));
                    rocketBtn1.setBackgroundResource(R.drawable.rocket_button_usata);
                    ischargeRck1=false;
                }
                if (isRck2 && ischargeRck2){
                    myTcpClient2.send(Commands.makeCommand(Commands.FIRE,1));
                    rocketBtn2.setBackgroundResource(R.drawable.rocket_button_usata);
                    ischargeRck2=false;
                }
                if (isRck3 && ischargeRck3){
                    myTcpClient2.send(Commands.makeCommand(Commands.FIRE,2));
                    rocketBtn3.setBackgroundResource(R.drawable.rocket_button_usata);
                    ischargeRck3=false;
                }
                if (isRck4 && ischargeRck4){
                    myTcpClient2.send(Commands.makeCommand(Commands.FIRE,3));
                    rocketBtn4.setBackgroundResource(R.drawable.rocket_button_usata);
                    ischargeRck4=false;
                }


            }
        });

        openButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!isOpen) {
                    myTcpClient2.send(makeCommand(Commands.OPEN));
                    isOpen = true;
                    Log.d("TEST_OPEn", "funziona");
                } else {
                    myTcpClient2.send(makeCommand(Commands.CLOSE));
                    isOpen = false;
                    Log.d("TEST_OPEn", "funziona");

                }
            }
        });
        final Handler handler = new Handler();

        handler.postDelayed(new Runnable() {
            public void run() {
                if (joystick.getX()==0 && joystick.getY()==0 ) {myTcpClient2.send(makeCommand(Commands.MOVE, 1, 1));}
                handler.postDelayed(this, delay);
            }
        }, delay);


        rocketBtn1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                isRck1=false;
                isRck2=false;
                isRck3=false;
                isRck4=false;
                isRck1=true;
                if (ischargeRck2) rocketBtn2.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck3) rocketBtn3.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck4) rocketBtn4.setBackgroundResource(R.drawable.rockt_button_libero);


                if (ischargeRck1) rocketBtn1.setBackgroundResource(R.drawable.rocket_selezionato);
            }
        });
        rocketBtn2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                isRck1=false;
                isRck2=false;
                isRck3=false;
                isRck4=false;
                isRck2=true;

                if (ischargeRck1) rocketBtn1.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck3) rocketBtn3.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck4) rocketBtn4.setBackgroundResource(R.drawable.rockt_button_libero);

                if (ischargeRck2) rocketBtn2.setBackgroundResource(R.drawable.rocket_selezionato);
            }
        });
        rocketBtn3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                isRck1=false;
                isRck2=false;
                isRck3=false;
                isRck4=false;
                isRck3=true;

                if (ischargeRck2) rocketBtn2.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck1) rocketBtn1.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck4) rocketBtn4.setBackgroundResource(R.drawable.rockt_button_libero);

                if (ischargeRck3) rocketBtn3.setBackgroundResource(R.drawable.rocket_selezionato);
            }
        });
        rocketBtn4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                isRck1=false;
                isRck2=false;
                isRck3=false;
                isRck4=false;
                isRck4=true;

                if (ischargeRck2) rocketBtn2.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck3) rocketBtn3.setBackgroundResource(R.drawable.rockt_button_libero);
                if (ischargeRck1) rocketBtn1.setBackgroundResource(R.drawable.rockt_button_libero);

                if (ischargeRck4) rocketBtn4.setBackgroundResource(R.drawable.rocket_selezionato);
            }
        });




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

                            case R.id.menu_connected://Connesso []
                                if (connected) {
                                    connected = false;
                                    spFileEditor.putBoolean(resources.getString(R.string.spFile_connected), connected);
                                    spFileEditor.apply();
                                } else startUdp();
                                break;
                            case R.id.menu_connectivity://Connessione
                                //new ConnectivityDialog().show(getSupportFragmentManager(), ConnectivityDialog.TAG);


                                myTcpClient2.connect();
                                //riceviMessaggiTask.execute();
                                //myTcpClient2.send("RIPROVA COL 2".getBytes());

                                break;

                                    /*) case R.id.menu_reload:
                                        send(makeCommand(Commands.REQUEST_STATUS_ROCKETS));
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                updateSpinner();
                                            }
                                        });
                                        break;*/
                            case R.id.menu_reload:
                                rocketsIndicator.offPaint.setColor(Color.GREEN);
                                Toast.makeText(getApplicationContext(), "Ciaooo", Toast.LENGTH_LONG).show();
                                rocketBtn1.setBackgroundResource(R.drawable.rockt_button_libero);
                                rocketBtn2.setBackgroundResource(R.drawable.rockt_button_libero);
                                rocketBtn3.setBackgroundResource(R.drawable.rockt_button_libero);
                                rocketBtn4.setBackgroundResource(R.drawable.rockt_button_libero);
                                 ischargeRck1=true;
                                 ischargeRck2=true;
                                 ischargeRck3=true;
                                 ischargeRck4=true;




                                break;
                                    /*case R.id.menu_orientation:
                                        new OrientationDialog().show(getSupportFragmentManager(), OrientationDialog.TAG);
                                        break;*/
                                    /*case R.id.menu_clear:
                                        clearTerminal();
                                        break;*/
                            case R.id.menu_information://informazioni

                                Toast.makeText(MainActivity.this, "PROVA", Toast.LENGTH_SHORT).show();
                                myTcpClient2.send(Commands.makeCommand(Commands.CHECK_CONNECTION));
                                break;
                                    /*case R.id.menu_download:
                                        // Create the txt file and save it
                                        saveFile("" + terminalView.getText());
                                        break;*/
                                    /*case R.id.menu_songs://canzoni( da rimuovere e spostare nell'altra layut)
                                        new SongsDialog().show(getSupportFragmentManager(), SongsDialog.TAG);
                                        break;*/
                        }
                        return true;
                    }
                });

                popupMenu.show();
            }
        });


        switchSongBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switchToMusicActivity();
            }
        });


        emergencyButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myTcpClient2.send(makeCommand(Commands.MOVE, 1, 1));
            }
        });

        joystick.setOnMoveListener(new JoystickView.OnMoveListener() {
                                       @Override
                                       public void onMove(int angle, int strength) {

                                           joystick(angle, strength);
                                       }
                                   },loopInt
        );

        


        slider.addOnChangeListener(new Slider.OnChangeListener() {
            @SuppressLint("RestrictedApi")
            @Override
            public void onValueChange(@NonNull Slider slider, float _value, boolean fromUser) {
                int value = (int) _value;
                System.out.println("" + value);
            }
        });
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

    /*public  void send(byte[] message, boolean important) {
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

    private  void send(byte[] message) {
        send(message, false);
    }
*/
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

    /**
     * DA MODIFICARE
     **/

    private void updateSpinner() {
        try {
            songs = new JSONArray(spFile.getString(resources.getString(R.string.spFile_songs), new JSONArray().toString()));
            ArrayList<String> songList = new ArrayList<>();
            songList.add("CANZONI...");
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









    /*
     * Serve per controllare la schemata della musica
     * */

    public void switchToMusicActivity() {
        setContentView(R.layout.activity_song_control);
        playButton = findViewById(R.id.play_button);
        muteButton = findViewById(R.id.mute_button);
        songsSpinner = (Spinner) findViewById(R.id.songs_spinner);
        volumeSeekBar = findViewById(R.id.volume_seekBar);
        nextButton = findViewById(R.id.next_button);
        previousButton = findViewById(R.id.previous_button);
        switchMainBtn = (Button) findViewById(R.id.switchMainBtn);
        addButton = findViewById(R.id.addButton);

        updateSpinner();


        playButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playing = !playing;
                if (playing) {
                    int selection = songsSpinner.getSelectedItemPosition();
                    myTcpClient2.send(makeCommand(Commands.SET_VOLUME, volume));
                    if (selection == 0) myTcpClient2.send(makeCommand(Commands.PLAY));
                    else myTcpClient2.send(makeCommand(Commands.PLAY_INDEX, selection - 1));

                    playButton.setText("PAUSE");
                } else {
                    myTcpClient2.send(makeCommand(Commands.PAUSE));
                    playButton.setText("PLAY");
                }
            }
        });

        muteButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                muted = !muted;
                if (muted) {
                    myTcpClient2.send(makeCommand(Commands.SET_VOLUME, 0));
                    muteButton.setText("UNMUTE");
                    myTcpClient2.send("TASTO PREMUTO".getBytes());
                } else {
                    myTcpClient2.send(makeCommand(Commands.SET_VOLUME, volume));
                    muteButton.setText("MUTE");
                }
            }
        });

        nextButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myTcpClient2.send(makeCommand(Commands.NEXT_SONG));
            }
        });

        previousButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myTcpClient2.send(makeCommand(Commands.PREVIOUS_SONG));
            }
        });

        volumeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                if (Math.abs(progress - lastNotifiedProgress) >= 10) {
                    int newVolume = progress / 5;
                    newVolume *= 5;
                    if (newVolume != volume) {
                        volume = newVolume;
                        myTcpClient2.send(makeCommand(Commands.SET_VOLUME, volume));
                    }
                    lastNotifiedProgress = progress;
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

        switchMainBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                setContentView(R.layout.activity_main);
                basicConfiguration();
            }
        });

        addButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new SongsDialog().show(getSupportFragmentManager(), SongsDialog.TAG);
            }
        });


    }


    public void joystick(int oldangle, int strenght) {
        double angle= (double) (oldangle* (Math.PI/180));
        float val_x = (float) (Math.cos((double) angle) * 100);
        float val_y = (float) (Math.sin((double) angle) * 100);
        float val_tot = (float) (Math.sqrt(Math.pow(val_x, 2) + Math.pow(val_y, 2)));

        if(strenght==0){
            val_x=0;
            val_y=0;
        }

        float potenzaDx = calcolaPotenze(val_x, val_y, strenght)[0];
        float potenzaSx = calcolaPotenze(val_x, val_y, strenght)[1];

        myTcpClient2.send(costruisciComando(potenzaDx, potenzaSx));
        Log.d("Joystick", "PotenzaDx: " + String.valueOf(potenzaDx) + " PotenzaSx: " + String.valueOf(potenzaSx));


    }


    /*
     * Spiegazione funzioni utilizzate:
     *
     * joystick:
     * .percentualeX() restituisce la posizione x della manopola come float che va da -100 a 100 dove 0 è al centro
     * .percentualeY() come per la x ma con la coordinata y della manopola
     * .percentualeTot() resitiuisce la distanza dal centro della manopola in percentuale (pitagora sulle coordinate x e y e poi divido per il raggio massimo). Il valore va da 0 a 100
     *
     */


    public float[] calcolaPotenze(float percentualeX, float percentualeY, float percentualeTot) {
        float potenzaDx = 0;
        float potenzaSx = 0;

        // Se la manopola è puntata verso l'avanti
        if (percentualeY >= 0) {
            if (percentualeX < 0)
                potenzaDx = 1;
            else {
                // percentualeX va da 0 a 1 (dividendolo per 100) e lo devo fare andare da 1 a -1
                potenzaDx = percentualeX / 100f;
                potenzaDx *= -2;     // Inverto il senso e aumento il range
                potenzaDx += 1;      // Sommo l'offset
            }
            if (percentualeX > 0)
                potenzaSx = 1;
            else {
                potenzaSx = percentualeX / 100f;
                potenzaSx *= 2;      // Aumento il range
                potenzaSx += 1;      // Sommo l'offset
            }
        }
        // Nel caso in cui la manopola fosse sotto lo zero (puntata verso il dietro) gestisco i due motori in modo inverso
        else {
            // Invertendo la potenza (e quindi la direzione)
            percentualeTot *= -1;
            if (percentualeX < 0)
                potenzaSx = 1;
            else {
                potenzaSx = percentualeX / 100f;
                potenzaSx *= -2;
                potenzaSx += 1;
            }
            if (percentualeX > 0)
                potenzaDx = 1;
            else {
                potenzaDx = percentualeX / 100f;
                potenzaDx *= 2;
                potenzaDx += 1;
            }
        }
        // potenzaDx e potenzaSx vanno da -1 a 1 e li faccio andare da -100 a 100 in base a quanto dista
        // la manopola dal centro del joystick
        potenzaDx *= percentualeTot;
        potenzaSx *= percentualeTot;

        return new float[]{potenzaDx, potenzaSx};
    }

    public byte[] costruisciComando(float potenzaDx, float potenzaSx) {
        // Conversione dei dati
        /* In data[0] è contenuta la potenza da dare al motore DX
         * In data[1] quella per il motore SX
         * Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
         * mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
         */

        // Recupero la direzione (vero = avanti, falso = indietro)
        boolean direzioneDx = potenzaDx >= 0;
        boolean direzioneSx = potenzaSx >= 0;
        // Espando il range da [0,100] (togliendo il segno) a [0,255]
        potenzaDx = (float) Math.abs(potenzaDx) * 2.55f;
        if (potenzaDx > 255)
            potenzaDx = 255;

        potenzaSx = (float) Math.abs(potenzaSx) * 2.55f;
        if (potenzaSx > 255)
            potenzaSx = 255;
        // Converto il dato in byte che devo inviare
        byte datoDx = (byte) potenzaDx;
        byte datoSx = (byte) potenzaSx;
        // Sovrascrivo la direzione sul LSB del dato da inviare
        if (direzioneDx)
            datoDx = (byte) (datoDx | 0x01);
        else
            datoDx = (byte) (datoDx & 0xFE);

        if (direzioneSx)
            datoSx = (byte) (datoSx | 0x01);
        else
            datoSx = (byte) (datoSx & 0xFE);

        return makeCommand(Commands.MOVE, datoDx, datoSx);
    }

    public static int checkTrueBoolean(boolean bool1, boolean bool2, boolean bool3, boolean bool4) {
        if (bool1) {
            return 1;
        } else if (bool2) {
            return 2;
        } else if (bool3) {
            return 3;
        } else if (bool4) {
            return 4;
        } else {
            // Nel caso nessuna variabile sia vera, è possibile gestire questo caso come necessario.
            // In questo esempio, restituiamo 0.
            return 0;
        }
    }

    @Override
    public void onMessageReceived(String message) {
        Log.d("MESSAGGIO ROOMBAAA" ,message);
    }


// IMPORTANTE: ricordarsi della questione dei messaggi importanti!!!
// Serve se abbiamo il problema dell'accumulo di messaggi
// Vedere prima se si riesce a risolvere
// Se i motori si devono fermare (pot = 00000001, in caso di "0" la direzione è preimpostata in avanti)
// segna il messaggio come importante (qualsiasi altro messaggio in lista viene eliminato e viene inviato solo questo)
}


