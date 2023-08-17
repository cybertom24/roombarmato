package com.example.roombarmato;

public class Commands {
    static final byte START_BYTE = 0x69;
    static final byte END_BYTE = (byte) 0xFA;
    static final byte SIZE = 0x06;
    static final byte OPEN = 0x0A;
    static final byte CHECK_CONNECTION = 0x16;
    static final byte CLOSE = 0x0B;
    // MP3
    static final byte PLAY = 0x00;
    static final byte PLAY_INDEX = 0x01;
    static final byte PAUSE = 0x02;
    static final byte VOLUME_UP = 0x03;
    static final byte VOLUME_DOWN = 0x04;
    static final byte UN_MUTE = 0x05;
    static final byte SET_VOLUME = 0x06;
    static final byte REQUEST_SONGS = 0x07;
    static final byte SONGS = 0x08;
    static final byte NEXT_SONG = 0x12;
    static final byte PREVIOUS_SONG = 0x13;
    // Movement
    static final byte MOVE = 0x09;
    // Fire
    static final byte FIRE = 0x0C;
    static final byte REQUEST_STATUS_ROCKETS = 0x0B;
    static final byte STATUS_ROCKETS = 0x0C;
    // Connection
    static final byte CONNECTING = 0x0D;
    static final byte CONNECTED = 0x0E;
    static final byte REQUEST_ADDRESS = 0x0F;
    static final byte YOUR_ADDRESS = 0x10;
    static final byte ESP_READY = 0x10;

    CommandsListener listener;

    public boolean manageCommand(byte[] command) {
        if(command.length == 6 && listener != null) {
            if (command[0] == START_BYTE && command[5] == END_BYTE) {
                listener.onCommand(command);
                switch(command[1]) {
                    case CONNECTED:
                        listener.onConnected();
                        break;
                    case STATUS_ROCKETS:
                        listener.onStatusRockets(command[3]);
                        break;
                }
            }
        }
        return false;
    }

    static public byte[] makeCommand(byte command) {
        return new byte[]{START_BYTE, command, 0x00, 0x00, 0x00, END_BYTE};
    }

    static public byte[] makeCommand(byte command, byte data0) {
        return new byte[]{START_BYTE, command, 0x01, data0, 0x00, END_BYTE};
    }

    static public byte[] makeCommand(byte command, byte data0, byte data1) {
        return new byte[]{START_BYTE, command, 0x02, data0, data1, END_BYTE};
    }

    static public byte[] makeCommand(byte command, int data0) {
        return new byte[]{START_BYTE, command, 0x01, (byte) data0, 0x00, END_BYTE};
    }

    static public byte[] makeCommand(byte command, int data0, int data1) {
        return new byte[]{START_BYTE, command, 0x02, (byte) data0, (byte) data1, END_BYTE};
    }

    static public byte[] makeCommand(byte command, byte data0, int data1) {
        return new byte[]{START_BYTE, command, 0x02, data0, (byte) data1, END_BYTE};
    }

    static public byte[] makeCommand(byte command, int data0, byte data1) {
        return new byte[]{START_BYTE, command, 0x02, (byte) data0, data1, END_BYTE};
    }

    public interface CommandsListener {
        public void onCommand(byte[] command);

        public void onConnected();
        public void onStatusRockets(byte data);
    }

    public void setCommandsListener(CommandsListener listener) {
        this.listener = listener;
    }
}
