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
 * \brief ARM debug support (implementation).
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <hw/hw_cpufreq.h>     /* for CPU_FREQ */
#include "hw/hw_ser.h"     /* Required for bus macros overrides */

#include "cfg/cfg_debug.h"
#include <cfg/macros.h> /* for BV(), DIV_ROUND */

#include <io/lpc23xx.h>

#if CONFIG_KDEBUG_PORT == 0
	#define KDBG_WAIT_READY()     while (!(U0LSR & BV(5))) {}
	#define KDBG_WAIT_TXDONE()    while (!(U0LSR & BV(6))) {}

	#define KDBG_WRITE_CHAR(c)    do { U0THR = (c); } while(0)

	#define KDBG_MASK_IRQ(old)    do { \
		(old) = U0IER; \
		U0IER &= ~BV(1); \
	} while(0)

	#define KDBG_RESTORE_IRQ(old) do { \
		KDBG_WAIT_TXDONE(); \
		U0IER = (old); \
	} while(0)

	typedef uint32_t kdbg_irqsave_t;

#else
	#error CONFIG_KDEBUG_PORT should be 0
#endif


INLINE void kdbg_hw_init(void)
{
	#if CONFIG_KDEBUG_PORT == 0
		/* Enable clock for UART0 */
		PCONP = BV(3);
		/* Set UART0 clk to CPU_FREQ */
		PCLKSEL0 &= ~0xC0;
		PCLKSEL0 |= 0x40;
		/* Set 8bit, 1 stop bit, no parity, DLAB = 1 (enable divisor modify) */
		U0LCR = 0x83;
		U0DLL = DIV_ROUND(CPU_FREQ, 16 * CONFIG_KDEBUG_BAUDRATE) & 0xFF;
		U0DLM = (DIV_ROUND(CPU_FREQ, 16 * CONFIG_KDEBUG_BAUDRATE) >> 8) & 0xFF;
		U0FDR = 0x10;
		/* Assign TX pin to UART0*/
		PINSEL0 &= ~0x30;
		PINSEL0 |= 0x10;
		/* Set 8bit, 1 stop bit, no parity, DLAB = 0 (disable divisor modify) */
		U0LCR = 0x03;
		/* Enable transmitter */
		U0TER = BV(7);
	#else
		#error CONFIG_KDEBUG_PORT should be 0
	#endif /* CONFIG_KDEBUG_PORT == 0 */
}
