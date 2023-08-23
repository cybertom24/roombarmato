package com.example.roombarmato;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.Arrays;

public class TcpClient2 {

    class ReceiveThread extends Thread {
        public void run() {
            while(true) {

                //leggo i dati
                byte[] received = new byte[1024];
                int bytesRead = 0;
                try {
                    bytesRead = receive(received);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }

                if(bytesRead > 0) {
                    String receivedString = new String(received,0, bytesRead);



                    // elabora dati ricevuti
                    Log.d("DA PYTHON", receivedString);



                }
            }
        }
    }


    private String serverIp;
    private int serverPort;
    private Socket clientSocket;
    private DataOutputStream outputStream;

    private byte[] buffer = new byte[1024];

    InputStream in ;


    public TcpClient2(String serverIp, int serverPort) {
        this.serverIp = serverIp;
        this.serverPort = serverPort;
    }

    // Metodo per stabilire la connessione con il server in un thread separato
    public void connect() {
        new ConnectTask().execute();

        ReceiveThread receiveThread = new ReceiveThread();
        receiveThread.start();
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
                in = clientSocket.getInputStream();
                Log.d("Interno","CONNESSOOOO");
            } catch (IOException e) {
                Log.d("IL PROBLEMA" ,e.getMessage());
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

                Log.d("MESSAGGIO DA ROOMBA ", "CIOOSODOFKWEOJKF");




                byte[] message = messages[0];
                outputStream.write(message);
                outputStream.flush();

                int bytesRead = receive(buffer);

                if(bytesRead > 0) {
                    // process received bytes
                }


            } catch (IOException e) {
                e.printStackTrace();
                Log.d("ERROREEEE",e.getMessage());
            }
            return null;
        }



    }
    public int receive(byte[] buffer) throws IOException {

        if(in != null) {
            return in.read(buffer); // read va a prendere il buffer ( che è uguale a received essendo un opuntatore?????) e lo riempie dei byte ricevuti
        }
        else return 0;

    }



}
