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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Hitachi HD44780 and clones LCD module.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_name = "lcd_hd44"
 * $WIZ$ module_depends = "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_lcd_hd44.h"
 * $WIZ$ module_hw = "bertos/hw/hw_lcd_hd44.h"
 */

#ifndef DRV_LCD_HD44_H
#define DRV_LCD_HD44_H

#include "cfg/cfg_lcd_hd44.h"
#include <cfg/compiler.h> /* For stdint types */

/**
 * \name Values for CONFIG_LCD_ROWS.
 *
 * Select the number of rows which are available
 * on the HD44780 Display.
 * $WIZ$ lcd_hd44_rows = "LCD_HD44_ROWS_2", "LCD_HD44_ROWS_4"
 */
#define LCD_HD44_ROWS_2 2
#define LCD_HD44_ROWS_4 4

/**
 * \name Values for CONFIG_LCD_COLS.
 *
 * Select the number of columns which are available
 * on the HD44780 Display.
 * $WIZ$ lcd_hd44_cols = "LCD_HD44_COLS_16", "LCD_HD44_COLS_20"
 */
#define LCD_HD44_COLS_16 16
#define LCD_HD44_COLS_20 20

/**
 * \name Hitachi HD44 commands.
 * \{
 */
#define LCD_CMD_DISPLAY_INI      0x30

#if CONFIG_LCD_4BIT
	#define LCD_CMD_SETFUNC  0x28   /**< 4 bits, 2 lines, 5x7 dots */
#else
	#define LCD_CMD_SETFUNC  0x38   /**< 8 bits, 2 lines, 5x7 dots */
#endif

#define LCD_CMD_SET8BIT          0x30
#define LCD_CMD_DISPLAY_ON       0x0F   /**< Switch on display */
#define LCD_CMD_DISPLAY_OFF      0x08   /**< Switch off display */
#define LCD_CMD_CLEAR            0x01   /**< Clear display */
#define LCD_CMD_CURSOR_BLOCK     0x0D   /**< Show cursor (block) */
#define LCD_CMD_CURSOR_LINE      0x0F   /**< Show cursor (line) */
#define LCD_CMD_CURSOR_OFF       0x0C   /**< Hide cursor */
#define LCD_CMD_DISPLAYMODE      0x06
#define LCD_CMD_SET_CGRAMADDR    0x40
#define LCD_CMD_RESET_DDRAM      0x80
#define LCD_CMD_SET_DDRAMADDR    0x80
#define LCD_CMD_DISPLAY_SHIFT    0x18
#define LCD_CMD_MOVESHIFT_LEFT   0x00
#define LCD_CMD_MOVESHIFT_RIGHT  0x04
/*\}*/

/** Type for combined LCD cursor position (x,y). */
typedef uint8_t lcdpos_t;

void lcd_waitBusy(void);
void lcd_moveTo(uint8_t addr);
void lcd_setReg(uint8_t val);
void lcd_putc(uint8_t a, uint8_t c);
void lcd_remapChar(const char *glyph, char code);
void lcd_hw_init(void);
void lcd_hw_test(void);

#endif /* DRV_LCD_HD44_H */
