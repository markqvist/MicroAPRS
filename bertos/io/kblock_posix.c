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
 * \brief KBlock interface over libc files.
 *
 * notest: avr
 * notest: arm
 */


#include "kblock_posix.h"
#include <string.h>
#include <stdio.h>


static int kblockposix_load(KBlock *b, block_idx_t index)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	fseek(f->fp, index * b->blk_size, SEEK_SET);
	return (fread(f->b.priv.buf, 1, b->blk_size, f->fp) == b->blk_size) ? 0 : EOF;
}

static int kblockposix_store(struct KBlock *b, block_idx_t index)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	fseek(f->fp, index * b->blk_size, SEEK_SET);
	return (fwrite(f->b.priv.buf, 1, b->blk_size, f->fp) == b->blk_size) ? 0 : EOF;
}

static size_t kblockposix_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	memcpy(buf, (uint8_t *)f->b.priv.buf + offset, size);
	return size;
}

static size_t kblockposix_readDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	fseek(f->fp, index * b->blk_size + offset, SEEK_SET);
	return fread(buf, 1, size, f->fp);
}

static size_t kblockposix_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	memcpy((uint8_t *)f->b.priv.buf + offset, buf, size);
	return size;
}

static size_t kblockposix_writeDirect(struct KBlock *b, block_idx_t index, const void *buf, size_t offset, size_t size)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	ASSERT(buf);
	ASSERT(index < b->blk_cnt);
	fseek(f->fp, index * b->blk_size + offset, SEEK_SET);
	return fwrite(buf, 1, size, f->fp);
}

static int kblockposix_error(struct KBlock *b)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	return ferror(f->fp);
}


static void kblockposix_claererr(struct KBlock *b)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);
	clearerr(f->fp);
}


static int kblockposix_close(struct KBlock *b)
{
	KBlockPosix *f = KBLOCKPOSIX_CAST(b);

	return fflush(f->fp) | fclose(f->fp);
}


static const KBlockVTable kblockposix_hwbuffered_vt =
{
	.readDirect = kblockposix_readDirect,

	.readBuf = kblockposix_readBuf,
	.writeBuf = kblockposix_writeBuf,
	.load = kblockposix_load,
	.store = kblockposix_store,

	.error = kblockposix_error,
	.clearerr = kblockposix_claererr,
	.close = kblockposix_close,
};

static const KBlockVTable kblockposix_swbuffered_vt =
{
	.readDirect = kblockposix_readDirect,
	.writeDirect =kblockposix_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = kblockposix_error,
	.clearerr = kblockposix_claererr,
	.close = kblockposix_close,
};

static const KBlockVTable kblockposix_unbuffered_vt =
{
	.readDirect = kblockposix_readDirect,
	.writeDirect =kblockposix_writeDirect,

	.error = kblockposix_error,
	.clearerr = kblockposix_claererr,
	.close = kblockposix_close,
};



void kblockposix_init(KBlockPosix *f, FILE *fp, bool hwbuf, void *buf, size_t block_size, block_idx_t block_count)
{
	ASSERT(f);
	ASSERT(fp);
	ASSERT(block_size);

	memset(f, 0, sizeof(*f));

	DB(f->b.priv.type = KBT_KBLOCKPOSIX);

	f->fp = fp;
	f->b.blk_size = block_size;
	f->b.blk_cnt = block_count;

	f->b.priv.flags |= KB_PARTIAL_WRITE;
	if (buf)
	{
		f->b.priv.flags |= KB_BUFFERED;
		f->b.priv.buf = buf;
		if (hwbuf)
			f->b.priv.vt = &kblockposix_hwbuffered_vt;
		else
			f->b.priv.vt = &kblockposix_swbuffered_vt;
		kblockposix_load(&f->b, 0);
		f->b.priv.curr_blk = 0;
	}
	else
		f->b.priv.vt = &kblockposix_unbuffered_vt;
}
