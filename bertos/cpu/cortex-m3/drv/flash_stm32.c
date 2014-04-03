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
 * \brief STM32F103xx internal flash memory driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "flash_stm32.h"

#include "cfg/cfg_emb_flash.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <cpu/power.h>
#include <cpu/detect.h>

#include <io/stm32.h>

#include <string.h>

#define EMB_FLASH                ((struct stm32_flash*)FLASH_R_BASE)

struct FlashHardware
{
	uint8_t status;
};

static bool flash_wait(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	ticks_t start = timer_clock();
	while (true)
	{
		cpu_relax();
		if (!(EMB_FLASH->SR & FLASH_FLAG_BSY))
			break;

		if (EMB_FLASH->SR & FLASH_FLAG_PGERR)
		{
			fls->hw->status |= FLASH_NOT_ERASED;
			LOG_ERR("flash not erased..\n");
			return false;
		}

		if (EMB_FLASH->SR & FLASH_FLAG_WRPRTERR)
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
	}

	return true;
}

static bool stm32_erasePage(struct KBlock *blk, uint32_t page_add)
{

	EMB_FLASH->CR |= CR_PER_SET;
	EMB_FLASH->AR = page_add;
	EMB_FLASH->CR |= CR_STRT_SET;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_PER_RESET;

	return true;
}

#if 0
// not used for now
static bool stm32_eraseAll(struct KBlock *blk)
{
	EMB_FLASH->CR |= CR_MER_SET;
	EMB_FLASH->CR |= CR_STRT_SET;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_MER_RESET;

	return true;
}
#endif

static int stm32_flash_error(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	return fls->hw->status;
}

static void stm32_flash_clearerror(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
}

static size_t stm32_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)(idx * blk->blk_size + offset), size);
	return size;
}


INLINE bool stm32_writeWord(struct KBlock *blk, uint32_t addr, uint16_t data)
{
	ASSERT(!(addr % 2));

	EMB_FLASH->CR |= CR_PG_SET;

	*(reg16_t *)addr = data;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_PG_RESET;

	return true;
}

static size_t stm32_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	if (!stm32_erasePage(blk, (idx * blk->blk_size)))
		return 0;

	uint32_t addr = idx * blk->blk_size;
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size)
	{
		uint16_t data = (*(buf + 1) << 8) | *buf;
		if (!stm32_writeWord(blk, addr, data))
			return 0;

		buf += 2;
		size -= 2;
		addr += 2;
	}

	return blk->blk_size;
}

static const KBlockVTable flash_stm32_buffered_vt =
{
	.readDirect = stm32_flash_readDirect,
	.writeDirect = stm32_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.close = kblock_swClose,

	.error = stm32_flash_error,
	.clearerr = stm32_flash_clearerror,
};

static const KBlockVTable flash_stm32_unbuffered_vt =
{
	.readDirect = stm32_flash_readDirect,
	.writeDirect = stm32_flash_writeDirect,

	.close = kblock_swClose,

	.error = stm32_flash_error,
	.clearerr = stm32_flash_clearerror,
};

static struct FlashHardware flash_stm32_hw;
static uint8_t flash_buf[FLASH_PAGE_SIZE];

static void common_init(Flash *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	fls->hw = &flash_stm32_hw;

	fls->blk.blk_size = FLASH_PAGE_SIZE;
	fls->blk.blk_cnt = (F_SIZE * 1024) / FLASH_PAGE_SIZE;

	/* Unlock flash memory for the FPEC Access */
	EMB_FLASH->KEYR = FLASH_KEY1;
	EMB_FLASH->KEYR = FLASH_KEY2;
}


void flash_hw_init(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_stm32_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;

	/* Load the first block in the cache */
	void *flash_start = 0x0;
	memcpy(fls->blk.priv.buf, flash_start, fls->blk.blk_size);
}

void flash_hw_initUnbuffered(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_stm32_unbuffered_vt;
}
