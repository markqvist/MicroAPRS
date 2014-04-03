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
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief REVERSE macro test.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <cfg/macros.h>
#include <cfg/debug.h>
#include <cfg/test.h>


/* Silent compiler warning */
int reverse_testSetup(void);
int reverse_testRun(void);
int reverse_testTearDown(void);


int reverse_testSetup(void)
{
	kdbg_init();
	return 0;
}

int reverse_testTearDown(void)
{
	return 0;
}

/**
 * Naive reverse implementation.
 */
static uint8_t reverse(uint8_t b)
{
	uint8_t r = 0;

	for (int i = 0; i < 8; i++)
	{
		r <<= 1;
		r |= (b & BV(i)) ? 1 : 0;
	}

	return r;
}

int reverse_testRun(void)
{
	for (int i = 0; i < 256; i++)
	{
		kprintf("i [%02X], REVERSE(i) [%02X], reverse(i) [%02X]\n", i, REVERSE_UINT8(i), reverse(i));
		ASSERT(reverse(i) == REVERSE_UINT8(i));
	}
	return  0;
}

TEST_MAIN(reverse);
