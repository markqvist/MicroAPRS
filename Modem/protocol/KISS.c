#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <drv/timer.h>      // Timer driver from BertOS
#include "KISS.h"

static uint8_t serialBuffer[CONFIG_AX25_FRAME_BUF_LEN+1]; // Buffer for holding incoming serial data
AX25Ctx *ax25ctx;
Serial *serial;
size_t frame_len;
bool IN_FRAME;
bool ESCAPE;
uint8_t command = CMD_UNKNOWN;
unsigned long kiss_preamble = CONFIG_AFSK_PREAMBLE_LEN;
unsigned long kiss_tail = CONFIG_AFSK_TRAILER_LEN;

void kiss_init(AX25Ctx *ax25, Serial *ser) {
    ax25ctx = ax25;
    serial = ser;
}

void kiss_messageCallback(AX25Ctx *ctx) {
    kfile_putc(FEND, &serial->fd);
    kfile_putc(0x00, &serial->fd);
    for (unsigned i = 0; i < ctx->frm_len; i++) {
        uint8_t b = ctx->buf[i];
        if (b == FEND) {
            kfile_putc(FESC, &serial->fd);
            kfile_putc(TFEND, &serial->fd);
        } else if (b == FESC) {
            kfile_putc(FESC, &serial->fd);
            kfile_putc(TFESC, &serial->fd);
        } else {
            kfile_putc(b, &serial->fd);
        }
    }
    kfile_putc(FEND, &serial->fd);
}

void fon(void) {
    long ts = 300000;
    while (ts--) PORTB |= BV(2);
}


void kiss_serialCallback(uint8_t sbyte) {
    if (IN_FRAME && sbyte == FEND && command == CMD_DATA) {
        IN_FRAME = false;
        ax25_sendRaw(ax25ctx, serialBuffer, frame_len);
    } else if (sbyte == FEND) {
        IN_FRAME = true;
        command = CMD_UNKNOWN;
        frame_len = 0;
    } else if (IN_FRAME && frame_len < CONFIG_AX25_FRAME_BUF_LEN) {
        // Have a look at the command byte first
        if (frame_len == 0 && command == CMD_UNKNOWN) {
            // MicroModem supports only one HDLC port, so we
            // strip off the port nibble of the command byte
            sbyte = sbyte & 0x0F;
            command = sbyte;
        } else if (command == CMD_DATA) {
            if (sbyte == FESC) {
                ESCAPE = true;
            } else {
                if (ESCAPE) {
                    if (sbyte == TFEND) sbyte = FEND;
                    if (sbyte == TFESC) sbyte = FESC;
                    ESCAPE = false;
                }
                serialBuffer[frame_len++] = sbyte;
            }
        } else if (command == CMD_TXDELAY) {
            kiss_preamble = sbyte * 10UL;
        } else if (command == CMD_TXTAIL) {
            kiss_tail = sbyte * 10;
        } 
        
    }
}