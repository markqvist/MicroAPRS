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
 * All Rights Reserved.
 * -->
 *
 * \brief thermo hardware-specific control functions.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#ifndef HW_THERMO_H
#define HW_THERMO_H

#include "thermo_map.h"
#include "ntc_map.h"

#include <drv/phase.h>
#include <drv/ntc.h>

#include <cfg/debug.h>
#include <cfg/compiler.h>

#warning TODO:This is an example implentation, you must implement it!

/*!
 * This function should return the temperature set tolerance.
 */
INLINE deg_t thermo_hw_tolerance(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);

	switch (dev)
	{
	case THERMO_TEST: 
		/* Put here convertion function to temperature size */
		break;

	/* Put here your thermo device */

	default:
		ASSERT(0);
	}

	return 0;
}


/*!
 * This function should return the timeout for reaching the
 * target temperature.
 */
INLINE ticks_t thermo_hw_timeout(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);

	switch (dev)
	{
	case THERMO_TEST:
		/* return ms_to_ticks(60000); */
		break;

	/* Put here a time out for select thermo device */

	default:
		ASSERT(0);
	}

	return 0;
}



/*!
 * Read the temperature of the hw device \a dev.
 */
INLINE deg_t thermo_hw_read(ThermoDev dev)
{
	return ntc_read(dev);
}


/*!
 * Turns off a specific device.
 * This function is usefull to handle errors.
 */
INLINE void thermo_hw_off(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);

	switch (dev)
	{
	case THERMO_TEST:
		phase_setPower(TRIAC_TEST, 0);
		break;

	/* Put here a thermo device to turn off */

	default:
		ASSERT(0);
	}

}


/*!
 * Based on the current temperature \a cur_temp and the target temperature \a target, this function turns on and off specific
 * triac channel and handles the freezer alarm.
 * It may use also PID control for thermo-regolations.
 */
INLINE void thermo_hw_set(ThermoDev dev, deg_t target, deg_t cur_temp)
{
	ASSERT(dev < THERMO_CNT);

	deg_t dist = target - cur_temp;
	//kprintf("dev[%d], dist[%d]\n", dev, dist);

	switch(dev)
	{
	case THERMO_TEST:
		if (dist > 0)
		{
			/*	phase_setPower(TRIAC_TEST, dist * PID_TEST_K); */
		}
		else
		{
			/* phase_setPower(TRIAC_TEST, 0); */
		}
		break;

	/* Put here an other thermo device */

	default:
		ASSERT(0);
	}
}


#define THERMO_HW_INIT	_thermo_hw_init()

/*!
 * Init hw associated with thermo-control.
 */
INLINE void _thermo_hw_init(void)
{
	ASSERT(phase_initialized);
	ASSERT(ntc_initialized);

	phase_setPower(TRIAC_TEST, 0);

	/* Add here the other thermo device */
}

#endif /* HW_THERMO_H */
