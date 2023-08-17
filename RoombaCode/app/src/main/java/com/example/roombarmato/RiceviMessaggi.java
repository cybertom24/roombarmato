package com.example.roombarmato;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class RiceviMessaggi extends AsyncTask<Void, String, Void> {

    private static final String TAG = "ClientRiceviMessaggi";
    private String serverAddress;
    private int serverPort;
    private MessageCallback messageCallback;

    public interface MessageCallback {
        void onMessageReceived(String message);
    }



    public RiceviMessaggi(String serverAddress, int serverPort, MessageCallback messageCallback) {
        this.serverAddress = serverAddress;
        this.serverPort = serverPort;
        this.messageCallback = messageCallback;
    }

    @Override
    protected Void doInBackground(Void... params) {
        try {
            Socket socket = new Socket(serverAddress, serverPort);

            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            String message;
            while ((message = in.readLine()) != null) {
                publishProgress(message);
            }

            socket.close();
        } catch (IOException e) {
            Log.e(TAG, "Errore durante la connessione o la ricezione dei messaggi", e);
        }

        return null;
    }

    @Override
    protected void onProgressUpdate(String... values) {
        super.onProgressUpdate(values);
        if (messageCallback != null) {
            messageCallback.onMessageReceived(values[0]);
        }
    }
}
