#include "Command.h"

Command::Command(byte builder[COMMAND_SIZE]) {
  for ( byte i = 0; i < COMMAND_SIZE; i++) {
    buffer[i] = builder[i];
  }

  isRight = builder[0] == COMMAND_START_BYTE && builder[5] == COMMAND_END_BYTE;

  if (isRight) {
    code = builder[1];
    size = builder[2];
    for (byte i = 0; i < size; i++) {
      data[i] = builder[i + 3];
    }
  }
}

Command::Command(byte builder[COMMAND_SIZE], boolean makingCommand) {
  if (makingCommand) {
    for ( byte i = 0; i < COMMAND_SIZE; i++) {
      buffer[i] = builder[i];
    }
  } else {
    for ( byte i = 0; i < COMMAND_SIZE; i++) {
      buffer[i] = builder[i];
    }

    isRight = builder[0] == COMMAND_START_BYTE && builder[5] == COMMAND_END_BYTE;

    if (isRight) {
      code = builder[1];
      size = builder[2];
      for (byte i = 0; i < size; i++) {
        data[i] = builder[i + 3];
      }
    }
  }
}

static Command Command::makeCommand(byte code) {
  byte builder[6] = {COMMAND_START_BYTE, code, 0x00, 0x00, 0x00, COMMAND_END_BYTE};
  Command c(builder, true);
  return c;
}

static Command Command::makeCommand(byte code, byte data0) {
  byte builder[6] = {COMMAND_START_BYTE, code, 0x01, data0, 0x00, COMMAND_END_BYTE};
  Command c(builder, true);
  return c;
}

static Command Command::makeCommand(byte code, byte data0, byte data1) {
  byte builder[6] = {COMMAND_START_BYTE, code, 0x02, data0, data1, COMMAND_END_BYTE};
  Command c(builder, true);
  return c;
}
