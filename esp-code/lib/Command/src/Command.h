#ifndef Command_H
#define Command_H

/* LIBRERIE */
#include "Arduino.h"

#define COMMAND_SIZE 0x06

/* COMANDI */
#define COMMAND_START_BYTE 0x69
#define COMMAND_END_BYTE 0xFA
// MP3
#define CODE_PLAY 0x00
#define CODE_PLAY_INDEX 0x01
#define CODE_PAUSE 0x02
#define CODE_VOLUME_UP 0x03
#define CODE_VOLUME_DOWN 0x04
#define CODE_UN_MUTE 0x05
#define CODE_SET_VOLUME 0x06
#define CODE_REQUEST_SONGS 0x07
#define CODE_SONGS 0x08
#define CODE_NEXT_SONG 0x12
#define CODE_PREVIOUS_SONG 0x13
// Movement
#define CODE_MOVE 0x09
// Combact
#define CODE_REQUEST_STATUS_ROCKETS 0x0A
#define CODE_STATUS_ROCKETS 0x0B
#define CODE_FIRE 0x0C
// Connection
#define CODE_CONNECTING 0x0D
#define CODE_CONNECTED 0x0E
#define CODE_REQUEST_ADDRESS 0x0F
#define CODE_YOUR_ADDRESS 0x10
// ESP
#define CODE_ESP_READY 0x11
// Utility
#define CODE_NOT_FOUND 0xFF

class Command
{
public:
    Command(byte builder[COMMAND_SIZE])
    {
        for (byte i = 0; i < COMMAND_SIZE; i++)
        {
            buffer[i] = builder[i];
        }

        isRight = builder[0] == COMMAND_START_BYTE && builder[5] == COMMAND_END_BYTE;

        if (isRight)
        {
            code = builder[1];
            size = builder[2];
            for (byte i = 0; i < size; i++)
            {
                data[i] = builder[i + 3];
            }
        }
    };

    Command(byte builder[COMMAND_SIZE], boolean makingCommand)
    {
        if (makingCommand)
        {
            for (byte i = 0; i < COMMAND_SIZE; i++)
            {
                buffer[i] = builder[i];
            }
        }
        else
        {
            for (byte i = 0; i < COMMAND_SIZE; i++)
            {
                buffer[i] = builder[i];
            }

            isRight = builder[0] == COMMAND_START_BYTE && builder[5] == COMMAND_END_BYTE;

            if (isRight)
            {
                code = builder[1];
                size = builder[2];
                for (byte i = 0; i < size; i++)
                {
                    data[i] = builder[i + 3];
                }
            }
        }
    };

    static Command makeCommand(byte code)
    {
        byte builder[6] = {COMMAND_START_BYTE, code, 0x00, 0x00, 0x00, COMMAND_END_BYTE};
        Command c(builder, true);
        return c;
    };
    static Command makeCommand(byte code, byte data0)
    {
        byte builder[6] = {COMMAND_START_BYTE, code, 0x01, data0, 0x00, COMMAND_END_BYTE};
        Command c(builder, true);
        return c;
    };
    static Command makeCommand(byte code, byte data0, byte data1)
    {
        byte builder[6] = {COMMAND_START_BYTE, code, 0x02, data0, data1, COMMAND_END_BYTE};
        Command c(builder, true);
        return c;
    };

    byte code;
    byte size;
    byte data[2];
    boolean isRight;
    byte buffer[6];
};
#endif
