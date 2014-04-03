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
 * Copyright 2010 Mohamed <mtarek16@gmail.com>
 *
 * -->
 *
 * \brief MSP430 debug support (implementation).
 *
 * \author Mohamed Tarek <mtarek16@gmail.com>
 */

#include "kdebug_msp430.h"  /* for UART clock source definitions */

#include "hw/hw_ser.h"      /* bus macros overrides */
#include "cfg/cfg_debug.h"

#include <cfg/macros.h>     /* for DIV_ROUND */

#include <cpu/types.h>
#include <cpu/attr.h>

#include <io.h>

#if CONFIG_KDEBUG_PORT == 0

	#ifndef KDBG_UART0_BUS_INIT
	#define KDBG_UART0_BUS_INIT  do {} while (0)
	#endif
	#ifndef KDBG_UART0_BUS_RX
	#define KDBG_UART0_BUS_RX    do {} while (0)
	#endif
	#ifndef KDBG_UART0_BUS_TX
	#define KDBG_UART0_BUS_TX    do {} while (0)
	#endif

	/* USCI Register definitions */
	#define	UCSTAT	UCA0STAT
	#define	UCTXBUF	UCA0TXBUF
	#define	UCRXBUF	UCA0RXBUF
	#define	UCTXIFG	UCA0TXIFG
	#define	UCRXIFG	UCA0RXIFG
	#define	UCTXIE	UCA0TXIE
	#define	UCRXIE	UCA0RXIE
	#define UCCTL0	UCA0CTL0
	#define	UCCTL1	UCA0CTL1
	#define	UCBR0	UCA0BR0
	#define	UCBR1	UCA0BR1
	#define	UCMCTL	UCA0MCTL
	#define IE		IE2
	#define IFG		IFG2

	#if CPU_MSP430F2274
		#define KDBG_MSP430_UART_PINS_INIT()	do{ P3SEL = 0x30; }while(0)
	#endif

#else

	#if CPU_MSP430F2274
		#error only 1 UART availbale, CONFIG_KDEBUG_PORT should be 0
	#endif

#endif

#define KDBG_WAIT_READY()     do { while((UCSTAT & UCBUSY)); } while(0)
#define KDBG_WAIT_TXDONE()    do { while(!(IFG & UCTXIFG)); } while(0)

#define KDBG_WRITE_CHAR(c)    do { UCTXBUF = (c); } while(0)

#define KDBG_MASK_IRQ(old)    do { \
	(old) = IE; \
	IE &= ~(UCTXIE|UCRXIE);\
} while(0)

#define KDBG_RESTORE_IRQ(old) do { \
	KDBG_WAIT_TXDONE(); \
	IE = (old); \
} while(0)

#if CONFIG_KDEBUG_CLOCK_FREQ
	#define KDBG_MSP430_FREQ CONFIG_KDEBUG_CLOCK_FREQ
#else
	#define KDBG_MSP430_FREQ CPU_FREQ
#endif

typedef uint8_t kdbg_irqsave_t;

INLINE void kdbg_hw_init(void)
{
	/* Compute the clock prescaler for the desired baudrate */
	uint16_t quot = DIV_ROUND(KDBG_MSP430_FREQ, CONFIG_KDEBUG_BAUDRATE);
	KDBG_MSP430_UART_PINS_INIT();       // Configure USCI TX/RX pins

#if (CONFIG_KDEBUG_CLOCK_SOURCE == KDBG_UART_SMCLK)
	UCCTL1 |= UCSSEL_SMCLK;
#else
	UCCTL1 |= UCSSEL_ACLK;
#endif

	UCBR0	= quot & 0xFF;              // Setup clock prescaler for the UART
	UCBR1	= quot >> 8;

	UCMCTL	= UCBRS0;                   // No Modulation
	UCCTL0	= 0;                        // Default UART settings (8N1)
	UCCTL1 &= ~UCSWRST;                 // Initialize USCI state machine
	KDBG_MASK_IRQ(IE2);                 // Disable USCI interrupts
}

