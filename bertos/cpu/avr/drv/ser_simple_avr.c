/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Simple serial I/O driver
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#warning FIXME:This module is obsolete, yuo must refactor it.

#if 0
#include "ser_simple_avr.h"

#include <cfg/compiler.h>
#include <appconfig.h>
#include <cfg/macros.h> /* BV() */
#include <hw/hw_cpufreq.h>

#include <avr/io.h>

/**
 * Send a character over the serial line.
 *
 * \return the character sent.
 */
int _ser_putchar(int c)
{
	/* Disable Rx to avoid echo*/
	UCSR0B &= ~BV(RXEN);
	/* Enable tx*/
	UCSR0B |= BV(TXEN);
	/* Prepare transmission */
	UDR0 = c;
	/* Wait until byte sent */
	while (!(UCSR0A & BV(TXC))) {}
	/* Disable tx to avoid short circuit when tx and rx share the same wire. */
	UCSR0B &= ~BV(TXEN);
	/* Enable Rx */
	UCSR0B |= BV(RXEN);
	/* Delete TRANSMIT_COMPLETE_BIT flag */
	UCSR0A |= BV(TXC);
	return c;
}


/**
 * Get a character from the serial line.
 * If ther is no character in the buffer this function wait until
 * one is received (no timeout).
 *
 * \return the character received.
 */
int _ser_getchar(void)
{
	/* Wait for data */
	while (!(UCSR0A & BV(RXC))) {}
	return UDR0;

}


/**
 * Get a character from the receiver buffer
 * If the buffer is empty, ser_getchar_nowait() returns
 * immediatly EOF.
 */
int _ser_getchar_nowait(void)
{
	if (!(UCSR0A & BV(RXC))) return EOF;
	else return UDR0;
}

void _ser_settimeouts(void)
{
}

/**
 * Set the baudrate.
 */
void _ser_setbaudrate(unsigned long rate)
{
	/* Compute baud-rate period */
	uint16_t period = DIV_ROUND(CPU_FREQ / 16UL, rate) - 1;

	UBRR0H = (period) >> 8;
	UBRR0L = (period);
}

/**
 * Send a string.
 */
int _ser_print(const char *s)
{
	while(*s) _ser_putchar(*s++);
	return 0;
}


void _ser_setparity(int parity)
{
	/* Set the new parity */
	UCSR0C |= (UCSR0C & ~(BV(UPM1) | BV(UPM0))) | (parity << UPM0);	
}

/**
 * Dummy functions.
 */
void _ser_purge(void)
{
	while (_ser_getchar_nowait() != EOF) {}
}

/**
 * Initialize serial.
 */
struct Serial * _ser_open(void)
{
	/*
	 * Set Rx and Tx pins as input to avoid short
	 * circuit when serial is disabled.
	 */
	DDRE  &= ~(BV(PE0)|BV(PE1));
	PORTE &= ~BV(PE0);
	PORTE |=  BV(PE1);
	/* Enable only Rx section */
	UCSR0B = BV(RXEN);
	return NULL;
}


/**
 * Clean up serial port, disabling the associated hardware.
 */
void _ser_close(void)
{
	/* Disable Rx & Tx. */
	UCSR0B &= ~(BV(RXEN) | BV(TXEN));
}

#endif

