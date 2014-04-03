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
 * All Rights Reserved.
 * -->
 *
 * \brief Rotating Hash algorithm.
 *
 * This is a simple yet powerfull checksum algorithm.
 * Instead of just xor-ing the data, rotating hash
 * circular shift the checksum 4 place left before xoring.
 * This is a bit more stronger than simply sum the data.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "rotating_hash"
 */


#ifndef ALGO_ROTATING_H
#define ALGO_ROTATING_H

#include <cfg/compiler.h>

typedef uint16_t rotating_t;


/**
 * Init rotating checksum.
 */
INLINE void rotating_init(rotating_t *rot)
{
	*rot = 0;
}

/**
 * Update checksum pointed by \c rot with \c c data.
 */
INLINE void rotating_update1(uint8_t c, rotating_t *rot)
{
	*rot = (*rot << 4) ^ (*rot >> 12) ^ c;
}

/**
 * Update checksum pointed by \c rot with data supplied in \c buf.
 */
INLINE void rotating_update(const void *_buf, size_t len, rotating_t *rot)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (len--)
		rotating_update1(*buf++, rot);
}


#endif // ALGO_ROTATING_H
