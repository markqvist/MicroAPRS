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
 * Copyright 2003, 2004, 2005, 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (implementation)
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "eeprom.h"

#include "cfg/cfg_i2c.h"
#include "cfg/cfg_eeprom.h"

/* Define logging setting (for cfg/log.h module). */
#define LOG_LEVEL         EEPROM_LOG_LEVEL
#define LOG_FORMAT        EEPROM_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>  // MIN()

#include <cpu/attr.h>

#include <drv/i2c.h>

#include <string.h>  // memset()

/**
 * EEPROM ID code
 */
#define EEPROM_ID  0xA0

/**
 * This macros form the correct slave address for EEPROMs
 */
#define EEPROM_ADDR(x) (EEPROM_ID | (((uint8_t)((x) & 0x07)) << 1))


/**
 * Array used to describe EEPROM memory devices currently supported.
 */
static const EepromInfo mem_info[] =
{
	{
		/* 24XX08 */
		.has_dev_addr = false,
		.blk_size = 0x10,
		.e2_size = 0x400,
	},
	{
		/* 24XX16 */
		.has_dev_addr = false,
		.blk_size = 0x10,
		.e2_size = 0x800,
	},
	{
		/* 24XX256 */
		.has_dev_addr = true,
		.blk_size = 0x40,
		.e2_size = 0x8000,
	},
	{
		/* 24XX512 */
		.has_dev_addr = true,
		.blk_size = 0x80,
		.e2_size = 0x10000,
	},
	{
		/* 24XX1024 */
		.has_dev_addr = true,
		.blk_size = 0x100,
		.e2_size = 0x20000,
	},

	/* Add other memories here */
};

STATIC_ASSERT(countof(mem_info) == EEPROM_CNT);

#define CHUNCK_SIZE     16

/**
 * Erase EEPROM.
 * \param eep is the Kblock context.
 * \param addr eeprom address where start to erase
 * \param size number of byte to erase
 */
bool eeprom_erase(Eeprom *eep, e2addr_t addr, e2_size_t size)
{
	uint8_t tmp[CHUNCK_SIZE] = { [0 ... (CHUNCK_SIZE - 1)] = 0xFF };

	while (size)
	{
		block_idx_t idx = addr / eep->blk.blk_size;
		size_t offset = addr % eep->blk.blk_size;
		size_t count = MIN(size, (e2_size_t)CHUNCK_SIZE);
		size_t ret_len = eep->blk.priv.vt->writeDirect((KBlock *)eep, idx, tmp, offset, count);
		size -= ret_len;
		addr += ret_len;

		if (ret_len != count)
			return false;
	}
	return true;
}

/**
 * Verify EEPROM.
 * \param eep is the Kblock context.
 * \param addr eeprom address where start to verify.
 * \param buf buffer of data to compare with eeprom data read.
 * \param size number of byte to verify.
 */
bool eeprom_verify(Eeprom *eep, e2addr_t addr, const void *buf, size_t size)
{
    uint8_t verify_buf[CHUNCK_SIZE];
	while (size)
	{
		block_idx_t idx = addr / eep->blk.blk_size;
		size_t offset = addr % eep->blk.blk_size;
		size_t count = MIN(size, (size_t)CHUNCK_SIZE);

		size_t ret_len = eep->blk.priv.vt->readDirect((KBlock *)eep, idx, verify_buf, offset, count);

		if (ret_len != count)
		{
			LOG_ERR("Verify read fail.\n");
			return false;
		}

		if (memcmp(buf, verify_buf, ret_len) != 0)
		{
			LOG_ERR("Data mismatch!\n");
			return false;
		}

		size -= ret_len;
		addr += ret_len;
		buf = ((const char *)buf) + ret_len;
	}
	return true;
}


static size_t eeprom_write(KBlock *blk, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Eeprom *eep = EEPROM_CAST_KBLOCK(blk);
	e2dev_addr_t dev_addr;
	uint8_t addr_buf[2];
	uint8_t addr_len;
	uint32_t abs_addr = blk->blk_size * idx + offset;

	STATIC_ASSERT(countof(addr_buf) <= sizeof(e2addr_t));

	/* clamp size to memory limit (otherwise may roll back) */
	ASSERT(idx < blk->priv.blk_start + blk->blk_cnt);
	size = MIN(size, blk->blk_size - offset);

	if (mem_info[eep->type].has_dev_addr)
	{
		dev_addr = eep->addr;
		addr_len = 2;
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_len = 1;
	}

	if (mem_info[eep->type].has_dev_addr)
	{
		addr_buf[0] = (abs_addr >> 8) & 0xFF;
		addr_buf[1] = (abs_addr & 0xFF);
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_buf[0] = (abs_addr & 0xFF);
	}

	i2c_start_w(eep->i2c, EEPROM_ADDR(dev_addr),  addr_len + size, I2C_STOP);
	i2c_write(eep->i2c, addr_buf, addr_len);
	i2c_write(eep->i2c, buf, size);

	if (i2c_error(eep->i2c))
		return 0;

	return size;
}

static size_t eeprom_readDirect(struct KBlock *_blk, block_idx_t idx, void *_buf, size_t offset, size_t size)
{
	Eeprom *blk = EEPROM_CAST_KBLOCK(_blk);
	uint8_t addr_buf[2];
	uint8_t addr_len;
	size_t rd_len = 0;
	uint8_t *buf = (uint8_t *)_buf;
	uint32_t abs_addr = mem_info[blk->type].blk_size * idx + offset;

	STATIC_ASSERT(countof(addr_buf) <= sizeof(e2addr_t));

	/* clamp size to memory limit (otherwise may roll back) */
	ASSERT(idx < blk->blk.priv.blk_start + blk->blk.blk_cnt);
	size = MIN(size, blk->blk.blk_size - offset);

	e2dev_addr_t dev_addr;
	if (mem_info[blk->type].has_dev_addr)
	{
		dev_addr = blk->addr;
		addr_len = 2;
		addr_buf[0] = (abs_addr >> 8) & 0xFF;
		addr_buf[1] = (abs_addr & 0xFF);
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_len = 1;
		addr_buf[0] = (abs_addr & 0xFF);
	}


	i2c_start_w(blk->i2c, EEPROM_ADDR(dev_addr),  addr_len, I2C_NOSTOP);
	i2c_write(blk->i2c, addr_buf, addr_len);

	i2c_start_r(blk->i2c, EEPROM_ADDR(dev_addr), size, I2C_STOP);
	i2c_read(blk->i2c, buf, size);

	if (i2c_error(blk->i2c))
		   return rd_len;

	rd_len += size;

	return rd_len;
}

static size_t eeprom_writeDirect(KBlock *blk, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Eeprom *eep = EEPROM_CAST_KBLOCK(blk);
	if (!eep->verify)
		return eeprom_write(blk, idx, buf, offset, size);
	else
	{
		int retries = 5;
		while (retries--)
		{
			uint8_t verify_buf[CHUNCK_SIZE];
			size_t wr_len = 0;
			size_t len = 0;
			while (size)
			{
				/* Split read in smaller pieces */
				size_t count = MIN(size, (size_t)CHUNCK_SIZE);
				if ((wr_len = eeprom_write(blk, idx, buf, offset, count)) != 0)
				{
					if (eeprom_readDirect(blk, idx, verify_buf, offset, count) != wr_len)
					{
						LOG_ERR("Verify read fail.\n");
						return 0;
					}
					else if (memcmp(buf, verify_buf, wr_len) != 0)
					{
						LOG_ERR("Data mismatch!\n");
						continue;
					}
				}
				else
				{
					LOG_ERR("Write fail.\n");
					return 0;
				}
				size -= wr_len;
				len += wr_len;
				buf = ((const char *)buf) + wr_len;
			}
			return len;
		}
	}

	return 0;
}

static int kblockEeprom_dummy(UNUSED_ARG(struct KBlock *,b))
{
	return 0;
}


static const KBlockVTable eeprom_unbuffered_vt =
{
	.readDirect = eeprom_readDirect,
	.writeDirect = eeprom_writeDirect,

	.error = kblockEeprom_dummy,
	.clearerr = (kblock_clearerr_t)kblockEeprom_dummy,
};

/**
 * Initialize EEPROM module.
 * \param eep is the Kblock context.
 * \param type is the eeprom device we want to initialize (\see EepromType)
 * \param i2c context for i2c channel
 * \param addr is the i2c devide address (usually pins A0, A1, A2).
 * \param verify enable the write check.
 */
void eeprom_init_5(Eeprom *eep, I2c *i2c, EepromType type, e2dev_addr_t addr, bool verify)
{
	ASSERT(type < EEPROM_CNT);

	memset(eep, 0, sizeof(*eep));
	DB(eep->blk.priv.type = KBT_EEPROM);

	eep->type = type;
	eep->addr = addr;
	eep->i2c = i2c;
	eep->verify = verify;

	eep->blk.blk_size = mem_info[type].blk_size;
	eep->blk.blk_cnt = mem_info[type].e2_size / mem_info[type].blk_size;
	eep->blk.priv.flags |= KB_PARTIAL_WRITE;
	eep->blk.priv.vt = &eeprom_unbuffered_vt;
}


