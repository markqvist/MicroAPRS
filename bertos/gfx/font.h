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
 *
 * -->
 *
 * \brief Font 8x6 IBM-PC 8bit
 *
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */

#ifndef GFX_FONT_H
#define GFX_FONT_H

#include <cfg/compiler.h>  /* uint8_t */
#include <cpu/pgm.h>     /* PROGMEM */

typedef struct Font
{
	/**
	 * Pointer to glyph data.
	 *
	 * Data is an array of at most 256 glyphs packed together.
	 * Raster format must be the same of the bitmap.
	 */
	const pgm_uint8_t *glyph;

	uint8_t width;     /**< Pixel width of character cell. */
	uint8_t height;    /**< Pixel height of character cell. */

	uint8_t first;     /**< First encoded character in glyph array. */
	uint8_t last;      /**< Last encoded character in glyph array (inclusive). */

	/** Array of glyph offsets in bytes. NULL for fixed-width fonts. */
	const pgm_uint16_t *offset;
	const pgm_uint8_t  *widths;

} Font;


/** Return true if glyph \a c is available in \a font. */
#define FONT_HAS_GLYPH(font, c) ((c) >= (font)->first && (c) <= (font)->last)


/** The default font. */
#define default_font font_luBS14
extern const struct Font default_font;

#endif /* GFX_FONT_H */
