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
 * Copyright 2003, 2004, 2005, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 * Copyright 2011 Onno <developer@gorgoz.org>
 *
 * -->
 *
 * \brief Low-level timer module for AVR XMEGA (interface).
 *
 * This file is heavily inspired by the AVR implementation for BeRTOS,
 * but uses a different approach for implementing the different debug
 * ports, by using the timer structs.
 *
 * \author Onno <developer@gorgoz.org>
 *
 */

#ifndef DRV_TIMER_XMEGA_H
#define DRV_TIMER_XMEGA_H

#include <hw/hw_cpufreq.h>   /* CPU_FREQ */

#include "cfg/cfg_timer.h"   /* CONFIG_TIMER */
#include <cfg/compiler.h>    /* uint8_t */
#include <cfg/macros.h>      /* DIV_ROUND */

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 * $WIZ$ timer_select = "TIMER_USE_TCC0", "TIMER_USE_TCC1", "TIMER_USE_TCD0", "TIMER_USE_TCE0", "TIMER_USE_TCD1", "TIMER_DEFAULT"
 */
#define TIMER_USE_TCC0	  1
#define TIMER_USE_TCC1	  2
#define TIMER_USE_TCD0	  3
#define TIMER_USE_TCE0	  4
// The XMEGA A Family has one extra timer
#ifdef CPU_AVR_XMEGA_A
	#define TIMER_USE_TCD1	  5
#endif

#define TIMER_DEFAULT TIMER_USE_TCC1 ///< Default system timer

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER_USE_TCC0)
	#define TIMER_OVF_VECT	TCC0_OVF_vect
	#define TIMERCOUNTER	TCC0
#elif (CONFIG_TIMER == TIMER_USE_TCC1)
	#define TIMER_OVF_VECT	TCC1_OVF_vect
	#define TIMERCOUNTER	TCC1
#elif (CONFIG_TIMER == TIMER_USE_TCD0)
	#define TIMER_OVF_VECT	TCD0_OVF_vect
	#define TIMERCOUNTER	TCD0
#elif (CONFIG_TIMER == TIMER_USE_TCE0)
	#define TIMER_OVF_VECT	TCE0_OVF_vect
	#define TIMERCOUNTER	TCE0
#elif (CONFIG_TIMER == TIMER_USE_TCD1)
	#define TIMER_OVF_VECT	TCD1_OVF_vect
	#define TIMERCOUNTER	TCD1
#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

//define the Interrupt Service Routine for this Timer Mode
#define DEFINE_TIMER_ISR			DECLARE_ISR_CONTEXT_SWITCH(TIMER_OVF_VECT)
//define the Ticks per second we want
#define TIMER_TICKS_PER_SEC			1000
//define the Prescaler to use, which is dependend on the amount
//of ticks per second, the maximum value for the TOP value of the
//timer (0xFFFF) and the clock frequency.
//The maximum clock frequency is 32Mhz, so as long as the TIMER_TICKS_PER_SEC
//is larger then (about) 500 no prescaler is required.
#define TIMER_PRESCALER				1
//define the TOP/PERIOD value
#define TIMER_PERIOD_VALUE			DIV_ROUND(DIV_ROUND(CPU_FREQ, TIMER_PRESCALER), TIMER_TICKS_PER_SEC)
//check if the TIMER_PRESCALER is large enough to accomate for the TIMER_TICKS_PER_SEC
#if TIMER_PERIOD_VALUE > 0xFFFF
	#error Timer cannot generate the required Ticks per second, please adjust TIMER_PRESCALER
#endif
//define TIMER_HW_CNT it is used by the timer.c module to determine the 'edge' of the hardware counter
#define TIMER_HW_CNT				TIMER_PERIOD_VALUE
/** Frequency of the hardware high precision timer. */
#define TIMER_HW_HPTICKS_PER_SEC  	DIV_ROUND(CPU_FREQ, TIMER_PRESCALER)

// Type of time expressed in ticks of the hardware high-precision timer
typedef uint16_t hptime_t;
#define SIZEOF_HPTIME_T 2

INLINE hptime_t timer_hw_hpread(void)
{
	return (TIMERCOUNTER).CNT;
}

/* Not needed, IRQ timer flag cleared automatically */
#define timer_hw_irq() do {} while (0)

/* Not needed, timer IRQ handler called only for timer source */
#define timer_hw_triggered() (true)

void timer_hw_init(void);

#endif /* DRV_TIMER_XMEGA_H */
