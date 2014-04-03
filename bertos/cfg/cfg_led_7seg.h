/**
 * \file cfg_led_7seg.h
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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Configuration file for led 7 segment display.
 *
 * \author Fabio Bizzi <fbizzi@bizzi.org>
 *
 * \addtogroup SevenSegDisplay 7 Segments LED Displays Driver
 * \{
 *
 */

#ifndef CFG_LED_7SEG_H
#define CFG_LED_7SEG_H

/**
 * Use a Common Cathode display.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_LED_7SEG_CCAT       0

/**
 * Number of digit present in the LED display.
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 * $WIZ$ max = 8
 */
#define CONFIG_LED_7SEG_DIGIT      4

/**
 * Max lenght of the string to be displayed.
 * $WIZ$ type = "int"
 * $WIZ$ min = 16
 * $WIZ$ max = 255
 */
#define CONFIG_LED_7SEG_STRLEN     255

/**
 * Default scrolling speed (ms * CONFIG_LED_7SEG_RTIME).
 * $WIZ$ type = "int"
 */
#define CONFIG_LED_7SEG_SSPEED      10

/**
 * Default refresh time (ms).
 * $WIZ$ type = "int"
 */
#define CONFIG_LED_7SEG_RTIME      5

#endif /* CFG_LED_7SEG_H */
 /** \} */ //defgroup drivers

