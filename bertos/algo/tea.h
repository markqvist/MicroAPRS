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
 * \brief TEA Tiny Encription Algorith functions.
 *
 * Documentation for TEA is available at
 * http://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "tea"
 */

#ifndef ALGO_TEA_H
#define ALGO_TEA_H

#include <cfg/compiler.h>

#define TEA_KEY_LEN     16	//!< TEA key size.
#define TEA_BLOCK_LEN   8	//!< TEA block length.

#define DELTA   0x9E3779B9	//!< Magic value. (Golden number * 2^31)
#define ROUNDS  32		//!< Number of rounds.

void tea_enc(void *_v, void *_k);
void tea_dec(void *_v, void *_k);

#endif /* ALGO_TEA_H */

