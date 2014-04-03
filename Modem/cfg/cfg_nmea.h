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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Configuration file for NMEA module.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_NMEA_H
#define CFG_NMEA_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define NMEA_LOG_LEVEL      LOG_LVL_ERR

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define NMEA_LOG_FORMAT     LOG_FMT_TERSE


/**
 * Maximum number of sentence parsers supported.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_NMEAP_MAX_SENTENCES         8

/**
 * Max length of a complete sentence. The standard says 82 bytes, but its probably
 * better to go at least 128 since some units don't adhere to the 82 bytes
 * especially for proprietary sentences.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_NMEAP_MAX_SENTENCE_LENGTH   255

/**
 * Max tokens in one sentence. 24 is enough for any standard sentence.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_NMEAP_MAX_TOKENS            24

#endif /* CFG_NMEA_H */

