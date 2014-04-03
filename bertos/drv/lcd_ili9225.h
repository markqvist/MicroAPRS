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
 * \brief ILI9225B 4 wire interface graphic driver
 *
 * $WIZ$ module_name = "lcd_ili9225"
 * $WIZ$ module_hw = "bertos/hw/hw_ili9225.h"
 * $WIZ$ module_depends = "timer", "kfile"
 */

#ifndef LCD_ILI9225_H
#define LCD_ILI9225_H

#include <gfx/gfx.h> /* Bitmap */

#include <cpu/types.h>

#define LCD_WIDTH          176
#define LCD_HEIGHT         220

// Fwd decl
struct KFile;

void lcd_ili9225_init(struct KFile *spi);
void lcd_ili9225_on(void);
void lcd_ili9225_off(void);
void lcd_ili9225_blitRaw(const uint8_t *data,
		uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void lcd_ili9225_blitBitmap(const Bitmap *bm);
void lcd_ili9225_blitBitmap24(int x, int y, int width, int height, const char *bmp);

#endif /* LCD_ILI9225_H */
