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
 * \brief Thermo regulation control map definitions.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef THERMO_MAP_H
#define THERMO_MAP_H

#include <cfg/compiler.h>
#include <cfg/macros.h>

#warning TODO:This is an example implentation, you must implement it!

typedef uint8_t thermostatus_t;

#define THERMO_OFF          0
#define THERMO_HEATING      BV(0)
#define THERMO_FREEZING     BV(1)
#define THERMO_TGT_REACH    BV(2)
#define THERMOERRF_NTCSHORT BV(3)
#define THERMOERRF_NTCOPEN  BV(4)
#define THERMOERRF_TIMEOUT  BV(5)
#define THERMO_ACTIVE       BV(6)

#define THERMO_ERRMASK      (THERMOERRF_NTCSHORT | THERMOERRF_NTCOPEN | THERMOERRF_TIMEOUT)

typedef enum ThermoDev
{
	THERMO_TEST,

	/* Put here your thermo device */

	THERMO_CNT,
} ThermoDev;

#endif /* THERMO_MAP_H */
