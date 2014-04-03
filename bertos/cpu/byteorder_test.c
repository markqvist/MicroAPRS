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
 *
 * -->
 *
 * \brief byteorder.h macros test.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <float.h>
#include <cfg/debug.h>
#include <cfg/test.h>

#include "byteorder.h"

int byteorder_testSetup(void)
{
	kdbg_init();
	return 0;
}

int byteorder_testTearDown(void)
{
	return 0;
}

int byteorder_testRun(void)
{
	float a;
	float b;
	float c;

	for (a = 0; a < 12345; a += 0.01)
	{
		b = swab_float(a);
		c = swab_float(b);
//		kprintf("a=%08lX, b=%08lX, c=%08lX\n", *((uint32_t *)&a), *((uint32_t *)&b), *((uint32_t *)&c));
		ASSERT(a == c);
	}
	return 0;
}

TEST_MAIN(byteorder);
