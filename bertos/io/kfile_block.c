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
 * \brief KFile interface over a KBlock.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "kfile_block.h"
#include <string.h>


/**
 * ID for KFile over a KBlock.
 */
#define KFT_KFILEBLOCK MAKE_ID('K', 'F', 'B', 'L')

/**
 * Convert + ASSERT from generic KFile to KFileBlock.
 */
INLINE KFileBlock * KFILEBLOCK_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_KFILEBLOCK);
	return (KFileBlock *)fd;
}

#define KFILEBLOCK(dir, fd, buf, size) \
({ \
	KFileBlock *fb = KFILEBLOCK_CAST(fd); \
	size_t len = 0; \
	while (size) \
	{ \
		block_idx_t id = (fd)->seek_pos / fb->blk->blk_size; \
		if (id >= fb->blk->blk_cnt) \
			break; \
		size_t offset = (fd)->seek_pos % fb->blk->blk_size; \
		size_t count = MIN(size, (size_t)(fb->blk->blk_size - offset)); \
		size_t ret_len = kblock_##dir(fb->blk, id, buf, offset, count); \
		size -= ret_len; \
		(fd)->seek_pos += ret_len; \
		buf = buf + ret_len; \
		len += ret_len; \
		if (ret_len != count) \
			break; \
	} \
	len; \
})

static size_t kfileblock_read(struct KFile *fd, void *_buf, size_t size)
{
	uint8_t *buf = (uint8_t *)_buf;
	return KFILEBLOCK(read, fd, buf, size);
}

static size_t kfileblock_write(struct KFile *fd, const void *_buf, size_t size)
{
	const uint8_t *buf = (const uint8_t *)_buf;
	return KFILEBLOCK(write, fd, buf, size);
}

static int kfileblock_flush(struct KFile *fd)
{
	KFileBlock *fb = KFILEBLOCK_CAST(fd);
	return kblock_flush(fb->blk);
}

static int kfileblock_error(struct KFile *fd)
{
	KFileBlock *fb = KFILEBLOCK_CAST(fd);
	return kblock_error(fb->blk);
}

static void kfileblock_clearerr(struct KFile *fd)
{
	KFileBlock *fb = KFILEBLOCK_CAST(fd);
	return kblock_clearerr(fb->blk);
}

static int kfileblock_close(struct KFile *fd)
{
	KFileBlock *fb = KFILEBLOCK_CAST(fd);
	return kblock_close(fb->blk);
}

void kfileblock_init(KFileBlock *fb, KBlock *blk)
{
	ASSERT(fb);
	ASSERT(blk);
	memset(fb, 0, sizeof(*fb));
	kfile_init(&fb->fd);
	DB(fb->fd._type = KFT_KFILEBLOCK);
	fb->blk = blk;
	fb->fd.size = blk->blk_cnt * blk->blk_size;
	fb->fd.read = kfileblock_read;
	fb->fd.write = kfileblock_write;
	fb->fd.flush = kfileblock_flush;
	fb->fd.error = kfileblock_error;
	fb->fd.clearerr = kfileblock_clearerr;
	fb->fd.close = kfileblock_close;
}
