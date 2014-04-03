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
 * \brief Configuration file for AFSK1200 modem.
 *
 * \author Francesco Sacchi <asterix@develer.com>
 */

#ifndef CFG_AFSK_H
#define CFG_AFSK_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define AFSK_LOG_LEVEL      LOG_LVL_WARN

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define AFSK_LOG_FORMAT     LOG_FMT_TERSE


/**
 * AFSK discriminator filter type.
 *
 * $WIZ$ type = "enum"; value_list = "afsk_filter_list"
 */
#define CONFIG_AFSK_FILTER AFSK_CHEBYSHEV


/**
 * AFSK receiver buffer length.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 2
 */
#define CONFIG_AFSK_RX_BUFLEN 32

/**
 * AFSK transimtter buffer length.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 2
 */
#define CONFIG_AFSK_TX_BUFLEN 32

/**
 * AFSK DAC sample rate for modem outout.
 * $WIZ$ type = "int"
 * $WIZ$ min = 2400
 */
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

/**
 * AFSK RX timeout in ms, set to -1 to disable.
 * $WIZ$ type = "int"
 * $WIZ$ min = -1
 */
#define CONFIG_AFSK_RXTIMEOUT 0


/**
 * AFSK Preamble length in [ms], before starting transmissions.
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_AFSK_PREAMBLE_LEN 300UL



/**
 * AFSK Trailer length in [ms], before stopping transmissions.
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_AFSK_TRAILER_LEN 50UL

#endif /* CFG_AFSK_H */
