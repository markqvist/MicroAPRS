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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Graphics Menu bar widget
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "menubar.h"

#include <gfx/gfx.h>
#include <gfx/text.h>
#include <gfx/font.h>
#include <cfg/compiler.h>

#warning FIXME:This module is obsolete, you must refactor it!

#if 0
#if CPU_AVR
	#include <avr/pgmspace.h> /* strlen_P() */
#else
	#define strlen_P(s)             strlen(s)
	#define text_puts_P(s, b)       text_puts(s, b)
	#define pgm_read_uint16_t(addr) (*(addr))
#endif

#include <string.h> /* strlen, memcpy */


/** Predefined labels */
static const pgm_char lab_1[]  = "";
static const pgm_char lab_2[]  = "mute";
static const pgm_char lab_3[]  = "menu";
static const pgm_char lab_4[]  = "back";
static const pgm_char lab_5[]  = " ok ";
static const pgm_char lab_6[]  = "Ch 1";
static const pgm_char lab_7[]  = "Ch 2";
static const pgm_char lab_8[]  = "C1+2";
static const pgm_char lab_9[]  = " "UP_ARROW" ";
static const pgm_char lab_10[] = " "DOWN_ARROW" ";
static const pgm_char lab_11[] = " - ";
static const pgm_char lab_12[] = " + ";
static const pgm_char lab_13[] = "sel ";
static const pgm_char lab_14[] = "lock";
static const pgm_char lab_15[] = "unlock";
static const pgm_char lab_16[] = "more";
static const pgm_char lab_17[] = "edit";
static const pgm_char lab_18[] = "fast";
static const pgm_char lab_19[] = LEFT_ARROW" ";
static const pgm_char lab_20[] = " "RIGHT_ARROW;
static const pgm_char lab_21[] = "slow";
static const pgm_char lab_22[] = "yes";
static const pgm_char lab_23[] = "no";


static const pgm_char * PROGMEM label_strings[LABEL_CNT] = {
	lab_1, lab_2, lab_3, lab_4, lab_5, lab_6, lab_7, lab_8, lab_9,
	lab_10, lab_11, lab_12, lab_13, lab_14, lab_15, lab_16, lab_17,
	lab_18, lab_19, lab_20, lab_21, lab_22, lab_23
};

/**
 * Macro to access a label iptr_t: if a char pointer get the string pointed to
 * in program memory, otherwise return the corrispondent predefined string
 * (see label_strings in menubar.c)
 */
#define PTRLBL(x) ((unsigned int)(x) < 256 ? \
	(const pgm_char *)pgm_read_uint16_t(label_strings + (unsigned int)(x)) \
	: (const pgm_char *)(x))


/**
 * Initialize the MenuBar widget with the bitmap associated,
 * the label names and the number of labels.
 */
void mbar_init(
		struct MenuBar *mb,
		struct Bitmap *bmp,
		const_iptr_t labels[],
		int num_labels)
{
	mb->bitmap     = bmp;
	mb->labels     = labels;
	mb->num_labels = num_labels;
}


/**
 * Render the MenuBar on the bitmap.
 */
void mbar_draw(const struct MenuBar *mb)
{
	uint8_t oldstyle;
	int i;
	size_t maxlen = 0;  /* Length of the longest label */
	coord_t x1, x2, y1, y2, label_padding;

	/* Maximum space available for a label */
	coord_t slot_width = mb->bitmap->width / mb->num_labels;

	/* Find longest label */
	for (i = 0; i < mb->num_labels; i++)
		if (strlen_P(PTRLBL(mb->labels[i])) > maxlen)
			maxlen = strlen_P(PTRLBL(mb->labels[i]));

	oldstyle = text_style(mb->bitmap, STYLEF_INVERT, STYLEF_MASK);

	/* y coords for menubar: bottom of the bitmap */
	y1 = mb->bitmap->height - FONT_HEIGHT;
	y2 = mb->bitmap->height;

	/* Clear menubar area */
	gfx_rectClear(mb->bitmap, 0, y1, mb->bitmap->width, y2);

	for (i = 0; i < mb->num_labels; i++)
	{
		size_t lablen = strlen_P(PTRLBL(mb->labels[i]));

		/* Don't draw empty labels */
		if (mb->labels[i] == (const_iptr_t)LABEL_EMPTY)
			continue;

		/* x coords: magic formula for equal distribution of the
		 * labels along bitmap
		 */
		label_padding = slot_width - (FONT_WIDTH * lablen + 2);
		x1 = i * (slot_width + (label_padding / (mb->num_labels - 1)));
		x2 = x1 + lablen * FONT_WIDTH + 1;

		/* Draw vertical line before.
		 * Uncomment +1 for "rounded" menubars */
		gfx_line(mb->bitmap, x1, y1 /* + 1 */, x1, y2);

		/* Draw text */
		text_setCoord(mb->bitmap, x1 + 1, y1);
		text_puts_P(PTRLBL(mb->labels[i]), mb->bitmap);

		/* Draw vertical line after
		 * Uncomment +1 for "rounded" menubars */
		gfx_line(mb->bitmap, x2, y1 /* + 1 */, x2, y2);
	}

	text_style(mb->bitmap, oldstyle, STYLEF_MASK);
}
#endif

