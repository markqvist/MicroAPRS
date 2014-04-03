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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TWI driver for SAM3 (implementation)
 *
 * Only master mode is supported.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */


#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>

#include <hw/hw_cpufreq.h>  // CPU_FREQ
#include <cfg/debug.h>
#include <cfg/macros.h> // BV()
#include <cfg/module.h>
#include <cpu/detect.h>
#include <cpu/irq.h>
#include <cpu/power.h>
#include <drv/timer.h>
#include <drv/i2c.h>
#include <io/sam3.h>


struct I2cHardware
{
	uint32_t base;
	bool     first_xtranf;
};


INLINE bool waitTxRdy(I2c *i2c, time_t ms_timeout)
{
	ticks_t start = timer_clock();

	while (!(HWREG(i2c->hw->base + TWI_SR_OFF) & TWI_SR_TXRDY))
	{
		if (timer_clock() - start > ms_to_ticks(ms_timeout))
			return false;
		cpu_relax();
	}

	return true;
}

INLINE bool waitRxRdy(I2c *i2c, time_t ms_timeout)
{
	ticks_t start = timer_clock();

	while (!(HWREG(i2c->hw->base + TWI_SR_OFF) & TWI_SR_RXRDY))
	{
		if (timer_clock() - start > ms_to_ticks(ms_timeout))
			return false;
		cpu_relax();
	}

	return true;
}

INLINE void waitXferComplete(I2c *i2c)
{
	while (!(HWREG(i2c->hw->base + TWI_SR_OFF) & TWI_SR_TXCOMP))
		cpu_relax();
}


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
static void i2c_sam3_start(struct I2c *i2c, uint16_t slave_addr)
{
	i2c->hw->first_xtranf = true;

	if (I2C_TEST_START(i2c->flags) == I2C_START_R)
		HWREG(i2c->hw->base + TWI_MMR_OFF) = TWI_MMR_DADR(slave_addr >> 1) | TWI_MMR_MREAD;
	else
		HWREG(i2c->hw->base + TWI_MMR_OFF) = TWI_MMR_DADR(slave_addr >> 1);
}

static void i2c_sam3_putc(I2c *i2c, const uint8_t data)
{
	if (!waitTxRdy(i2c, CONFIG_I2C_START_TIMEOUT))
	{
		LOG_ERR("i2c: txready timeout\n");
		i2c->errors |= I2C_START_TIMEOUT;
		return;
	}

	HWREG(i2c->hw->base + TWI_THR_OFF) = data;

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
		HWREG(i2c->hw->base + TWI_CR_OFF) = TWI_CR_STOP;

	// On first byte sent wait for start timeout
	if (i2c->hw->first_xtranf && !waitTxRdy(i2c, CONFIG_I2C_START_TIMEOUT))
	{
		LOG_ERR("i2c: write start timeout\n");
		i2c->errors |= I2C_START_TIMEOUT;
		HWREG(i2c->hw->base + TWI_CR_OFF) = TWI_CR_STOP;
		waitXferComplete(i2c);
		return;
	}
	i2c->hw->first_xtranf = false;

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
		waitXferComplete(i2c);
}

static uint8_t i2c_sam3_getc(I2c *i2c)
{
	uint8_t data;
	uint32_t cr = 0;

	if (i2c->hw->first_xtranf)
	{
		cr |= TWI_CR_START;
		i2c->hw->first_xtranf = false;
	}
	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
		cr |= TWI_CR_STOP;

	HWREG(i2c->hw->base + TWI_CR_OFF) = cr;

	if (!waitRxRdy(i2c, CONFIG_I2C_START_TIMEOUT))
	{
		LOG_ERR("i2c: read start timeout\n");
		i2c->errors |= I2C_START_TIMEOUT;
		return 0xFF;
	}

	data = HWREG(i2c->hw->base + TWI_RHR_OFF);

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
		waitXferComplete(i2c);

	return data;
}

static void i2c_setClock(I2c *i2c, int clock)
{
	uint32_t ck_div = 0;
	uint32_t cl_div;

	for (;;)
	{
		cl_div = ((CPU_FREQ / (2 * clock)) - 4) / (1 << ck_div);

		if (cl_div <= 255)
			break;

		ck_div++;
	}

	ASSERT(ck_div < 8);
	LOG_INFO("i2c: using CKDIV = %lu and CLDIV/CHDIV = %lu\n\n", ck_div, cl_div);

	HWREG(i2c->hw->base + TWI_CWGR_OFF) = 0;
	HWREG(i2c->hw->base + TWI_CWGR_OFF) = (ck_div << 16) | (cl_div << 8) | cl_div;
}


static const I2cVT i2c_sam3_vt =
{
	.start = i2c_sam3_start,
	.getc = i2c_sam3_getc,
	.putc = i2c_sam3_putc,
	.write = i2c_genericWrite,
	.read = i2c_genericRead,
};

struct I2cHardware i2c_sam3_hw[I2C_CNT];


/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{
	ASSERT(dev < I2C_CNT);

	i2c->hw = &i2c_sam3_hw[dev];
	i2c->vt = &i2c_sam3_vt;

	pmc_periphEnable(PIOA_ID);

	switch (dev)
	{
		case I2C0:
			i2c->hw->base = TWI0_BASE;
			PIO_PERIPH_SEL(TWI0_PORT, BV(TWI0_TWD) | BV(TWI0_TWCK), TWI0_PERIPH);
			HWREG(TWI0_PORT + PIO_PDR_OFF) = BV(TWI0_TWD) | BV(TWI0_TWCK);
			pmc_periphEnable(TWI0_ID);
			break;
		case I2C1:
			i2c->hw->base = TWI1_BASE;
			PIO_PERIPH_SEL(TWI1_PORT, BV(TWI1_TWD) | BV(TWI1_TWCK), TWI1_PERIPH);
			HWREG(TWI1_PORT + PIO_PDR_OFF) = BV(TWI1_TWD) | BV(TWI1_TWCK);
			pmc_periphEnable(TWI1_ID);
			break;
		default:
			ASSERT(!"i2c: invalid dev number");
			return;
	}


	// Reset and set master mode
	HWREG(i2c->hw->base + TWI_CR_OFF) = TWI_CR_SWRST;
	HWREG(i2c->hw->base + TWI_CR_OFF) = TWI_CR_MSEN | TWI_CR_SVDIS;

	i2c_setClock(i2c, clock);
}
