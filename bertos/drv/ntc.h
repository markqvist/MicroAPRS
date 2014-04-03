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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for NTC sensors (reads a temperature through an ADC).
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "ntc"
 * $WIZ$ module_hw = "bertos/hw/hw_ntc.h", "bertos/hw/ntc_map.h", "bertos/hw/hw_ntc.c"
 */

#ifndef DRV_NTC_H
#define DRV_NTC_H

#include "hw/ntc_map.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#define NTC_OPEN_CIRCUIT  -32768
#define NTC_SHORT_CIRCUIT 32767

typedef int16_t  deg_t; /** type for celsius degrees deg_t = °C * 10 */

/** Macro for converting from deg to deg_t type */
#define DEG_TO_DEG_T(x)         ((deg_t)((x) * 10))

/** Macro for converting from deg_t to celsius degrees (returns only the integer part) */
#define DEG_T_TO_INTDEG(x)      ((x) / 10)

/** Macro for converting from deg_t to celsius degrees (returns only the decimal part) */
#define DEG_T_TO_DECIMALDEG(x)  ((x) % 10)

/** Macro for converting from deg_t to celsius degrees (returns type is float) */
#define DEG_T_TO_FLOATDEG(x)    ((x) / 10.0)


typedef uint32_t res_t; /** type for resistor res_t = Ohm * 100 */
typedef float    amp_t; /** type for defining amplifications  amp_t = A, where A is a pure number */

DB(extern bool ntc_initialized;)


/** Describe a NTC chip */
typedef struct NtcHwInfo
{
	const res_t *resistances; ///< resistances of the NTC (ohms * 100)
	size_t num_resistances;   ///< number of resistances
	deg_t degrees_min;        ///< degrees corresponding to the first entry in the table (celsius * 10)
	deg_t degrees_step;       ///< difference in degrees between two consecutive elements in the table (celsius * 10)
} NtcHwInfo;

/** Initialize the NTC module */
void ntc_init(void);

/** Read a single temperature value from the NTC */
deg_t ntc_read(NtcDev dev);

#endif /* DRV_NTC_H */
