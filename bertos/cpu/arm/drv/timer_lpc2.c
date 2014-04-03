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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Low-level timer module for LPC2xxx (implementation).
 *
 * notest:arm
 */
#include "cfg/cfg_timer.h"
#include <cfg/macros.h> // BV()
#include <cfg/debug.h> // BV()

#include <drv/vic_lpc2.h>
#include <io/lpc23xx.h>
#include "timer_lpc2.h"

/** HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER0_COMPARE0)
	#define TIMER0_ID 4
	void timer_hw_init(void)
	{
		/* Power on timer0 */
		PCONP |= BV(1);

		/* Set TIMER0 clk to CPU_FREQ */
		PCLKSEL0 &= ~0x0C;
		PCLKSEL0 |= 0x04;

		/* reset prescaler counter */
		T0PR = 0;

		/* Set match register 0 */
		T0MR0 = TIMER_HW_CNT;
		/* IRQ and reset counter on compare match 0 */
		T0MCR &= ~0x03;
		T0MCR |= 0x03;
		/* Reset timer0 counter and prescaler */
		T0TCR = 0x02;

		vic_setVector(TIMER0_ID, timer_handler);
		vic_enable(TIMER0_ID);

		/* Start timer0 */
		T0TCR = 0x01;
	}


#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

