#ifndef Command_H
#define Command_H
// Librerie
#include "arduino.h"

class Command {
  public:
    static const byte SIZE = 0x06;
    
    Command(byte builder[SIZE]);
    Command(byte builder[SIZE], boolean makingCommand);
    
    byte _command;
    byte _size;
    byte _data[2];
    boolean isRight = false;
    byte _buffer[6];
    
    static const Command makeCommand(byte command);
    static const Command makeCommand(byte command, byte data0);
    static const Command makeCommand(byte command, byte data0, byte data1);
    // Lista comandi:
    // ATTENZIONE: DA DISPORRE IN ORDINE PER IMPORTANZA (vedi constructor)
    static const byte START_BYTE = 0x69;
    static const byte END_BYTE = 0xFA;
    // MP3
    static const byte PLAY = 0x00;
    static const byte PLAY_INDEX = 0x01;
    static const byte PAUSE = 0x02;
    static const byte VOLUME_UP = 0x03;
    static const byte VOLUME_DOWN = 0x04;
    static const byte UN_MUTE = 0x05;
    static const byte SET_VOLUME = 0x06;
    static const byte REQUEST_SONGS = 0x07;
    static const byte SONGS = 0x08;
    static const byte NEXT_SONG = 0x12;
    static const byte PREVIOUS_SONG = 0x13;
    // Movement
    static const byte MOVE = 0x09;
    // Combact
    static const byte REQUEST_STATUS_ROCKETS = 0x0A;
    static const byte STATUS_ROCKETS = 0x0B;
    static const byte FIRE = 0x0C;
    // Connection
    static const byte CONNECTING = 0x0D;
    static const byte CONNECTED = 0x0E;
    static const byte REQUEST_ADDRESS = 0x0F;
    static const byte YOUR_ADDRESS = 0x10;
    // ESP
    static const byte ESP_READY = 0x11;
    // Utility
    static const byte NOT_FOUND = 0xFF;
};
#endif
