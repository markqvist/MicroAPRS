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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Phase control driver (implementation)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "hw/hw_phase.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <cpu/irq.h>
#include <cpu/types.h>

#include <drv/timer.h>
#include <drv/phase.h>

#include <math.h>

/** Array  of triacs */
static Triac triacs[TRIAC_CNT];

DB(bool phase_initialized;)

/**
 * Zerocross interrupt, call when 220V cross zero.
 *
 * This function turn off all triacs that have duty < 100%
 * and arm the triac timers for phase control.
 * This function is frequency adaptive so can work both at 50 or 60Hz.
 */
DEFINE_ZEROCROSS_ISR()
{
	ticks_t period, now;
	static ticks_t prev_time;
	TriacDev dev;

	now = timer_clock_unlocked();
	period = now - prev_time;

	for (dev = 0; dev < TRIAC_CNT; dev++)
	{
		/* Only turn off triac if duty is != 100% */
		if (triacs[dev].duty != CONFIG_TRIAC_MAX_DUTY)
			TRIAC_OFF(dev);
		/* Compute delay from duty */
		timer_setDelay(&triacs[dev].timer, DIV_ROUND(period * (CONFIG_TRIAC_MAX_DUTY - triacs[dev].duty), CONFIG_TRIAC_MAX_DUTY));

		/* This check avoids inserting the same timer twice
		 * in case of an intempestive zerocross or spike */
		if (triacs[dev].running)
		{
			timer_abort(&triacs[dev].timer);
			//kprintf("[%lu]\n", timer_clock());
		}

		triacs[dev].running = true;
		timer_add(&triacs[dev].timer);
	}
	prev_time = now;
}



/**
 * Set duty of the triac channel \a dev (interrupt safe).
 */
void phase_setDuty(TriacDev dev, triac_duty_t duty)
{
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	phase_setDutyUnlock(dev,duty);

	IRQ_RESTORE(flags);
}



/**
 * Set duty of the triac channel \a dev (NOT INTERRUPT SAFE).
 */
void phase_setDutyUnlock(TriacDev dev, triac_duty_t duty)
{
	triacs[dev].duty = MIN(duty, (triac_duty_t)CONFIG_TRIAC_MAX_DUTY);
}



/**
 * Set power of the triac channel \a dev (interrupt safe).
 *
 * This function approsimate the sine wave to a triangular wave to compute
 * RMS power.
 */
void phase_setPower(TriacDev dev, triac_power_t power)
{
	bool greater_fifty = false;
	triac_duty_t duty;

	power = MIN(power, (triac_power_t)CONFIG_TRIAC_MAX_POWER);

	if (power > CONFIG_TRIAC_MAX_POWER / 2)
	{
		greater_fifty = true;
		power = CONFIG_TRIAC_MAX_POWER - power;
	}

	duty = TRIAC_POWER_K * sqrt(power);

	if (greater_fifty)
		duty = CONFIG_TRIAC_MAX_DUTY - duty;
	phase_setDuty(dev, duty);
}



/**
 * Soft int for each \a _dev triac.
 *
 * The triacs are turned on at different time to achieve phase control.
 */
static void phase_softint(void *_dev)
{
	TriacDev dev = (TriacDev)_dev;

	/* Only turn on if duty is !=0 */
	if (triacs[dev].duty)
		TRIAC_ON(dev);
	triacs[dev].running = false;
}



/**
 * Initialize phase control driver
 */
void phase_init(void)
{
	cpu_flags_t flags;
	TriacDev dev;

	/* Init timers and ensure that all triac are off */
	for (dev = 0; dev < TRIAC_CNT; dev++)
	{
		triacs[dev].duty = 0;
		triacs[dev].running = false;
		SET_TRIAC_DDR(dev);
		TRIAC_OFF(dev);
		timer_setSoftint(&triacs[dev].timer, (Hook)phase_softint, (void *)dev);
	}
	IRQ_SAVE_DISABLE(flags);

	/* Init zero cross interrupt */
	PHASE_HW_INIT;
	DB(phase_initialized = true;)
	IRQ_RESTORE(flags);
}

