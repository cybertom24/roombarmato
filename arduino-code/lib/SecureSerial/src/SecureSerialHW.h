/**
 * @file SecureSerialSW.h
 * @author your name (you@domain.com)
 * @brief Classe SecureSerial che funziona per una seriale costruita con HardwareSerial
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SecureSerialHW_H
#define SecureSerialHW_H

/* LIBRERIE */
#include "Arduino.h"
#include "RS-FEC.h"

// Il pacchetto inviato è lungo MESSAGE_LENGTH + ECC_SIZE

namespace SecureSerialHW
{
    template <const uint8_t MESSAGE_LENGTH, // Message length without correction code
              const uint8_t ECC_SIZE,
              const size_t  TIMEOUT_SERIAL>

    #define PACKET_LENGTH (MESSAGE_LENGTH + ECC_SIZE)

    class SSHW
    {
    public:
        void begin(HardwareSerial *serialPointer, unsigned long baudRate)
        {
            serial = serialPointer;
            serial->begin(baudRate);
        }

        // Ritorna il numero di pacchetti (e non di byte) in attesa di essere letti
        size_t available()
        {
            // return serial->available() / PACKET_LENGTH;
            return serial->available() / MESSAGE_LENGTH;
        }

        // Copia il pacchetto più vecchio nell'array passato. ATTENZIONE: il pacchetto è sempre lungo MESSAGE_LENGTH
        // Ritorna vero se la decodifica è andata a buon fine
        boolean getPacket(uint8_t *array)
        {
            if (available() == 0)
                return false;

            // serial->readBytes(encoded, PACKET_LENGTH);
            // return rs.Decode(encoded, array) == 0;
            serial->readBytes(array, MESSAGE_LENGTH);
            return true;
        }

        // Invia un pacchetto contentente il messaggio passato. Questo può essere lungo max MESSAGE_LENGTH
        boolean sendPacket(uint8_t *message, size_t length)
        {
            return serial->write(message, length) == length;
            
            if (length > MESSAGE_LENGTH)
                return false;

            if (length < MESSAGE_LENGTH)
            {
                memset(original, 0, PACKET_LENGTH);
                memcpy(original, message, length);
                rs.Encode(original, encoded);
            }
            else
            {
                rs.Encode(message, encoded);
            }

            return serial->write(encoded, PACKET_LENGTH) == PACKET_LENGTH;
        }

        // Da eseguire il più possibile. Mantiene la seriale pulita e funzionante in modo da non generare errori
        void handleCleaness()
        {
            // if (serial->available() % PACKET_LENGTH == 0)
            if (serial->available() % MESSAGE_LENGTH == 0)
            {
                timeoutActive = false;
                return;
            }

            if (!timeoutActive)
            {
                timeoutActive = true;
                startTimeout = millis();
                return;
            }

            if (millis() > startTimeout + TIMEOUT_SERIAL)
            {
                // Ripulisci
                size_t size = serial->available();
                uint8_t garbage[size];
                serial->readBytes(garbage, size);
                // Ferma il timeout
                timeoutActive = false;
            }
        }

    private:
        HardwareSerial *serial;
        RS::ReedSolomon<MESSAGE_LENGTH, ECC_SIZE> rs;
        uint8_t encoded[PACKET_LENGTH], original[MESSAGE_LENGTH];
        unsigned long startTimeout;
        boolean timeoutActive = false;
    };
}

using namespace std;

#endif
