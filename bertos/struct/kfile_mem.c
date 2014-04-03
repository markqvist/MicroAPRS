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
 * \brief KFile interface over a memory buffer.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "kfile_mem.h"

#include <io/kfile.h>

#include <string.h>

static size_t kfilemem_read(struct KFile *_fd, void *buf, size_t size)
{
	KFileMem *fd = KFILEMEM_CAST(_fd);

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);
	uint8_t *mem = (uint8_t *)fd->mem;
	memcpy(buf, mem + fd->fd.seek_pos, size);
	fd->fd.seek_pos += size;

	return size;
}

static size_t kfilemem_write(struct KFile *_fd, const void *buf, size_t size)
{
	KFileMem *fd = KFILEMEM_CAST(_fd);

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);
	uint8_t *mem = (uint8_t *)fd->mem;
	memcpy(mem + fd->fd.seek_pos, buf, size);
	fd->fd.seek_pos += size;

	return size;
}

void kfilemem_init(KFileMem *km, void *mem, size_t len)
{
	ASSERT(km);
	ASSERT(mem);
	ASSERT(len);

	memset(km, 0, sizeof(*km));

	km->mem = mem;
	kfile_init(&km->fd);
	km->fd.read = kfilemem_read;
	km->fd.write = kfilemem_write;
	km->fd.size = len;
	DB(km->fd._type = KFT_KFILEMEM);
}
