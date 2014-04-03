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
 *
 * -->
 *
 * \brief Himax HX8347 LCD controller driver
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_name = "lcd_hx8347"
 * $WIZ$ module_hw = "bertos/hw/hw_hx8347.h"
 * $WIZ$ module_depends = "timer"
 */

#ifndef LCD_HX8347_H
#define LCD_HX8347_H

#include <gfx/gfx.h> /* Bitmap */

#include <cpu/types.h>

#define LCD_WIDTH          320
#define LCD_HEIGHT         240

void lcd_hx8347_init(void);
void lcd_hx8347_on(void);
void lcd_hx8347_off(void);
void lcd_hx8347_blitBitmap(const Bitmap *bm);
void lcd_hx8347_blitBitmap24(int x, int y, int width, int height, const char *bmp);

#endif /* LCD_HX8347_H */
