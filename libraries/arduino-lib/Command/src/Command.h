#ifndef Command_H
#define Command_H

/* LIBRERIE */
#include "Arduino.h"

#define COMMAND_SIZE     0x06
#define INDEX_CODE       0x01
#define INDEX_SIZE       0x02
#define INDEX_START_DATA 0x03

/* COMANDI */
#define COMMAND_START_BYTE 0x69
#define COMMAND_END_BYTE   0xFA
// MP3
#define CODE_PLAY             0x00
#define CODE_PLAY_INDEX       0x01
#define CODE_PAUSE            0x02
#define CODE_VOLUME_UP        0x03
#define CODE_VOLUME_DOWN      0x04
#define CODE_TOGGLE_MUTE      0x05
#define CODE_SET_VOLUME       0x06
#define CODE_NEXT_SONG        0x07
#define CODE_PREVIOUS_SONG    0x08
// Micellaneous
#define CODE_EYES             0x0D
// Movement
#define CODE_MOVE             0x09
#define CODE_OPEN_LID         0x0A
#define CODE_CLOSE_LID        0x0B
// Combact
#define CODE_FIRE             0x0C
// Checks
#define CODE_CHECK_CONNECTION 0x16
#define CODE_CONNECTION_OK    0x17
#define CODE_CHECK_BATTERY    0x0F
#define CODE_BATTERY_STATUS   0x10
#define CODE_CHECK_LID        0x11
#define CODE_LID_STATUS       0x12
// ESP
#define CODE_SYNC             0x18
#define CODE_CONNECTED        0x14
#define CODE_DISCONNECTED     0x15
// Errors
#define CODE_LAUNCH_FAILURE   0xC8
#define CODE_BATTERY_LOW      0xC9
#define CODE_HOLDER_FULL      0xCA
#define CODE_WRONG_PACKET     0xCB
// Utility
#define CODE_NOT_FOUND        0xFF


class Command
{
public:
    Command(uint8_t builder[COMMAND_SIZE])
    {
        memcpy(buffer, builder, COMMAND_SIZE);
    };

    uint8_t code()
    {
        return buffer[INDEX_CODE];
    };

    uint8_t size()
    {
        return buffer[INDEX_SIZE];
    }

    void data(uint8_t *data)
    {
        memcpy(data, &buffer[INDEX_START_DATA], size());
    }

    boolean isRight()
    {
        return isRight(buffer);
    };

    static boolean isRight(uint8_t *commandBuffer)
    {
        return commandBuffer[0] == COMMAND_START_BYTE && commandBuffer[COMMAND_SIZE - 1] == COMMAND_END_BYTE;
    };

    static Command makeCommand(uint8_t code)
    {
        uint8_t builder[COMMAND_SIZE] = {COMMAND_START_BYTE, code, 0x00, 0x00, 0x00, COMMAND_END_BYTE};
        Command c(builder);
        return c;
    };
    static Command makeCommand(uint8_t code, uint8_t data0)
    {
        uint8_t builder[COMMAND_SIZE] = {COMMAND_START_BYTE, code, 0x01, data0, 0x00, COMMAND_END_BYTE};
        Command c(builder);
        return c;
    };
    static Command makeCommand(uint8_t code, uint8_t data0, uint8_t data1)
    {
        uint8_t builder[COMMAND_SIZE] = {COMMAND_START_BYTE, code, 0x02, data0, data1, COMMAND_END_BYTE};
        Command c(builder);
        return c;
    };

    uint8_t buffer[COMMAND_SIZE];
};
#endif
