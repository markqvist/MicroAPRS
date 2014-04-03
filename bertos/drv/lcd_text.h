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
 * \brief Generic text LCD driver (interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_name = "lcd_text"
 * $WIZ$ module_depends = "lcd_hd44", "timer", "formatwr"
 */

#ifndef DRV_LCD_H
#define DRV_LCD_H

#include "lcd_hd44.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>
#include <struct/list.h>

#include <stdarg.h> // vprintf()


/* flags for lcd_printf() */
#define LCD_NORMAL     0       /* Scrittura normale */
#define LCD_FILL       BV(0)   /* Fill rest of line with spaces */
#define LCD_CENTER     BV(1)   /* Center string in line */
#define LCD_NOCURSOR   BV(2)   /* Scrittura senza spostamento cursore */

/** Special priority value for lcd_setLayerDepth(). */
#define LAYER_HIDDEN   -127

/* Compute LCD address from x/y coordinates */
#define LCD_POS(x,y)  ((lcdpos_t)((uint8_t)(x) + (uint8_t)(y) * (uint8_t)CONFIG_LCD_COLS))
#define LCD_ROW0  (CONFIG_LCD_COLS * 0)
#define LCD_ROW1  (CONFIG_LCD_COLS * 1)
#define LCD_ROW2  (CONFIG_LCD_COLS * 2)
#define LCD_ROW3  (CONFIG_LCD_COLS * 3)

/**
 * Overwrapping layer context.
 */
typedef struct _Layer
{
	/**
	 * Active layers are linked together in a list
	 * sorted in top to bottom order.
	 */
	DECLARE_NODE_ANON(struct _Layer)

	/** Current XY address into this layer, for write operations. */
	lcdpos_t addr;

	/** Priority of this layer (greater in front of lesser). */
	signed char pri;

	/**
	 * Layer backing store buffer.
	 *
	 * All writes through the layer are copied into this buffer.
	 * Characters cells with value 0 are transparent with respect
	 * to other layers in the background.
	 */
	char buf[CONFIG_LCD_COLS * CONFIG_LCD_ROWS];
} Layer;


/* Global variables */
extern Layer *lcd_DefLayer;

/* Function prototypes */
extern void lcd_init(void);
extern void lcd_test(void);

extern void lcd_moveCursor(lcdpos_t addr);
extern char lcd_setCursor(char state);

extern void lcd_setAddr(Layer *layer, lcdpos_t addr);
extern void lcd_putChar(char c, Layer *layer);
extern int  lcd_vprintf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, va_list ap) FORMAT(printf, 4, 0);
extern int  lcd_printf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, ...) FORMAT(printf, 4, 5);
extern void lcd_clear(Layer *layer);
extern void lcd_layerSet(Layer *layer, char c);
extern void lcd_clearLine(Layer *layer, int y);

extern void lcd_setLayerDepth(Layer *layer, char pri);
extern Layer *lcd_newLayer(char pri);
extern void lcd_deleteLayer(Layer *layer);
extern void lcd_lock(void);
extern void lcd_unlock(void);

#endif /* DRV_LCD_H */
