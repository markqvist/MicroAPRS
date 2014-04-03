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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM3S1968 UART interface driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef SER_LM3S_H
#define SER_LM3S_H

#include <cfg/cfg_debug.h>
#include <cpu/power.h> /* cpu_relax() */
#include <drv/clock_lm3s.h> /* lm3s_busyWait() */
#include <io/lm3s.h>

/* Serial hardware numbers */
enum
{
	SER_UART0,
	SER_UART1,
	SER_UART2,

	SER_CNT //< Number of serial ports
};

/* Software errors */
#define SERRF_RXFIFOOVERRUN  BV(0) //< Rx FIFO buffer overrun
#define SERRF_RXTIMEOUT      BV(1) //< Receive timeout
#define SERRF_TXTIMEOUT      BV(2) //< Transmit timeout

/*
 * Hardware errors.
 */
#define SERRF_RXSROVERRUN    0 //< Input overrun
#define SERRF_FRAMEERROR     0 //< Stop bit missing
#define SERRF_PARITYERROR    0 //< Parity error
#define SERRF_NOISEERROR     0 //< Noise error

/* Serial error/status flags */
typedef uint32_t serstatus_t;

INLINE void lm3s_uartDisable(uint32_t base)
{
	/* Disable the hardware FIFO */
	HWREG(base + UART_O_LCRH) &= ~UART_LCRH_FEN;

	/* Disable the UART */
	HWREG(base + UART_O_CTL) &=
		~(UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
	lm3s_busyWait(512);
}

INLINE void lm3s_uartEnable(uint32_t base)
{
	/* Enable the hardware FIFO */
	HWREG(base + UART_O_LCRH) |= UART_LCRH_FEN;

	/* Enable RX, TX, and the UART */
	HWREG(base + UART_O_CTL) |=
			UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
	lm3s_busyWait(512);
}

/* Clear the flags register */
INLINE void lm3s_uartClear(uint32_t base)
{
	HWREG(base + UART_O_FR) = 0;
}

INLINE bool lm3s_uartTxDone(uint32_t base)
{
	return HWREG(base + UART_O_FR) & UART_FR_TXFE ? true : false;
}

INLINE bool lm3s_uartTxReady(uint32_t base)
{
	return HWREG(base + UART_O_FR) & UART_FR_TXFF ? false : true;
}

INLINE bool lm3s_uartRxReady(uint32_t base)
{
	return HWREG(base + UART_O_FR) & UART_FR_RXFE ? false : true;
}

INLINE bool lm3s_uartReady(uint32_t base)
{
	return HWREG(base + UART_O_FR) & UART_FR_BUSY ? false : true;
}

INLINE int lm3s_uartPutCharNonBlocking(uint32_t base, unsigned char c)
{
	if (!lm3s_uartTxReady(base))
		return EOF;
	HWREG(base + UART_O_DR) = c;
	return c;
}

INLINE int lm3s_uartPutChar(uint32_t base, unsigned char c)
{
	while (!lm3s_uartTxReady(base))
		cpu_relax();
	HWREG(base + UART_O_DR) = c;
	return c;
}

INLINE int lm3s_uartGetCharNonBlocking(uint32_t base)
{
	if (!lm3s_uartRxReady(base))
		return EOF;
	return HWREG(base + UART_O_DR);
}

INLINE int lm3s_uartGetChar(uint32_t base)
{
	while (!lm3s_uartRxReady(base))
		cpu_relax();
	return HWREG(base + UART_O_DR);
}

void lm3s_uartSetBaudRate(uint32_t base, unsigned long baud);
void lm3s_uartSetParity(uint32_t base, int parity);
void lm3s_uartInit(int port);

#endif /* SER_LM3S_H */
