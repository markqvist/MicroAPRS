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
 * \brief Configuration file Secure Digital module.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef CFG_SD_H
#define CFG_SD_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define SD_LOG_LEVEL      LOG_LVL_ERR

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define SD_LOG_FORMAT     LOG_FMT_VERBOSE


/**
 * Enable autoassignment of SD driver to disk drive number 0 of FatFs module.
 * $WIZ$ type = "boolean"
 * $WIZ$ conditional_deps = "fat"
 */
#define CONFIG_SD_AUTOASSIGN_FAT   1

/**
 * Enable backward compatibility for sd_init().
 * If enabled, sd_init() will allocate internally an Sd context,
 * otherwise sd_init() will need the context to be passed explicitly.
 *
 * $WIZ$ type = "boolean"
 */
#define CONFIG_SD_OLD_INIT   1

#endif /* CFG_SD_H */
