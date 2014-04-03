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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/);
 *
 * -->
 *
 * \brief Collection of functions to manage entropy pool.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "randpool"
 * $WIZ$ module_depends = "timer", "sprintf"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_randpool.h"
 */

#ifndef ALGO_RANDPOOL_H
#define ALGO_RANDPOOL_H

#include "cfg/cfg_randpool.h"
#include <cfg/compiler.h>


/**
 * Sturct data of entropy pool.
 */
typedef struct EntropyPool
{
	size_t entropy;                                  ///< Actual value of entropy (byte).
	size_t pos_add;                                  ///< Number of byte added in entropy pool.
	size_t pos_get;                                  ///< Number of byte got in entropy pool.
	size_t counter;                                  ///< Counter.

#if CONFIG_RANDPOOL_TIMER
	size_t last_counter;                             ///< Last timer value.
#endif

	uint8_t pool_entropy[CONFIG_SIZE_ENTROPY_POOL];  ///< Entropy pool.

} EntropyPool;


void randpool_add(EntropyPool *pool, void *data, size_t entropy);
void randpool_init(EntropyPool *pool, void *_data, size_t len);
size_t randpool_size(EntropyPool *pool);
void randpool_get(EntropyPool *pool, void *data, size_t n_byte);
uint8_t *randpool_pool(EntropyPool *pool);

#endif /* ALGO_RANDPOOL_H */
