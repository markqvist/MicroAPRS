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
 * \brief KFile interface over a FIFO buffer.
 *
 * \author Francesco Sacchi <asterix@develer.com>
 */

#include "kfile_fifo.h"
#include "fifobuf.h"

#include <io/kfile.h>

#include <string.h>

static size_t kfilefifo_read(struct KFile *_fd, void *_buf, size_t size)
{
	KFileFifo *fd = KFILEFIFO_CAST(_fd);
	uint8_t *buf = (uint8_t *)_buf;

	while (size-- && !fifo_isempty_locked(fd->fifo))
		*buf++ = fifo_pop_locked(fd->fifo);

	return buf - (uint8_t *)_buf;
}

static size_t kfilefifo_write(struct KFile *_fd, const void *_buf, size_t size)
{
	KFileFifo *fd = KFILEFIFO_CAST(_fd);
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size-- && !fifo_isfull_locked(fd->fifo))
		fifo_push_locked(fd->fifo, *buf++);

	return buf - (const uint8_t *)_buf;
}

void kfilefifo_init(KFileFifo *kf, FIFOBuffer *fifo)
{
	memset(kf, 0, sizeof(*kf));

	kf->fifo = fifo;
	kf->fd.read = kfilefifo_read;
	kf->fd.write = kfilefifo_write;
	DB(kf->fd._type = KFT_KFILEFIFO);
}
