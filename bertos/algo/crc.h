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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Cyclic Redundancy Check 16 (CRC). This algorithm is the one used by the XMODEM protocol.
 *
 * \note This algorithm is incompatible with the CCITT-CRC16.
 *
 * This code is based on the article Copyright 1986 Stephen Satchell.
 *
 * Programmers may incorporate any or all code into their programs,
 * giving proper credit within the source. Publication of the
 * source routines is permitted so long as proper credit is given
 * to Stephen Satchell, Satchell Evaluations and Chuck Forsberg,
 * Omen Technology.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "crc16"
 */

#ifndef ALGO_CRC_H
#define ALGO_CRC_H

#include "cfg/cfg_arch.h"

#include <cfg/compiler.h>
#include <cpu/pgm.h>

EXTERN_C_BEGIN

/* CRC table */
extern const uint16_t crc16tab[256];


/**
 * \brief Compute the updated CRC16 value for one octet (macro version)
 *
 * \note This version is only intended for old/broken compilers.
 *       Use the inline function in new code.
 *
 * \param c New octet (range 0-255)
 * \param oldcrc Previous CRC16 value (referenced twice, beware of side effects)
 */
#if CPU_HARVARD && !(defined(ARCH_BOOT) && (ARCH & ARCH_BOOT))
	#define UPDCRC16(c, oldcrc) (pgm_read_uint16_t(&crc16tab[((oldcrc) >> 8) ^ ((unsigned char)(c))]) ^ ((oldcrc) << 8))
#else
	#define UPDCRC16(c, oldcrc) ((crc16tab[((oldcrc) >> 8) ^ ((unsigned char)(c))]) ^ ((oldcrc) << 8))
#endif

/** CRC-16 init value */
#define CRC16_INIT_VAL ((uint16_t)0)

#ifdef INLINE
/**
 * \brief Compute the updated CRC16 value for one octet (inline version)
 */
INLINE uint16_t updcrc16(uint8_t c, uint16_t oldcrc)
{
#if CPU_HARVARD && !(defined(ARCH_BOOT) && (ARCH & ARCH_BOOT))
	return pgm_read_uint16_t(&crc16tab[(oldcrc >> 8) ^ c]) ^ (oldcrc << 8);
#else
	return crc16tab[(oldcrc >> 8) ^ c] ^ (oldcrc << 8);
#endif
}
#endif // INLINE


/**
 * This function implements the CRC 16 calculation on a buffer.
 *
 * \param crc  Current CRC16 value.
 * \param buf  The buffer to perform CRC calculation on.
 * \param len  The length of the Buffer.
 *
 * \return The updated CRC16 value.
 */
extern uint16_t crc16(uint16_t crc, const void *buf, size_t len);

int crc_testSetup(void);
int crc_testRun(void);
int crc_testTearDown(void);

EXTERN_C_END

#endif /* ALGO_CRC_H */
