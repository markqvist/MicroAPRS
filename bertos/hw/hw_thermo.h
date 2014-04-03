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

#include "hw/thermo_map.h"

#include <cfg/debug.h>

#include <drv/ntc.h>


INLINE ticks_t thermo_hw_timeout(ThermoDev dev)
{
	(void)dev;
	return 0;
}

/**
 * This function should return the temperature set tolerance.
 */
INLINE deg_t thermo_hw_tolerance(ThermoDev dev)
{
	(void)dev;
	return 0;
}

/**
 * Read the temperature of the hw device \a dev.
 */
INLINE deg_t thermo_hw_read(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);
	/* Put here the code to read current temperature */
	return 0;
}


/**
 * Turns off a specific device.
 * This function is usefull to handle errors.
 */
INLINE void thermo_hw_off(ThermoDev dev)
{
	ASSERT(dev < THERMO_CNT);
	/* Put here the code to turn off the thermo device */
}

/**
 * Based on the current temperature \a cur_temp and the target temperature \a target,
 * this function turns on and off specific thermo device.
 * It may use also PID control for thermo-regolations.
 */
INLINE void thermo_hw_set(ThermoDev dev, deg_t target, deg_t cur_temp)
{
	ASSERT(dev < THERMO_CNT);

	if (target - cur_temp > 0)
	{
		/*
		 * We are leveaving the target temperature, so
		 * turn on the thermo device!
		 */
	}
	else
	{
		/*
		 * Ok, we are near the target temperature, so
		 * turn off the thermo device!
		 */
	}

}

#define THERMO_HW_INIT	_thermo_hw_init()

/**
 * Init hw associated with thermo-control.
 */
INLINE void _thermo_hw_init(void)
{
	/* Init your devices here! */
}

#endif /* HW_THERMO_H */
