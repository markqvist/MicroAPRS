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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM3S1968 GPIO control interface.
 */

#ifndef GPIO_LM3S_H
#define GPIO_LM3S_H

#include <io/lm3s.h>

/**
 * GPIO mode
 */
/*\{*/
enum
{
	GPIO_DIR_MODE_IN = 0, //< Pin is a GPIO input
	GPIO_DIR_MODE_OUT,    //< Pin is a GPIO output
	GPIO_DIR_MODE_HW,     //< Pin is a peripheral function
};
/*\}*/

/**
 * GPIO strenght
 */
/*\{*/
enum
{
	GPIO_STRENGTH_2MA = 0, //< 2mA drive strength
	GPIO_STRENGTH_4MA,     //< 4mA drive strength
	GPIO_STRENGTH_8MA,     //< 8mA drive strength
	GPIO_STRENGTH_8MA_SC,  //< 8mA drive with slew rate control
};
/*\}*/

/**
 * GPIO type
 */
/*\{*/
enum
{
	GPIO_PIN_TYPE_ANALOG = 0, //< Analog comparator
	GPIO_PIN_TYPE_STD,        //< Push-pull
	GPIO_PIN_TYPE_STD_WPU,    //< Push-pull with weak pull-up
	GPIO_PIN_TYPE_STD_WPD,    //< Push-pull with weak pull-down
	GPIO_PIN_TYPE_OD,         //< Open-drain
	GPIO_PIN_TYPE_OD_WPU,     //< Open-drain with weak pull-up
	GPIO_PIN_TYPE_OD_WPD,     //< Open-drain with weak pull-down
};
/*\}*/

/* Write a value to the specified pin(s) */
INLINE void lm3s_gpioPinWrite(uint32_t port, uint8_t pins, uint8_t val)
{
	HWREG(port + GPIO_O_DATA + (pins << 2)) = val;
}

/* Read a value from the specified pin(s) */
INLINE uint32_t lm3s_gpioPinRead(uint32_t port, uint8_t pins)
{
	return HWREG(port + GPIO_O_DATA + (pins << 2));
}

int lm3s_gpioPinConfig(uint32_t port, uint8_t pins,
		uint32_t mode, uint32_t strength, uint32_t type);

#endif /* GPIO_LM3S_H */
