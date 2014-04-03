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
 * Copyright 2005, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2011 Onno <developer@gorgoz.org>
 *
 * -->
 *
 * \author Onno <developer@gorgoz.org>
 *
 * \brief Low-level timer module for AVR XMEGA (implementation).
 *
 * This file is heavily inspired by the AVR implementation for BeRTOS,
 * but uses a different approach for implementing the different debug
 * ports, by using the timer structs.
 *
 * This module is automatically included so no need to include
 * in test list.
 * notest: all
 */

#include <drv/timer_xmega.h>
#include <cfg/macros.h> // BV()

#include <cpu/types.h>
#include <cpu/irq.h>

#include <avr/io.h>

/* Helper MACROS taken from the Atmel examples and altered
 *
 *  The AVR XMEGA has different structures for TC0 and TC1,
 *  however these only differ in the amount of compare/capture
 *  channels, so for this purpose they can be used exactly the same
  */

//check if the bitvalues of the TC0 and TC1 Timer/Counters are really the same
#if TC0_CLKSEL_gm != TC1_CLKSEL_gm || TC0_WGMODE_gm != TC1_WGMODE_gm || TC0_OVFINTLVL_gm != TC1_OVFINTLVL_gm
	#error TC0 and TC1 Timer/Counters cannot be configured with the same bitvalues
#endif

#define TIMER_CONFIG_CLOCK_SOURCE(_clkSel)\
	((TIMERCOUNTER).CTRLA = ((TIMERCOUNTER).CTRLA & ~TC0_CLKSEL_gm) | _clkSel)

#define TIMER_CLEAR_FLAGS() ((TIMERCOUNTER).INTFLAGS = 0xFF)

#define TIMER_SET_PERIOD( _period ) ( (TIMERCOUNTER).PER = (_period) )

#define TIMER_SET_OVERFLOW_INTERRUPT_LEVEL( _interruptLevel )\
	((TIMERCOUNTER).INTCTRLA = ( (TIMERCOUNTER).INTCTRLA & ~TC0_OVFINTLVL_gm ) | _interruptLevel)

#define TIMER_CONFIG_WGM(_wgm)\
	((TIMERCOUNTER).CTRLB = ( (TIMERCOUNTER).CTRLB & ~TC0_WGMODE_gm ) | _wgm)

#define TIMER_RESET() ( (TIMERCOUNTER).CTRLFSET = TC_CMD_RESET_gc )

//Define TIMER_PRESCALE_REG_VALUE bases on the provided
//TIMER_PRESCALER value
#if TIMER_PRESCALER == 0
	#define TIMER_CLKSEL_gc				TC_CLKSEL_OFF_gc
#elif TIMER_PRESCALER == 1
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV1_gc
#elif TIMER_PRESCALER == 2
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV2_gc
#elif TIMER_PRESCALER == 4
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV4_gc
#elif TIMER_PRESCALER == 16
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV16_gc
#elif TIMER_PRESCALER == 64
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV64_gc
#elif TIMER_PRESCALER == 256
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV256_gc
#elif TIMER_PRESCALER == 1024
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV1024_gc
#else
	#error Invalid value for TIMER_PRESCALER has been defined! Using default of 1
	#define TIMER_CLKSEL_gc				TC_CLKSEL_DIV1_gc
#endif

void timer_hw_init(void)
{
	//Save and disable IRQ
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);
	//disable the timer
	TIMER_CONFIG_CLOCK_SOURCE(TC_CLKSEL_OFF_gc);
	//clear all flags
	TIMER_CLEAR_FLAGS();
	//setup the Periode register value
	//CNT register will be reset to 0 when CNT == PER
	TIMER_SET_PERIOD(TIMER_PERIOD_VALUE);
	//set the Waveform Generation Mode to Normal
	TIMER_CONFIG_WGM(TC_WGMODE_NORMAL_gc);
	//enable the overflow interrupt
	//use the highest priority
	TIMER_SET_OVERFLOW_INTERRUPT_LEVEL(TC_OVFINTLVL_HI_gc);
	//enable timer by setting the correct prescaler/clock
	TIMER_CONFIG_CLOCK_SOURCE(TIMER_CLKSEL_gc);
	//Restore IRQ
	IRQ_RESTORE(flags);
}
