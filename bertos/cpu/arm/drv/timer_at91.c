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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Low-level timer module for Atmel AT91 (inplementation).
 */

#include "timer_at91.h"
#include <io/arm.h>
#include "sysirq_at91.h"

#include <cfg/macros.h> // BV()
#include <cfg/module.h>
#include <cpu/irq.h>
#include <cpu/types.h>


/** HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_PIT)

	ISR_PROTO_CONTEXT_SWITCH(timer_handler);

	void timer_hw_init(void)
	{
		sysirq_init();

		cpu_flags_t flags;

		MOD_CHECK(sysirq);

		IRQ_SAVE_DISABLE(flags);

		PIT_MR = TIMER_HW_CNT;
		/* Register system interrupt handler. */
		sysirq_setHandler(SYSIRQ_PIT, timer_handler);

		/* Enable interval timer and interval timer interrupts */
		PIT_MR |= BV(PITEN);
		sysirq_setEnable(SYSIRQ_PIT, true);

		/* Reset counters, this is needed to start timer and interrupt flags */
		uint32_t dummy = PIVR;
		(void) dummy;

		IRQ_RESTORE(flags);
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */
