#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;

#define PORT        "/dev/ttyACM0"
#define BUFFER_SIZE 8

/*
 * ! ATTENZIONE !
 * Il programma è stato strutturato per UNIX e quindi non funziona su WIN
*/

using namespace std;

int main(int argc, char **argv)
{
    printf("--- Serial Debugger ---\n");

    // Apri la connessione con arduino
    // Visto che siamo su linux la porta è interpretata come file da cui posso leggere e scrivere
    fstream serial(PORT);

    while(1)
    {
        // Loop
        // Controlla che ci siano dati disponibili
        serial.peek();
        // Se non ci sono dati disponibili da leggere viene posta a 1 la eofbit flag
        if(serial.eof())
            // In caso fosse settata skippa al prossimo ciclo
            continue;
        
        // Peek non è fallito -> ci sono dati da leggere
        char buffer[BUFFER_SIZE] = { 0 };
        serial.read(buffer, BUFFER_SIZE);

        printf("> Arrivato: ");
        for(int i = 0; i < BUFFER_SIZE; i++)
        {
            printf("%02x ", buffer[i]);
        }
        printf("\n");

    };

    serial.close();

    return 0;
}