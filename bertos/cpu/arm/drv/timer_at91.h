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
 * \brief Low-level timer module for Atmel AT91 (interface).
 */

#ifndef DRV_AT91_TIMER_H
#define DRV_AT91_TIMER_H

#include <hw/hw_cpufreq.h>     /* CPU_FREQ */

#include "cfg/cfg_timer.h"     /* CONFIG_TIMER */
#include <cfg/compiler.h>      /* uint8_t */
#include <cfg/macros.h>        /* BV */

#include <io/arm.h>

/**
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 *
 * $WIZ$ timer_select = "TIMER_ON_PIT", "TIMER_DEFAULT"
 */
#define TIMER_ON_PIT 1  ///< System timer on Periodic interval timer

#define TIMER_DEFAULT TIMER_ON_PIT  ///< Default system timer

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER_ON_PIT)

	/*
	 * On ARM all system IRQs are handled by the sysirq_dispatcher, so the actual
	 * timer handler can be treated like any other normal routine.
	 */
	#define DEFINE_TIMER_ISR	void timer_handler(void);	\
					void timer_handler(void)

	#define TIMER_TICKS_PER_SEC  1000
	#define TIMER_HW_CNT         (CPU_FREQ / (16 * TIMER_TICKS_PER_SEC) - 1)

	/** Frequency of the hardware high-precision timer. */
	#define TIMER_HW_HPTICKS_PER_SEC (CPU_FREQ / 16)

	/** Type of time expressed in ticks of the hardware high-precision timer */
	typedef uint32_t hptime_t;
	#define SIZEOF_HPTIME_T 4

	INLINE void timer_hw_irq(void)
	{
		/* Reset counters, this is needed to reset timer and interrupt flags */
		uint32_t dummy = PIVR;
		(void) dummy;
	}

	INLINE bool timer_hw_triggered(void)
	{
		return PIT_SR & BV(PITS);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		/* In the upper part of PIT_PIIR there is unused data */
		return PIIR & CPIV_MASK;
	}

#else

	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

void timer_hw_init(void);


#endif /* DRV_TIMER_AT91_H */
