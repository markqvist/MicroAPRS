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
 *
 * \author Andrea Righi <arighi@develer.com>
 */


#include "gpio_lm3s.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <io/lm3s.h>


/* Set the pin(s) direction and mode */
INLINE int lm3s_gpioPinConfigMode(uint32_t port, uint8_t pins, uint32_t mode)
{
	if (mode == GPIO_DIR_MODE_IN)
	{
		HWREG(port + GPIO_O_DIR)   &= ~pins;
		HWREG(port + GPIO_O_AFSEL) &= ~pins;
	}
	else if (mode == GPIO_DIR_MODE_OUT)
	{
		HWREG(port + GPIO_O_DIR)   |= pins;
		HWREG(port + GPIO_O_AFSEL) &= ~pins;
	}
	else if (mode == GPIO_DIR_MODE_HW)
	{
		HWREG(port + GPIO_O_DIR)   &= ~pins;
		HWREG(port + GPIO_O_AFSEL) |= pins;
	}
	else
	{
		ASSERT(0);
		return -1;
	}
	return 0;
}

/* Set the pin(s) output strength */
INLINE int
lm3s_gpioPinConfigStrength(uint32_t port, uint8_t pins, uint32_t strength)
{
	if (strength == GPIO_STRENGTH_2MA)
	{
		HWREG(port + GPIO_O_DR2R) |= pins;
		HWREG(port + GPIO_O_DR4R) &= ~pins;
		HWREG(port + GPIO_O_DR8R) &= ~pins;
		HWREG(port + GPIO_O_SLR)  &= ~pins;
	}
	else if (strength == GPIO_STRENGTH_4MA)
	{
		HWREG(port + GPIO_O_DR2R) &= ~pins;
		HWREG(port + GPIO_O_DR4R) |= pins;
		HWREG(port + GPIO_O_DR8R) &= ~pins;
		HWREG(port + GPIO_O_SLR)  &= ~pins;
	}
	else if (strength == GPIO_STRENGTH_8MA)
	{
		HWREG(port + GPIO_O_DR2R) &= ~pins;
		HWREG(port + GPIO_O_DR4R) &= ~pins;
		HWREG(port + GPIO_O_DR8R) |= pins;
		HWREG(port + GPIO_O_SLR)  &= ~pins;
	}
	else if (strength == GPIO_STRENGTH_8MA_SC)
	{
		HWREG(port + GPIO_O_DR2R) &= ~pins;
		HWREG(port + GPIO_O_DR4R) &= ~pins;
		HWREG(port + GPIO_O_DR8R) |= pins;
		HWREG(port + GPIO_O_SLR)  |= pins;
	}
	else
	{
		ASSERT(0);
		return -1;
	}
	return 0;
}

/* Set the pin(s) type */
INLINE int lm3s_gpioPinConfigType(uint32_t port, uint8_t pins, uint32_t type)
{
	if (type == GPIO_PIN_TYPE_STD)
	{
		HWREG(port + GPIO_O_ODR)   &= ~pins;
		HWREG(port + GPIO_O_PUR)   &= ~pins;
		HWREG(port + GPIO_O_PDR)   &= ~pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_STD_WPU)
	{
		HWREG(port + GPIO_O_ODR)   &= ~pins;
		HWREG(port + GPIO_O_PUR)   |= pins;
		HWREG(port + GPIO_O_PDR)   &= ~pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_STD_WPD)
	{
		HWREG(port + GPIO_O_ODR)   &= ~pins;
		HWREG(port + GPIO_O_PUR)   &= ~pins;
		HWREG(port + GPIO_O_PDR)   |= pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_OD)
	{
		HWREG(port + GPIO_O_ODR)   |= pins;
		HWREG(port + GPIO_O_PUR)   &= ~pins;
		HWREG(port + GPIO_O_PDR)   &= ~pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_OD_WPU)
	{
		HWREG(port + GPIO_O_ODR)   |= pins;
		HWREG(port + GPIO_O_PUR)   |= pins;
		HWREG(port + GPIO_O_PDR)   &= ~pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_OD_WPD)
	{
		HWREG(port + GPIO_O_ODR)   |= pins;
		HWREG(port + GPIO_O_PUR)   &= pins;
		HWREG(port + GPIO_O_PDR)   |= pins;
		HWREG(port + GPIO_O_DEN)   |= pins;
		HWREG(port + GPIO_O_AMSEL) &= ~pins;
	}
	else if (type == GPIO_PIN_TYPE_ANALOG)
	{
		HWREG(port + GPIO_O_ODR)   &= ~pins;
		HWREG(port + GPIO_O_PUR)   &= ~pins;
		HWREG(port + GPIO_O_PDR)   &= ~pins;
		HWREG(port + GPIO_O_DEN)   &= ~pins;
		HWREG(port + GPIO_O_AMSEL) |= pins;
	}
	else
	{
		ASSERT(0);
		return -1;
	}
	return 0;
}

/**
 * Configure a GPIO pin
 *
 * \param port Base address of the GPIO port
 * \param pins Bit-packed representation of the pin(s)
 * \param mode Pin(s) configuration mode
 * \param strength Output drive strength
 * \param type Pin(s) type
 *
 * Return 0 on success, otherwise a negative value.
 */
int lm3s_gpioPinConfig(uint32_t port, uint8_t pins,
		uint32_t mode, uint32_t strength, uint32_t type)
{
	int ret;

	ret = lm3s_gpioPinConfigMode(port, pins, mode);
	if (UNLIKELY(ret < 0))
		return ret;
	ret = lm3s_gpioPinConfigStrength(port, pins, strength);
	if (UNLIKELY(ret < 0))
		return ret;
	ret = lm3s_gpioPinConfigType(port, pins, type);
	if (UNLIKELY(ret < 0))
		return ret;
	return 0;
}
