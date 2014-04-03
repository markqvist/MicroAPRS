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
 * \brief STM32 GPIO control interface.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "gpio_stm32.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <io/stm32.h>


/**
 * Configure a GPIO pin
 *
 * \param base Base address of the GPIO port
 * \param pins Bit-packed representation of the pin(s)
 * \param mode Pin(s) configuration mode
 * \param speed Output drive speed
 *
 * Return 0 on success, otherwise a negative value.
 */
int stm32_gpioPinConfig(struct stm32_gpio *base,
			uint16_t pins, uint8_t mode, uint8_t speed)
{
	uint32_t reg_mode = mode & 0x0f;
	int i;

	if (mode & 0x10)
		reg_mode |= speed;

	if (pins & 0xff)
	{
		uint32_t reg = base->CRL;

		for (i = 0; i < 8; i++)
		{
			uint32_t pos = 1 << i;

			if (pins & pos)
			{
				pos = i << 2;
				reg &= ~(0x0f << pos);
				reg |= reg_mode << pos;

				if (mode == GPIO_MODE_IPD)
					base->BRR = 0x01 << i;
				if (mode == GPIO_MODE_IPU)
					base->BSRR = 0x01 << i;
			}
		}
		base->CRL = reg;
	}
	if (pins > 0xff)
	{
		uint32_t reg = base->CRH;

		for (i = 0; i < 8; i++)
		{
			uint32_t pos = 1 << (i + 8);

			if (pins & pos)
			{
				pos = i << 2;
				reg &= ~(0x0f << pos);
				reg |= reg_mode << pos;

				if (mode == GPIO_MODE_IPD)
					base->BRR = 0x01 << (i + 8);
				if (mode == GPIO_MODE_IPU)
					base->BSRR = 0x01 << (i + 8);
			}
		}
		base->CRH = reg;
	}
	return 0;
}
