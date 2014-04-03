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
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for DAC module.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_DAC_H
#define CFG_DAC_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define DAC_LOG_LEVEL      LOG_LVL_WARN

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define DAC_LOG_FORMAT     LOG_FMT_TERSE

/**
 * DAC Refresh Period = 1024*REFRESH/DACC Clock
 *
 * $WIZ$ type = "int"
 * $WIZ$ supports = "sam3x"
 * $WIZ$ min = 0
 * $WIZ$ max = 65536
 */
#define CONFIG_DAC_REFRESH            16

/**
 * DAC Startup Time Selection.
 * see datasheet table.
 *
 * $WIZ$ type = "int"
 * $WIZ$ supports = "sam3x"
 * $WIZ$ min = 0
 * $WIZ$ max = 63
 */
#define CONFIG_DAC_STARTUP             0

/**
 * DAC Trigger Selection.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "sam3x_dac_tc"
 * $WIZ$ supports = "sam3x"
 */
#define CONFIG_DAC_TIMER  DACC_TRGSEL_TIO_CH0

#endif /* CFG_DAC_H */
