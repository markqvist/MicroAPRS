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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \defgroup text Text handling on graphical displays.
 * \ingroup graphics
 * \{
 * \brief Text graphic routines (interface)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_name = "text"
 * $WIZ$ module_depends = "bitmap", "line"
 */

#ifndef GFX_TEXT_H
#define GFX_TEXT_H

#include <cfg/compiler.h>
#include <cfg/macros.h> /* BV() */
#include <cpu/attr.h> /* CPU_HARVARD */
#include <gfx/gfx.h> /* coord_t */

#include <stdarg.h>

/**
 * \name Style flags
 * \see text_style()
 * \{
 */
#define STYLEF_BOLD        BV(0)
#define STYLEF_ITALIC      BV(1)
#define STYLEF_UNDERLINE   BV(2)
#define STYLEF_INVERT      BV(3)
#define STYLEF_EXPANDED    BV(4)
#define STYLEF_CONDENSED   BV(5)
#define STYLEF_STRIKEOUT   BV(6)  /*<! Not implemented */
#define STYLEF_TALL        BV(7)  /*<! Not implemented */

#define STYLEF_MASK \
	(STYLEF_BOLD | STYLEF_ITALIC | STYLEF_UNDERLINE \
	| STYLEF_INVERT | STYLEF_EXPANDED | STYLEF_CONDENSED \
	| STYLEF_STRIKEOUT | STYLEF_TALL)
/*\}*/

/**
 * \name Formatting flags for text rendering
 * \see text_xprintf()
 * \{
 */
#define TEXT_NORMAL   0       /**< Normal mode */
#define TEXT_FILL     BV(13)  /**< Fill rest of line with spaces */
#define TEXT_CENTER   BV(14)  /**< Center string in line */
#define TEXT_RIGHT    BV(15)  /**< Right aligned */
/*\}*/

/** Escape sequences codes */
#define ANSI_ESC_CLEARSCREEN 'c'


/* Fwd decl */
struct Bitmap;

/* Low-level text functions (mware/text.c) */
void text_moveTo(struct Bitmap *bm, int row, int col);
void text_setCoord(struct Bitmap *bm, int x, int y);
int text_putchar(char c, struct Bitmap *bm);
uint8_t text_style(struct Bitmap *bm, uint8_t flags, uint8_t mask);
void text_clear(struct Bitmap *bm);
void text_clearLine(struct Bitmap *bm, int line);

/* Text formatting functions (mware/text_format.c) */
int text_puts(const char *str, struct Bitmap *bm);
int text_vprintf(struct Bitmap *bm, const char *fmt, va_list ap);
int text_printf(struct Bitmap *bm, const char *fmt, ...) FORMAT(__printf__, 2, 3);
int text_xyvprintf(struct Bitmap *bm, coord_t x, coord_t y, uint16_t mode, const char *fmt, va_list ap);
int text_xyprintf(struct Bitmap *bm, coord_t x, coord_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_xprintf(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf(struct Bitmap *bm, const char * fmt, va_list ap);
int text_widthf(struct Bitmap *bm, const char * fmt, ...) FORMAT(__printf__, 2, 3);

/* Text formatting functions for program-memory strings (mware/text_format.c) */
#if CPU_HARVARD
#include <cpu/pgm.h>
int text_puts_P(const char * PROGMEM str, struct Bitmap *bm);
int text_vprintf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_printf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...) FORMAT(__printf__, 2, 3);
int text_xyvprintf_P(struct Bitmap *bm, coord_t x, coord_t y, uint16_t mode, const char *fmt, va_list ap);
int text_xyprintf_P(struct Bitmap *bm, coord_t x, coord_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_xprintf_P(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char * PROGMEM fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_widthf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...);
#endif /* CPU_HARVARD */

/** \} */ //defgroup text
#endif /* GFX_TEXT_H */
