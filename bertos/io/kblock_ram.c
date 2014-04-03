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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief KBlock interface on RAM memory
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "kfile_ram"
 * $WIZ$ module_depends = "kblock"
 */


#include "kblock_ram.h"
#include <string.h>


static int kblockram_load(KBlock *b, block_idx_t index)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(r->b.priv.buf, r->membuf + index * r->b.blk_size, r->b.blk_size);
	return 0;
}

static int kblockram_store(struct KBlock *b, block_idx_t index)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(r->membuf + index * r->b.blk_size, r->b.priv.buf, r->b.blk_size);
	return 0;
}

static size_t kblockram_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(buf, (uint8_t *)r->b.priv.buf + offset, size);
	return size;
}

static size_t kblockram_readDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(buf, r->membuf + index * r->b.blk_size + offset, size);
	return size;
}

static size_t kblockram_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy((uint8_t *)r->b.priv.buf + offset, buf, size);
	return size;
}

static size_t kblockram_writeDirect(struct KBlock *b, block_idx_t index, const void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	ASSERT(buf);
	ASSERT(index < b->blk_cnt);

	memcpy(r->membuf + index * r->b.blk_size + offset, buf, size);
	return size;
}

static int kblockram_dummy(UNUSED_ARG(struct KBlock *,b))
{
	return 0;
}

static const KBlockVTable kblockram_hwbuffered_vt =
{
	.readDirect = kblockram_readDirect,

	.readBuf = kblockram_readBuf,
	.writeBuf = kblockram_writeBuf,
	.load = kblockram_load,
	.store = kblockram_store,

	.error = kblockram_dummy,
	.clearerr = (kblock_clearerr_t)kblockram_dummy,
	.close = kblockram_dummy,
};


static const KBlockVTable kblockram_swbuffered_vt =
{
	.readDirect = kblockram_readDirect,
	.writeDirect = kblockram_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = kblockram_dummy,
	.clearerr = (kblock_clearerr_t)kblockram_dummy,
	.close = kblockram_dummy,
};

static const KBlockVTable kblockram_unbuffered_vt =
{
	.readDirect = kblockram_readDirect,
	.writeDirect = kblockram_writeDirect,

	.error = kblockram_dummy,
	.clearerr = (kblock_clearerr_t)kblockram_dummy,
	.close = kblockram_dummy,
};

void kblockram_init(KBlockRam *ram, void *buf, size_t size, size_t block_size, bool buffered, bool hwbuffered)
{
	ASSERT(buf);
	ASSERT(size);
	ASSERT(block_size);

	memset(ram, 0, sizeof(*ram));

	DB(ram->b.priv.type = KBT_KBLOCKRAM);
	ram->b.blk_size = block_size;
	ram->b.priv.flags |= KB_PARTIAL_WRITE;

	if (buffered)
	{
		ram->b.priv.flags |= KB_BUFFERED;
		ram->b.blk_cnt = (size / block_size) - 1;
		ram->b.priv.buf = buf;
		// First page used as page buffer
		ram->membuf = (uint8_t *)buf + block_size;

		if (hwbuffered)
			ram->b.priv.vt = &kblockram_hwbuffered_vt;
		else
			ram->b.priv.vt = &kblockram_swbuffered_vt;

		kblockram_load(&ram->b, 0);
	}
	else
	{
		ram->b.blk_cnt = (size / block_size);
		ram->membuf = (uint8_t *)buf;
		ram->b.priv.vt = &kblockram_unbuffered_vt;
	}
}
