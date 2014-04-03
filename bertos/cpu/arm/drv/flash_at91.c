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
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief At91sam7 Internal flash read/write driver.
 *
 *
 */

#include "flash_at91.h"

#include "cfg/cfg_emb_flash.h"
#include <cfg/macros.h>

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>

#include <cpu/irq.h>
#include <cpu/attr.h>
#include <cpu/power.h>

#include <io/kfile.h>
#include <io/kblock.h>
#include <io/arm.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <string.h>

struct FlashHardware
{
	uint8_t status;
};


/**
 * Really send the flash write command.
 *
 * \note This function has to be placed in RAM because
 *       executing code from flash while a writing process
 *       is in progress is forbidden.
 */
RAM_FUNC NOINLINE static void write_page(uint32_t page)
{
	// Send the 'write page' command
	MC_FCR = MC_KEY | MC_FCMD_WP | (MC_PAGEN_MASK & (page << 8));

	// Wait for the end of command
	while(!(MC_FSR & BV(MC_FRDY)))
	{
		//NOP;
	}
}


/**
 * Send write command.
 *
 * After WR command cpu write bufferd page into flash memory.
 *
 */
INLINE void flash_sendWRcmd(uint32_t page)
{
	cpu_flags_t flags;

	LOG_INFO("Writing page %ld...\n", page);

	IRQ_SAVE_DISABLE(flags);
	write_page(page);

	IRQ_RESTORE(flags);
	LOG_INFO("Done\n");
}

/**
 * Return true if no error are occurred after flash memory
 * read or write operation, otherwise return error code.
 */
static bool flash_getStatus(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	/*
	 * This bit is set to one if an invalid command and/or a bad keywords was/were
	 * written in the Flash Command Register.
	 */
	if(MC_FSR & BV(MC_PROGE))
	{
		fls->hw->status |= FLASH_WR_ERR;
		LOG_ERR("flash not erased..\n");
		return false;
	}

	/*
	 * This bit is set to one if we programming of at least one locked lock
	 * region.
	 */
	if(MC_FSR & BV(MC_LOCKE))
	{
		fls->hw->status |= FLASH_WR_PROTECT;
		LOG_ERR("wr protect..\n");
		return false;
	}

	return true;
}

static size_t at91_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)(idx * blk->blk_size +  FLASH_BASE + offset), size);
	return size;
}

static size_t at91_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	uint32_t *addr = (uint32_t *)(idx * blk->blk_size +  FLASH_BASE);
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size)
	{
		uint32_t data = (*(buf + 3) << 24) |
						(*(buf + 2) << 16) |
						(*(buf + 1) << 8)  |
						*buf;
		*addr = data;

		size -= 4;
		buf += 4;
		addr++;
	}

	flash_sendWRcmd(idx);

	if (!flash_getStatus(blk))
		return 0;

	return blk->blk_size;
}


static int at91_flash_error(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	return fls->hw->status;
}

static void at91_flash_clearerror(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
}

static const KBlockVTable flash_at91_buffered_vt =
{
	.readDirect = at91_flash_readDirect,
	.writeDirect = at91_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = at91_flash_error,
	.clearerr = at91_flash_clearerror,
};

static const KBlockVTable flash_at91_unbuffered_vt =
{
	.readDirect = at91_flash_readDirect,
	.writeDirect = at91_flash_writeDirect,

	.error = at91_flash_error,
	.clearerr = at91_flash_clearerror,
};

static struct FlashHardware flash_at91_hw;
static uint8_t flash_buf[FLASH_PAGE_SIZE_BYTES];

static void common_init(Flash *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	fls->hw = &flash_at91_hw;

	fls->blk.blk_size = FLASH_PAGE_SIZE_BYTES;
	fls->blk.blk_cnt = FLASH_MEM_SIZE / FLASH_PAGE_SIZE_BYTES;
}

void flash_hw_init(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_at91_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;

	/* Load the first block in the cache */
	memcpy(fls->blk.priv.buf, (void *)(FLASH_BASE), fls->blk.blk_size);
}

void flash_hw_initUnbuffered(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_at91_unbuffered_vt;
}

