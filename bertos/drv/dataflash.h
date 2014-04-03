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
 * \brief Function library for dataflash AT45DB family.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "dataflash"
 * $WIZ$ module_depends = "kfile"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_dataflash.h"
 * $WIZ$ module_hw = "bertos/hw/hw_dataflash.h", "bertos/hw/hw_dataflash.c"
 */


#ifndef DRV_DATAFLASH_H
#define DRV_DATAFLASH_H

#include <cfg/compiler.h>

#include <io/kfile.h>
#include <fs/battfs.h>

/**
 * Type definitions for dflash memory.
 * \{
 */
typedef uint32_t dataflash_page_t;
typedef uint32_t dataflash_offset_t;
typedef uint32_t dataflash_size_t;
/*\}*/

/**
 *
 */
typedef void (dataflash_setReset_t)(bool);
typedef void (dataflash_setCS_t)(bool);

/**
 * Memory definitions.
 *
 * List of supported memory devices by this drive.
 * Every time we call dataflash_init() we check device id to
 * ensure we choose the right memory configuration.
 * (see dataflash.c for more details).
 */
typedef enum DataflashType
{
	DFT_AT45DB041B = 0,
	DFT_AT45DB081D,
	DFT_AT45DB161D,
	DFT_AT45DB642D,
	DFT_CNT
} DataflashType;


/**
 * Dataflash KFile context structure.
 */
typedef struct DataFlash
{
	KFile fd;                       ///< File descriptor.
	KFile *channel;                 ///< Dataflash comm channel (usually SPI).
	DataflashType dev;              ///< Memory device type;
	dataflash_page_t current_page;  ///< Current loaded dataflash page.
	bool page_dirty;                ///< True if current_page is dirty (needs to be flushed).
	dataflash_setReset_t *setReset; ///< Callback used to set reset pin of dataflash.
	dataflash_setCS_t *setCS;       ///< Callback used to set CS pin of dataflash.
} DataFlash;

/**
 * ID for dataflash.
 */
#define KFT_DATAFLASH MAKE_ID('D', 'F', 'L', 'H')

/**
 * Convert + ASSERT from generic KFile to DataFlash.
 */
INLINE DataFlash * DATAFLASH_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_DATAFLASH);
	return (DataFlash *)fd;
}

#define RESET_PULSE_WIDTH     10 ///< Width of reset pulse in usec.
#define BUSY_BIT            0x80 ///< Select a busy bit in status register.
#define CMP_BIT             0x40 ///< Select a compare bit in status register.

/**
 * Select bits 2-5 of status register. These
 * bits indicate device density (see datasheet for
 * more details).
 */
#define GET_ID_DESITY_DEVICE(reg_stat) (((reg_stat) & 0x3C) >> 2)

/**
 * Data flash opcode commands.
 */
typedef enum DataFlashOpcode {
	/**
	* Dataflash read commands.
	* \{
	*/
	DFO_READ_FLASH_MEM_BYTE_D  = 0x0B, ///< Continuos array read for D type memories.
	DFO_READ_FLASH_MEM_BYTE_B  = 0xE8, ///< Continuos array read for B type memories.

	DFO_READ_FLASH_MEM       = 0xD2, ///< Main memory page read.
	DFO_READ_BUFF1           = 0xD4, ///< SRAM buffer 1 read.
	DFO_READ_BUFF2           = 0xD6, ///< SRAM buffer 2 read.
	/* \}*/

	/**
	* Program and erase commands data flash.
	* \{
	*/
	DFO_WRITE_BUFF1          =  0x84, ///< SRAM buffer 1 write.
	DFO_WRITE_BUFF2          =  0x87, ///< SRAM buffer 2 write.
	DFO_WRITE_BUFF1_TO_MEM_E =  0x83, ///< Buffer 1 to main memory page program with built-in erase.
	DFO_WRITE_BUFF2_TO_MEM_E =  0x86, ///< Buffer 2 to main memory page program with built-in erase.
	DFO_WRITE_BUFF1_TO_MEM   =  0x88, ///< Buffer 1 to main memory page program without built-in erase.
	DFO_WRITE_BUFF2_TO_MEM   =  0x89, ///< Buffer 2 to main memory page program without built-in erase.
	DFO_ERASE_PAGE           =  0x81, ///< Erase page.
	DFO_ERASE_BLOCK          =  0x50, ///< Erase block.
	DFO_ERASE_SECTOR         =  0x7C, ///< Erase sector.
	DFO_WRITE_MEM_TR_BUFF1   =  0x82, ///< Write main memory page through buffer 1.
	DFO_WRITE_MEM_TR_BUFF2   =  0x85, ///< Write main memory page through buffer 2.
	/* \}*/

	/**
	* Additional dataflash commands.
	* \{
	*/
	DFO_MOV_MEM_TO_BUFF1     =  0x53, ///< Transfer main mmemory to buffer 1.
	DFO_MOV_MEM_TO_BUFF2     =  0x55, ///< Transfer main mmemory to buffer 2.
	DFO_CMP_MEM_TO_BUFF1     =  0x60, ///< Compare main mmemory with buffer 1.
	DFO_CMP_MEM_TO_BUFF2     =  0x61, ///< Compare main mmemory with buffer 2.
	DFO_ARW_MEM_TR_BUFF1     =  0x58, ///< Auto page rewrite through buffer 1.
	DFO_ARW_MEM_TR_BUFF2     =  0x59, ///< Auto page rewrite through buffer 2
	DFO_PWR_DOWN             =  0xB9, ///< Deep power-down.
	DFO_RESUME_PWR_DOWN      =  0xAB, ///< Resume from deep power-down.
	DFO_READ_STATUS          =  0xD7, ///< Read status register.
	DFO_ID_DEV               =  0x9F  ///< Read manufacturer and device ID.
	/* \}*/
} DataFlashOpcode;


/**
 * Structure used to describe a dataflash memory.
 */
typedef struct DataflashInfo
{
	uint8_t density_id;       ///< Density id, used to check memory type.
	dataflash_size_t page_size;       ///< Page size, in bytes.
	uint8_t page_bits;        ///< Number of bits needed to access a page.
	uint16_t page_cnt;        ///< Number of pages on memory.
	DataFlashOpcode read_cmd; ///< Command to be used to perform a continuous array.
} DataflashInfo;


bool dataflash_init(DataFlash *fd, KFile *ch, DataflashType type, dataflash_setCS_t *setCS, dataflash_setReset_t *setReset);
bool dataflash_diskInit(struct BattFsSuper *d, DataFlash *fd, pgcnt_t *page_array);

/**
 * To test data falsh drive you could use
 * this functions. To use these functions make sure to include in your make file the
 * drv/datafalsh_test.c source.
 *
 * (see drv/datafalsh_test.c for more detail)
 */
int dataflash_testSetup(void);
/* For backward compatibility */
#define dataflash_testSetUp() dataflash_testSetup()
int dataflash_testRun(void);
int dataflash_testTearDown(void);

#endif /* DRV_DATAFLASH_H */
