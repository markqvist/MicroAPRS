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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TAS5706A Power DAC i2c driver.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "tas5706a"
 * $WIZ$ module_depends = "timer", "i2c"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_tas5706a.h"
 * $WIZ$ module_hw = "bertos/hw/hw_tas5706a.h"
 */

#ifndef DRV_TAS5706A_H
#define DRV_TAS5706A_H

#include "cfg/cfg_i2c.h"

#include <cfg/compiler.h>

#include <drv/i2c.h>

#if COMPILER_C99
	#define tas5706a_init(...)               PP_CAT(tas5706a_init ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define tas5706a_setLowPower(...)        PP_CAT(tas5706a_setLowPower ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define tas5706a_setVolume(...)          PP_CAT(tas5706a_setVolume ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
#else
	#define tas5706a_init(args...)           PP_CAT(tas5706a_init ## _, COUNT_PARMS(args)) (args)
	#define tas5706a_setLowPower(args...)    PP_CAT(tas5706a_setLowPower ## _, COUNT_PARMS(args)) (args)
	#define tas5706a_setVolume(args...)      PP_CAT(tas5706a_setVolume ## _, COUNT_PARMS(args)) (args)
#endif

typedef enum Tas5706aCh
{
	TAS_CH1,
	TAS_CH2,
	TAS_CNT,
} Tas5706aCh;

/**
 * TAS minimum volume (%).
 */
#define TAS_VOL_MIN 0

/**
 * TAS maximum volume (%).
 */
#define TAS_VOL_MAX 100

typedef uint8_t tas5706a_vol_t;

/**
 * Set the volume for the specified channel.
 *
 * The volume must be expressed in % and will be at maximum CONFIG_TAS_MAX_VOL.
 *
 * \param i2c The i2c context.
 * \param ch The channel to be controlled.
 * \param vol The volume you want to set.
 */
void tas5706a_setVolume_3(I2c *i2c, Tas5706aCh ch, tas5706a_vol_t vol);

/**
 * Set TAS chip to low power mode.
 *
 * When in low power mode, the TAS will not play any sound. You should put the TAS chip in low
 * power whenever possible to prevent overheating and to save power.
 *
 * \param i2c The i2c context.
 * \param val True if you want to enable low power mode, false otherwise.
 */
void tas5706a_setLowPower_2(I2c *i2c, bool val);

/**
 * Initialize the TAS chip.
 *
 * \param i2c The i2c context.
 */
void tas5706a_init_1(I2c *i2c);

#if !CONFIG_I2C_DISABLE_OLD_API

DEPRECATED INLINE void tas5706a_setVolume_2(Tas5706aCh ch, tas5706a_vol_t vol)
{
	tas5706a_setVolume_3(&local_i2c_old_api, ch, vol);
}
DEPRECATED INLINE void tas5706a_setLowPower_1(bool val)
{
	tas5706a_setLowPower_2(&local_i2c_old_api, val);
}
DEPRECATED INLINE void tas5706a_init_0(void)
{
	tas5706a_init_1(&local_i2c_old_api);
}

#endif /* !CONFIG_I2C_DISABLE_OLD_API */

#endif /* DRV_TAS5706A_H */
