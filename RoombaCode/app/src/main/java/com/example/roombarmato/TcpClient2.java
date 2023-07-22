package com.example.roombarmato;

import android.os.AsyncTask;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class TcpClient2 {

    private String serverIp;
    private int serverPort;
    private Socket clientSocket;
    private DataOutputStream outputStream;

    public TcpClient2(String serverIp, int serverPort) {
        this.serverIp = serverIp;
        this.serverPort = serverPort;
    }

    // Metodo per stabilire la connessione con il server in un thread separato
    public void connect() {
        new ConnectTask().execute();
    }

    // Metodo per inviare un messaggio al server in un thread separato
    public void send(byte[] message) {
        new SendTask().execute(message);
    }

    // Metodo per chiudere la connessione con il server
    public void disconnect() throws IOException {
        if (clientSocket != null) {
            clientSocket.close();
        }
    }

    // AsyncTask per stabilire la connessione
    private class ConnectTask extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... voids) {
            try {
                clientSocket = new Socket(serverIp, serverPort);
                outputStream = new DataOutputStream(clientSocket.getOutputStream());
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }

    // AsyncTask per inviare messaggi
    private class SendTask extends AsyncTask<byte[], Void, Void> {
        @Override
        protected Void doInBackground(byte[]... messages) {
            try {
                if (clientSocket == null || outputStream == null) {
                    throw new IOException("La connessione con il server non è stata stabilita.");
                }

                byte[] message = messages[0];
                outputStream.write(message);
                outputStream.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }
}
