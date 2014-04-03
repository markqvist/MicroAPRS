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
 * \brief CCITT Cyclic Redundancy Check (CRC-CCITT).
 *
 * \note This algorithm is incompatible with the CRC16.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "crc-ccitt"
 */

#ifndef ALGO_CRC_CCITT_H
#define ALGO_CRC_CCITT_H

#include <cfg/compiler.h>
#include <cpu/pgm.h>

EXTERN_C_BEGIN

/* CRC table */
extern const uint16_t crc_ccitt_tab[256];

/**
 * \brief Compute the updated CRC-CCITT value for one octet (inline version)
 */
INLINE uint16_t updcrc_ccitt(uint8_t c, uint16_t oldcrc)
{
	return (oldcrc >> 8) ^ pgm_read16(&crc_ccitt_tab[(oldcrc ^ c) & 0xff]);
}

/** CRC-CCITT init value */
#define CRC_CCITT_INIT_VAL ((uint16_t)0xFFFF)


/**
 * This function implements the CRC-CCITT calculation on a buffer.
 *
 * \param crc  Current CRC-CCITT value.
 * \param buf  The buffer to perform CRC calculation on.
 * \param len  The length of the Buffer.
 *
 * \return The updated CRC-CCITT value.
 */
extern uint16_t crc_ccitt(uint16_t crc, const void *buf, size_t len);

EXTERN_C_END

#endif /* ALGO_CRC_CCITT_H */
