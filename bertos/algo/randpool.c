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
 *
 * -->
 *
 * \brief API function for to manage entropy pool.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "randpool.h"
#include "md2.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>       //ASSERT()
#include <cfg/macros.h>      //MIN(), ROUND_UP();

#include <stdio.h>           //sprintf();
#include <string.h>          //memset(), memcpy();

#if CONFIG_RANDPOOL_TIMER
	#include <drv/timer.h>       //timer_clock();
#endif



/*
 * Insert bytes in entropy pool, making a XOR of bytes present
 * in entropy pool.
 */
static void randpool_push(EntropyPool *pool, void *_byte, size_t n_byte)
{
	size_t i = pool->pos_add; // Current number of byte insert in entropy pool.
	uint8_t *byte;

	byte = (uint8_t *)_byte;

	/*
	 * Insert a bytes in entropy pool.
	 */
	for(size_t j = 0; j < n_byte; j++)
	{
		pool->pool_entropy[i] = pool->pool_entropy[i] ^ byte[j];
		i++;
		i = i % CONFIG_SIZE_ENTROPY_POOL;
	}

	pool->pos_add  =  i; // Update a insert bytes.
}


/*
 * This function stir entropy pool with MD2 function hash.
 *
 */
static void randpool_stir(EntropyPool *pool)
{
	size_t entropy = pool->entropy; //Save current calue of entropy.
	Md2Context context;
	uint8_t tmp_buf[((sizeof(size_t) * 2) + sizeof(int)) * 2 + 1]; //Temporary buffer.

	md2_init(&context); //Init MD2 algorithm.

	randpool_add(pool, NULL, 0);

	for (int i = 0; i < (CONFIG_SIZE_ENTROPY_POOL / MD2_DIGEST_LEN); i++)
	{
		sprintf((char *)tmp_buf, "%0x%0x%0x", pool->counter, i, pool->pos_add);

		/*
		 * Hash with MD2 algorithm the entropy pool.
		 */
		md2_update(&context, pool->pool_entropy, CONFIG_SIZE_ENTROPY_POOL);

		md2_update(&context, tmp_buf, sizeof(tmp_buf) - 1);

		/*Insert a message digest in entropy pool.*/
		randpool_push(pool, md2_end(&context), MD2_DIGEST_LEN);

		pool->counter = pool->counter + 1;

	}

	/*Insert in pool the difference between a two call of this function (see above).*/
	randpool_add(pool, NULL, 0);

	pool->entropy = entropy; //Restore old value of entropy. We haven't add entropy.
}

/**
 * Add \param entropy bits from \param data buffer to the entropy \param pool
 */
void randpool_add(EntropyPool *pool, void *data, size_t entropy)
{
	uint8_t sep[] = "\xaa\xaa\xaa\xaa";  // ??
	size_t data_len = ROUND_UP(entropy, 8) / 8; //Number of entropy byte in input.

	randpool_push(pool, data, data_len); //Insert data to entropy pool.

#if CONFIG_RANDPOOL_TIMER

	ticks_t event = timer_clock();
	ticks_t delta;

	/*Difference of time between a two accese to entropy pool.*/
	delta = event - pool->last_counter;

	randpool_push(pool, &event, sizeof(ticks_t));
	randpool_push(pool, sep, sizeof(sep) - 1); // ??
	randpool_push(pool, &delta, sizeof(delta));

	/*
	 * Count of number entropy bit add with delta.
	 */
	delta = delta & 0xff;
	while(delta)
	{
		delta >>= 1;
		entropy++;
	}

	pool->last_counter = event;

#endif

	pool->entropy += entropy;      //Update a entropy of the pool.
}

/**
 * Randpool function initialization.
 * The entropy pool can be initialize also with
 * a previous entropy pool.
 */
void randpool_init(EntropyPool *pool, void *_data, size_t len)
{
	uint8_t *data;

	data = (uint8_t *)_data;

	memset(pool, 0, sizeof(EntropyPool));
	pool->pos_get = MD2_DIGEST_LEN;

#if CONFIG_RANDPOOL_TIMER
	pool->last_counter = timer_clock();
#endif

	if(data)
	{
		/*
		 * Initialize a entropy pool with a
		 * previous pool, and assume all pool as
		 * entropy.
		 */
		len = MIN(len,(size_t)CONFIG_SIZE_ENTROPY_POOL);
		memcpy(pool->pool_entropy, data, len);
		pool->entropy = len;
	}

}

/**
 * Get the actual value of entropy.
 */
size_t randpool_size(EntropyPool *pool)
{
	return pool->entropy;
}

/**
 * Get \param n_byte from entropy pool. If n_byte is larger than number
 * byte of entropy in entropy pool, randpool_get continue
 * to generate pseudocasual value from previous state of
 * pool.
 * \param n_byte number fo bytes to read.
 * \param pool is the pool entropy context.
 * \param _data is the pointer to write the random data to.
 */
void randpool_get(EntropyPool *pool, void *_data, size_t n_byte)
{
	Md2Context context;
	size_t i = pool->pos_get;
	size_t n = n_byte;
	size_t pos_write = 0;  //Number of block has been written in data.
	size_t len = MIN((size_t)MD2_DIGEST_LEN, n_byte);
	uint8_t *data;

	data = (uint8_t *)_data;

	/* Test if i + CONFIG_MD2_BLOCK_LEN  is inside of entropy pool.*/
	ASSERT((MD2_DIGEST_LEN + i) <= CONFIG_SIZE_ENTROPY_POOL);

	md2_init(&context);

	while(n > 0)
	{

		/*Hash previous state of pool*/
		md2_update(&context, &pool->pool_entropy[i], MD2_DIGEST_LEN);

		memcpy(&data[pos_write], md2_end(&context), len);

		pos_write += len;   //Update number of block has been written in data.
		n -= len;           //Number of byte copied in data.

		len = MIN(n,(size_t)MD2_DIGEST_LEN);

		i = (i + MD2_DIGEST_LEN) % CONFIG_SIZE_ENTROPY_POOL;

		/* If we haven't more entropy pool to hash, we stir it.*/
		if(i < MD2_DIGEST_LEN)
		{
			randpool_stir(pool);
			i = pool->pos_get;
		}

	}

	pool->pos_get = i; //Current number of byte we get from pool.
	pool->entropy -= n_byte; //Update a entropy.

}

/**
 * Return a pointer to entropy pool.
 */
uint8_t *randpool_pool(EntropyPool *pool)
{
	return pool->pool_entropy;
}

