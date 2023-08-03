/**
 * @file SecureSerialSW.h
 * @author your name (you@domain.com)
 * @brief Classe SecureSerial che funziona per una seriale costruita con SoftwareSerial
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SecureSerialSW_H
#define SecureSerialSW_H

/* LIBRERIE */
#include "Arduino.h"
#include "RS-FEC.h"
#include <SoftwareSerial.h>

// Il pacchetto inviato è lungo MESSAGE_LENGTH + ECC_SIZE

/* COSTANTI */
#ifndef MESSAGE_LENGTH
    #define MESSAGE_LENGTH 16
#endif
#ifndef ECC_SIZE
    #define ECC_SIZE 4
#endif
#ifndef TIMEOUT_SERIAL
    #define TIMEOUT_SERIAL 10
#endif

#define PACKET_LENGTH   (MESSAGE_LENGTH + ECC_SIZE)

class SecureSerialSW
{
public:
    void begin(SoftwareSerial *serialPointer, unsigned long baudRate);
    
    // Ritorna il numero di pacchetti (e non di byte) in attesa di essere letti
    size_t available();

    // Copia il pacchetto più vecchio nell'array passato. ATTENZIONE: il pacchetto è sempre lungo MESSAGE_LENGTH
    // Ritorna il codice di errore / successo della funzione RS::Decode
    uint8_t getPacket(uint8_t *array);

    // Invia un pacchetto contentente il messaggio passato. Questo può essere lungo max MESSAGE_LENGTH
    boolean sendPacket(uint8_t *message, size_t length);

    // Da eseguire il più possibile. Mantiene la seriale pulita e funzionante in modo da non generare errori
    void handleCleaness();

private:
    SoftwareSerial *serial;
    RS::ReedSolomon<MESSAGE_LENGTH, ECC_SIZE> rs;
    uint8_t encoded[PACKET_LENGTH], original[MESSAGE_LENGTH];
    unsigned long startTimeout;
    boolean timeoutActive = false;
};

#endif
