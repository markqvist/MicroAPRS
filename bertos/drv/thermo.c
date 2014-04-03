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
 *
 * -->
 *
 * \brief Thermo-control driver.
 *
 * The Thermo controll can works both with kernel or without it. In the case
 * we use kernel, the thermo controll is done by one process that poll every
 * CONFIG_THERMO_INTERVAL_MS the temperature sensor and make all operation to
 * follow the target temperature. While we not use the kernel the module works
 * with one timer interrupt in the same way of the kenel case.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "hw/thermo_map.h"
#include "hw/hw_thermo.h"

#include "cfg/cfg_thermo.h"

#include <cfg/module.h>
#include <cfg/macros.h>
#include <cfg/debug.h>
// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         CONFIG_THERMO_LOG_LEVEL
#define LOG_VERBOSITY     CONFIG_THERMO_LOG_FORMAT
#include <cfg/log.h>

#include <drv/thermo.h>
#include <drv/timer.h>
#include <drv/ntc.h>

#include <kern/proc.h>

#define THERMO_OFF          0
#define THERMO_HEATING      BV(0)
#define THERMO_FREEZING     BV(1)
#define THERMO_TGT_REACH    BV(2)
#define THERMOERRF_NTCSHORT BV(3)
#define THERMOERRF_NTCOPEN  BV(4)
#define THERMOERRF_TIMEOUT  BV(5)
#define THERMO_ACTIVE       BV(6)
#define THERMO_TIMER        BV(7)

#define THERMO_ERRMASK      (THERMOERRF_NTCSHORT | THERMOERRF_NTCOPEN | THERMOERRF_TIMEOUT)


#if CONFIG_KERN
	/** Stack process for Thermo process. */
	static PROC_DEFINE_STACK(thermo_poll_stack, 400);
#else
	/** Timer for thermo-regulation. */
	static Timer thermo_timer;
#endif

typedef struct ThermoControlDev
{
	deg_t          hifi_samples[CONFIG_THERMO_HIFI_NUM_SAMPLES];
	deg_t          cur_hifi_sample;
	deg_t          target;
	thermostatus_t status;
	ticks_t        expire;
	ticks_t        on_time;
} ThermoControlDev;

/** Array of thermo-devices. */
ThermoControlDev devs[THERMO_CNT];

/**
 * Return the status of the specific \a dev thermo-device.
 */
thermostatus_t thermo_status(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);
	return devs[dev].status;
}


/**
 * Do a single thermo control for device \a dev.
 */
static void thermo_do(ThermoDev index)
{
	ThermoControlDev* dev = &devs[index];
	deg_t cur_temp;
	deg_t tolerance = thermo_hw_tolerance(index);

	cur_temp = thermo_hw_read(index);

	// Store the sample into the hifi FIFO buffer for later interpolation
	dev->hifi_samples[dev->cur_hifi_sample] = cur_temp;
	if (++dev->cur_hifi_sample == CONFIG_THERMO_HIFI_NUM_SAMPLES)
		dev->cur_hifi_sample = 0;

	cur_temp = thermo_readTemperature(index);

	if (cur_temp == NTC_SHORT_CIRCUIT || cur_temp == NTC_OPEN_CIRCUIT)
	{
		if (cur_temp == NTC_SHORT_CIRCUIT)
		{
			LOG_INFOB(if (!(dev->status & THERMOERRF_NTCSHORT))
				LOG_INFO("dev[%d], thermo_do: NTC_SHORT\n",index););

			dev->status |= THERMOERRF_NTCSHORT;
		}
		else
		{

			LOG_INFOB(if (!(dev->status & THERMOERRF_NTCOPEN))
				LOG_INFO("dev[%d], thermo_do: NTC_OPEN\n", index););

			dev->status |= THERMOERRF_NTCOPEN;
		}

		/* Reset timeout when there is an ntc error */
		dev->expire = thermo_hw_timeout(index) + timer_clock();
		thermo_hw_off(index);
		return;
	}
	dev->status &= ~(THERMOERRF_NTCOPEN | THERMOERRF_NTCSHORT);

	if ((cur_temp < dev->target - tolerance) || (cur_temp > dev->target + tolerance))
	{
		dev->status &= ~THERMO_TGT_REACH;

		/* Check for timeout */
		if (timer_clock() - dev->expire > 0)
		{
			dev->status |= THERMOERRF_TIMEOUT;
			LOG_INFO("dev[%d], thermo_do: TIMEOUT\n", index);
		}
	}
	else /* In target */
	{
		/* Clear errors */
		dev->status &= ~THERMO_ERRMASK;
		dev->status |= THERMO_TGT_REACH;

		/* Reset timeout in case we go out of target in the future */
		dev->expire = thermo_hw_timeout(index) + timer_clock();
	}

	if (cur_temp < dev->target)
		dev->status = (dev->status | THERMO_HEATING) & ~THERMO_FREEZING;
	else
		dev->status = (dev->status & ~THERMO_HEATING) | THERMO_FREEZING;

	thermo_hw_set(index, dev->target, cur_temp);

}

static void poll(void)
{
	for (int i = 0; i < THERMO_CNT; ++i)
		if (devs[i].status & THERMO_ACTIVE)
		{
			LOG_INFO("THERMO [%d] on_time[%ld],\n", i, ticks_to_ms(devs[i].on_time));
			if ((devs[i].status & THERMO_TIMER) && (devs[i].on_time - timer_clock() < 0))
			{
				thermo_stop(i);
				continue;
			}

			thermo_do((ThermoDev)i);
		}
}

#if CONFIG_KERN
	static void NORETURN thermo_poll(void)
	{
		for (;;)
		{
			poll();
			timer_delay(CONFIG_THERMO_INTERVAL_MS);
		}
	}
#else
	/**
	 * Thermo soft interrupt.
	 */
	static void thermo_softint(void)
	{
		poll();
		timer_add(&thermo_timer);
	}
#endif

/**
 * Starts a thermo-regulation for channel \a dev, and turn off timer
 * when \a on_time was elapsed.
 */
void thermo_timer(ThermoDev dev, mtime_t on_time)
{
	ASSERT(dev < THERMO_CNT);
	devs[dev].on_time = timer_clock() + ms_to_ticks(on_time);
	devs[dev].status |= THERMO_TIMER;
	thermo_start(dev);
}


/**
 * Set the target temperature \a temperature for a specific \a dev thermo-device.
 */
void thermo_setTarget(ThermoDev dev, deg_t temperature)
{
	ASSERT(dev < THERMO_CNT);
	devs[dev].target = temperature;
	devs[dev].expire = timer_clock() + thermo_hw_timeout(dev);

	LOG_INFO("THERMO Set Target dev[%d], T[%d.%d]\n", dev, temperature / 10, temperature % 10);
}

/**
 * Starts a thermo-regulation for channel \a dev.
 */
void thermo_start(ThermoDev dev)
{
	int i;
	deg_t temp;

	ASSERT(dev < THERMO_CNT);

	devs[dev].status |= THERMO_ACTIVE;
	LOG_INFO("THERMO Start dev[%d], status[%04x]\n", dev, devs[dev].status);

	/* Initialize the hifi FIFO with a constant value (the current temperature) */
	temp = thermo_hw_read(dev);
	for (i = 0; i < CONFIG_THERMO_HIFI_NUM_SAMPLES; ++i)
		devs[dev].hifi_samples[i] = temp;
	devs[dev].cur_hifi_sample = 0;

	/* Reset timeout */
	devs[dev].expire = timer_clock() + thermo_hw_timeout(dev);
}

/**
 * Stops a thermo-regulation for channel \a dev.
 */
void thermo_stop(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);

	devs[dev].status &= ~THERMO_ACTIVE;
	thermo_hw_off(dev);
}


/**
 * Clear errors for channel \a dev.
 */
void thermo_clearErrors(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);
	devs[dev].status &= ~(THERMO_ERRMASK);
}


/**
 * Read the temperature of the thermo-device \a dev using mobile mean.
 */
deg_t thermo_readTemperature(ThermoDev dev)
{
	int i;
	long accum = 0;

	MOD_CHECK(thermo);

	for (i = 0; i < CONFIG_THERMO_HIFI_NUM_SAMPLES; i++)
		accum += devs[dev].hifi_samples[i];

	return (deg_t)(accum / CONFIG_THERMO_HIFI_NUM_SAMPLES);
}

MOD_DEFINE(thermo)

/**
 * Init thermo-control and associated hw.
 */
void thermo_init(void)
{
	THERMO_HW_INIT;

	/* Set all status to off */
	for (int i = 0; i < THERMO_CNT; i++)
		devs[i].status = THERMO_OFF;

	MOD_INIT(thermo);

	#if CONFIG_KERN
		proc_new_with_name("Thermo", thermo_poll, NULL, sizeof(thermo_poll_stack), thermo_poll_stack);
	#else
		timer_setDelay(&thermo_timer, ms_to_ticks(CONFIG_THERMO_INTERVAL_MS));
		timer_setSoftint(&thermo_timer, (Hook)thermo_softint, 0);
		timer_add(&thermo_timer);
	#endif
}
