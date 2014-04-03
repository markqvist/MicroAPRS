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
 * Copyright 2003, 2004, 2005, 2006, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Displaytech 32122A LCD driver configuration file.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */
#ifndef CFG_LCD_32122A_H
#define CFG_LCD_32122A_H

/**
 * Enable soft interrupt to refresh the LCD.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_LCD_SOFTINT_REFRESH     0

/**
 * Enable wait macro when display is busy.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_LCD_WAIT                0

/**
 * Display refresh time 32122a.
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 */
#define CONFIG_LCD_REFRESH           100

#endif /* CFG_LCD_32122A_H */

