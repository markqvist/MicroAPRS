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
 * \brief Low level disk access for FatFs emulated.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/

#include <fs/fatfs/diskio.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define SECTOR_SIZE 512

static volatile DSTATUS Stat = STA_NOINIT;

/**
 * This is an example implementation, used to simulate the the calls to normal filesystem calls
 * It only works for drive 0.
 */


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

static FILE *fake_disk = 0;

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	if (drv)
		return STA_NOINIT; /* Support only drive 0 */
	// XXX: pay attention here: some functions call disk_initialize *after* it has
	//  been initialized for the first time.
	//  Here we just return the status (that should always be ~STA_NOINIT after the first
	//  call)
	if (fake_disk)
		return Stat;

	const char *path = "emuldisk.dsk";
	fake_disk = fopen(path, "w+");
	int err = errno;
	if (!fake_disk)
	{
		switch (err)
		{
		case EINVAL:
			fprintf(stderr, "invalid mode\n");
		default:
			return STA_NOINIT;
		}
	}
	Stat &= ~STA_NOINIT;
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	if (drv)
		return STA_NOINIT; /* Support only drive 0 */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	fseek(fake_disk, sector * SECTOR_SIZE, SEEK_SET);
	size_t read_items = fread(buff, SECTOR_SIZE, count, fake_disk);
	if (read_items == count)
		return RES_OK;
	else
	{
		if (feof(fake_disk))
			fprintf(stderr, "end-of-file\n");
		if (ferror(fake_disk))
			fprintf(stderr, "error\n");
		return RES_ERROR;
	}
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	fseek(fake_disk, sector * SECTOR_SIZE, SEEK_SET);
	size_t write_items = fwrite(buff, SECTOR_SIZE, count, fake_disk);
	if (write_items == count)
		return RES_OK;
	else
	{
		if (feof(fake_disk))
			fprintf(stderr, "end-of-file\n");
		if (ferror(fake_disk))
			fprintf(stderr, "error\n");
		return RES_ERROR;
	}
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	switch (ctrl)
	{
	case GET_SECTOR_SIZE:
		*(WORD*)buff = SECTOR_SIZE;
		break;
	case GET_SECTOR_COUNT:
		*(DWORD*)buff = 65536;
		break;
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1;
		break;
	case CTRL_SYNC:
		fflush(fake_disk);
		break;
	default:
		return RES_PARERR;
	}
	return RES_OK;
}

DWORD get_fattime(void)
{
	time_t tmp = time(0);
	struct tm *t = localtime(&tmp);
	DWORD tim = 0;
	// seconds
	tim |= (t->tm_sec / 2);
	// min
	tim |= (t->tm_min << 5);
	// hour
	tim |= (t->tm_hour << 11);
	// month day (1..31)
	tim |= (t->tm_mday << 16);
	// month (1..12)
	tim |= ((t->tm_mon + 1) << 21);
	// year (0..127)
	tim |= ((t->tm_year - 80) << 25);
	return tim;
}

