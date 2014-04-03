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
 * \brief Low-level timer module for NXP LPC2xxx (interface).
 */

#ifndef DRV_LPC2_TIMER_H
#define DRV_LPC2_TIMER_H

#include <hw/hw_cpufreq.h>     /* CPU_FREQ */

#include "cfg/cfg_timer.h"     /* CONFIG_TIMER */
#include <cfg/compiler.h>      /* uint8_t */
#include <cfg/macros.h>        /* BV */

#include <io/lpc23xx.h>

/**
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 *
 * $WIZ$ timer_select = "TIMER0_COMPARE0", "TIMER_DEFAULT"
 */
#define TIMER0_COMPARE0 0  ///< System timer on Timer0 Compare match0

#define TIMER_DEFAULT TIMER0_COMPARE0  ///< Default system timer

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER0_COMPARE0)
	ISR_PROTO_CONTEXT_SWITCH(timer_handler);
	#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(timer_handler)

	#define TIMER_TICKS_PER_SEC  1000
	#define TIMER_HW_CNT         (CPU_FREQ / TIMER_TICKS_PER_SEC - 1)

	/** Frequency of the hardware high-precision timer. */
	#define TIMER_HW_HPTICKS_PER_SEC (CPU_FREQ)

	/** Type of time expressed in ticks of the hardware high-precision timer */
	typedef uint32_t hptime_t;
	#define SIZEOF_HPTIME_T 4

	INLINE void timer_hw_irq(void)
	{
		/* Reset The match0 irq flag */
		T0IR = 0x01;
		/* Signal the VIC we have completed the ISR */
		VICVectAddr = 0;	
	}

	INLINE bool timer_hw_triggered(void)
	{
		return true;
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		return T0TC;
	}

#else

	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

void timer_hw_init(void);


#endif /* DRV_LPC2_TIMER_H */
