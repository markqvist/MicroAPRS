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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TEA Tiny Encription Algorith functions (implementation).
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * The Tiny Encryption Algorithm (TEA) by David Wheeler and Roger Needham
 * of the Cambridge Computer Laboratory
 *
 * Placed in the Public Domain by David Wheeler and Roger Needham.
 *
 * **** ANSI C VERSION ****
 *
 * Notes:
 *
 * TEA is a Feistel cipher with XOR and and addition as the non-linear
 * mixing functions.
 *
 * Takes 64 bits of data in v[0] and v[1].  Returns 64 bits of data in w[0]
 * and w[1].  Takes 128 bits of key in k[0] - k[3].
 *
 * TEA can be operated in any of the modes of DES. Cipher Block Chaining is,
 * for example, simple to implement.
 *
 * n is the number of iterations. 32 is ample, 16 is sufficient, as few
 * as eight may be OK.  The algorithm achieves good dispersion after six
 * iterations. The iteration count can be made variable if required.
 *
 * Note this is optimised for 32-bit CPUs with fast shift capabilities. It
 * can very easily be ported to assembly language on most CPUs.
 *
 * delta is chosen to be the real part of (the golden ratio Sqrt(5/4) -
 * 1/2 ~ 0.618034 multiplied by 2^32).
 */

#include "tea.h"
#include <cpu/byteorder.h>

static uint32_t tea_func(uint32_t *in, uint32_t *sum, uint32_t *k)
{
	return ((*in << 4) + cpu_to_le32(k[0])) ^ (*in + *sum) ^ ((*in >> 5) + cpu_to_le32(k[1]));
}

/**
 * \brief TEA encryption function.
 * This function encrypts <EM>v</EM> with <EM>k</EM> and returns the
 * encrypted data in <EM>v</EM>.
 * \param _v Array of two long values containing the data block.
 * \param _k Array of four long values containing the key.
 */
void tea_enc(void *_v, void *_k)
{
	uint32_t y, z;
	uint32_t sum = 0;
	uint8_t n = ROUNDS;
	uint32_t *v = (uint32_t *)_v;
	uint32_t *k = (uint32_t *)_k;

	y=cpu_to_le32(v[0]);
	z=cpu_to_le32(v[1]);

	while(n-- > 0)
	{
		sum += DELTA;
		y += tea_func(&z, &sum, &(k[0]));
		z += tea_func(&y, &sum, &(k[2]));
	}

	v[0] = le32_to_cpu(y);
	v[1] = le32_to_cpu(z);
}

/**
 * \brief TEA decryption function.
 * This function decrypts <EM>v</EM> with <EM>k</EM> and returns the
 * decrypted data in <EM>v</EM>.
 * \param _v Array of two long values containing the data block.
 * \param _k Array of four long values containing the key.
 */
void tea_dec(void *_v, void *_k)
{
	uint32_t y, z;
	uint32_t sum = DELTA * ROUNDS;
	uint8_t n = ROUNDS;
	uint32_t *v = (uint32_t *)_v;
	uint32_t *k = (uint32_t *)_k;

	y = cpu_to_le32(v[0]);
	z = cpu_to_le32(v[1]);

	while(n-- > 0)
	{
		z -= tea_func(&y, &sum, &(k[2]));
		y -= tea_func(&z, &sum, &(k[0]));
		sum -= DELTA;
	}

	v[0] = le32_to_cpu(y);
	v[1] = le32_to_cpu(z);
}

