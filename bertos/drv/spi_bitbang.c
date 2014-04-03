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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Emulated SPI Master for DSP firmware download (impl.)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */


#include "spi_bitbang.h"
#include "hw/hw_spi.h"

#include "cfg/cfg_spi_bitbang.h"
#include <cfg/module.h>

#include <cpu/irq.h>

void spi_assertSS(void)
{
	ATOMIC(SS_ACTIVE());
}

void spi_deassertSS(void)
{
	ATOMIC(SS_INACTIVE());
}

/**
 * Send byte \c c over MOSI line, CONFIG_SPI_DATAORDER first.
 * SS pin state is left unchanged.
 */
uint8_t spi_sendRecv(uint8_t c)
{
	uint8_t data = 0;
	uint8_t shift = SPI_DATAORDER_START;


	ATOMIC(
		for (int i = 0; i < 8; i++)
		{
			/* Shift the i-th bit to MOSI */
			if (c & shift)
				MOSI_HIGH();
			else
				MOSI_LOW();
			/* Assert clock */
			SCK_ACTIVE();
			data |= IS_MISO_HIGH() ? shift : 0;
			/* De-assert clock */
			SCK_INACTIVE();
			SPI_DATAORDER_SHIFT(shift);
		}
	);
	return data;
}

MOD_DEFINE(spi);
void spi_init(void)
{
	ATOMIC(SPI_HW_INIT());
	MOD_INIT(spi);
}

/**
 * Read \param len from spi, and put it in \param _buff .
 */
void spi_read(void *_buff, size_t len)
{
	uint8_t *buff = (uint8_t *)_buff;

	while (len--)
		/* Read byte from spi and put it in buffer. */
		*buff++ = spi_sendRecv(0);

}

/**
 * Write \param len to spi, and take it from \param _buff .
 */
void spi_write(const void *_buff, size_t len)
{
	const uint8_t *buff = (const uint8_t *)_buff;

	while (len--)
		/* Write byte pointed at by *buff to spi */
		spi_sendRecv(*buff++);

}
