#ifndef SERIAL_H
#define SERIAL_H

#include "device.h"

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>

typedef struct Serial {
    FILE uart0;
} Serial;

void serial_init(Serial *serial);
bool serial_available(uint8_t index);
void uart0_putchar(char c);
char uart0_getchar(void);
char uart0_getchar_nowait(void);

#endif