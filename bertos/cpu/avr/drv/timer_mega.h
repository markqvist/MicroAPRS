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
 *
 * -->
 *
 * \brief Low-level timer module for AVR MEGA (interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 */

#ifndef DRV_TIMER_MEGA_H
#define DRV_TIMER_MEGA_H

#include <hw/hw_cpufreq.h>   /* CPU_FREQ */

#include "cfg/cfg_timer.h"   /* CONFIG_TIMER */
#include <cfg/compiler.h>    /* uint8_t */
#include <cfg/macros.h>      /* DIV_ROUND */

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 * \note The timer 1 overflow mode set the timer as a 24 kHz PWM.
 * $WIZ$ timer_select = "TIMER_ON_OUTPUT_COMPARE0", "TIMER_ON_OVERFLOW1", "TIMER_ON_OUTPUT_COMPARE2", "TIMER_ON_OVERFLOW3", "TIMER_DEFAULT"
 */
#define TIMER_ON_OUTPUT_COMPARE0  1
#define TIMER_ON_OVERFLOW1        2
#define TIMER_ON_OUTPUT_COMPARE2  3
#define TIMER_ON_OVERFLOW3        4

#define TIMER_DEFAULT TIMER_ON_OUTPUT_COMPARE0 ///< Default system timer

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	#define TIMER_PRESCALER      64
	#define TIMER_HW_BITS        8
	#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA2560
		#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER0_COMPA_vect)
	#else
		#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER0_COMP_vect)
	#endif
	#define TIMER_TICKS_PER_SEC  1000
	#define TIMER_HW_CNT         OCR_DIVISOR

	/// Type of time expressed in ticks of the hardware high-precision timer
	typedef uint8_t hptime_t;
	#define SIZEOF_HPTIME_T 1

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT0;
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	#define TIMER_PRESCALER      1
	#define TIMER_HW_BITS        8
	/** This value is the maximum in overflow based timers. */
	#define TIMER_HW_CNT         (1 << TIMER_HW_BITS)
	#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER1_OVF_vect)
	#define TIMER_TICKS_PER_SEC  DIV_ROUND(TIMER_HW_HPTICKS_PER_SEC, TIMER_HW_CNT)

	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint16_t hptime_t;
	#define SIZEOF_HPTIME_T 2

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT1;
	}

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)

	#define TIMER_PRESCALER      64
	#define TIMER_HW_BITS        8
	#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA2560
		#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER2_COMPA_vect)
	#else
		#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER2_COMP_vect)
	#endif
	#define TIMER_TICKS_PER_SEC  1000
	/** Value for OCR register in output-compare based timers. */
	#define TIMER_HW_CNT         OCR_DIVISOR

	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint8_t hptime_t;
	#define SIZEOF_HPTIME_T 1

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT2;
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW3)

	#define TIMER_PRESCALER      1
	#define TIMER_HW_BITS        8
	/** This value is the maximum in overflow based timers. */
	#define TIMER_HW_CNT         (1 << TIMER_HW_BITS)
	#define DEFINE_TIMER_ISR     DECLARE_ISR_CONTEXT_SWITCH(TIMER3_OVF_vect)
	#define TIMER_TICKS_PER_SEC  DIV_ROUND(TIMER_HW_HPTICKS_PER_SEC, TIMER_HW_CNT)

	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint16_t hptime_t;
	#define SIZEOF_HPTIME_T 2

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT3;
	}

#else

	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */


/** Frequency of the hardware high precision timer. */
#define TIMER_HW_HPTICKS_PER_SEC  DIV_ROUND(CPU_FREQ, TIMER_PRESCALER)

/**
 * System timer: additional division after the prescaler
 * 12288000 / 64 / 192 (0..191) = 1 ms
 */
#define OCR_DIVISOR  (DIV_ROUND(DIV_ROUND(CPU_FREQ, TIMER_PRESCALER), TIMER_TICKS_PER_SEC) - 1)

/** Not needed, IRQ timer flag cleared automatically */
#define timer_hw_irq() do {} while (0)

/** Not needed, timer IRQ handler called only for timer source */
#define timer_hw_triggered() (true)

void timer_hw_init(void);

#endif /* DRV_TIMER_MEGA_H */
