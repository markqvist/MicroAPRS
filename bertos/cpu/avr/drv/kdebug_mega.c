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
 * Copyright 2003, 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief AVR debug support (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <hw/hw_cpufreq.h>     /* for CPU_FREQ */
#include "hw/hw_ser.h"     /* Required for bus macros overrides */

#include "cfg/cfg_debug.h"
#include <cfg/macros.h> /* for BV(), DIV_ROUND */

#include <cpu/types.h>
#include <cpu/attr.h>

#include <avr/io.h>

#if CONFIG_KDEBUG_PORT == 0

	/*
	 * Support for special bus policies or external transceivers
	 * on UART0 (to be overridden in "hw/hw_ser.h").
	 *
	 * HACK: if we don't set TXEN, kdbg disables the transmitter
	 * after each output statement until the serial driver
	 * is initialized.  These glitches confuse the debug
	 * terminal that ends up printing some trash.
	 */
	#ifndef KDBG_UART0_BUS_INIT
	#define KDBG_UART0_BUS_INIT  do { \
			UCR = BV(TXEN0); \
		} while (0)
	#endif
	#ifndef KDBG_UART0_BUS_RX
	#define KDBG_UART0_BUS_RX    do {} while (0)
	#endif
	#ifndef KDBG_UART0_BUS_TX
	#define KDBG_UART0_BUS_TX    do {} while (0)
	#endif

	#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 \
	    || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA2560
		#define UCR UCSR0B
		#define UDR UDR0
		#define USR UCSR0A
	#elif CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA32
		#define UCR    UCSRB
		#define USR    UCSRA
		#define TXEN0  TXEN
		#define UDRE0  UDRE
		#define TXC0   TXC
		#define TXCIE0 TXCIE
		#define UDRIE0 UDRIE
	#else
		#error Unknown CPU
	#endif

	#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(USR, UDRE0); } while(0)
	#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(USR, TXC0); } while(0)

	/*
	 * We must clear the TXC flag before sending a new character to allow
	 * KDBG_WAIT_TXDONE() to work properly.
	 *
	 * BUG: if KDBG_WRITE_CHAR() is called after the TXC flag is set by hardware,
	 * a new TXC could be generated after we've cleared it and before the new
	 * character is written to UDR.  On a 485 bus, the transceiver will be put
	 * in RX mode while still transmitting the last char.
	 */
	#define KDBG_WRITE_CHAR(c)    do { USR |= BV(TXC0); UDR = (c); } while(0)

	#define KDBG_MASK_IRQ(old)    do { \
		(old) = UCR; \
		UCR |= BV(TXEN0); \
		UCR &= ~(BV(TXCIE0) | BV(UDRIE0)); \
		KDBG_UART0_BUS_TX; \
	} while(0)

	#define KDBG_RESTORE_IRQ(old) do { \
		KDBG_WAIT_TXDONE(); \
		KDBG_UART0_BUS_RX; \
		UCR = (old); \
	} while(0)

	typedef uint8_t kdbg_irqsave_t;

#elif CONFIG_KDEBUG_PORT == 1

	/*
	 * Support for special bus policies or external transceivers
	 * on UART1 (to be overridden in "hw/hw_ser.h").
	 *
	 * HACK: if we don't set TXEN, kdbg disables the transmitter
	 * after each output statement until the serial driver
	 * is initialized.  These glitches confuse the debug
	 * terminal that ends up printing some trash.
	 */
	#ifndef KDBG_UART1_BUS_INIT
	#define KDBG_UART1_BUS_INIT  do { \
			UCSR1B = BV(TXEN1); \
		} while (0)
	#endif
	#ifndef KDBG_UART1_BUS_RX
	#define KDBG_UART1_BUS_RX    do {} while (0)
	#endif
	#ifndef KDBG_UART1_BUS_TX
	#define KDBG_UART1_BUS_TX    do {} while (0)
	#endif

	#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR1A, UDRE1); } while(0)
	#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(UCSR1A, TXC1); } while(0)
	#define KDBG_WRITE_CHAR(c)    do { UCSR1A |= BV(TXC1); UDR1 = (c); } while(0)

	#define KDBG_MASK_IRQ(old)    do { \
		(old) = UCSR1B; \
		UCSR1B |= BV(TXEN1); \
		UCSR1B &= ~(BV(TXCIE1) | BV(UDRIE1)); \
		KDBG_UART1_BUS_TX; \
	} while(0)

	#define KDBG_RESTORE_IRQ(old) do { \
		KDBG_WAIT_TXDONE(); \
		KDBG_UART1_BUS_RX; \
		UCSR1B = (old); \
	} while(0)

	typedef uint8_t kdbg_irqsave_t;

#elif CONFIG_KDEBUG_PORT == 2

	/*
	 * Support for special bus policies or external transceivers
	 * on UART2 (to be overridden in "hw/hw_ser.h").
	 *
	 * HACK: if we don't set TXEN, kdbg disables the transmitter
	 * after each output statement until the serial driver
	 * is initialized.  These glitches confuse the debug
	 * terminal that ends up printing some trash.
	 */
	#ifndef KDBG_UART2_BUS_INIT
	#define KDBG_UART2_BUS_INIT  do { \
			UCSR2B = BV(TXEN2); \
		} while (0)
	#endif
	#ifndef KDBG_UART2_BUS_RX
	#define KDBG_UART2_BUS_RX    do {} while (0)
	#endif
	#ifndef KDBG_UART2_BUS_TX
	#define KDBG_UART2_BUS_TX    do {} while (0)
	#endif

	#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR2A, UDRE2); } while(0)
	#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(UCSR2A, TXC2); } while(0)
	#define KDBG_WRITE_CHAR(c)    do { UCSR2A |= BV(TXC2); UDR2 = (c); } while(0)

	#define KDBG_MASK_IRQ(old)    do { \
		(old) = UCSR2B; \
		UCSR2B |= BV(TXEN2); \
		UCSR2B &= ~(BV(TXCIE2) | BV(UDRIE2)); \
		KDBG_UART2_BUS_TX; \
	} while(0)

	#define KDBG_RESTORE_IRQ(old) do { \
		KDBG_WAIT_TXDONE(); \
		KDBG_UART2_BUS_RX; \
		UCSR2B = (old); \
	} while(0)

	typedef uint8_t kdbg_irqsave_t;

#elif CONFIG_KDEBUG_PORT == 3

	/*
	 * Support for special bus policies or external transceivers
	 * on UART3 (to be overridden in "hw/hw_ser.h").
	 *
	 * HACK: if we don't set TXEN, kdbg disables the transmitter
	 * after each output statement until the serial driver
	 * is initialized.  These glitches confuse the debug
	 * terminal that ends up printing some trash.
	 */
	#ifndef KDBG_UART3_BUS_INIT
	#define KDBG_UART3_BUS_INIT  do { \
			UCSR3B = BV(TXEN3); \
		} while (0)
	#endif
	#ifndef KDBG_UART3_BUS_RX
	#define KDBG_UART3_BUS_RX    do {} while (0)
	#endif
	#ifndef KDBG_UART3_BUS_TX
	#define KDBG_UART3_BUS_TX    do {} while (0)
	#endif

	#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR3A, UDRE3); } while(0)
	#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(UCSR3A, TXC3); } while(0)
	#define KDBG_WRITE_CHAR(c)    do { UCSR3A |= BV(TXC3); UDR3 = (c); } while(0)

	#define KDBG_MASK_IRQ(old)    do { \
		(old) = UCSR3B; \
		UCSR3B |= BV(TXEN3); \
		UCSR3B &= ~(BV(TXCIE3) | BV(UDRIE3)); \
		KDBG_UART3_BUS_TX; \
	} while(0)

	#define KDBG_RESTORE_IRQ(old) do { \
		KDBG_WAIT_TXDONE(); \
		KDBG_UART3_BUS_RX; \
		UCSR3B = (old); \
	} while(0)

	typedef uint8_t kdbg_irqsave_t;


/*
 * Special debug port for BitBanged Serial see below for details...
 */
#elif CONFIG_KDEBUG_PORT == 666
	#include "hw/hw_ser.h"
	#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
	#define KDBG_WRITE_CHAR(c)     _kdebug_bitbang_putchar((c))
	#define KDBG_MASK_IRQ(old)     do { IRQ_SAVE_DISABLE((old)); } while(0)
	#define KDBG_RESTORE_IRQ(old)  do { IRQ_RESTORE((old)); } while(0)
	typedef cpu_flags_t kdbg_irqsave_t;

	#define KDBG_DELAY (((CPU_FREQ + CONFIG_KDEBUG_BAUDRATE / 2) / CONFIG_KDEBUG_BAUDRATE) + 7) / 14

	static void _kdebug_bitbang_delay(void)
	{
		unsigned long i;

		for (i = 0; i < KDBG_DELAY; i++)
		{
			NOP;
			NOP;
			NOP;
			NOP;
			NOP;
		}
	}

	/**
	 * Putchar for BITBANG serial debug console.
	 * Sometimes, we can't permit to use a whole serial for debugging purpose.
	 * Since debug console is in output only it is useful to use a single generic I/O pin for debug.
	 * This is achieved by this simple function, that shift out the data like a UART, but
	 * in software :)
	 * The only requirement is that SER_BITBANG_* macros will be defined somewhere (usually hw_ser.h)
	 * \note All interrupts are disabled during debug prints!
	 */
	static void _kdebug_bitbang_putchar(char c)
	{
		int i;
		uint16_t data = c;

		/* Add stop bit */
		data |= 0x0100;

		/* Add start bit*/
		data <<= 1;

		/* Shift out data */
		uint16_t shift = 1;
		for (i = 0; i < 10; i++)
		{
			if (data & shift)
				SER_BITBANG_HIGH;
			else
				SER_BITBANG_LOW;
			_kdebug_bitbang_delay();
			shift <<= 1;
		}
	}
#else
	#error CONFIG_KDEBUG_PORT should be either 0, 1, 2, 3 or 666
#endif


INLINE void kdbg_hw_init(void)
{
	#if CONFIG_KDEBUG_PORT == 666
		SER_BITBANG_INIT;
	#else /* CONFIG_KDEBUG_PORT != 666 */
		/* Compute the baud rate */
		uint16_t period = DIV_ROUND(CPU_FREQ / 16UL, CONFIG_KDEBUG_BAUDRATE) - 1;

		#if (CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281)
			#if CONFIG_KDEBUG_PORT == 0
				UBRR0H = (uint8_t)(period>>8);
				UBRR0L = (uint8_t)period;
				KDBG_UART0_BUS_INIT;
			#elif CONFIG_KDEBUG_PORT == 1
				UBRR1H = (uint8_t)(period>>8);
				UBRR1L = (uint8_t)period;
				KDBG_UART1_BUS_INIT;
			#else
				#error CONFIG_KDEBUG_PORT must be either 0 or 1
			#endif

		#elif CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
			#if CONFIG_KDEBUG_PORT == 0
				UBRR0H = (uint8_t)(period>>8);
				UBRR0L = (uint8_t)period;
				KDBG_UART0_BUS_INIT;
			#elif CONFIG_KDEBUG_PORT == 1
				UBRR1H = (uint8_t)(period>>8);
				UBRR1L = (uint8_t)period;
				KDBG_UART1_BUS_INIT;
			#elif CONFIG_KDEBUG_PORT == 2
				UBRR2H = (uint8_t)(period>>8);
				UBRR2L = (uint8_t)period;
				KDBG_UART2_BUS_INIT;
			#elif CONFIG_KDEBUG_PORT == 3
				UBRR3H = (uint8_t)(period>>8);
				UBRR3L = (uint8_t)period;
				KDBG_UART3_BUS_INIT;
			#else
				#error CONFIG_KDEBUG_PORT must be either 0 or 1 or 2 or 3
			#endif

		#elif CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P
			#if CONFIG_KDEBUG_PORT == 0
				UBRR0H = (uint8_t)(period>>8);
				UBRR0L = (uint8_t)period;
				UCSR0A = 0; /* The Arduino Uno bootloader turns on U2X0 */
				KDBG_UART0_BUS_INIT;
			#else
				#error Only CONFIG_KDEBUG_PORT 0 is supported for this cpu
			#endif

		#elif CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA32
			#if CONFIG_KDEBUG_PORT == 0
				UBRRH = (uint8_t)(period>>8);
				UBRRL = (uint8_t)period;
				KDBG_UART0_BUS_INIT;
			#else
				#error Only CONFIG_KDEBUG_PORT 0 is supported for this cpu
			#endif
		#elif CPU_AVR_ATMEGA103
			#if CONFIG_KDEBUG_PORT == 0
				UBRR = (uint8_t)period;
				KDBG_UART0_BUS_INIT;
			#else
				#error Only CONFIG_KDEBUG_PORT 0 is supported for this cpu
			#endif
		#else
			#error Unknown CPU
		#endif
	#endif /* CONFIG_KDEBUG_PORT == 666 */
}

