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
 *
 * -->
 *
 * \brief Function library for serial Flash memory.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "flash25"
 * $WIZ$ module_depends = "kfile"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_flash25.h"
 */


#ifndef DRV_FLASH25_H
#define DRV_FLASH25_H

#include "cfg/cfg_flash25.h"
#include <cfg/compiler.h>

#include <io/kfile.h>

/**
 * Type definition for serial flash memory.
 */
typedef uint32_t flash25Addr_t;
typedef uint32_t flash25Size_t;
typedef uint8_t flash25Offset_t;

/**
 * Flash25 KFile context structure.
 */
typedef struct Flash25
{
	KFile fd;                       ///< File descriptor.
	KFile *channel;                 ///< Dataflash comm channel (usually SPI).
} Flash25;

/**
 * ID for dataflash.
 */
#define KFT_FLASH25 MAKE_ID('F', 'L', '2', '5')


/**
 * Convert + ASSERT from generic KFile to Flash25.
 */
INLINE Flash25 * FLASH25_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FLASH25);
	return (Flash25 *)fd;
}

/**
 * Memory definition.
 *
 * \note Below are defined valid serial flash memory support to
 * this drive. Every time we call flash25_init() function we check
 * if memory defined are right (see flash25.c form more detail).
 *
 * $WIZ$ flash25_list = "FLASH25_AT25F2048"
 */
#define FLASH25_AT25F2048         1

#if CONFIG_FLASH25 == FLASH25_AT25F2048
	#define FLASH25_MANUFACTURER_ID    0x1F  // ATMEL
	#define FLASH25_DEVICE_ID          0x63  // Device ID
	#define FLASH25_PAGE_SIZE          256   // Page size in byte
	#define FLASH25_NUM_SECTOR         4     // Number of section in serial memory
	#define FLASH25_SECTOR_SIZE        65536UL // Section size in byte
	#define FLASH25_MEM_SIZE           FLASH25_NUM_SECTOR * FLASH25_SECTOR_SIZE
	#define FLASH25_NUM_PAGE           FLASH25_MEM_SIZE / FLASH25_PAGE_SIZE
#else
	#error Nothing memory defined in CONFIG_FLASH25 are support.
#endif

#define RDY_BIT     0x1 // Statuts of write cycle

/**
 * Serial flash opcode commands.
 */
typedef enum {
	FLASH25_WREN            = 0x6,  ///< Set write enable latch
	FLASH25_WRDI            = 0x4,  ///< Reset enable write latch
	FLASH25_RDSR            = 0x5,  ///< Read status register
	FLASH25_WRSR            = 0x1,  ///< Write status register
	FLASH25_READ            = 0x3,  ///< Read data from memory array
	FLASH25_PROGRAM         = 0x2,  ///< Program data into memory array
	FLASH25_SECTORE_ERASE   = 0x52, ///< Erase one sector in memory array
	FLASH25_CHIP_ERASE      = 0x62, ///< Erase all sector in memory array
	FLASH25_RDID            = 0x15 ///< Read Manufacturer and product ID
} Flash25Opcode;

/**
 * Serial flash sector memory address.
 */
#if CONFIG_FLASH25 != FLASH25_AT25F2048
	#error Nothing memory defined in CONFIG_FLASH25 are support.
#endif

typedef enum {
	FLASH25_SECT1            = 0x0,      ///< Sector 1 (0x0 -0xFFFF)
	FLASH25_SECT2            = 0x10000,  ///< Sector 2 (0x10000 -0x1FFFF)
	FLASH25_SECT3            = 0x20000,  ///< Sector 3 (0x20000 -0x2FFFF)
	FLASH25_SECT4            = 0x30000,  ///< Sector 4 (0x30000 -0x3FFFF)
} Flash25Sector;

void flash25_init(Flash25 *fd, KFile *ch);
void flash25_chipErase(Flash25 *fd);
void flash25_sectorErase(Flash25 *fd, Flash25Sector sector);
bool flash25_test(KFile *channel);

#endif /* DRV_FLASH25_H */

