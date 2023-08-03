#include "SecureSerialHW.h"
#include "RS-FEC.h"

SecureSerialHW::SecureSerialHW() {

}

void SecureSerialHW::begin(HardwareSerial *serialPointer, unsigned long baudRate) {
    serial = serialPointer;
    serial->begin(baudRate);
}

size_t SecureSerialHW::available() {
    return serial->available() / PACKET_LENGTH;
}

uint8_t SecureSerialHW::getPacket(uint8_t *array) {
    if(available() == 0)
        return false;

    serial->readBytes(encoded, PACKET_LENGTH);
    return rs.Decode(encoded, array);
}

boolean SecureSerialHW::sendPacket(uint8_t *message, size_t length) {
    if(length > MESSAGE_LENGTH || length < 0)
        return false;

    uint8_t encoded[PACKET_LENGTH];
    if(length < MESSAGE_LENGTH) {
        memset(original, 0, PACKET_LENGTH);
        memcpy(original, message, length);
        rs.Encode(message, encoded);
    }
    else {
        rs.Encode(message, encoded);
    }
    
    return serial->write(encoded, PACKET_LENGTH) == PACKET_LENGTH;
}

void SecureSerialHW::handleCleaness() {
    if(serial->available() % PACKET_LENGTH == 0) {
        timeoutActive = false;
        return;
    }
    
    if(!timeoutActive)
    {
        timeoutActive = true;
        startTimeout = millis();
        return;
    }

    if(millis() > startTimeout + TIMEOUT_SERIAL) {
        // Ripulisci
        size_t size = serial->available();
        uint8_t garbage[size];
        serial->readBytes(garbage, size);
        // Ferma il timeout
        timeoutActive = false;
    }
}