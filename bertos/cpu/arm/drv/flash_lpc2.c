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
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief NPX lpc23xx embedded flash read/write driver.
 *
 * notest:arm
 */

#include "flash_lpc2.h"
#include "cfg/cfg_emb_flash.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/macros.h>

#include <cpu/irq.h>
#include <cpu/attr.h>
#include <cpu/power.h>
#include <cpu/types.h>

#include <io/kblock.h>
#include <io/arm.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <struct/bitarray.h>

#include <string.h>

/* Embedded flash programming defines. */
#define IAP_ADDRESS 0x7ffffff1

typedef enum IapCommands
{
	PREPARE_SECTOR_FOR_WRITE = 50,
	COPY_RAM_TO_FLASH = 51,
	ERASE_SECTOR = 52,
	BLANK_CHECK_SECTOR = 53,
	READ_PART_ID = 54,
	READ_BOOT_VER = 55,
	COMPARE = 56,
	REINVOKE_ISP = 57,
} IapCommands;

#if CPU_ARM_LPC2378
	#define FLASH_MEM_SIZE         (504 * 1024L)
	#define FLASH_PAGE_SIZE_BYTES          4096
	#define FLASH_REAL_PAGE_CNT              28
#else
	#error Unknown CPU
#endif

#define CMD_SUCCESS 0

struct FlashHardware
{
	uint8_t status;
	int flags;
};

#define FLASH_PAGE_CNT  FLASH_MEM_SIZE / FLASH_PAGE_SIZE_BYTES

BITARRAY_ALLOC(page_dirty, FLASH_PAGE_CNT);
static BitArray lpc2_bitx;

uint8_t erase_group[] = {

	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,
	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,

	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,
	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,

	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,
	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,

	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,
	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,

	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,
	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,

	32768 / FLASH_PAGE_SIZE_BYTES, 32768 / FLASH_PAGE_SIZE_BYTES,
	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,

	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,
	4096 / FLASH_PAGE_SIZE_BYTES, 4096 / FLASH_PAGE_SIZE_BYTES,
};

typedef struct IapCmd
{
	uint32_t cmd;
	uint32_t param[4];
} IapCmd;

typedef struct IapRes
{
	uint32_t status;
	uint32_t res[2];
} IapRes;

typedef void (*iap_callback_t)(IapCmd *, IapRes *);

iap_callback_t iap = (iap_callback_t)IAP_ADDRESS;

static size_t sector_size(uint32_t page)
{
	if (page < 8)
		return 4096;
	else if (page < 22)
		return 32768;
	else if (page < 28)
		return 4096;

	ASSERT(0);
	return 0;
}

static size_t sector_addr(uint32_t page)
{
	if (page < 8)
		return page * 4096;
	else if (page < 22)
		return (page - 8) * 32768 + 4096 * 8;
	else if (page < 28)
		return (page - 22) * 4096 + 32768 * 14 + 4096 * 8;

	ASSERT(0);
	return 0;
}


static uint32_t addr_to_sector(size_t addr)
{
	if (addr < 4096 * 8)
		return addr / 4096;
	else if (addr < 4096 * 8 + 32768L * 14)
		return ((addr - 4096 * 8) / 32768) + 8;
	else if (addr < 4096 * 8 + 32768L * 14 + 4096 * 6)
		return ((addr - 4096 * 8 - 32768L * 14) / 4096) + 22;

	ASSERT(0);
	return 0;
}

static size_t lpc2_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)(idx * blk->blk_size + offset), size);
	return size;
}

static size_t lpc2_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	ASSERT(offset == 0);
	ASSERT(FLASH_PAGE_SIZE_BYTES == size);

	Flash *fls = FLASH_CAST(blk);
	if (!(fls->hw->flags & FLASH_WRITE_ONCE))
		ASSERT(sector_size(idx) <= FLASH_PAGE_SIZE_BYTES);

	const uint8_t *buf = (const uint8_t *)_buf;
	cpu_flags_t flags;

	//Compute page address of current page.
	uint32_t addr = idx * blk->blk_size;
	uint32_t sector = addr_to_sector(addr);
	// Compute the first page index in the sector to manage the status
	int idx_sector = sector_addr(sector) /  blk->blk_size;

	LOG_INFO("Writing page[%ld]sector[%ld]idx[%d]\n", idx, sector, idx_sector);
	IRQ_SAVE_DISABLE(flags);

	IapCmd cmd;
	IapRes res;
	cmd.cmd = PREPARE_SECTOR_FOR_WRITE;
	cmd.param[0] = cmd.param[1] = sector;
	iap(&cmd, &res);

	if (res.status != CMD_SUCCESS)
		goto flash_error;

	if ((fls->hw->flags & FLASH_WRITE_ONCE) &&
			bitarray_isRangeFull(&lpc2_bitx, idx_sector, erase_group[sector]))
	{
		kputs("blocchi pieni\n");
		ASSERT(0);
		goto flash_error;
	}

	bool erase = false;
	if ((fls->hw->flags & FLASH_WRITE_ONCE) &&
			bitarray_isRangeEmpty(&lpc2_bitx, idx_sector, erase_group[sector]))
		erase = true;

	if (!(fls->hw->flags & FLASH_WRITE_ONCE))
		erase = true;

	if (erase)
	{
		cmd.cmd = ERASE_SECTOR;
		cmd.param[0] = cmd.param[1] = sector;
		cmd.param[2] = CPU_FREQ / 1000;
		iap(&cmd, &res);

		if (res.status != CMD_SUCCESS)
			goto flash_error;
	}

	LOG_INFO("Writing page [%ld], addr [%ld] in sector[%ld]\n", idx, addr, sector);
	cmd.cmd = PREPARE_SECTOR_FOR_WRITE;
	cmd.param[0] = cmd.param[1] = sector;
	iap(&cmd, &res);

	if (res.status != CMD_SUCCESS)
		goto flash_error;

	if (fls->hw->flags & FLASH_WRITE_ONCE)
	{
		if (bitarray_test(&lpc2_bitx, idx))
		{
			ASSERT(0);
			goto flash_error;
		}
		else
			bitarray_set(&lpc2_bitx, idx);
	}

	cmd.cmd = COPY_RAM_TO_FLASH;
	cmd.param[0] = addr;
	cmd.param[1] = (uint32_t)buf;
	cmd.param[2] = FLASH_PAGE_SIZE_BYTES;
	cmd.param[3] = CPU_FREQ / 1000;
	iap(&cmd, &res);

	if (res.status != CMD_SUCCESS)
		goto flash_error;

	IRQ_RESTORE(flags);
	LOG_INFO("Done\n");

	return blk->blk_size;

flash_error:
	IRQ_RESTORE(flags);
	LOG_ERR("%ld\n", res.status);
	fls->hw->status |= FLASH_WR_ERR;
	return 0;
}

static int lpc2_flash_close(UNUSED_ARG(struct KBlock, *blk))
{
	memset(page_dirty, 0, sizeof(page_dirty));
	return 0;
}


static int lpc2_flash_error(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	return fls->hw->status;
}

static void lpc2_flash_clearerror(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
}

static const KBlockVTable flash_lpc2_buffered_vt =
{
	.readDirect = lpc2_flash_readDirect,
	.writeDirect = lpc2_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.close = lpc2_flash_close,

	.error = lpc2_flash_error,
	.clearerr = lpc2_flash_clearerror,
};

static const KBlockVTable flash_lpc2_unbuffered_vt =
{
	.readDirect = lpc2_flash_readDirect,
	.writeDirect = lpc2_flash_writeDirect,

	.close = lpc2_flash_close,

	.error = lpc2_flash_error,
	.clearerr = lpc2_flash_clearerror,
};

static struct FlashHardware flash_lpc2_hw;
static uint8_t flash_buf[FLASH_PAGE_SIZE_BYTES];

static void common_init(Flash *fls, int flags)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	fls->hw = &flash_lpc2_hw;
	fls->hw->flags = flags;

	fls->blk.blk_size = FLASH_PAGE_SIZE_BYTES;
	fls->blk.blk_cnt = FLASH_MEM_SIZE / FLASH_PAGE_SIZE_BYTES;

	bitarray_init(&lpc2_bitx, FLASH_PAGE_CNT, page_dirty, sizeof(page_dirty));
}

void flash_hw_init(Flash *fls, int flags)
{
	common_init(fls, flags);
	fls->blk.priv.vt = &flash_lpc2_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;


	/* Load the first block in the cache */
	void *flash_start = 0x0;
	memcpy(fls->blk.priv.buf, flash_start, fls->blk.blk_size);
}

void flash_hw_initUnbuffered(Flash *fls, int flags)
{
	common_init(fls, flags);
	fls->blk.priv.vt = &flash_lpc2_unbuffered_vt;
}
