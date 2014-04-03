#error This code must be revised for the new timer API
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
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Low-level timer module for AVR
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2006/02/21 21:28:02  bernie
 *#* New time handling based on TIMER_TICKS_PER_SEC to support slow timers with ticks longer than 1ms.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.4  2004/12/13 11:51:08  bernie
 *#* DISABLE_INTS/ENABLE_INTS: Convert to IRQ_DISABLE/IRQ_ENABLE.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:23:30  bernie
 *#* Import drv/timer module.
 *#*
 *#*/

#ifndef TIMER_I196_H
#define TIMER_I196_H

/**
 * Retrigger TIMER2, adjusting the time to account for
 * the interrupt prologue latency.
 */
#define TIMER_RETRIGGER (TIMER2 -= TICKS_RATE)

#define TIMER_INIT \
	TIMER2 = (65535 - TICKS_RATE); \
	INT_MASK1 |= INT1F_T2OVF; \
	ATOMIC( \
		WSR = 1; \
		IOC3 |= IOC3F_T2_ENA; \
		WSR = 0; \
	)

#define DEFINE_TIMER_ISR \
	INTERRUPT(0x38) void TM2_OVFL_interrupt(void);  \
	INTERRUPT(0x38) void TM2_OVFL_interrupt(void)

#endif /* DRV_TIMER_I196_H */
