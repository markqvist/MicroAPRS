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
 * \brief I2C bitbang driver (implementation)
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "hw/hw_i2c_bitbang.h"

#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/macros.h>
#include <cfg/module.h>

#include <drv/timer.h>
#include <drv/i2c.h>

#include <cpu/irq.h>

#include <cpu/attr.h>

#if !CONFIG_I2C_DISABLE_OLD_API

INLINE bool i2c_bitbang_start(void)
{
	SDA_HI;
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SDA_LO;
	I2C_HALFBIT_DELAY();

	return !SDA_IN;
}

void i2c_bitbang_stop(void)
{
	SDA_LO;
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SDA_HI;
}

bool i2c_bitbang_put(uint8_t _data)
{
	/* Add ACK bit */
	uint16_t data = (_data << 1) | 1;

	for (uint16_t i = 0x100; i != 0; i >>= 1)
	{
		SCL_LO;
		if (data & i)
			SDA_HI;
		else
			SDA_LO;
		I2C_HALFBIT_DELAY();

		SCL_HI;
		I2C_HALFBIT_DELAY();
	}

	bool ack = !SDA_IN;
	SCL_LO;
	I2C_HALFBIT_DELAY();
	return ack;
}

bool i2c_bitbang_start_w(uint8_t id)
{
	id &= ~I2C_READBIT;
	/*
	 * Loop on the select write sequence: when the device is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the deveice responds with an ACK.
	 */
	ticks_t start = timer_clock();
	while (i2c_bitbang_start())
	{
		if (i2c_bitbang_put(id))
			return true;
		else if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C start write\n");
			break;
		}
		//LOG_INFO("Rep start\n");
	}

	return false;
}

bool i2c_bitbang_start_r(uint8_t id)
{
	id |= I2C_READBIT;
	if (i2c_bitbang_start())
	{
		if (i2c_bitbang_put(id))
			return true;

		LOG_ERR("NACK on I2c start read\n");
	}

	return false;
}

int i2c_bitbang_get(bool ack)
{
	uint8_t data = 0;
	for (uint8_t i = 0x80; i != 0; i >>= 1)
	{
		SCL_LO;
		I2C_HALFBIT_DELAY();
		SCL_HI;
		if (SDA_IN)
			data |= i;
		else
			data &= ~i;

		I2C_HALFBIT_DELAY();
	}
	SCL_LO;

	if (ack)
		SDA_LO;
	else
		SDA_HI;

	I2C_HALFBIT_DELAY();
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SCL_LO;
	SDA_HI;
	/* avoid sign extension */
	return (int)(uint8_t)data;
}

#endif /* !CONFIG_I2C_DISABLE_OLD_API */

/*
 * New I2C API
 */
static bool old_api = false;
#define I2C_DEV(i2c)            ((int)((i2c)->hw))

static void i2c_bitbang_stop_1(struct I2c *i2c)
{
	if (old_api)
	{
		SDA_LO;
		SCL_HI;
		I2C_HALFBIT_DELAY();
		SDA_HI;
	}
	else
	{
		i2c_sdaLo(I2C_DEV(i2c));
		i2c_sclHi(I2C_DEV(i2c));
		i2c_halfbitDelay(I2C_DEV(i2c));
		i2c_sdaHi(I2C_DEV(i2c));
	}
}

INLINE bool i2c_bitbang_start_1(struct I2c *i2c)
{
	bool ret;
	/* Clear all error, we restart */
	i2c->errors &= ~(I2C_NO_ACK | I2C_ARB_LOST);

	if (old_api)
	{
		SDA_HI;
		SCL_HI;
		I2C_HALFBIT_DELAY();
		SDA_LO;
		I2C_HALFBIT_DELAY();

		ret = !SDA_IN;
	}
	else
	{
		i2c_sdaHi(I2C_DEV(i2c));
		i2c_sclHi(I2C_DEV(i2c));
		i2c_halfbitDelay(I2C_DEV(i2c));
		i2c_sdaLo(I2C_DEV(i2c));
		i2c_halfbitDelay(I2C_DEV(i2c));

		ret = !i2c_sdaIn(I2C_DEV(i2c));
	}

	return ret;
}


static uint8_t i2c_bitbang_getc(struct I2c *i2c)
{
	uint8_t data = 0;
	if (old_api)
	{
		for (uint8_t i = 0x80; i != 0; i >>= 1)
		{
			SCL_LO;
			I2C_HALFBIT_DELAY();
			SCL_HI;
			if (SDA_IN)
				data |= i;
			else
				data &= ~i;

			I2C_HALFBIT_DELAY();
		}
		SCL_LO;

		/* Generate ACK/NACK */
		if (i2c->xfer_size > 1)
			SDA_LO;
		else
			SDA_HI;

		I2C_HALFBIT_DELAY();
		SCL_HI;
		I2C_HALFBIT_DELAY();
		SCL_LO;
		SDA_HI;
	}
	else
	{
		for (uint8_t i = 0x80; i != 0; i >>= 1)
		{
			i2c_sclLo(I2C_DEV(i2c));
			i2c_halfbitDelay(I2C_DEV(i2c));
			i2c_sclHi(I2C_DEV(i2c));
			if (i2c_sdaIn(I2C_DEV(i2c)))
				data |= i;
			else
				data &= ~i;

			i2c_halfbitDelay(I2C_DEV(i2c));
		}
		i2c_sclLo(I2C_DEV(i2c));

		/* Generate ACK/NACK */
		if (i2c->xfer_size > 1)
			i2c_sdaLo(I2C_DEV(i2c));
		else
			i2c_sdaHi(I2C_DEV(i2c));

		i2c_halfbitDelay(I2C_DEV(i2c));
		i2c_sclHi(I2C_DEV(i2c));
		i2c_halfbitDelay(I2C_DEV(i2c));
		i2c_sclLo(I2C_DEV(i2c));
		i2c_sdaHi(I2C_DEV(i2c));
	}

	/* Generate stop condition (if requested) */
	if ((i2c->xfer_size == 1) && (i2c->flags & I2C_STOP))
		i2c_bitbang_stop_1(i2c);

	return data;
}

INLINE void i2c_bitbang_putcStop(struct I2c *i2c, uint8_t _data, bool stop)
{
	/* Add ACK bit */
	uint16_t data = (_data << 1) | 1;
	bool ack;

	if (old_api)
	{
		for (uint16_t i = 0x100; i != 0; i >>= 1)
		{
			SCL_LO;
			if (data & i)
				SDA_HI;
			else
				SDA_LO;
			I2C_HALFBIT_DELAY();

			SCL_HI;
			I2C_HALFBIT_DELAY();
		}

		ack = !SDA_IN;
		SCL_LO;
		I2C_HALFBIT_DELAY();
	}
	else
	{
		for (uint16_t i = 0x100; i != 0; i >>= 1)
		{
			i2c_sclLo(I2C_DEV(i2c));
			if (data & i)
				i2c_sdaHi(I2C_DEV(i2c));
			else
				i2c_sdaLo(I2C_DEV(i2c));
			i2c_halfbitDelay(I2C_DEV(i2c));

			i2c_sclHi(I2C_DEV(i2c));
			i2c_halfbitDelay(I2C_DEV(i2c));
		}
		ack = !i2c_sdaIn(I2C_DEV(i2c));

		i2c_sclLo(I2C_DEV(i2c));
		i2c_halfbitDelay(I2C_DEV(i2c));
	}

	if (!ack)
		i2c->errors |= I2C_NO_ACK;

	/* Generate stop condition (if requested) */
	if (stop || i2c->errors)
		i2c_bitbang_stop_1(i2c);
}

static void i2c_bitbang_putc(struct I2c *i2c, uint8_t data)
{
	i2c_bitbang_putcStop(i2c, data,
		(i2c->xfer_size == 1) && (i2c->flags & I2C_STOP));
}

static void i2c_bitbang_start_2(struct I2c *i2c, uint16_t slave_addr)
{
	if (i2c->flags & I2C_START_R)
		slave_addr |= I2C_READBIT;
	else
		slave_addr &= ~I2C_READBIT;

	/*
	 * Loop on the select write sequence: when the device is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the device responds with an ACK.
	 */
	ticks_t start = timer_clock();
	while (i2c_bitbang_start_1(i2c))
	{
		i2c_bitbang_putcStop(i2c, slave_addr, false);

		if (!(i2c->errors & I2C_NO_ACK))
			return;
		else if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C start\n");
			i2c->errors |= I2C_START_TIMEOUT;
			i2c_bitbang_stop_1(i2c);
			return;
		}
	}

	LOG_ERR("START arbitration lost\n");
	i2c->errors |= I2C_ARB_LOST;
	i2c_bitbang_stop_1(i2c);
	return;
}


static const I2cVT i2c_bitbang_vt =
{
	.start = i2c_bitbang_start_2,
	.getc = i2c_bitbang_getc,
	.putc = i2c_bitbang_putc,
	.write = i2c_genericWrite,
	.read = i2c_genericRead,
};


/**
 * Initialize i2c module.
 */
void i2c_hw_bitbangInit(I2c *i2c, int dev)
{
	MOD_CHECK(timer);
	if (dev == I2C_BITBANG_OLD)
		old_api = true;
	else
		i2c->hw = (struct I2cHardware *)(dev - I2C_BITBANG0);

	i2c->vt = &i2c_bitbang_vt;

	if (old_api)
	{
		I2C_BITBANG_HW_INIT;
		SDA_HI;
		SCL_HI;
	}
	else
	{
		i2c_bitbangInit(I2C_DEV(i2c));
		i2c_sdaHi(I2C_DEV(i2c));
		i2c_sclHi(I2C_DEV(i2c));
	}
}

