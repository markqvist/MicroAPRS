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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Low-level stepper timer module for AVR (inplementation).
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "stepper_avr.h"

#include "cfg/cfg_stepper.h"
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/types.h>
#include <cpu/irq.h>


#warning TODO:This is an example, you must implement it!

///< Static array of timer counter struct for stepper.
/*
	static struct TimerCounter stepper_timers[CONFIG_TC_STEPPER_MAX_NUM] =
	{
		{ //Timer Counter settings for TIO0 output pin
				// fill with stepper timer channel settings
		}

		// Add here other stepper timer channel settings
	};
*/

/**
 * Timer couter setup.
 *
 * This function apply to select timer couter all needed settings.
 * Every settings are stored in stepper_timers[].
 */
void stepper_tc_setup(int index, stepper_isr_t callback, struct Stepper *motor)
{
	/* Put here the code to setup the stepper timer drive */

	//Only for test remove when implement this function
	(void)index;
	(void)callback;
	(void)motor;
}

/**
 * Timer counter init.
 */
void stepper_tc_init(void)
{
	/* Put here the code to init the stepper timer drive */
}

