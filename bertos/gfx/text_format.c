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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief printf-family routines for text output
 *
 * $WIZ$ module_name = "text_format"
 * $WIZ$ module_depends = "sprintf", "formatwr", "text"
 * $WIZ$ module_harvard = "both"
 */

#include "text.h"

#include <mware/formatwr.h> /* _formatted_write() */
#include <gfx/font.h>
#include <gfx/gfx.h>

#include <stdio.h> /* vsprintf() */
#include <stdarg.h>
#include <string.h> /* strlen() */

/**
 * Render string \a str in Bitmap \a bm at current cursor position
 *
 * \note Text formatting functions are also available with an _P suffix
 *       accepting the source string from program memory.  This feature
 *       is only available (and useful) on Harvard microprocessors such
 *       as the AVR.
 *
 * \see text_putchar()
 */
int PGM_FUNC(text_puts)(const char * PGM_ATTR str, struct Bitmap *bm)
{
	char c;

	while ((c = PGM_READ_CHAR(str++)))
		text_putchar(c, bm);

	return 0;
}


/**
 * vprintf()-like formatter to render text in a Bitmap.
 *
 * Perform vprintf()-like formatting on the \a fmt format string using the
 * variable-argument list \a ap.
 * Render the resulting string in Bitmap \a bm starting at the current
 * cursor position.
 *
 * \see text_puts() text_putchar() text_printf()
 */
int PGM_FUNC(text_vprintf)(struct Bitmap *bm, const char * PGM_ATTR fmt, va_list ap)
{
	return PGM_FUNC(_formatted_write)(fmt, (void (*)(char, void *))text_putchar, bm, ap);
}

/**
 * printf()-like formatter to render text in a Bitmap.
 *
 * Perform printf()-like formatting on the \a fmt format string.
 * Render the resulting string in Bitmap \a bm starting at the
 * current cursor position.
 *
 * \see text_puts() text_putchar() text_vprintf()
 */
int PGM_FUNC(text_printf)(struct Bitmap *bm, const char * PGM_ATTR fmt, ...)
{
	int len;

	va_list ap;
	va_start(ap, fmt);
	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);
	va_end(ap);

	return len;
}

/**
 * Render text with vprintf()-like formatting at a specified pixel position.
 *
 * \see text_xyprintf()
 */
int PGM_FUNC(text_xyvprintf)(struct Bitmap *bm,
		coord_t x, coord_t y, uint16_t style, const char * PGM_ATTR fmt, va_list ap)
{
	int len;
	uint8_t oldstyle = 0;

	text_setCoord(bm, x, y);

	if (style & STYLEF_MASK)
		oldstyle = text_style(bm, style, STYLEF_MASK);

	if (style & (TEXT_CENTER | TEXT_RIGHT))
	{
		uint8_t pad = bm->width - PGM_FUNC(text_vwidthf)(bm, fmt, ap);

		if (style & TEXT_CENTER)
			pad /= 2;

		if (style & TEXT_FILL)
			gfx_rectFillC(bm, 0, y, pad, y + bm->font->height,
				(style & STYLEF_INVERT) ? 0xFF : 0x00);

		text_setCoord(bm, pad, y);
	}

	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);

	if (style & TEXT_FILL)
		gfx_rectFillC(bm, bm->penX, y, bm->width, y + bm->font->height,
			(style & STYLEF_INVERT) ? 0xFF : 0x00);

	/* Restore old style */
	if (style & STYLEF_MASK)
		text_style(bm, oldstyle, STYLEF_MASK);

	return len;
}


/**
 * Render text with printf()-like formatting at a specified pixel position.
 *
 * \param bm Bitmap where to render the text
 * \param x     [pixels] Initial X coordinate of text.
 * \param y     [pixels] Coordinate of top border of text.
 * \param style Formatting style to use.  In addition to any STYLEF_
 *        flag, it can be TEXT_NORMAL, TEXT_FILL, TEXT_INVERT or
 *        TEXT_RIGHT, or a combination of these flags ORed together.
 * \param fmt  String possibly containing printf() formatting commands.
 *
 * \see text_puts() text_putchar() text_printf() text_vprintf()
 * \see text_moveTo() text_style()
 */
int PGM_FUNC(text_xyprintf)(struct Bitmap *bm,
		coord_t x, coord_t y, uint16_t style, const char * PGM_ATTR fmt, ...)
{
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = PGM_FUNC(text_xyvprintf)(bm, x, y, style, fmt, ap);
	va_end(ap);

	return len;
}


/**
 * Render text with printf()-like formatting at a specified row/column position.
 *
 * \see text_xyprintf()
 */
int PGM_FUNC(text_xprintf)(struct Bitmap *bm,
		uint8_t row, uint8_t col, uint16_t style, const char * PGM_ATTR fmt, ...)
{
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = PGM_FUNC(text_xyvprintf)(
			bm, col * bm->font->width, row * bm->font->height,
			style, fmt, ap);
	va_end(ap);

	return len;
}


struct TextWidthData
{
	Bitmap *bitmap;
	coord_t width;
};

/**
 * Compute width in pixels of a character.
 *
 * Compute the on screen width of a character, taking the
 * current style and font into account.
 *
 * The width is accumulated in the WidthData structure
 * passed as second argument.
 *
 * This is a formatted_write() callback used by text_vwidthf()
 * to compute the length of a formatted string.
 */
static int text_charWidth(int c, struct TextWidthData *twd)
{
	unsigned char index = (unsigned char)c;
	Bitmap *bm = twd->bitmap;
	coord_t glyph_width;


	if (UNLIKELY(!FONT_HAS_GLYPH(bm->font, index)))
	{
		if (!FONT_HAS_GLYPH(bm->font, '?'))
			index = '?';
		else
			index = bm->font->first;
	}

	/* Make character relative to font start */
	index -= bm->font->first;

	if (bm->font->offset)
		/* Proportional font */
		glyph_width = bm->font->widths[index]; /* TODO: optimize away */
	else
		/* Fixed width font */
		glyph_width = bm->font->width;

	if (bm->styles & STYLEF_CONDENSED)
		--glyph_width;

	if (bm->styles & STYLEF_EXPANDED)
		glyph_width *= 2;

	twd->width += glyph_width;

	return c;
}

/**
 * Return the width in pixels of a vprintf()-formatted string.
 */
int PGM_FUNC(text_vwidthf)(
	UNUSED_ARG(struct Bitmap *, bm),
	const char * PGM_ATTR fmt,
	va_list ap)
{
	/* Fixed font with no styles affecting the width? */
	if (!bm->font->offset && !(bm->styles & (STYLEF_CONDENSED | STYLEF_EXPANDED)))
		return PGM_FUNC(vsprintf)(NULL, fmt, ap) * bm->font->width;
	else
	{
		struct TextWidthData twd;
		twd.bitmap = bm;
		twd.width = 0;
		_formatted_write(fmt, (void (*)(char, void *))text_charWidth, &twd, ap);
		return twd.width;
	}
}


/**
 * Return the width in pixels of a printf()-formatted string.
 */
int PGM_FUNC(text_widthf)(struct Bitmap *bm, const char * PGM_ATTR fmt, ...)
{
	int width;

	va_list ap;
	va_start(ap, fmt);
	width = PGM_FUNC(text_vwidthf)(bm, fmt, ap);
	va_end(ap);

	return width;
}
