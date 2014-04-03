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
* Copyright 2005 Develer S.r.l. (http://www.develer.com/)
* -->
*
* \defgroup drv_emb_flash Embedded flash driver
* \ingroup drivers
* \{
*
* \brief Embedded flash for cpu.
*
* This module allows to access in reading and writing to the internal
* flash memory of the micro. It is a block device, so it must be
* accessed using the KBlock interface functions (see kblock.h).
*
* Once you have opened the flash for writing, you may want to use
* kblock_trim() to avoid overwriting data on other flash banks.
*
* Example usage:
* \code
* Flash fls;
* flash_init(&fls, 0);
* // enable access only on desired blocks
* // start block = 50, num blocks = 20
* kblock_trim(&fls.blk, 50, 20);
* // ...
* // now write to the flash
* // block number is automatically converted
* kblock_write(&fls.blk, 0, buf, 0, 128);
* \endcode
*
* \author Francesco Sacchi <batt@develer.com>
* \author Daniele Basile <asterix@develer.com>
*
* $WIZ$ module_name = "flash"
* $WIZ$ module_depends = "kfile", "kfile_block", "kblock"
* $WIZ$ module_configuration = "bertos/cfg/cfg_emb_flash.h"
*/

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#include "cfg/cfg_emb_flash.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <io/kblock.h>
#include <io/kfile.h>
#include <io/kfile_block.h>

#include <cpu/attr.h>

#if COMPILER_C99
	#define flash_init(...)           PP_CAT(flash_init_, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
#else
	/**
	 * Init function for flash driver.
	 *
	 * This macro cannot fail, so no error conditions are reported.
	 *
	 * This macro expands to
	 *  - flash_init_2(Flash *fls, flags), the new KBlock API
	 *  - flash_init_1(Flash *fls), old API, provided for compatibility
	 *
	 * Do NOT use the above functions directly, use flash_init() instead.
	 * Disable old API if you are not upgrading an existing project.
	 */
	#define flash_init(args...)       PP_CAT(flash_init_, COUNT_PARMS(args)) (args)
#endif

/**
 * \name Embedded flash error values
 * \{
 */
#define FLASH_WR_OK             0     ///< Write ok.
#define FLASH_NOT_ERASED     BV(1)    ///< Flash memory was not erased before to write it.
#define FLASH_WR_PROTECT     BV(2)    ///< Write not allowed the flash memory was protected.
#define FLASH_WR_TIMEOUT     BV(3)    ///< Timeout while writing
#define FLASH_WR_ERR         BV(4)    ///< Invalid command and/or a bad keywords
/** \} */

struct FlashHardware;

/**
 * EmbFlash KBlock context structure.
 */
typedef struct Flash
{
	KBlock blk;                  ///< KBlock context
	struct FlashHardware *hw;
	#if !CONFIG_FLASH_DISABLE_OLD_API
	union {
		KFile fd;
		KFileBlock fdblk;
	} DEPRECATED;
	#endif /* !CONFIG_FLASH_DISABLE_OLD_API */
} Flash;

/*
 * ID for FLASH
 */
#define KBT_FLASH MAKE_ID('F', 'L', 'A', 'S')

/**
* Convert + ASSERT from generic KBlock to Flash.
*/
INLINE Flash *FLASH_CAST(KBlock *fls)
{
	ASSERT(fls->priv.type == KBT_FLASH);
	return (Flash *)fls;
}

void flash_hw_init(Flash *fls, int flags);
void flash_hw_initUnbuffered(Flash *fls, int flags);

#include CPU_HEADER(flash)

/**
 * \name Flash init flags
 * \{
 */
#define FLASH_WRITE_ONCE   BV(0) ///< Allow only one write per block.
#define FLASH_UNBUFFERED   BV(1) ///< Open flash memory disabling page caching, no modification and partial write are allowed.
/** \} */

/**
 * Initialize \a fls Flash context structure.
 * \param fls Flash context structure
 * \param flags A combination of flash init flags
 */
#define flash_init_2(fls, flags)    (flags & FLASH_UNBUFFERED) ? \
										flash_hw_initUnbuffered(fls, flags) : flash_hw_init(fls, flags)

#if !CONFIG_FLASH_DISABLE_OLD_API
INLINE DEPRECATED void flash_init_1(Flash *fls)
{
	flash_hw_init(fls, 0);
	kfileblock_init(&fls->fdblk, &fls->blk);
}
#endif /* !CONFIG_FLASH_DISABLE_OLD_API */

/** \} */

#endif /* DRV_FLASH_H */
