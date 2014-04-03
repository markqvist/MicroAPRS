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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief I2C generic driver functions (implementation).
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "i2c.h"

#include "cfg/cfg_i2c.h"

#if !CONFIG_I2C_DISABLE_OLD_API

I2c local_i2c_old_api;

/**
 * Send a sequence of bytes in master transmitter mode
 * to the selected slave device through the I2C bus.
 *
 * \return true on success, false on error.
 */
bool i2c_send(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (count--)
	{
		if (!i2c_put(*buf++))
			return false;
	}
	return true;
}

/**
 * Receive a sequence of one or more bytes from the
 * selected slave device in master receive mode through
 * the I2C bus.
 *
 * Received data is placed in \c buf.
 *
 * \note a NACK is automatically given on the last received
 *         byte.
 *
 * \return true on success, false on error
 */
bool i2c_recv(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;

	while (count--)
	{
		/*
		 * The last byte read does not has an ACK
		 * to stop communication.
		 */
		int c = i2c_get(count);

		if (c == EOF)
			return false;
		else
			*buf++ = c;
	}

	return true;
}
#endif /* !CONFIG_I2C_DISABLE_OLD_API */

void i2c_genericWrite(struct I2c *i2c, const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (count--)
		i2c_putc(i2c, *buf++);
}

void i2c_genericRead(struct I2c *i2c, void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;

	while (count--)
		*buf++ = i2c_getc(i2c);
}

