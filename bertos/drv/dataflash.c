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
 * \brief Function library for dataflash AT45DB family (implementation).
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "dataflash.h"

#include "cfg/cfg_dataflash.h"
#include <cfg/macros.h>
#include <cfg/debug.h>
#include <cfg/module.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   DATAFLASH_LOG_LEVEL
#define LOG_FORMAT  DATAFLASH_LOG_FORMAT
#include <cfg/log.h>

#include <drv/timer.h>

#include <fs/battfs.h>

#include <io/kfile.h>

#include <cpu/power.h> /* cpu_relax() */

#include <string.h>

/**
 * Array used to describe dataflash memory devices currently supported.
 */
static const DataflashInfo mem_info[] =
{
	{
		/* AT45DB041B */
		.density_id = 0x07,
		.page_size = 264,
		.page_bits = 9,
		.page_cnt = 2048,
		.read_cmd = DFO_READ_FLASH_MEM_BYTE_B,
	},
	{
		/* AT45DB081D */
		.density_id = 0x09,
		.page_size = 264,
		.page_bits = 9,
		.page_cnt = 4096,
		.read_cmd = DFO_READ_FLASH_MEM_BYTE_D,
	},
	{
		/* AT45DB161D */
		.density_id = 0x0B,
		.page_size = 528,
		.page_bits = 10,
		.page_cnt = 4096,
		.read_cmd = DFO_READ_FLASH_MEM_BYTE_D,
	},
	{
		/* AT45DB642D */
		.density_id = 0x0F,
		.page_size = 1056,
		.page_bits = 11,
		.page_cnt = 8192,
		.read_cmd = DFO_READ_FLASH_MEM_BYTE_D,
	},
	/* Add other memories here */
};

STATIC_ASSERT(countof(mem_info) == DFT_CNT);

/**
 * Macro that toggle CS of dataflash.
 * \note This is equivalent to fd->setCS(false) immediately followed by fd->setCS(true).
 */
INLINE void CS_TOGGLE(DataFlash *fd)
{
	fd->setCS(false);
	fd->setCS(true);
}

/**
 * Send a generic command to dataflash memory.
 * This function send only 4 byte: opcode, page address and
 * byte address.
 */
static void send_cmd(DataFlash *fd, dataflash_page_t page_addr, dataflash_offset_t byte_addr, DataFlashOpcode opcode)
{

	/*
	 * Make sure to toggle CS signal in order,
	 * and reset dataflash command decoder.
	 */
	CS_TOGGLE(fd);


	/*
	 * To send one command to data flash memory, we send 4 byte.
	 * First byte is opcode command, second and third byte are
	 * page address, in last byte we write a byte page address.
	 * (see datasheet for more detail).
	 *
	 * \note Generally a default memory page size is more than 256 byte.
	 *  In this case we need for addressing a byte in one page more than
	 *  8 bit, so we put in fourth byte low part of address byte, and
	 *  hight part of address byte in third byte togheter low par of page
	 *  address.
	 *
	 */

	/*
	 * Send opcode.
	 */
	kfile_putc(opcode, fd->channel);

	/*
	 *  Send page address.
	 */
	kfile_putc((uint8_t)(page_addr >> (16 - mem_info[fd->dev].page_bits)), fd->channel);
	kfile_putc((uint8_t)((page_addr << (mem_info[fd->dev].page_bits - 8)) + (byte_addr >> 8)), fd->channel);

	/*
	 * Send byte page address.
	 */
	kfile_putc((uint8_t)byte_addr, fd->channel);
}

/**
 * Reset dataflash memory function.
 *
 * If \a fd->setReset function is not NULL,
 * this function resets data flash memory
 * with one pulse reset long about 10usec.
 *
 */
static void dataflash_reset(DataFlash *fd)
{
	fd->setCS(false);

	if (fd->setReset)
	{
		fd->setReset(true);
		timer_delayHp(us_to_hptime(RESET_PULSE_WIDTH));
		fd->setReset(false);
		timer_delayHp(us_to_hptime(RESET_PULSE_WIDTH));
	}
}


/**
 * Read status register of dataflah memory.
 */
static uint8_t dataflash_stat(DataFlash *fd)
{
	/*
	 * Make sure to toggle CS signal
	 * and reset dataflash command decoder.
	 */
	CS_TOGGLE(fd);
	kfile_putc(DFO_READ_STATUS, fd->channel);
	return kfile_getc(fd->channel);
}


/**
 * Send one command to data flash memory, and
 * return status register value.
 *
 */
static uint8_t dataflash_cmd(DataFlash *fd, dataflash_page_t page_addr, dataflash_offset_t byte_addr, DataFlashOpcode opcode)
{
	uint8_t stat;

	send_cmd(fd, page_addr, byte_addr, opcode);

	CS_TOGGLE(fd);

	/*
	 * We chech data flash memory state, and wait until busy-flag
	 * is high.
	 */
	while (!(dataflash_stat(fd) & BUSY_BIT))
		cpu_relax();

	stat = dataflash_stat(fd);

	kfile_flush(fd->channel); // Flush channel
	/*
	 * Data flash has completed a bus cycle, so disable CS.
	 */
	fd->setCS(false);

	return stat;
}

/**
 * Read \a len bytes from main data flash memory or buffer data
 * flash memory, and put it in \a *block.
 */
static void dataflash_readBlock(DataFlash *fd, dataflash_page_t page_addr, dataflash_offset_t byte_addr, uint8_t *block, dataflash_size_t len)
{
	DataFlashOpcode opcode = mem_info[fd->dev].read_cmd;
	send_cmd(fd, page_addr, byte_addr, opcode);

	if (opcode == DFO_READ_FLASH_MEM_BYTE_B)
	{
		/*
		 * Send 24 don't care bits.
		 */
		uint8_t dummy[] = { 0, 0, 0 };
		kfile_write(fd->channel, dummy, sizeof(dummy));
	}

	kfile_putc(0, fd->channel); //Send 8 don't care bit.
	kfile_read(fd->channel, block, len); //Read len bytes ad put in block buffer.
	kfile_flush(fd->channel); // Flush channel
	fd->setCS(false);
}


/**
 * Write \a len bytes in dataflash memory buffer.
 *
 * \note Is not possible to write directly in dataflash main memory.
 * To perform a write in main memory you must first write in dataflash buffer
 * memory and then send a command to write the page in main memory.
 */
static void dataflash_writeBlock(DataFlash *fd, dataflash_offset_t offset, const uint8_t *block, dataflash_size_t len)
{
	ASSERT(offset + len <= mem_info[fd->dev].page_size);

	send_cmd(fd, 0x00, offset, DFO_WRITE_BUFF1);

	kfile_write(fd->channel, block, len); //Write len bytes.
	kfile_flush(fd->channel); // Flush channel

	fd->setCS(false);
}


/**
 * Load selected page from dataflash memory to buffer.
 */
static void dataflash_loadPage(DataFlash *fd, dataflash_page_t page_addr)
{
	dataflash_cmd(fd, page_addr, 0x00, DFO_MOV_MEM_TO_BUFF1);
}

/* Battfs disk interface section */
#if 0
static size_t dataflash_disk_page_read(struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t len)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	dataflash_readBlock(fd, page, addr, buf, len);
	return len;
}

static bool dataflash_disk_page_load(struct BattFsSuper *d, pgcnt_t page)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	dataflash_loadPage(fd, page);
	return true;
}

static size_t dataflash_disk_buffer_write(struct BattFsSuper *d, pgaddr_t addr, const void *buf, size_t len)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	dataflash_writeBlock(fd, addr, buf, len);
	return len;
}

static size_t dataflash_disk_buffer_read(struct BattFsSuper *d, pgaddr_t addr, void *buf, size_t len)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	ASSERT(addr + len <= mem_info[fd->dev].page_size);

	CS_TOGGLE(fd);

	kfile_putc(DFO_READ_BUFF1, fd->channel);

	uint32_t byte_addr = addr;

	kfile_putc((byte_addr >> 16) & 0xff, fd->channel);
	kfile_putc((byte_addr >> 8) & 0xff, fd->channel);
	kfile_putc(byte_addr & 0xff, fd->channel);

	/* Send additional don't care byte to start read operation */
	kfile_putc(0, fd->channel);

	kfile_read(fd->channel, buf, len); //Read len bytes ad put in buffer.
	kfile_flush(fd->channel); // Flush channel
	fd->setCS(false);
	return len;
}

static bool dataflash_disk_page_save(struct BattFsSuper *d, pgcnt_t page)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	dataflash_cmd(fd, page, 0x00, DFO_WRITE_BUFF1_TO_MEM);
	return true;
}

static bool dataflash_disk_page_erase(struct BattFsSuper *d, pgcnt_t page)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	dataflash_cmd(fd, page, 0x00, DFO_ERASE_PAGE);
	return true;
}

static int dataflash_close(struct KFile *_fd);

static bool dataflash_disk_close(struct BattFsSuper *d)
{
	DataFlash *fd = DATAFLASH_CAST((KFile *)d->disk_ctx);
	return dataflash_close(&fd->fd) == 0;
}

bool dataflash_diskInit(struct BattFsSuper *d, DataFlash *fd, pgcnt_t *page_array)
{
	ASSERT(d);
	ASSERT(fd);
	d->read = dataflash_disk_page_read;
	d->load = dataflash_disk_page_load;
	d->bufferWrite = dataflash_disk_buffer_write;
	d->bufferRead = dataflash_disk_buffer_read;
	d->save = dataflash_disk_page_save;
	d->erase = dataflash_disk_page_erase;
	d->close = dataflash_disk_close;
	d->disk_ctx = fd;
	d->page_size = mem_info[fd->dev].page_size;
	d->page_count = mem_info[fd->dev].page_cnt;
	ASSERT(page_array);
	d->page_array = page_array;
	return d->page_array && fd;
}
#endif

/* Kfile interface section */

/**
 * Flush select page (stored in buffer) in data flash main memory page.
 */
static int dataflash_flush(KFile *_fd)
{
	DataFlash *fd = DATAFLASH_CAST(_fd);
	if (fd->page_dirty)
	{
		dataflash_cmd(fd, fd->current_page, 0x00, DFO_WRITE_BUFF1_TO_MEM_E);

		fd->page_dirty = false;

		LOG_INFO("Flushing page {%ld}\n", fd->current_page);
	}
	return 0;
}


/**
 * Close file \a fd.
 */
static int dataflash_close(struct KFile *_fd)
{
	dataflash_flush(_fd);
	LOG_INFO("Close.\n");
	return 0;
}

/**
 * Reopen dataflash file \a fd.
 */
static KFile *dataflash_reopen(KFile *_fd)
{
	DataFlash *fd = DATAFLASH_CAST(_fd);
	dataflash_close(_fd);

	fd->current_page = 0;
	fd->fd.seek_pos = 0;

	/* Load selected page from dataflash memory */
	dataflash_loadPage(fd, fd->current_page);

	LOG_INFO("Reopen.\n");
	return &fd->fd;
}


/**
 * Read in \a buf \a size bytes from dataflash memmory.
 *
 * \note For reading data flash memory, we
 * check flag page_dirty, if is true (that mean
 * we have written a byte in buffer memory) we
 * flush current page in main memory and
 * then read from memory, else we read byte
 * directly from data flash main memory.
 *
 * \return the number of bytes read.
 */
static size_t dataflash_read(struct KFile *_fd, void *buf, size_t size)
{
	DataFlash *fd = DATAFLASH_CAST(_fd);

	dataflash_offset_t byte_addr;
	dataflash_page_t page_addr;
	uint8_t *data = (uint8_t *)buf;


	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= fd->fd.size);
	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Reading at pos[%lu]\n", fd->fd.seek_pos);

	/*
	 * We select page and offest from absolute address.
	 */
	page_addr = fd->fd.seek_pos / mem_info[fd->dev].page_size;
	byte_addr = fd->fd.seek_pos % mem_info[fd->dev].page_size;

	LOG_INFO("[page-{%ld}, byte-{%ld}]\n", page_addr, byte_addr);

	/*
	 * Flush current page in main memory if
	 * we had been written a byte in memory
	 */
	dataflash_flush(&fd->fd);

	/*
	 * Read byte in main page data flash memory.
	 */
	dataflash_readBlock(fd, page_addr, byte_addr, data, size);

	fd->fd.seek_pos += size;
	LOG_INFO("Read %ld bytes\n", (long int)size);

	return size;
}

/**
 * Write \a _buf in dataflash memory
 *
 * \note For writing \a _buf in dataflash memory, we must
 * first write in buffer data flash memory. At the end of write,
 * we can put page in dataflash main memory.
 * If we write in two contiguous pages, we put in main memory current
 * page and then reload the page which we want to write.
 *
 * \return the number of bytes write.
 */
static size_t dataflash_write(struct KFile *_fd, const void *_buf, size_t size)
{
	DataFlash *fd = DATAFLASH_CAST(_fd);

	dataflash_offset_t offset;
	dataflash_page_t new_page;
	size_t total_write = 0;

	const uint8_t *data = (const uint8_t *) _buf;

	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= fd->fd.size);
	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Writing at pos[%lu]\n", fd->fd.seek_pos);

	while (size)
	{
		/*
		* We select page and offest from absolute address.
		*/
		new_page = fd->fd.seek_pos / mem_info[fd->dev].page_size;
		offset = fd->fd.seek_pos % mem_info[fd->dev].page_size;


		size_t wr_len = MIN((dataflash_size_t)size, mem_info[fd->dev].page_size - offset);

		LOG_INFO("[page-{%ld}, byte-{%ld}]\n",new_page, offset);

		if (new_page != fd->current_page)
		{
			/* Flush current page in main memory*/
			dataflash_flush(&fd->fd);
			/* Load select page memory from data flash memory*/
			dataflash_loadPage(fd, new_page);

			fd->current_page = new_page;
			LOG_INFO(" >> Load page: {%ld}\n", new_page);
		}
		/*
		* Write byte in current page, and set true
		* page_dirty flag.
		*/
		dataflash_writeBlock(fd, offset, data, wr_len);
		fd->page_dirty = true;

		data += wr_len;
		fd->fd.seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}

	LOG_INFO("written %lu bytes\n", (long unsigned)total_write);
	return total_write;
}

MOD_DEFINE(dataflash);

/**
 * Dataflash init function.
 * This function initialize \a fd with SPI channel \a ch and test if data flash memory
 * density is the same specified by device \a dev.
 * \a setCS is a callback used to set/reset CS line.
 * \a setReset is a callback used to set/reset the dataflash (can be NULL if reset is unconnected)
 * \return true if ok, false if memory density read from dataflash is not compliant with the
 * configured one.
 */
bool dataflash_init(DataFlash *fd, KFile *ch, DataflashType dev, dataflash_setCS_t *setCS, dataflash_setReset_t *setReset)
{
	uint8_t stat;

	MOD_CHECK(hw_dataflash);

	ASSERT(fd);
	ASSERT(ch);
	ASSERT(setCS);
	ASSERT(dev < DFT_CNT);

	memset(fd, 0, sizeof(*fd));
	DB(fd->fd._type = KFT_DATAFLASH);
	fd->dev = dev;
	fd->channel = ch;
	fd->setReset = setReset;
	fd->setCS = setCS;

	// Setup data flash programming functions.
	fd->fd.reopen = dataflash_reopen;
	fd->fd.close = dataflash_close;
	fd->fd.read = dataflash_read;
	fd->fd.write = dataflash_write;
	fd->fd.seek = kfile_genericSeek;
	fd->fd.flush = dataflash_flush;

	dataflash_reset(fd);
	stat = dataflash_stat(fd);

	/*
	 * 2,3,4,5 bits of 1 byte status register
	 * indicate a device density of dataflash memory
	 * (see datasheet for more detail.)
	 */
	if (GET_ID_DESITY_DEVICE(stat) != mem_info[fd->dev].density_id)
		return false;

	fd->current_page = 0;
	fd->fd.seek_pos = 0;
	fd->fd.size = mem_info[fd->dev].page_size * mem_info[fd->dev].page_cnt;

	/* Load selected page from dataflash memory */
	dataflash_loadPage(fd, fd->current_page);
	MOD_INIT(dataflash);
	return true;
}
