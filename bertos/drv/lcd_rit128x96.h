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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief OLED-RIT-128x96 (P14201) graphic display driver
 *
 * $WIZ$ module_name = "lcd_rit128x96"
 * $WIZ$ module_hw = "bertos/hw/hw_rit128x96.h"
 */

#ifndef LCD_LM3S_H
#define LCD_LM3S_H

#include <gfx/gfx.h> /* Bitmap */

#include <cpu/types.h>

#define LCD_WIDTH	128
#define LCD_HEIGHT	96

void rit128x96_blitRaw(const uint8_t *data,
		uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void rit128x96_blitBitmap(const Bitmap *bm);
void rit128x96_on(void);
void rit128x96_off(void);
void rit128x96_init(void);

#endif /* LCD_LM3S_H */
