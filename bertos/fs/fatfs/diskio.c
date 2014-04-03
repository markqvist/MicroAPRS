/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "ff.h"

#include <io/kblock.h>

#include "cfg/cfg_fat.h"
#define LOG_LEVEL   FAT_LOG_LEVEL
#define LOG_FORMAT  FAT_LOG_FORMAT
#include <cfg/log.h>

static KBlock *devs[_DRIVES];

void disk_assignDrive(KBlock *dev, int dev_num)
{
	ASSERT(dev_num < _DRIVES);
	devs[dev_num] = dev;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	return disk_status(drv);
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	KBlock *dev = devs[drv];
	ASSERT(dev);

	if (kblock_error(dev) != 0)
		return STA_NOINIT;
	else
		return RES_OK;
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
	KBlock *dev = devs[drv];
	ASSERT(dev);


	while (count--)
	{
		if (kblock_read(dev, sector++, buff, 0, dev->blk_size) != dev->blk_size)
			return RES_ERROR;
		buff += dev->blk_size;
	}
	return RES_OK;
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
	KBlock *dev = devs[drv];
	ASSERT(dev);

	while (count--)
	{
		if (kblock_write(dev, sector++, buff, 0, dev->blk_size) != dev->blk_size)
			return RES_ERROR;
		buff += dev->blk_size;
	}
	return RES_OK;
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
	KBlock *dev = devs[drv];
	ASSERT(dev);


	switch (ctrl)
	{
		case CTRL_SYNC:
			if (kblock_flush(dev) == 0)
				return RES_OK;
			else
				return RES_ERROR;

		case GET_SECTOR_SIZE:
			*(WORD *)buff = dev->blk_size;
			return RES_OK;

		case GET_SECTOR_COUNT:
			*(DWORD *)buff = dev->blk_cnt;
			return RES_OK;

		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 1;
			return RES_OK;

		default:
			LOG_ERR("unknown command: [%d]\n", ctrl);
			return RES_PARERR;
	}
}


DWORD get_fattime(void)
{
	return 0;
}


