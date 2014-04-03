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
 * \brief Self programming routines.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * This module implements a kfile-like access for Atmel avr internal flash.
 * Internal flash writing access is controlled by BOOTSZ fuses, check
 * datasheet for details.
 */

#include "flash_avr.h"

#include "cfg/cfg_emb_flash.h"

#include <cfg/macros.h> // MIN()
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cpu/irq.h>

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>

#include <drv/wdt.h>
#include <drv/flash.h>

#include <io/kfile.h>
#include <io/kfile_block.h>
#include <io/kblock.h>

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#include <string.h>

struct FlashHardware;

static size_t avr_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy_P(buf, (const void *)(uint16_t)(idx * blk->blk_size + offset), size);
	return blk->blk_size;
}

static size_t avr_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	uint32_t page_addr = idx * blk->blk_size;
	uint32_t addr = idx * blk->blk_size;
	const uint8_t *buf = (const uint8_t *)_buf;

	/* Wait while the SPM instruction is busy. */
	boot_spm_busy_wait();

	/* Fill the temporary buffer of the AVR */
	while (size)
	{
		uint16_t data = ((*buf + 1) << 8) | *buf;
		ATOMIC(boot_page_fill(addr, data));

		buf += 2;
		size -= 2;
		addr += 2;
	}

	wdt_reset();

	/* Page erase */
	ATOMIC(boot_page_erase(page_addr));

	/* Wait until the memory is erased. */
	boot_spm_busy_wait();

	/* Store buffer in flash page. */
	ATOMIC(boot_page_write(page_addr));

	/* Wait while the SPM instruction is busy. */
	boot_spm_busy_wait();

	/*
	* Reenable RWW-section again. We need this if we want to jump back
	* to the application after bootloading.
	*/
	ATOMIC(boot_rww_enable());

	return blk->blk_size;
}

static int avr_flash_dummy(UNUSED_ARG(struct KBlock, *blk))
{
	return 0;
}

static const KBlockVTable flash_avr_buffered_vt =
{
	.readDirect = avr_flash_readDirect,
	.writeDirect = avr_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = avr_flash_dummy,
	.clearerr = (kblock_clearerr_t)avr_flash_dummy,
};

static const KBlockVTable flash_avr_unbuffered_vt =
{
	.readDirect = avr_flash_readDirect,
	.writeDirect = avr_flash_writeDirect,

	.error = avr_flash_dummy,
	.clearerr = (kblock_clearerr_t)avr_flash_dummy,
};

static uint8_t flash_buf[SPM_PAGESIZE];

static void common_init(Flash *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	fls->blk.blk_size = SPM_PAGESIZE;
	fls->blk.blk_cnt =  (FLASHEND + 1) / SPM_PAGESIZE;
}


void flash_hw_init(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_avr_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;
}

void flash_hw_initUnbuffered(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_avr_unbuffered_vt;
}


