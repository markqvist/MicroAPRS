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
 * \brief Heap test.
 *
 * \author Francesco Sacchi <batt@codewiz.org>
 */


#include <struct/fifobuf.h>
#include <struct/heap.h>

#include <cfg/compiler.h>
#include <cfg/test.h>
#include <cfg/debug.h>

#define TEST_LEN 31
#define ALLOC_SIZE 113

#define TEST_LEN2 32
#define ALLOC_SIZE2 128

#define HEAP_SIZE 4096

HEAP_DEFINE_BUF(heap_buf, HEAP_SIZE);
STATIC_ASSERT(sizeof(heap_buf) % sizeof(heap_buf_t) == 0);

Heap h;

int heap_testSetup(void)
{
	kdbg_init();
	heap_init(&h, heap_buf, sizeof(heap_buf));
	return 0;
}

static void alloc_test(size_t size, size_t test_len)
{
	//Simple test
	uint8_t *a[test_len];

	for (size_t i = 0; i < test_len; i++)
	{
		a[i] = heap_allocmem(&h, size);
		ASSERT(a[i]);
		for (size_t j = 0; j < size; j++)
			a[i][j] = i;
	}

	ASSERT(heap_freeSpace(&h) == HEAP_SIZE - test_len * ROUND_UP2(size, sizeof(MemChunk)));

	for (size_t i = 0; i < test_len; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			kprintf("a[%d][%d] = %d\n", i, j, a[i][j]);
			ASSERT(a[i][j] == i);
		}
		heap_freemem(&h, a[i], size);
	}
	ASSERT(heap_freeSpace(&h) == HEAP_SIZE);
}

int heap_testRun(void)
{
	alloc_test(ALLOC_SIZE, TEST_LEN);
	alloc_test(ALLOC_SIZE2, TEST_LEN2);
	/* Try to allocate the whole heap */
	uint8_t *b = heap_allocmem(&h, HEAP_SIZE);
	ASSERT(b);
	ASSERT(heap_freeSpace(&h) == 0);

	ASSERT(!heap_allocmem(&h, HEAP_SIZE));

	for (int j = 0; j < HEAP_SIZE; j++)
		b[j] = j;
	
	for (int j = 0; j < HEAP_SIZE; j++)
	{
		kprintf("b[%d] = %d\n", j, j);
		ASSERT(b[j] == (j & 0xff));
	}
	heap_freemem(&h, b, HEAP_SIZE);
	ASSERT(heap_freeSpace(&h) == HEAP_SIZE);

	return 0;
}

int heap_testTearDown(void)
{
	return 0;
}

TEST_MAIN(heap);
