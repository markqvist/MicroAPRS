#include <stdbool.h>
#include <avr/io.h>

#include "device.h"
#include "util/FIFO.h"
#include "hardware/AFSK.h"
#include "hardware/Serial.h"
#include "protocol/AX25.h"
#include "protocol/KISS.h"

#define FEND 0xC0
#define FESC 0xDB
#define TFEND 0xDC
#define TFESC 0xDD

Serial serial;
Afsk modem;
AX25Ctx AX25;

static void ax25_callback(struct AX25Ctx *ctx) {
    kiss_messageCallback(ctx);
}

void init(void) {
    AFSK_init(&modem);
    serial_init(&serial);
    ax25_init(&AX25, &modem.fd, ax25_callback);
    kiss_init(&AX25, &modem, &serial);

    stdout = &serial.uart0;
    stdin  = &serial.uart0;
}

int main (void) {
    init();

    while (true) {
        ax25_poll(&AX25);
        
        if (serial_available(0)) {
            char sbyte = uart0_getchar_nowait();
            kiss_serialCallback(sbyte);
        }
    
    }

    return(0);
}