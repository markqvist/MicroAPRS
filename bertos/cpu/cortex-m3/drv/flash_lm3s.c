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
 * \brief LM3S1968 internal flash memory driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "flash_lm3s.h"
#include "cfg/cfg_emb_flash.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/macros.h>

#include <io/kblock.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <cpu/power.h> /* cpu_relax() */
#include <cpu/types.h>

#include <string.h> /* memcpy() */

struct FlashHardware
{
	int status;
};

static bool flash_wait(struct KBlock *blk, uint32_t event)
{
	Flash *fls = FLASH_CAST(blk);
	ticks_t start = timer_clock();
	while (true)
	{
		if (!(FLASH_FMC_R & event))
			break;

		if (FLASH_FCRIS_R & FLASH_FCRIS_ARIS)
		{
			fls->hw->status |= FLASH_WR_PROTECT;
			LOG_ERR("wr protect..\n");
			return false;
		}

		if (timer_clock() - start > ms_to_ticks(CONFIG_FLASH_WR_TIMEOUT))
		{
			fls->hw->status |= FLASH_WR_TIMEOUT;
			LOG_ERR("Timeout..\n");
			return false;
		}

		cpu_relax();
	}

	return true;
}

static int lm3s_erasePage(struct KBlock *blk, uint32_t addr)
{
	FLASH_FCMISC_R = FLASH_FCMISC_AMISC;

	FLASH_FMA_R = (volatile uint32_t)addr;
	FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

	return flash_wait(blk, FLASH_FMC_ERASE);
}

static int lm3s_writeWord(struct KBlock *blk, uint32_t addr, uint32_t data)
{
	FLASH_FCMISC_R = FLASH_FCMISC_AMISC;

	FLASH_FMA_R = (volatile uint32_t)addr;
	FLASH_FMD_R = (volatile uint32_t)data;
	FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

	return flash_wait(blk, FLASH_FMC_WRITE);
}

static size_t lm3s_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)(idx * blk->blk_size + offset), size);
	return size;
}

static size_t lm3s_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	(void)offset;
	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	if (!lm3s_erasePage(blk, (idx * blk->blk_size)))
		return 0;

	uint32_t addr = idx * blk->blk_size;
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size)
	{
		uint32_t data = (*(buf + 3) << 24) |
						(*(buf + 2) << 16) |
						(*(buf + 1) << 8)  |
						*buf;

		if (!lm3s_writeWord(blk, addr, data))
			return 0;

		size -= 4;
		buf += 4;
		addr += 4;
	}

	return blk->blk_size;
}

static int lm3s_flash_error(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	return fls->hw->status;
}

static void lm3s_flash_clearerror(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
}

static const KBlockVTable flash_lm3s_buffered_vt =
{
	.readDirect = lm3s_flash_readDirect,
	.writeDirect = lm3s_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.close = kblock_swClose,

	.error = lm3s_flash_error,
	.clearerr = lm3s_flash_clearerror,
};

static const KBlockVTable flash_lm3s_unbuffered_vt =
{
	.readDirect = lm3s_flash_readDirect,
	.writeDirect = lm3s_flash_writeDirect,

	.close = kblock_swClose,

	.error = lm3s_flash_error,
	.clearerr = lm3s_flash_clearerror,
};

static struct FlashHardware flash_lm3s_hw;
static uint8_t flash_buf[FLASH_PAGE_SIZE];

static void common_init(Flash *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	FLASH_USECRL_R = CPU_FREQ / 1000000 - 1;

	fls->hw = &flash_lm3s_hw;

	fls->blk.blk_size = FLASH_PAGE_SIZE;
	fls->blk.blk_cnt =  FLASH_SIZE / FLASH_PAGE_SIZE;
}


void flash_hw_init(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_lm3s_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;

	/* Load the first block in the cache */
	void *flash_start = 0x0;
	memcpy(fls->blk.priv.buf, flash_start, fls->blk.blk_size);
}

void flash_hw_initUnbuffered(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_lm3s_unbuffered_vt;
}



