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
 * \brief PCF8574 i2c port expander driver.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "pcf8574"
 * $WIZ$ module_depends = "i2c"
 */

#ifndef DRV_PCF8574_H
#define DRV_PCF8574_H

#include "cfg/cfg_i2c.h"

#include <cfg/compiler.h>

#include <drv/i2c.h>

#if COMPILER_C99
	#define pcf8574_init(...)       PP_CAT(pcf8574_init ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define pcf8574_get(...)        PP_CAT(pcf8574_get ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define pcf8574_put(...)        PP_CAT(pcf8574_put ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
#else
	#define pcf8574_init(args...)   PP_CAT(pcf8574_init ## _, COUNT_PARMS(args)) (args)
	#define pcf8574_get(args...)    PP_CAT(pcf8574_get ## _, COUNT_PARMS(args)) (args)
	#define pcf8574_put(args...)    PP_CAT(pcf8574_put ## _, COUNT_PARMS(args)) (args)
#endif

typedef uint8_t pcf8574_addr;

/**
 * Context for accessing a PCF8574.
 */
typedef struct Pcf8574
{
	pcf8574_addr addr;
} Pcf8574;

#define PCF8574ID 0x40 ///< I2C address

/**
 * Read PCF8574 \a pcf bit status.
 * \return the pins status or EOF on errors.
 */
int pcf8574_get_2(I2c *i2c, Pcf8574 *pcf);

/**
 * Write to PCF8574 \a pcf port \a data.
 * \return true if ok, false on errors.
 */
bool pcf8574_put_3(I2c *i2c, Pcf8574 *pcf, uint8_t data);

/**
 * Init a PCF8574 on the bus with addr \a addr.
 * \return true if device is found, false otherwise.
 */
bool pcf8574_init_3(I2c *i2c, Pcf8574 *pcf, pcf8574_addr addr);

#if !CONFIG_I2C_DISABLE_OLD_API

DEPRECATED INLINE int pcf8574_get_1(Pcf8574 *pcf)
{
	return pcf8574_get_2(&local_i2c_old_api, pcf);
}

DEPRECATED INLINE bool pcf8574_put_2(Pcf8574 *pcf, uint8_t data)
{
	return pcf8574_put_3(&local_i2c_old_api, pcf, data);
}

DEPRECATED INLINE bool pcf8574_init_2(Pcf8574 *pcf, pcf8574_addr addr)
{
	return pcf8574_init_3(&local_i2c_old_api, pcf, addr);
}
#endif /* !CONFIG_I2C_DISABLE_OLD_API */

#endif /* DRV_PCF8574_H */
