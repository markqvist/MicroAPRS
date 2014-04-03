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
 * \brief Low-level PWM module for AVR (inplementation).
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */


#include "pwm_avr.h"
#include <hw/hw_cpufreq.h>

#warning TODO:This is an exmple of implementation of PWM low level channel for AVR, implemnt it!

/**
 * Set PWM polarity to select pwm channel
 */
void pwm_hw_setPolarity(PwmDev dev, bool pol)
{

	/*
	 * Put here a code to PWM polarity of select
	 * PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;
	(void)pol;
}

/**
 * Get preiod from select channel
 *
 * \a dev channel
 */
pwm_period_t pwm_hw_getPeriod(PwmDev dev)
{
	/*
	 * Put here a code to get period value of select
	 * PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;

	return 0;
}

/**
 * Set pwm waveform frequecy.
 *
 * \a freq in Hz
 */
void pwm_hw_setFrequency(PwmDev dev, uint32_t freq)
{
	/*
	 * Put here a code to set frequency of select
	 * PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;
	(void)freq;

}

/**
 * Set pwm duty cycle.
 *
 * \a duty value 0 - 2^16
 */
void pwm_hw_setDutyUnlock(PwmDev dev, uint16_t duty)
{
	/*
	 * Put here a code to set duty of select
	 * PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;
	(void)duty;

}


/**
 * Enable select pwm channel
 */
void pwm_hw_enable(PwmDev dev)
{
	/*
	 * Put here a code to enable
	 * a select PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;
}

/**
 * Disable select pwm channel
 */
void pwm_hw_disable(PwmDev dev)
{
	/*
	 * Put here a code to disable
	 * a select PWM channel
	 */

	//Only for test remove when implement this function
	(void)dev;
}


/**
 * Init pwm.
 */
void pwm_hw_init(void)
{
	/*
	 * Put here a code to init
	 * a PWM hawdware
	 */
}




