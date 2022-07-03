#include <stdio.h>

int main() {
    
    unsigned char data = 0xFF;
    printf("\n0) Data: 0x%02x", data);
    if(data & 0x01) 
        printf("\nAvanti");
    else
        printf("\nIndietro");
    printf("\n1) Data: 0x%02x", data);

    printf("\nData: 0x%02x", (data & 0xFE));
    
    return 0;
}