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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Configuration file for NAND driver module.
 */

#ifndef CFG_NAND_H
#define CFG_NAND_H

/**
 * Page data size
 *
 * Size of the data section of a programmable page in bytes.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_DATA_SIZE         2048

/**
 * Page spare area size
 *
 * Size of the spare section of a programmable page in bytes.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_SPARE_SIZE        64

/**
 * Pages per block
 *
 * Number of pages in a erase block.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_PAGES_PER_BLOCK   64

/**
 * Number of blocks
 *
 * Total number of erase blocks in one NAND chip.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_NUM_BLOCK        2048

/**
 * Number of reserved blocks
 *
 * Blocks reserved for remapping defective NAND blocks.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_NUM_REMAP_BLOCKS  128

/**
 * NAND operations timeout
 *
 * How many milliseconds the cpu waits for
 * completion of NAND operations.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_NAND_TMOUT      100

/**
 * Module logging level
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define CONFIG_NAND_LOG_LEVEL      LOG_LVL_WARN

/**
 * Module logging format
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define CONFIG_NAND_LOG_FORMAT     LOG_FMT_TERSE

#endif /* CFG_NAND_H */
