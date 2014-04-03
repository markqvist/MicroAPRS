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
 * -->
 *
 * \brief Function library for secure digital memory.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "sd.h"
#include "hw/hw_sd.h"
#include <io/kfile.h>
#include <io/kblock.h>
#include <drv/timer.h>

#include <fs/fat.h>

#include "cfg/cfg_sd.h"

#define LOG_LEVEL  SD_LOG_LEVEL
#define LOG_FORMAT SD_LOG_FORMAT
#include <cfg/log.h>
#include <cpu/power.h>

#include <string.h> /* memset */

/**
 * Card Specific Data
 * read directly from the card.
 */
typedef struct CardCSD
{
	uint16_t block_len;  ///< Length of a block
	uint32_t block_num;  ///< Number of block on the card
	uint16_t capacity;   ///< Card capacity in MB
} CardCSD;

#define SD_IN_IDLE    0x01
#define SD_STARTTOKEN 0xFE

#define TIMEOUT_NAC   16384
#define SD_DEFAULT_BLOCKLEN 512

#define SD_BUSY_TIMEOUT ms_to_ticks(200)

static bool sd_select(Sd *sd, bool state)
{
	KFile *fd = sd->ch;

	if (state)
	{
		SD_CS_ON();

		ticks_t start = timer_clock();
		do
		{
			if (kfile_getc(fd) == 0xff)
				return true;

			cpu_relax();
		}
		while (timer_clock() - start < SD_BUSY_TIMEOUT);

		SD_CS_OFF();
		LOG_ERR("sd_select timeout\n");
		return false;
	}
	else
	{
		kfile_putc(0xff, fd);
		kfile_flush(fd);
		SD_CS_OFF();
		return true;
	}
}

static int16_t sd_waitR1(Sd *sd)
{
	uint8_t datain;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
		datain = kfile_getc(sd->ch);
		if (datain != 0xff)
			return (int16_t)datain;
	}
	LOG_ERR("Timeout waiting R1\n");
	return EOF;
}

static int16_t sd_sendCommand(Sd *sd, uint8_t cmd, uint32_t param, uint8_t crc)
{
	KFile *fd = sd->ch;
	/* The 7th bit of command must be a 1 */
	kfile_putc(cmd | 0x40, fd);

	/* send parameter */
	kfile_putc((param >> 24) & 0xFF, fd);
	kfile_putc((param >> 16) & 0xFF, fd);
	kfile_putc((param >> 8) & 0xFF, fd);
	kfile_putc((param) & 0xFF, fd);

	kfile_putc(crc, fd);

	return sd_waitR1(sd);
}

static bool sd_getBlock(Sd *sd, void *buf, size_t len)
{
	uint8_t token;
	uint16_t crc;

	KFile *fd = sd->ch;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
		token = kfile_getc(fd);
		if (token != 0xff)
		{
			if (token == SD_STARTTOKEN)
			{
				if (kfile_read(fd, buf, len) == len)
				{
					if (kfile_read(fd, &crc, sizeof(crc)) == sizeof(crc))
						/* check CRC here if needed */
						return true;
					else
						LOG_ERR("get_block error getting crc\n");
				}
				else
					LOG_ERR("get_block len error: %d\n", (int)len);
			}
			else
				LOG_ERR("get_block token error: %02X\n", token);

			return false;
		}
	}

	LOG_ERR("get_block timeout waiting token\n");
	return false;
}

#define SD_SELECT(sd) \
do \
{ \
	if (!sd_select((sd), true)) \
	{ \
		LOG_ERR("%s failed, card busy\n", __func__); \
		return EOF; \
	} \
} \
while (0)

#define SD_SETBLOCKLEN 0x50

static int16_t sd_setBlockLen(Sd *sd, uint32_t newlen)
{
	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_SETBLOCKLEN, newlen, 0);

	sd_select(sd, false);
	return sd->r1;
}

#define SD_SEND_CSD 0x49

static int16_t sd_getCSD(Sd *sd, CardCSD *csd)
{
	SD_SELECT(sd);

	int16_t r1 = sd_sendCommand(sd, SD_SEND_CSD, 0, 0);

	if (r1)
	{
		LOG_ERR("send_csd failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return r1;
	}

	uint8_t buf[16];
	bool res = sd_getBlock(sd, buf, sizeof(buf));
	sd_select(sd, false);

	if (res)
	{
		#if LOG_LEVEL >= LOG_LVL_INFO
			LOG_INFO("CSD: [");
			for (int i = 0; i < 16; i++)
				kprintf("%02X ", buf[i]);
			kprintf("]\n");
		#endif

		uint16_t mult = (1L << ((((buf[9] & 0x03) << 1) | ((buf[10] & 0x80) >> 7)) + 2));
		uint16_t c_size = (((uint16_t)(buf[6] & 0x03)) << 10) | (((uint16_t)buf[7]) << 2) |
				  (((uint16_t)(buf[8] & 0xC0)) >> 6);

		csd->block_len = (1L << (buf[5] & 0x0F));
		csd->block_num = (c_size + 1) * mult;
		csd->capacity = (csd->block_len * csd->block_num) >> 20; // in MB

		LOG_INFO("block_len %d bytes, block_num %ld, total capacity %dMB\n", csd->block_len, csd->block_num, csd->capacity);
		return 0;
	}
	else
		return EOF;
}


#define SD_READ_SINGLEBLOCK 0x51

static size_t sd_readDirect(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	Sd *sd = SD_CAST(b);
	LOG_INFO("reading from block %ld, offset %d, size %d\n", idx, offset, size);

	if (sd->tranfer_len != size)
	{
		if ((sd->r1 = sd_setBlockLen(sd, size)))
		{
			LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
			return 0;
		}
		sd->tranfer_len = size;
	}

	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_READ_SINGLEBLOCK, idx * SD_DEFAULT_BLOCKLEN + offset, 0);

	if (sd->r1)
	{
		LOG_ERR("read single block failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return 0;
	}

	bool res = sd_getBlock(sd, buf, size);
	sd_select(sd, false);
	if (!res)
	{
		LOG_ERR("read single block failed reading data\n");
		return 0;
	}
	else
		return size;
}

#define SD_WRITE_SINGLEBLOCK 0x58
#define SD_DATA_ACCEPTED     0x05

static size_t sd_writeDirect(KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Sd *sd = SD_CAST(b);
	KFile *fd = sd->ch;
	ASSERT(offset == 0);
	ASSERT(size == SD_DEFAULT_BLOCKLEN);

	LOG_INFO("writing block %ld\n", idx);
	if (sd->tranfer_len != SD_DEFAULT_BLOCKLEN)
	{
		if ((sd->r1 = sd_setBlockLen(sd, SD_DEFAULT_BLOCKLEN)))
		{
			LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
			return 0;
		}
		sd->tranfer_len = SD_DEFAULT_BLOCKLEN;
	}

	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_WRITE_SINGLEBLOCK, idx * SD_DEFAULT_BLOCKLEN, 0);

	if (sd->r1)
	{
		LOG_ERR("write single block failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return 0;
	}

	kfile_putc(SD_STARTTOKEN, fd);
	kfile_write(fd, buf, SD_DEFAULT_BLOCKLEN);
	/* send fake crc */
	kfile_putc(0, fd);
	kfile_putc(0, fd);

	uint8_t dataresp = kfile_getc(fd);
	sd_select(sd, false);

	if ((dataresp & 0x1f) != SD_DATA_ACCEPTED)
	{
		LOG_ERR("write block %ld failed: %02X\n", idx, dataresp);
		return EOF;
	}

	return SD_DEFAULT_BLOCKLEN;
}

void sd_writeTest(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];
	memset(buf, 0, sizeof(buf));

	for (block_idx_t i = 0; i < sd->b.blk_cnt; i++)
	{
		LOG_INFO("writing block %ld: %s\n", i, (sd_writeDirect(&sd->b, i, buf, 0, SD_DEFAULT_BLOCKLEN) == SD_DEFAULT_BLOCKLEN) ? "OK" : "FAIL");
	}
}


bool sd_test(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];

	if (sd_readDirect(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (int i = 0; i < SD_DEFAULT_BLOCKLEN; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	if (sd_writeDirect(&sd->b, 0, buf, 0, SD_DEFAULT_BLOCKLEN) != SD_DEFAULT_BLOCKLEN)
		return false;

	memset(buf, 0, sizeof(buf));
	if (sd_readDirect(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (block_idx_t i = 0; i < sd->b.blk_size; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	return true;
}

static int sd_error(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	return sd->r1;
}

static void sd_clearerr(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	sd->r1 = 0;
}

static const KBlockVTable sd_unbuffered_vt =
{
	.readDirect = sd_readDirect,
	.writeDirect = sd_writeDirect,

	.error = sd_error,
	.clearerr = sd_clearerr,
};

static const KBlockVTable sd_buffered_vt =
{
	.readDirect = sd_readDirect,
	.writeDirect = sd_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = sd_error,
	.clearerr = sd_clearerr,
};

#define SD_GO_IDLE_STATE     0x40
#define SD_GO_IDLE_STATE_CRC 0x95
#define SD_SEND_OP_COND      0x41
#define SD_SEND_OP_COND_CRC  0xF9

#define SD_START_DELAY  ms_to_ticks(10)
#define SD_INIT_TIMEOUT ms_to_ticks(1000)
#define SD_IDLE_RETRIES 4

static bool sd_blockInit(Sd *sd, KFile *ch)
{
	ASSERT(sd);
	ASSERT(ch);
	memset(sd, 0, sizeof(*sd));
	DB(sd->b.priv.type = KBT_SD);
	sd->ch = ch;

	SD_CS_INIT();
	SD_CS_OFF();

	/* Wait a few moments for supply voltage to stabilize */
	timer_delay(SD_START_DELAY);

	/* Give 80 clk pulses to wake up the card */
	for (int i = 0; i < 10; i++)
		kfile_putc(0xff, ch);
	kfile_flush(ch);

	for (int i = 0; i < SD_IDLE_RETRIES; i++)
	{
		SD_SELECT(sd);
		sd->r1 = sd_sendCommand(sd, SD_GO_IDLE_STATE, 0, SD_GO_IDLE_STATE_CRC);
		sd_select(sd, false);

		if (sd->r1 == SD_IN_IDLE)
			break;
	}

	if (sd->r1 != SD_IN_IDLE)
	{
		LOG_ERR("go_idle_state failed: %04X\n", sd->r1);
		return false;
	}

	ticks_t start = timer_clock();

	/* Wait for card to start */
	do
	{
		SD_SELECT(sd);
		sd->r1 = sd_sendCommand(sd, SD_SEND_OP_COND, 0, SD_SEND_OP_COND_CRC);
		sd_select(sd, false);
		cpu_relax();
	}
	while (sd->r1 != 0 && timer_clock() - start < SD_INIT_TIMEOUT);

	if (sd->r1)
	{
		LOG_ERR("send_op_cond failed: %04X\n", sd->r1);
		return false;
	}

	sd->r1 = sd_setBlockLen(sd, SD_DEFAULT_BLOCKLEN);
	sd->tranfer_len = SD_DEFAULT_BLOCKLEN;

	if (sd->r1)
	{
		LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
		return false;
	}

	/* Avoid warning for uninitialized csd use (gcc bug?) */
	CardCSD csd = csd;

	sd->r1 = sd_getCSD(sd, &csd);

	if (sd->r1)
	{
		LOG_ERR("getCSD failed: %04X\n", sd->r1);
		return false;
	}

	sd->b.blk_size = SD_DEFAULT_BLOCKLEN;
	sd->b.blk_cnt = csd.block_num * (csd.block_len / SD_DEFAULT_BLOCKLEN);
	LOG_INFO("blk_size %d, blk_cnt %ld\n", sd->b.blk_size, sd->b.blk_cnt);

#if CONFIG_SD_AUTOASSIGN_FAT
	disk_assignDrive(&sd->b, 0);
#endif

	return true;
}

bool sd_initUnbuf(Sd *sd, KFile *ch)
{
	if (sd_blockInit(sd, ch))
	{
		sd->b.priv.vt = &sd_unbuffered_vt;
		return true;
	}
	else
		return false;
}

static uint8_t sd_buf[SD_DEFAULT_BLOCKLEN];

bool sd_initBuf(Sd *sd, KFile *ch)
{
	if (sd_blockInit(sd, ch))
	{
		sd->b.priv.buf = sd_buf;
		sd->b.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
		sd->b.priv.vt = &sd_buffered_vt;
		sd->b.priv.vt->load(&sd->b, 0);
		return true;
	}
	else
		return false;
}

