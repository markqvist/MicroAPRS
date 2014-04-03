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
 * -->
 *
 *
 * \brief TC520 ADC driver (implementation)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Marco Benelli <marco@develer.com>
 */


#include "hw/hw_tc520.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <drv/tc520.h>
#include <drv/timer.h>

#warning FIXME:This implementation is obsolete. Refactor with KFile interface.

#if 0

#include <drv/ser.h>

static Serial *spi_ser;

#define TC520_CONVERSION_TIMEOUT ms_to_ticks(1000)
#define INIT_LOAD_VALUE 0x00

/**
 * Start an AD conversion and return result.
 *
 * To start a conversion first we must pull down CE pin.
 * The ADC starts a convertion and keeps the DV pin high until the end.
 * At this point, we can read the conversion value by SPI.
 * The convertion result is yield in 3 bytes.
 *
 * \verbatim
 *
 * First byte:
 * bit | Value
 * ----|-------
 *  7  | Overrange
 *  6  | Polarity
 * 5:0 | data bits 15:10
 *
 * Second byte: data 9:2
 *
 * Third byte:
 * bit | Value
 * ----|-------
 *  7  | data bit 1
 *  6  | data bit 0
 * 5:0 | '0'
 *
 * \endverbatim
 *
 * So, to get the result we must shift and recompose the bits.
 * \note Overrange bit is handled as the 17th data bit.
 */
tc520_data_t tc520_read(void)
{
	/* Start convertion and wait */
	CE_LOW();
	ticks_t start = timer_clock();
	do
	{
		/* timeout check */
		if (timer_clock() - start >= TC520_CONVERSION_TIMEOUT)
		{
			ASSERT(0);
			CE_HIGH();
			return TC520_MAX_VALUE;
		}
	}
	while(DV_LOW());

	start = timer_clock();
	do
	{
		/* timeout check */
		if (timer_clock() - start >= TC520_CONVERSION_TIMEOUT)
		{
			ASSERT(0);
			CE_HIGH();
			return TC520_MAX_VALUE;
		}
	}
	while(DV_HIGH());

	/* Ok, convertion finished, read result */
	CE_HIGH();
	READ_LOW();

	/* RX buffer could be dirty...*/
	ser_purge(spi_ser);

	/* I/O buffer */
	uint8_t buf[3] = "\x0\x0\x0";

	/* Dummy write to activate recv */
	ser_write(spi_ser, buf, sizeof(buf));
	ser_drain(spi_ser);
	READ_HIGH();

	/* recv */
	ASSERT(ser_read(spi_ser, buf, sizeof(buf)) == sizeof(buf));

	tc520_data_t res;

	/* Recompose data */
	res = (((tc520_data_t)(buf[0] & 0x3F)) << 10) | (((tc520_data_t)buf[1]) << 2) | (((tc520_data_t)buf[2]) >> 6);

	#define OVERRANGE_BIT BV(7)
	/* Handle overrange bit as 17th bit */
	if (buf[0] & OVERRANGE_BIT)
		res |= BV32(16);

	return res;
}


/**
 * Initialize tc520 A/D converter driver
 */
void tc520_init(Serial *spi_port)
{
	spi_ser = spi_port;
	/* init io ports */
	TC520_HW_INIT;
	/* Send initial load value */
	LOAD_LOW();
	ser_putchar(INIT_LOAD_VALUE, spi_ser);
	ser_drain(spi_ser);
	LOAD_HIGH();
}

#endif
