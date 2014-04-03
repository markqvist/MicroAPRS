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
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#error Revise me!

#include <cfg/debug.h>
#include <cfg/macros.h> /* for BV() */
#include <appconfig.h>
#include <hw/hw_cpufreq.h>     /* for CPU_FREQ */
#include <hw_ser.h>     /* Required for bus macros overrides */


#include "Util196.h"
#define KDBG_WAIT_READY()      do {} while (!(SP_STAT & (SPSF_TX_EMPTY | SPSF_TX_INT)))
#define KDBG_WRITE_CHAR(c)     do { SBUF = (c); } while(0)
#define KDBG_MASK_IRQ(old) \
	do { \
		(old) = INT_MASK1 & INT1F_TI; \
		INT_MASK1 &= ~INT1F_TI; \
	} while(0)
#define KDBG_RESTORE_IRQ(old)  do { INT_MASK1 |= (old); }
typedef uint16_t kdbg_irqsave_t; /* FIXME: unconfirmed */

#if CONFIG_KDEBUG_PORT == 666
	#error BITBANG debug console missing for this platform
#endif


INLINE void kdbg_hw_init(void)
{
	/* Set serial port for 19200bps 8N1 */
	INT_MASK1 &= ~(INT1F_TI | INT1F_RI);
	SP_CON = SPCF_RECEIVE_ENABLE | SPCF_MODE1;
	ioc1_img |= IOC1F_TXD_SEL | IOC1F_EXTINT_SRC;
	IOC1 = ioc1_img;
	BAUD_RATE = 0x33;
	BAUD_RATE = 0x80;
}
