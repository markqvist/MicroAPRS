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
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for GFX module.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_GFX_H
#define CFG_GFX_H

/**
 * Enable line clipping algorithm.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_GFX_CLIPPING  1

/**
 * Enable text rendering in bitmaps.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_GFX_TEXT  1

/**
 * Enable virtual coordinate system.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_GFX_VCOORDS  1

/**
 * Select bitmap pixel format.
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "bitmap_format"
 */
#define CONFIG_BITMAP_FMT  BITMAP_FMT_PLANAR_V_LSB

#endif /* CFG_GFX_H */

