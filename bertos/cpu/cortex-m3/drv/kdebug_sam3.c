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
 * \brief SAM3 debug support (implementation).
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include <cfg/cfg_debug.h>
#include <cfg/macros.h> /* for BV() */

#include <cpu/types.h>

#include <io/sam3.h>


#if (CONFIG_KDEBUG_PORT == 0)
	#define UART_BASE       UART0_BASE
	#define UART_ID         UART0_ID
	#define UART_PIO_BASE   UART0_PORT
	#define UART_PERIPH     UART0_PERIPH
	#define UART_PINS       (BV(URXD0) | BV(UTXD0))
#elif (CONFIG_KDEBUG_PORT == 1) && UART_PORTS > 1
	#define UART_BASE       UART1_BASE
	#define UART_ID         UART1_ID
	#define UART_PIO_BASE   UART1_PORT
	#define UART_PERIPH     UART1_PERIPH
	#define UART_PINS       (BV(URXD1) | BV(UTXD1))
#else
	#error "UART port not supported in this board"
#endif

// TODO: refactor serial simple functions and use them, see lm3s kdebug
#define KDBG_WAIT_READY()     while (!(HWREG(UART_BASE + UART_SR_OFF) & BV(UART_SR_TXRDY))) {}
#define KDBG_WAIT_TXDONE()    while (!(HWREG(UART_BASE + UART_SR_OFF) & BV(UART_SR_TXEMPTY))) {}

#define KDBG_WRITE_CHAR(c)    do { HWREG(UART_BASE + UART_THR_OFF) = (c); } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)    do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old) do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;


INLINE void kdbg_hw_init(void)
{
	/*
	 * Disable PIO mode and set appropriate UART pins peripheral mode.
	 * SAM3X,A,N,S,U: all of them has all UARTs on peripheral A.
	 */
	HWREG(UART_PIO_BASE + PIO_PDR_OFF) = UART_PINS;
	PIO_PERIPH_SEL(UART_PIO_BASE, UART_PINS, UART_PERIPH);

	/* Enable the peripheral clock */
	pmc_periphEnable(UART_ID);

	/* Reset and disable receiver & transmitter */
	HWREG(UART_BASE + UART_CR_OFF) = BV(UART_CR_RSTRX) | BV(UART_CR_RSTTX) | BV(UART_CR_RXDIS) | BV(UART_CR_TXDIS);

	/* Set mode: normal, no parity */
	HWREG(UART_BASE + UART_MR_OFF) = UART_MR_PAR_NO;

	/* Set baud rate */
	HWREG(UART_BASE + UART_BRGR_OFF) = CPU_FREQ / CONFIG_KDEBUG_BAUDRATE / 16;

	/* Enable receiver & transmitter */
	HWREG(UART_BASE + UART_CR_OFF) = BV(UART_CR_RXEN) | BV(UART_CR_TXEN);
}
