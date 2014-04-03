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
 * \brief Function library for Accessing FT245RL USB interface.
 *
 * This module handles USB communication with FT245RL chip.
 * This chip is a parallel USB interface with data flow control.
 * A kfile-like interface is supplied.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "hw/hw_ft245rl.h"
#include "ft245rl.h"

#include <cfg/macros.h>
#include <cfg/debug.h>
#include <cfg/module.h>

#include <drv/timer.h>

#include <io/kfile.h>

#include <string.h>


MOD_DEFINE(ft245rl);

/**
 * Read \a size bytes in buffer \a buf, from fid \a _fd.
 */
static size_t ft245rl_read(struct KFile *_fd, void *_buf, size_t size)
{
	Ft245rl *fd = FT245RL_CAST(_fd);
	(void)fd; //unused
	uint8_t *buf = (uint8_t *)_buf;
	size_t total_read = 0;

	while (size--)
	{
		while(!FT245RL_DATA_RDY())
			/* busy waiy */;

		*buf++ = FT245RL_GETDATA();
		total_read++;
	}

	return total_read;
}

/**
 * Write \a size bytes from buffer \a buf, of fid \a _fd.
 */
static size_t ft245rl_write(struct KFile *_fd, const void *_buf, size_t size)
{
	Ft245rl *fd = FT245RL_CAST(_fd);
	(void)fd; //unused
	const uint8_t *buf = (const uint8_t *)_buf;
	size_t total_write = 0;

	while (size--)
	{
		while(!FT245RL_TX_ALLOWED())
			/* busy waiy */;

		FT245RL_SETDATA(*buf++);
		total_write++;
	}

	return total_write;
}

/**
 * Ft245rl init function.
 */
void ft245rl_init(Ft245rl *fd)
{
	memset(fd, 0, sizeof(*fd));
	DB(fd->fd._type = KFT_FT245RL);

	// Setup data ft245rl communication functions.
	fd->fd.read = ft245rl_read;
	fd->fd.write = ft245rl_write;

	FT245RL_INIT();
	while (FT245RL_DATA_RDY())
		FT245RL_GETDATA();

	MOD_INIT(ft245rl);
}
