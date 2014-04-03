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
 * \brief Function library for serial Flash memory.
 *
 * Module provide a kfile interface, that ensure an abstraction
 * from comunication channel and give a standard interface.
 * Typicaly this kind of memory use an SPI bus, but you should
 * use another comunication channel you have defined.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

/*
 * We use a spi bus, thus include hardware specific definition.
 * If you use another channel you must redefine this macros.
 */
#include "hw/hw_spi.h"

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <drv/timer.h>
#include <drv/flash25.h>

#include <io/kfile.h>

#include <cpu/power.h> /* cpu_relax() */

#warning FIXME:This file was changed, but is untested!

/**
 * Wait until flash memory is ready.
 */
static void flash25_waitReady(Flash25 *fd)
{
	uint8_t stat;

	while (1)
	{
		CS_ENABLE();

		kfile_putc(FLASH25_RDSR, fd->channel);
		stat = kfile_getc(fd->channel);

		CS_DISABLE();

		if (!(stat & RDY_BIT))
			break;

		cpu_relax();
	}
}

/**
 * Send a single command to serial flash memory.
 */
static void flash25_sendCmd(Flash25 *fd, Flash25Opcode cmd)
{
	CS_ENABLE();

	kfile_putc(cmd, fd->channel);

	CS_DISABLE();
}

/**
 * flash25 init function.
 * This function init a comunication channel and
 * try to read manufacturer id of serial memory,
 * then check if is equal to selected type.
 */
static bool flash25_pin_init(Flash25 *fd)
{
	uint8_t device_id;
	uint8_t manufacturer;

	SPI_HW_INIT();

	CS_ENABLE();
	/*
	 * Send read id productor opcode on
	 * comunication channel
	 * TODO:controllare se ha senso
	 */
	kfile_putc(FLASH25_RDID, fd->channel);

	manufacturer = kfile_getc(fd->channel);
	device_id = kfile_getc(fd->channel);

	CS_DISABLE();

	if((FLASH25_MANUFACTURER_ID == manufacturer) &&
		(FLASH25_DEVICE_ID == device_id))
		return true;
	else
		return false;
}

/**
 * Reopen a serial memory interface.
 *
 * For serial memory this function reinit only
 * the size and seek_pos in kfile stucture.
 * Return a kfile pointer, after assert check.
 */
static KFile * flash25_reopen(struct KFile *_fd)
{
	Flash25 *fd = FLASH25_CAST(_fd);

	fd->fd.seek_pos = 0;
	fd->fd.size = FLASH25_MEM_SIZE;

	kprintf("flash25 file opened\n");
	return &fd->fd;
}

/**
 * Close a serial memory interface.
 *
 * For serial memory this funtion do nothing,
 * and return always 0.
 */
static int flash25_close(UNUSED_ARG(struct KFile *,fd))
{
	kprintf("flash25 file closed\n");
	return 0;
}

/**
 * Read \a _buf lenght \a size byte from serial flash memmory.
 *
 * For read in serial flash memory we
 * enble cs pin and send one byte of read opcode,
 * and then 3 byte of address of memory cell we
 * want to read. After the last byte of address we
 * can read data from so pin.
 *
 * \return the number of bytes read.
 */
static size_t flash25_read(struct KFile *_fd, void *buf, size_t size)
{
	uint8_t *data = (uint8_t *)buf;

	Flash25 *fd = FLASH25_CAST(_fd);

	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= fd->fd.size);
	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	//kprintf("Reading at addr[%lu], size[%d]\n", fd->seek_pos, size);
	CS_ENABLE();

	kfile_putc(FLASH25_READ, fd->channel);


	/*
	 * Address that we want to read.
	 */
	kfile_putc((fd->fd.seek_pos >> 16) & 0xFF, fd->channel);
	kfile_putc((fd->fd.seek_pos >> 8) & 0xFF, fd->channel);
	kfile_putc(fd->fd.seek_pos & 0xFF, fd->channel);

	kfile_read(fd->channel, data, size);

	CS_DISABLE();

	fd->fd.seek_pos += size;

	return size;
}

/**
 * Write \a _buf in serial flash memory
 *
 * Before to write data into flash we must enable
 * memory writing. To do this we send a WRE command opcode.
 * After this command the flash is ready to be write, and so
 * we send a PROGRAM opcode followed to 3 byte of
 * address memory, at the end of last address byte
 * we can send the data.
 * When we finish to send all data, we disable cs
 * and flash write received data bytes on its memory.
 *
 * \note: WARNING: you could write only on erased memory section!
 * Each write time you could write max a memory page size,
 * because if you write more than memory page size the
 * address roll over to first byte of page.
 *
 * \return the number of bytes write.
 */
static size_t flash25_write(struct KFile *_fd, const void *_buf, size_t size)
{
	flash25Offset_t offset;
	flash25Size_t total_write = 0;
	flash25Size_t wr_len;
	const uint8_t *data = (const uint8_t *) _buf;

	Flash25 *fd = FLASH25_CAST(_fd);

	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= fd->fd.size);

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	while (size)
	{
		offset = fd->fd.seek_pos % (flash25Size_t)FLASH25_PAGE_SIZE;
		wr_len = MIN((flash25Size_t)size, FLASH25_PAGE_SIZE - (flash25Size_t)offset);

		kprintf("[seek_pos-<%lu>, offset-<%d>]\n", fd->fd.seek_pos, offset);

		/*
		 * We check serial flash memory state, and wait until ready-flag
		 * is high.
		 */
		flash25_waitReady(fd);

		/*
		 * Start write cycle.
		 * We could write only data not more long than one
		 * page size.
		 *
		 * To write on serial flash memory we must first
		 * enable write with a WREN opcode command, before
		 * the PROGRAM opcode.
		 *
		 * \note: the same byte cannot be reprogrammed without
		 * erasing the whole sector first.
		 */
		flash25_sendCmd(fd, FLASH25_WREN);

		CS_ENABLE();
		kfile_putc(FLASH25_PROGRAM, fd->channel);

		/*
		 * Address that we want to write.
		 */
		kfile_putc((fd->fd.seek_pos >> 16) & 0xFF, fd->channel);
		kfile_putc((fd->fd.seek_pos >> 8) & 0xFF, fd->channel);
		kfile_putc(fd->fd.seek_pos & 0xFF, fd->channel);

		kfile_write(fd->channel, data, wr_len);

		CS_DISABLE();

		data += wr_len;
		fd->fd.seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}

	kprintf("written %lu bytes\n", total_write);
	return total_write;
}

/**
 * Sector erase function.
 *
 * Erase a select \p sector of serial flash memory.
 *
 * \note A sector size is FLASH25_SECTOR_SIZE.
 * This operation could take a while.
 */
void flash25_sectorErase(Flash25 *fd, Flash25Sector sector)
{

	/*
	 * Erase a sector could take a while,
	 * for debug we measure that time
	 * see datasheet to compare this time.
	 */
	DB(ticks_t start_time = timer_clock());

	CS_ENABLE();

	/*
	 * To erase a sector of serial flash memory we must first
	 * enable write with a WREN opcode command, before
	 * the SECTOR_ERASE opcode. Sector is automatically
	 * determinate if any address within the sector
	 * is selected.
	 */
	kfile_putc(FLASH25_WREN, fd->channel);
	kfile_putc(FLASH25_SECTORE_ERASE,fd-> channel);

	/*
	 * Address inside the sector that we want to
	 * erase.
	 */
	kfile_putc(sector, fd->channel);

	CS_DISABLE();

	/*
	 * We check serial flash memory state, and wait until ready-flag
	 * is hight.
	 */
	flash25_waitReady(fd);

	DB(kprintf("Erased sector [%ld] in %ld ms\n", (unsigned long)sector, (unsigned long)ticks_to_ms(timer_clock() - start_time)));
}

/**
 * Chip erase function.
 *
 * Erase all sector of serial flash memory.
 *
 * \note This operation could take a while.
 */
void flash25_chipErase(Flash25 *fd)
{
	/*
	 * Erase all chip could take a while,
	 * for debug we measure that time
	 * see datasheet to compare this time.
	 */
	DB(ticks_t start_time = timer_clock());

	/*
	 * To erase serial flash memory we must first
	 * enable write with a WREN opcode command, before
	 * the CHIP_ERASE opcode.
	 */
	flash25_sendCmd(fd, FLASH25_WREN);
	flash25_sendCmd(fd, FLASH25_CHIP_ERASE);

	/*
	 * We check serial flash memory state, and wait until ready-flag
	 * is high.
	 */
	flash25_waitReady(fd);

	DB(kprintf("Erased all memory in %ld ms\n", ticks_to_ms(timer_clock() - start_time)));

}

/**
 * Init data flash memory interface.
 */
void flash25_init(Flash25 *fd, KFile *ch)
{

	ASSERT(fd);
	ASSERT(ch);

	 //Set kfile struct type as a generic kfile structure.
	DB(fd->fd._type = KFT_FLASH25);

	// Set up data flash programming functions.
	fd->fd.reopen = flash25_reopen;
	fd->fd.close = flash25_close;
	fd->fd.read = flash25_read;
	fd->fd.write = flash25_write;
	fd->fd.seek = kfile_genericSeek;

	/*
	 * Init a local channel structure and flash kfile interface.
	 */
	fd->channel = ch;
	flash25_reopen(&fd->fd);

	/*
	 * Init data flash memory and micro pin.
	 */
	if (!flash25_pin_init(fd))
		ASSERT(0);
}


