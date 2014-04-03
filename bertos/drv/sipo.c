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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \brief SIPO Module
 *
 * The SIPO module transforms a serial input in a parallel output. Please check hw_sipo.h
 * file to customize hardware related parameters.
 *
 * \author Andrea Grandi <andrea@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "sipo.h"

#include "hw/hw_sipo.h"

#include <cfg/compiler.h>
#include <cfg/log.h>

#include <io/kfile.h>

#include <string.h>


#define SIPO_DATAORDER_START(order)          (order ? SIPO_DATAORDER_START_LSB : SIPO_DATAORDER_START_MSB)
#define SIPO_DATAORDER_SHIFT(shift, order)   (order ?  ((shift) <<= 1) : ((shift) >>= 1))

/**
 * Write a char in sipo shift register
 */
INLINE void sipo_putchar(uint8_t c, uint8_t bit_order, uint8_t clock_pol)
{
	uint8_t shift = SIPO_DATAORDER_START(bit_order);

	for(int i = 0; i < 8; i++)
	{
		if((c & shift) == 0)
			SIPO_SI_LOW();
		else
			SIPO_SI_HIGH();

		SIPO_SI_CLOCK(clock_pol);

		SIPO_DATAORDER_SHIFT(shift, bit_order);
	}
}

/**
 * Write a buffer into the sipo register and, when finished, give a load pulse.
 */
static size_t sipo_write(struct KFile *_fd, const void *_buf, size_t size)
{
	const uint8_t *buf = (const uint8_t *)_buf;
	Sipo *fd = SIPO_CAST(_fd);
	size_t write_len = size;

	ASSERT(buf);

	SIPO_SET_SI_LEVEL();
	SIPO_SET_CLK_LEVEL(fd->clock_pol);
	SIPO_SET_LD_LEVEL(fd->load_device, fd->load_pol);

	// Load into the shift register all the buffer bytes
	while(size--)
		sipo_putchar(*buf++, fd->bit_order, fd->clock_pol);

	// We finsh to load bytes, so load it.
	SIPO_LOAD(fd->load_device, fd->load_pol);

	return write_len;
}

/**
 * Initialize the SIPO
 */
void sipo_init(Sipo *fd)
{
	ASSERT(fd);

	memset(fd, 0, sizeof(Sipo));

	//Set kfile struct type as a generic kfile structure.
	DB(fd->fd._type = KFT_SIPO);

	// Set up SIPO writing functions.
	fd->fd.write = sipo_write;

	SIPO_INIT_PIN();

	/* Enable sipo output */
	SIPO_ENABLE();
}
