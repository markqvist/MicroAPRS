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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Test hashtable module.
 *
 * Test the hashtable module (insertion and find).
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_hashtable.h $cfgdir/
 */

#include <cfg/debug.h>
#include <cfg/test.h>
#include <string.h> /* strlen() */
#include "struct/hashtable.h"

static const void *test_get_key(const void *ptr, uint8_t *length)
{
	const char *s = ptr;

	*length = strlen(s);
	return s;
}

#define NUM_ELEMENTS   256
DECLARE_HASHTABLE_STATIC(hash1, NUM_ELEMENTS, test_get_key);
DECLARE_HASHTABLE_INTERNALKEY_STATIC(hash2, NUM_ELEMENTS);

static char data[NUM_ELEMENTS][10];
static char keydomain[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static bool single_test(void)
{
	int i;

	ht_init(&hash1);
	ht_init(&hash2);

	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		int k, klen;

		klen = (i % 8) + 1;
		for (k = 0; k < klen; k++)
			data[i][k] = keydomain[i % (sizeof(keydomain) - 1)];
		data[i][k] = 0;

		ASSERT(ht_insert(&hash1, data[i]));
		ASSERT(ht_insert_str(&hash2, data[i], data[i]));
	}
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		const char *found1, *found2;

		found1 = ht_find_str(&hash1, data[i]);
		if (strcmp(found1, data[i]))
			return false;
		kprintf("hash1: found data[%d] = %s\n", i, found1);

		found2 = ht_find_str(&hash2, data[i]);
		if (strcmp(found2, data[i]))
			return false;
		kprintf("hash2: found data[%d] = %s\n", i, found2);
	}
	return true;
}

int hashtable_testRun(void)
{
	if (!single_test())
	{
		kprintf("hashtable_test failed\n");
		return -1;
	}
	kprintf("hashtable_test successful\n");
	return 0;
}

int hashtable_testSetup(void)
{
	kdbg_init();
	return 0;
}

int hashtable_testTearDown(void)
{
	kputs("TearDown hashtable test.\n");
	return 0;
}

TEST_MAIN(hashtable);
