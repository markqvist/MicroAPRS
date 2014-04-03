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
 * This driver controls the PCF8574.
 * The PCF8574 is an 8bit i2c port expander.
 * You can read/write 8 pins through an i2c bus.
 * The pins are quasi-bidirectionals, this mean that
 * without the need of a direction register you can use
 * each pin as input or output, see datasheet on how this
 * is achieved.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "pcf8574.h"

#include "cfg/cfg_i2c.h"

#include <cfg/module.h>

#include <drv/i2c.h>

/**
 * Read PCF8574 \a pcf bit status.
 * \return the pins status or EOF on errors.
 */
int pcf8574_get_2(I2c *i2c, Pcf8574 *pcf)
{
	i2c_start_r(i2c, PCF8574ID | ((pcf->addr << 1) & 0xF7), 1, I2C_STOP);

	int data = i2c_getc(i2c);

	if (i2c_error(i2c))
		data = EOF;

	return data;
}

/**
 * Write to PCF8574 \a pcf port \a data.
 * \return true if ok, false on errors.
 */
bool pcf8574_put_3(I2c *i2c, Pcf8574 *pcf, uint8_t data)
{
	i2c_start_w(i2c, PCF8574ID | ((pcf->addr << 1) & 0xF7), 1, I2C_STOP);
	i2c_putc(i2c, data);

	if (i2c_error(i2c))
		return false;

	return true;
}

/**
 * Init a PCF8574 on the bus with addr \a addr.
 * \return true if device is found, false otherwise.
 */
bool pcf8574_init_3(I2c *i2c, Pcf8574 *pcf, pcf8574_addr addr)
{
	ASSERT(i2c);
	pcf->addr = addr;

	return (pcf8574_get(i2c, pcf) != EOF);
}

