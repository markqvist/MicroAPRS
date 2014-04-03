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
 * \brief Driver for the LPC23xx I2C (implementation)
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>

#include <cfg/debug.h>
#include <cfg/macros.h> // BV()

#include <cpu/detect.h>
#include <cpu/irq.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/i2c.h>

#include <io/lpc23xx.h>

struct I2cHardware
{
	uint32_t base;
	uint32_t pconp;
	uint32_t pinsel_port;
	uint32_t pinsel;
	uint32_t pinsel_mask;
	uint32_t pclksel;
	uint32_t pclk_mask;
	uint32_t pclk_div;
};

/*
 * Wait that SI bit is set.
 *
 * Note: this bit is set when the I2C state changes. However, entering
 * state F8 does not set SI since there is nothing for an interrupt service
 * routine to do in that case.
 */
#define WAIT_SI(i2c) \
		do { \
			ticks_t start = timer_clock(); \
			while( !(HWREG(i2c->hw->base + I2C_CONSET_OFF) & BV(I2CON_SI)) ) \
			{ \
				if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT)) \
				{ \
					LOG_ERR("Timeout SI assert\n"); \
					LOG_ERR("[%08lx]\n", HWREG(i2c->hw->base + I2C_STAT_OFF)); \
					break; \
				} \
				cpu_relax(); \
			} \
	} while (0)

static void i2c_hw_restart(I2c *i2c)
{
	// Clear all pending flags.
	HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	// Set start and ack bit.
	HWREG(i2c->hw->base + I2C_CONSET_OFF) = BV(I2CON_STA);

	WAIT_SI(i2c);
}


static void i2c_hw_stop(I2c *i2c)
{
	/* Set the stop bit */
	HWREG(i2c->hw->base + I2C_CONSET_OFF) = BV(I2CON_STO);
	/* Clear pending flags */
	HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);
}

static void i2c_lpc2_putc(I2c *i2c, uint8_t data)
{
	HWREG(i2c->hw->base + I2C_DAT_OFF) = data;
	HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_SIC);

	WAIT_SI(i2c);

	uint32_t status = HWREG(i2c->hw->base + I2C_STAT_OFF);


	/* Generate the stop if we finish to send all programmed bytes */
	if (i2c->xfer_size == 1)
	{
		if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
			i2c_hw_stop(i2c);
	}

	if (status == I2C_STAT_DATA_NACK)
	{
		LOG_ERR("Data NACK\n");
		i2c->errors |= I2C_NO_ACK;
		i2c_hw_stop(i2c);
	}
	else if ((status == I2C_STAT_ERROR) || (status == I2C_STAT_UNKNOW))
	{
		LOG_ERR("I2C error.\n");
		i2c->errors |= I2C_ERR;
		i2c_hw_stop(i2c);
	}
}

static uint8_t i2c_lpc2_getc(I2c *i2c)
{
	/*
	 * Set ack bit if we want read more byte, otherwise
	 * we disable it
	 */
	if (i2c->xfer_size > 1)
		HWREG(i2c->hw->base + I2C_CONSET_OFF) = BV(I2CON_AA);
	else
		HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_AAC);

	HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_SIC);

	WAIT_SI(i2c);

	uint32_t status = HWREG(i2c->hw->base + I2C_STAT_OFF);
	uint8_t data = (uint8_t)HWREG(i2c->hw->base + I2C_DAT_OFF);

	if (status == I2C_STAT_RDATA_ACK)
	{
		return data;
	}
	else if (status == I2C_STAT_RDATA_NACK)
	{
		/*
		 * last byte to read generate the stop if
		 * required
		 */
		if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
			i2c_hw_stop(i2c);

		return data;
	}
	else if ((status == I2C_STAT_ERROR) || (status == I2C_STAT_UNKNOW))
	{
		LOG_ERR("I2C error.\n");
		i2c->errors |= I2C_ERR;
		i2c_hw_stop(i2c);
	}

	return 0xFF;
}

static void i2c_lpc2_start(struct I2c *i2c, uint16_t slave_addr)
{
	if (I2C_TEST_START(i2c->flags) == I2C_START_W)
	{
		ticks_t start = timer_clock();
		while (true)
		{
			i2c_hw_restart(i2c);

			uint8_t status = HWREG(i2c->hw->base + I2C_STAT_OFF);

			/* Start status ok, set addres and the R/W bit */
			if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
				HWREG(i2c->hw->base + I2C_DAT_OFF) = slave_addr & ~I2C_READBIT;

			/* Clear the start bit and clear the SI bit */
			HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_SIC) | BV(I2CON_STAC);

			if (status == I2C_STAT_SLAW_ACK)
				break;

			if (status == I2C_STAT_ARB_LOST)
			{
				LOG_ERR("Arbitration lost\n");
				i2c->errors |= I2C_ARB_LOST;
				i2c_hw_stop(i2c);
			}

			if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
			{
				LOG_ERR("Timeout on I2C START\n");
				i2c->errors |= I2C_NO_ACK;
				i2c_hw_stop(i2c);
				break;
			}
		}
	}
	else if (I2C_TEST_START(i2c->flags) == I2C_START_R)
	{
		i2c_hw_restart(i2c);

		uint8_t status = HWREG(i2c->hw->base + I2C_STAT_OFF);

		/* Start status ok, set addres and the R/W bit */
		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			HWREG(i2c->hw->base + I2C_DAT_OFF) = slave_addr | I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_SIC) | BV(I2CON_STAC);

		WAIT_SI(i2c);

		status = HWREG(i2c->hw->base + I2C_STAT_OFF);

		if (status == I2C_STAT_SLAR_NACK)
		{
			LOG_ERR("SLAR NACK:%02x\n", status);
			i2c->errors |= I2C_NO_ACK;
			i2c_hw_stop(i2c);
		}

		if (status == I2C_STAT_ARB_LOST)
		{
			LOG_ERR("Arbitration lost\n");
			i2c->errors |= I2C_ARB_LOST;
			i2c_hw_stop(i2c);
		}
	}
	else
	{
		ASSERT(0);
	}
}

static const I2cVT i2c_lpc_vt =
{
	.start = i2c_lpc2_start,
	.getc = i2c_lpc2_getc,
	.putc = i2c_lpc2_putc,
	.write = i2c_genericWrite,
	.read = i2c_genericRead,
};

static struct I2cHardware i2c_lpc2_hw[] =
{
	{ /* I2C0 */
		.base = I2C0_BASE_ADDR,
		.pconp = BV(PCONP_PCI2C0),
		.pinsel_port = PINSEL1_OFF,
		.pinsel = I2C0_PINSEL,
		.pinsel_mask = I2C0_PINSEL_MASK,
		.pclksel = PCLKSEL0_OFF,
		.pclk_mask = I2C0_PCLK_MASK,
		.pclk_div = I2C0_PCLK_DIV8,
	},
	{ /* I2C1 */
		.base = I2C1_BASE_ADDR,
		.pconp = BV(PCONP_PCI2C1),
		.pinsel_port = PINSEL0_OFF,
		.pinsel = I2C1_PINSEL,
		.pinsel_mask = I2C1_PINSEL_MASK,
		.pclksel = PCLKSEL1_OFF,
		.pclk_mask = I2C1_PCLK_MASK,
		.pclk_div = I2C1_PCLK_DIV8,
	},
	{ /* I2C2 */
		.base = I2C2_BASE_ADDR,
		.pconp = BV(PCONP_PCI2C2),
		.pinsel_port = PINSEL0_OFF,
		.pinsel = I2C2_PINSEL,
		.pinsel_mask = I2C2_PINSEL_MASK,
		.pclksel = PCLKSEL1_OFF,
		.pclk_mask = I2C2_PCLK_MASK,
		.pclk_div = I2C2_PCLK_DIV8,
	},
};

/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{
	i2c->hw = &i2c_lpc2_hw[dev];
	i2c->vt = &i2c_lpc_vt;

	/* Enable I2C clock */
	PCONP |= i2c->hw->pconp;

	ASSERT(clock <= 400000);

	HWREG(i2c->hw->base + I2C_CONCLR_OFF) = BV(I2CON_I2ENC) | BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	/*
	 * Bit Frequency = Fplk / (I2C_I2SCLH + I2C_I2SCLL)
	 * value of I2SCLH and I2SCLL must be different
	 */
	HWREG(SCB_BASE_ADDR + i2c->hw->pclksel) &= ~i2c->hw->pclk_mask;
	HWREG(SCB_BASE_ADDR + i2c->hw->pclksel) |= i2c->hw->pclk_div;

	HWREG(i2c->hw->base + I2C_SCLH_OFF) = (((CPU_FREQ / 8) / clock) / 2) + 1;
	HWREG(i2c->hw->base + I2C_SCLL_OFF) = (((CPU_FREQ / 8) / clock) / 2);

	ASSERT(HWREG(i2c->hw->base + I2C_SCLH_OFF) > 4);
	ASSERT(HWREG(i2c->hw->base + I2C_SCLL_OFF) > 4);

	/* Assign pins to SCL and SDA */
	HWREG(PINSEL_BASE_ADDR + i2c->hw->pinsel_port) &= ~i2c->hw->pinsel_mask;
	HWREG(PINSEL_BASE_ADDR + i2c->hw->pinsel_port) |= i2c->hw->pinsel;

	// Enable I2C
	HWREG(i2c->hw->base + I2C_CONSET_OFF) = BV(I2CON_I2EN);
}
