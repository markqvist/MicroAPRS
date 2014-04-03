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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the AT91 ARM TWI (implementation)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "twi_at91.h"

#include "cfg/cfg_i2c.h"
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cfg/module.h>

#include <drv/timer.h>

#include <io/arm.h>

/**
 * Timeout for ACK slave waiting.
 */
#define TWI_TIMEOUT ms_to_ticks(50)

/**
 * Send \a size bytes over the twi line to slave \a id.
 * If the device requires internal addressing before writing, \a byte1 \a byte2 and \a byte3 can
 * be specified. Internal addressign bytes not used *must* be set to TWI_NO_IADDR. If 1 or 2 bytes
 * are required for internal addressing you *must* first use \a byte1 and than \a byte2.
 * \note Atmel TWI implementation is broken so it was not possible to supply a better
 *       interface. Additionally NACK handling is also broken, so if the i2c device reply nack
 *       this function will return after TWI_TIMEOUT.
 * \return true if ok, false on slave timeout.
 */
bool twi_write(uint8_t id, twi_iaddr_t byte1, twi_iaddr_t byte2, twi_iaddr_t byte3, const void *_buf, size_t size)
{
	uint8_t addr_size = 0;
	const uint8_t *buf = (const uint8_t *)_buf;
	ticks_t start;

	/* At least 1 byte *must* be transmitted, thanks to crappy hw implementation */
	ASSERT(size >= 1);

	/* Check internal byte address presence */
	if (byte1 != TWI_NO_IADDR)
		addr_size++;

	if (byte2 != TWI_NO_IADDR)
	{
		ASSERT(addr_size == 1);
		addr_size++;
	}

	if (byte3 != TWI_NO_IADDR)
	{
		ASSERT(addr_size == 2);
		addr_size++;
	}

	start = timer_clock();
	/* Wait tx buffer empty */
	while (!(TWI_SR & BV(TWI_TXRDY)))
	{
		if (timer_clock() - start > TWI_TIMEOUT)
			return false;
	}

	/* Set slave address and (optional) internal slave addresses */
	TWI_MMR = (uint32_t)id << TWI_DADR_SHIFT | (uint32_t)addr_size << TWI_IADRSZ_SHIFT;

	TWI_IADR = ((uint32_t)(byte3 & 0xff) << 16) | ((uint32_t)(byte2 & 0xff) << 8) | ((uint32_t)(byte1 & 0xff));

	while (size--)
	{
		/* Send data */
		TWI_THR = *buf++;

		start = timer_clock();
		/* Wait tx buffer empty */
		while (!(TWI_SR & BV(TWI_TXRDY)))
		{
			if (timer_clock() - start > TWI_TIMEOUT)
				return false;
		}
	}

	/* Wait transmit complete bit */
	start = timer_clock();
	while (!(TWI_SR & BV(TWI_TXCOMP)))
	{
		if (timer_clock() - start > TWI_TIMEOUT)
			return false;
	}

	return true;
}


/**
 * Read \a size bytes from the twi line from slave \a id.
 * If the device requires internal addressing before reading, \a byte1 \a byte2 and \a byte3 must
 * be specified. Internal addressign bytes not used *must* be set to TWI_NO_IADDR. If 1 or 2 bytes
 * are required for internal addressing you *must* first use \a byte1 and than \a byte2.
 * \note Atmel TWI implementation is broken so it was not possible to supply a better
 *       interface. Additionally NACK handling is also broken, so if the i2c device reply nack
 *       this function will return after TWI_TIMEOUT.
 * \return true if ok, false on slave timeout.
 */
bool twi_read(uint8_t id, twi_iaddr_t byte1, twi_iaddr_t byte2, twi_iaddr_t byte3, void *_buf, size_t size)
{
	uint8_t addr_size = 0;
	uint8_t *buf = (uint8_t *)_buf;
	bool stopped = false;
	ticks_t start;

	/* At least 1 byte *must* be transmitted, thanks to crappy twi implementation */
	ASSERT(size >= 1);

	/* Check internal byte address presence */
	if (byte1 != TWI_NO_IADDR)
		addr_size++;

	if (byte2 != TWI_NO_IADDR)
	{
		ASSERT(addr_size == 1);
		addr_size++;
	}

	if (byte3 != TWI_NO_IADDR)
	{
		ASSERT(addr_size == 2);
		addr_size++;
	}

	/* Wait tx buffer empty */
	start = timer_clock();
	while (!(TWI_SR & BV(TWI_TXRDY)))
	{
		if (timer_clock() - start > TWI_TIMEOUT)
			return false;
	}


	/* Set slave address and (optional) internal slave addresses */
	TWI_MMR = ((uint32_t)id << TWI_DADR_SHIFT) | BV(TWI_MREAD) | ((uint32_t)addr_size << TWI_IADRSZ_SHIFT);

	TWI_IADR = ((uint32_t)(byte3 & 0xff) << 16) | ((uint32_t)(byte2 & 0xff) << 8) | ((uint32_t)(byte1 & 0xff));

	/*
	 * Start reception.
	 * Kludge: if we want to receive only 1 byte, the stop but *must* be set here
	 * (thanks to crappy twi implementation again).
	 */
	if (size == 1)
	{
		TWI_CR = BV(TWI_START) | BV(TWI_STOP);
		stopped = true;
	}
	else
		TWI_CR = BV(TWI_START);

	while (size--)
	{
		/* If we are at the last byte, inform the crappy hw that we
		   want to stop the reception. */
		if (!size && !stopped)
			TWI_CR = BV(TWI_STOP);

		/* Wait until a byte is received */
		start = timer_clock();
		while (!(TWI_SR & BV(TWI_RXRDY)))
		{
			if (timer_clock() - start > TWI_TIMEOUT)
			{
				TWI_CR = BV(TWI_STOP);
				return false;
			}
		}


		*buf++ = TWI_RHR;
	}

	/* Wait transmit complete bit */
	start = timer_clock();
	while (!(TWI_SR & BV(TWI_TXCOMP)))
	{
		if (timer_clock() - start > TWI_TIMEOUT)
			return false;
	}

	return true;
}

MOD_DEFINE(twi);

/**
 * Init the (broken) sam7 twi driver.
 */
void twi_init(void)
{
	/* Disable PIO on TWI pins */
	PIOA_PDR = BV(TWD) | BV(TWCK);

	/* Enable oper drain on TWI pins */
	PIOA_MDER = BV(TWD);

	/* Disable all irqs */
	TWI_IDR = 0xFFFFFFFF;

	TWI_CR = BV(TWI_SWRST);

	/* Enable master mode */
	TWI_CR = BV(TWI_MSEN);

	PMC_PCER = BV(TWI_ID);

	/*
	 * Compute twi clock.
	 * CLDIV = ((Tlow * 2^CKDIV) -3) * Tmck
	 * CHDIV = ((THigh * 2^CKDIV) -3) * Tmck
	 * Only CLDIV is computed since CLDIV = CHDIV (50% duty cycle)
	 */
	uint16_t cldiv, ckdiv = 0;
	while ((cldiv = ((CPU_FREQ / (2 * CONFIG_I2C_FREQ)) - 3) / (1 << ckdiv)) > 255)
		ckdiv++;

	/* Atmel errata states that ckdiv *must* be less than 5 for unknown reason */
	ASSERT(ckdiv < 5);

	TWI_CWGR = ((uint32_t)ckdiv << TWI_CKDIV_SHIFT) | (cldiv << TWI_CLDIV_SHIFT) | (cldiv << TWI_CHDIV_SHIFT);
	TRACEMSG("TWI_CWGR [%08lx]", TWI_CWGR);

	MOD_INIT(twi);
}

