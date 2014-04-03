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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief KBlock block size reducer
 *
 * This module allows access to a KBlock device using a smaller block
 * size than the native one exported by the device.
 * Note that the device being remapped needs either to be opened in buffered
 * mode or to support partial writes.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_depends = "kblock"
 */

#include "reblock.h"
#include <string.h> /* memset */


static size_t reblock_readDirect(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	Reblock *r = REBLOCK_CAST(b);

	offset += idx % (r->native_fd->blk_size / r->fd.blk_size) * r->fd.blk_size;
	idx    =  idx / (r->native_fd->blk_size / r->fd.blk_size);

	return kblock_read(r->native_fd, idx, buf, offset, size);
}


static size_t reblock_writeDirect(struct KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Reblock *r = REBLOCK_CAST(b);

	offset += idx % (r->native_fd->blk_size / r->fd.blk_size) * r->fd.blk_size;
	idx    =  idx / (r->native_fd->blk_size / r->fd.blk_size);

	return kblock_write(r->native_fd, idx, buf, offset, size);
}


static int reblock_error(struct KBlock *b)
{
	return kblock_error(REBLOCK_CAST(b)->native_fd);
}

static void reblock_clearerr(struct KBlock *b)
{
	kblock_clearerr(REBLOCK_CAST(b)->native_fd);
}

static int reblock_close(struct KBlock *b)
{
	return kblock_close(REBLOCK_CAST(b)->native_fd);
}


static const KBlockVTable reblock_vt =
{
	.readDirect = reblock_readDirect,
	.writeDirect = reblock_writeDirect,

	.error = reblock_error,
	.clearerr = reblock_clearerr,
	.close = reblock_close,
};


/*
 * Initialize reblock device.
 *
 * \param rbl           kblock reblock device
 * \param native_fd     kblock descriptor of the reblocked device
 * \param new_blk_size  new block size to export
 *
 * \note new block size is required to be a submultiple of the
 *       native device block size.
 */
void reblock_init(Reblock *rbl, KBlock *native_fd, size_t new_blk_size)
{
	ASSERT(new_blk_size);
	ASSERT(new_blk_size < native_fd->blk_size);
	ASSERT(native_fd->blk_size % new_blk_size == 0);
	ASSERT(kblock_buffered(native_fd) || kblock_partialWrite(native_fd));

	memset(rbl, 0, sizeof(Reblock));

	DB(rbl->fd.priv.type = KBT_REBLOCK);

	rbl->fd.blk_size = new_blk_size;
	rbl->fd.blk_cnt = native_fd->blk_cnt * (native_fd->blk_size / new_blk_size);

	rbl->fd.priv.flags |= KB_PARTIAL_WRITE;
	rbl->fd.priv.vt = &reblock_vt;

	rbl->native_fd = native_fd;
}
