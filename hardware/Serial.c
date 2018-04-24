#include "Serial.h"
#include <util/setbaud.h>
#include <stdio.h>
#include <string.h>

void serial_init(Serial *serial) {
    memset(serial, 0, sizeof(*serial));
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X
        UCSR0A |= _BV(U2X0);
    #else
        UCSR0A &= ~(_BV(U2X0));
    #endif

    // Set to 8-bit data, enable RX and TX
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    FILE uart0_fd = FDEV_SETUP_STREAM(uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);
    //FILE uart0_fd = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);

    serial->uart0 = uart0_fd;
}

bool serial_available(uint8_t index) {
    if (index == 0) {
        if (UCSR0A & _BV(RXC0)) return true;
    }
    return false;
}


int uart0_putchar(char c, FILE *stream) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 1;
}

int uart0_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

char uart0_getchar_nowait(void) {
    if (!(UCSR0A & _BV(RXC0))) return EOF;
    return UDR0;
}