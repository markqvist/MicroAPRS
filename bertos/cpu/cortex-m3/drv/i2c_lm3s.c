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
 * \brief Driver for the LM3S I2C (implementation)
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>

#include <cfg/debug.h>
#include <cfg/macros.h> // BV()

#include <cpu/detect.h>
#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <io/lm3s.h>

#include <drv/timer.h>
#include <drv/i2c.h>
#include <drv/gpio_lm3s.h>
#include <drv/clock_lm3s.h>


struct I2cHardware
{
	uint32_t base;
	uint32_t sys_cntl;
	uint32_t sys_gpio;
	uint32_t pin_mask;
	uint32_t gpio_base;
	bool first_xtranf;
};

#define WAIT_BUSY(base) \
	do { \
		while (HWREG(base + I2C_O_MCS) & I2C_MCS_BUSY ) \
			cpu_relax(); \
	} while (0);


/*
 * The start is not performed when we call the start function
 * because the hardware should know the first data byte to send.
 * Generally to perform a byte send we should write the slave address
 * in slave address register and the first byte to send in data registry.
 * After then we can perform the start write procedure, and send really
 * the our data. To use common bertos i2c api the really start will be
 * performed when the user "put" or "send" its data. These tricks are hide
 * from the driver implementation.
 */
static void i2c_lm3s_start(struct I2c *i2c, uint16_t slave_addr)
{
	i2c->hw->first_xtranf = true;

	if (I2C_TEST_START(i2c->flags) == I2C_START_W)
		HWREG(i2c->hw->base + I2C_O_MSA) = slave_addr & ~BV(0);
	else /* (I2C_TEST_START(i2c->flags) == I2C_START_R) */
		HWREG(i2c->hw->base + I2C_O_MSA) = slave_addr | BV(0);
}

INLINE bool wait_addrAck(I2c *i2c, uint32_t mode_mask)
{
	ticks_t start = timer_clock();
	while (1)
	{
		uint32_t status = HWREG(i2c->hw->base + I2C_O_MCS);

		if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
			return false;

		if(status & I2C_MCS_ADRACK)
		{
			HWREG(i2c->hw->base + I2C_O_MCS) = mode_mask;
			WAIT_BUSY(i2c->hw->base);
		}
		else
			break;

		cpu_relax();
	}
	return true;
}

static void i2c_lm3s_putc(I2c *i2c, const uint8_t data)
{
	HWREG(i2c->hw->base + I2C_O_MDR) = data;

	if (i2c->hw->first_xtranf)
	{
		HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_RUN | I2C_MCS_START;
		while( HWREG(i2c->hw->base + I2C_O_MCS) & I2C_MCS_BUSY );

		if (!wait_addrAck(i2c, I2C_MCS_RUN | I2C_MCS_START))
		{
			LOG_ERR("Start timeout\n");
			i2c->errors |= I2C_START_TIMEOUT;
			HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_STOP;
			WAIT_BUSY(i2c->hw->base);
			return;
		}

		i2c->hw->first_xtranf = false;
	}
	else
	{
		HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_RUN;
		WAIT_BUSY(i2c->hw->base);
	}

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
	{
		HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_STOP;
		WAIT_BUSY(i2c->hw->base);
	}
}

static uint8_t i2c_lm3s_getc(I2c *i2c)
{
	uint8_t data;
	if (i2c->hw->first_xtranf)
	{
		uint32_t start_mode;
		if (i2c->xfer_size == 1)
			start_mode = I2C_MCS_RUN | I2C_MCS_START;
		else
			start_mode = I2C_MCS_ACK | I2C_MCS_RUN | I2C_MCS_START;

		HWREG(i2c->hw->base + I2C_O_MCS) = start_mode;
		WAIT_BUSY(i2c->hw->base);
		if (!wait_addrAck(i2c, start_mode))
		{
			LOG_ERR("Start timeout\n");
			i2c->errors |= I2C_START_TIMEOUT;
			HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_STOP;
			WAIT_BUSY(i2c->hw->base);
			return 0xFF;
		}

		data = HWREG(i2c->hw->base + I2C_O_MDR);
		i2c->hw->first_xtranf = false;
	}
	else
	{
		if (i2c->xfer_size > 1)
			HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_ACK | I2C_MCS_RUN;
		else
			HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_RUN;

		WAIT_BUSY(i2c->hw->base);
		data = HWREG(i2c->hw->base + I2C_O_MDR);
	}

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
	{
		HWREG(i2c->hw->base + I2C_O_MCS) = I2C_MCS_STOP;
		WAIT_BUSY(i2c->hw->base);
	}
	return data;
}

static const I2cVT i2c_lm3s_vt =
{
	.start = i2c_lm3s_start,
	.getc = i2c_lm3s_getc,
	.putc = i2c_lm3s_putc,
	.write = i2c_genericWrite,
	.read = i2c_genericRead,
};

static struct I2cHardware i2c_lm3s_hw[] =
{
	{ /* I2C0 */
		.base = I2C0_MASTER_BASE,
		.sys_cntl = SYSCTL_RCGC1_I2C0,
		.sys_gpio = SYSCTL_RCGC2_GPIOB,
		.pin_mask = (GPIO_I2C0_SCL_PIN | GPIO_I2C0_SDA_PIN),
		.gpio_base = GPIO_PORTB_BASE,
	},
	{ /* I2C1 */
		.base = I2C1_MASTER_BASE,
		.sys_cntl = SYSCTL_RCGC1_I2C1,
		.sys_gpio = SYSCTL_RCGC2_GPIOA,
		.pin_mask = (GPIO_I2C1_SCL_PIN | GPIO_I2C1_SDA_PIN),
		.gpio_base = GPIO_PORTA_BASE,
	},
};

/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{
	i2c->hw = &i2c_lm3s_hw[dev];
	i2c->vt = &i2c_lm3s_vt;

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= i2c->hw->sys_cntl;
	SYSCTL_RCGC2_R |= i2c->hw->sys_gpio;

	/* Configure GPIO pins to work as I2C pins */
	lm3s_gpioPinConfig(i2c->hw->gpio_base, i2c->hw->pin_mask,
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);
	/*
	 * Note: to set correctly the i2c speed we shold before enable the i2c
	 * device and then set in master time period the correct value
	 */

	/* Enable i2c device */
	HWREG(i2c->hw->base + I2C_O_MCR) |= I2C_MCR_MFE;

    /*
	 * Compute the clock divider that achieves the fastest speed less than or
     * equal to the desired speed.  The numerator is biased to favor a larger
     * clock divider so that the resulting clock is always less than or equal
     * to the desired clock, never greater.
	 */
    HWREG(i2c->hw->base + I2C_O_MTPR) = ((CPU_FREQ + (2 * 10 * clock) - 1) / (2 * 10 * clock)) - 1;
}
