package com.example.roombarmato;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
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

            InputStream in = socket.getInputStream();;

            byte[] buffer = new byte[1024];

            int bytesRead;
            while ((bytesRead = in.read(buffer)) != -1)  {
                Log.d("Messaggio da roombaaaa", String.valueOf(bytesRead));
            }

            socket.close();
        } catch (IOException e) {
            Log.e(TAG, "Errore durante la connessione", e);
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
