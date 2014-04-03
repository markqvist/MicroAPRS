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
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for Fat module.
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef CFG_FAT_H
#define CFG_FAT_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define FAT_LOG_LEVEL      LOG_LVL_ERR

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define FAT_LOG_FORMAT     LOG_FMT_VERBOSE


/**
 * Use word alignment to access FAT structure.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_WORD_ACCESS   0
#define _WORD_ACCESS CONFIG_FAT_WORD_ACCESS

/**
 * Enable read functions only.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_FS_READONLY   0
#define _FS_READONLY CONFIG_FAT_FS_READONLY

/**
 * Minimization level to remove some functions.
 * $WIZ$ type = "int"; min = 0; max = 3
 */
#define CONFIG_FAT_FS_MINIMIZE 0
#define _FS_MINIMIZE CONFIG_FAT_FS_MINIMIZE

/**
 * If enabled, this reduces memory consumption 512 bytes each file object by using a shared buffer.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_FS_TINY 1
#define	_FS_TINY CONFIG_FAT_FS_TINY

/**
 * To enable string functions, set _USE_STRFUNC to 1 or 2.
 * $WIZ$ type = "int"
 * $WIZ$ supports = "False"
 */
#define CONFIG_FAT_USE_STRFUNC 0
#define	_USE_STRFUNC CONFIG_FAT_USE_STRFUNC

/**
 * Enable f_mkfs function. Requires CONFIG_FAT_FS_READONLY = 0.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_USE_MKFS 0
#define	_USE_MKFS (CONFIG_FAT_USE_MKFS && !CONFIG_FAT_FS_READONLY)

/**
 * Enable f_forward function. Requires CONFIG_FAT_FS_TINY.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_USE_FORWARD 0
#define	_USE_FORWARD (CONFIG_FAT_USE_FORWARD && CONFIG_FAT_FS_TINY)

/**
 * Number of volumes (logical drives) to be used.
 * $WIZ$ type = "int"; min = 1; max = 255
 */
#define CONFIG_FAT_DRIVES 1
#define _DRIVES CONFIG_FAT_DRIVES

/**
 * Maximum sector size to be handled. (512/1024/2048/4096).
 * 512 for memory card and hard disk, 1024 for floppy disk, 2048 for MO disk
 * $WIZ$ type = "int"; min = 512; max = 4096
 */
#define CONFIG_FAT_MAX_SS 512
#define	_MAX_SS CONFIG_FAT_MAX_SS

/**
 * When _MULTI_PARTITION is set to 0, each volume is bound to the same physical
 * drive number and can mount only first primaly partition. When it is set to 1,
 * each volume is tied to the partitions listed in Drives[].
 * $WIZ$ type = "boolean"
 * $WIZ$ supports = "False"
 */
#define CONFIG_FAT_MULTI_PARTITION 0
#define	_MULTI_PARTITION CONFIG_FAT_MULTI_PARTITION

/**
 * Specifies the OEM code page to be used on the target system.
 * $WIZ$ type = "int"
 */
#define CONFIG_FAT_CODE_PAGE 850
#define _CODE_PAGE CONFIG_FAT_CODE_PAGE

/**
 * Support for long filenames. Enable only if you have a valid Microsoft license.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_FAT_USE_LFN 0
#define	_USE_LFN CONFIG_FAT_USE_LFN

/**
 * Maximum Long File Name length to handle.
 * $WIZ$ type = "int"; min = 8; max = 255
 */
#define CONFIG_FAT_MAX_LFN 255
#define	_MAX_LFN CONFIG_FAT_MAX_LFN

#endif /* CFG_FAT_H */
