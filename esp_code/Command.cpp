#include "Command.h"

Command::Command(byte builder[SIZE]) {
  for ( byte i = 0; i < SIZE; i++) {
    _buffer[i] = builder[i];
  }

  isRight = builder[0] == START_BYTE && builder[5] == END_BYTE;

  if (isRight) {
    _command = builder[1];
    _size = builder[2];
    for (byte i = 0; i < _size; i++) {
      _data[i] = builder[i + 3];
    }
  }
}

Command::Command(byte builder[SIZE], boolean makingCommand) {
  if (makingCommand) {
    for ( byte i = 0; i < SIZE; i++) {
      _buffer[i] = builder[i];
    }
  } else {
    for ( byte i = 0; i < SIZE; i++) {
      _buffer[i] = builder[i];
    }

    isRight = builder[0] == START_BYTE && builder[5] == END_BYTE;

    if (isRight) {
      _command = builder[1];
      _size = builder[2];
      for (byte i = 0; i < _size; i++) {
        _data[i] = builder[i + 3];
      }
    }
  }
}

const Command Command::makeCommand(byte command) {
  byte builder[6] = {START_BYTE, command, 0x00, 0x00, 0x00, END_BYTE};
  Command c(builder, true);
  return c;
}

const Command Command::makeCommand(byte command, byte data0) {
  byte builder[6] = {START_BYTE, command, 0x01, data0, 0x00, END_BYTE};
  Command c(builder, true);
  return c;
}

const Command Command::makeCommand(byte command, byte data0, byte data1) {
  byte builder[6] = {START_BYTE, command, 0x02, data0, data1, END_BYTE};
  Command c(builder, true);
  return c;
}
