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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.de+veler.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for NTC (reads a temperature through an ADC)
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 *
 * This module handles an external NTC bound to an AD converter. As usual,
 * it relies on a low-level API (ntc_hw_*) (see below):
 *
 */

#include "hw/hw_ntc.h"
#include "hw/ntc_map.h"

#include "cfg/cfg_ntc.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         CONFIG_NTC_LOG_LEVEL
#define LOG_VERBOSITY     CONFIG_NTC_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/debug.h>

#include <drv/ntc.h>

DB(bool ntc_initialized;)

/**
 * Find in a table of values \a orig_table of size \a size, the index which
 * value is less or equal to \a val.
 *
 * \retval 0 When \a val is higher than the first table entry.
 * \retval size When \a val is lower than the last table entry.
 * \retval 1..size-1 When \a val is within the table.
 */
static size_t upper_bound(const res_t *orig_table, size_t size, res_t val)
{
	const res_t *table = orig_table;

	while (size)
	{
		size_t pos = size / 2;
		if (val > table[pos])
			size = pos;
		else
		{
			table += pos+1;
			size -= pos+1;
		}
	}

	return table - orig_table;
}


/**
 * Read the temperature for the NTC channel \a dev.
 * First read the resistence of the NTC through ntc_hw_read(), then,
 * for the conversion from resistance to temperature, since the formula
 * varies from device to device, we implemented a generic system using
 * a table of data which maps temperature (index) to resistance (data).
 * The range of the table (min/max temperature) and the step
 * (temperature difference between two consecutive elements of the table)
 * is variable and can be specified. Notice that values inbetween the
 * table elements are still possible as the library does a linear
 * interpolation using the actual calculated resistance to find out
 * the exact temperature.
 *
 * The low-level API provides a function to get access to a description
 * of the NTC (ntc_hw_getInfo()), including the resistance table.
 *
 */
deg_t ntc_read(NtcDev dev)
{
	const NtcHwInfo* hw = ntc_hw_getInfo(dev);
	const res_t* r = hw->resistances;

	res_t rx;
	size_t i;
	deg_t degrees = 0;

	rx = ntc_hw_read(dev);


	i = upper_bound(r, hw->num_resistances, rx);
	ASSERT(i <= hw->num_resistances);

	if (i >= hw->num_resistances)
		return NTC_SHORT_CIRCUIT;
	else if (i == 0)
		return NTC_OPEN_CIRCUIT;

	/*
	 * Interpolated value in 0.1 degrees multiplied by 10:
	 *   delta t          step t
	 * ----------  = ----------------
	 * (rx - r[i])   (r[i-1] - r [i])
	 */
	float tmp;
	tmp = 10 * hw->degrees_step * (rx - r[i]) / (r[i - 1] - r[i]);

	/*
	 * degrees = integer part corresponding to the superior index
	 *           in the table multiplied by 10
	 *           - decimal part interpolated (already multiplied by 10)
	 */
	degrees = (i * hw->degrees_step + hw->degrees_min) * 10 - (int)(tmp);

	//kprintf("dev= %d, I=%d, degrees = %d\n", dev, i , degrees);

	return degrees;
}


/**
 * Init NTC hardware.
 */
void ntc_init(void)
{
	NTC_HW_INIT;
	DB(ntc_initialized = true;)
}

