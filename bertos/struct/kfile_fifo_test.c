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
 * \brief FIFO and KFileFifo test.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */


#include <struct/fifobuf.h>
#include <struct/kfile_fifo.h>

#include <cfg/compiler.h>
#include <cfg/test.h>
#include <cfg/debug.h>


int kfilefifo_testSetup(void)
{
	kdbg_init();
	return 0;
}

int kfilefifo_testRun(void)
{
	#define FIFOBUF_LEN 256

	uint8_t buf[FIFOBUF_LEN];

	FIFOBuffer fifo;
	fifo_init(&fifo, buf, sizeof(buf));

	ASSERT(fifo_isempty(&fifo));
	ASSERT(!fifo_isfull(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
	{
		ASSERT(!fifo_isfull(&fifo));
		fifo_push(&fifo, i);
	}

	ASSERT(fifo_isfull(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
	{
		ASSERT(!fifo_isempty(&fifo));
		ASSERT(fifo_pop(&fifo) == i);
	}

	ASSERT(fifo_isempty(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
	{
		ASSERT(!fifo_isfull(&fifo));
		fifo_push(&fifo, i);
	}
	ASSERT(fifo_isfull(&fifo));
	fifo_flush(&fifo);
	ASSERT(!fifo_isfull(&fifo));
	ASSERT(fifo_isempty(&fifo));

	KFileFifo kfifo;
	kfilefifo_init(&kfifo, &fifo);

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
	{
		ASSERT(!fifo_isfull(&fifo));
		fifo_push(&fifo, i);
	}

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
		ASSERT(kfile_getc(&kfifo.fd) == i);

	ASSERT(kfile_getc(&kfifo.fd) == EOF);
	ASSERT(fifo_isempty(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
		ASSERT(kfile_putc(i, &kfifo.fd) == i);

	ASSERT(fifo_isfull(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
	{
		ASSERT(!fifo_isempty(&fifo));
		ASSERT(fifo_pop(&fifo) == i);
	}

	ASSERT(fifo_isempty(&fifo));

	for (int i = 0; i < FIFOBUF_LEN - 1; i++)
		ASSERT(kfile_putc(i, &kfifo.fd) == i);

	ASSERT(fifo_isfull(&fifo));
	ASSERT(kfile_putc('a', &kfifo.fd) == EOF);

	fifo_flush(&fifo);
	ASSERT(!fifo_isfull(&fifo));
	ASSERT(fifo_isempty(&fifo));
	ASSERT(kfile_getc(&kfifo.fd) == EOF);

	ASSERT(kfile_write(&kfifo.fd, "hello world", 11) == 11);
	ASSERT(kfile_write(&kfifo.fd, "hello world", FIFOBUF_LEN) == FIFOBUF_LEN - 1 - 11);

	uint8_t test_buf[FIFOBUF_LEN];
	ASSERT(kfile_read(&kfifo.fd, test_buf, FIFOBUF_LEN + 20) == FIFOBUF_LEN - 1);

	ASSERT(!fifo_isfull(&fifo));
	ASSERT(fifo_isempty(&fifo));
	ASSERT(kfile_getc(&kfifo.fd) == EOF);
	return 0;
}

int kfilefifo_testTearDown(void)
{
	return 0;
}

TEST_MAIN(kfilefifo);
