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
 * Copyright 2004, 2005, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Graphic menu bar widget.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */

#ifndef GUI_MENUBAR_H
#define GUI_MENUBAR_H

#include <cfg/compiler.h>

/** Predefined labels ids */
enum LabelId
{
	LABEL_EMPTY,  /* empty label */
	LABEL_MUTE,
	LABEL_MENU,
	LABEL_BACK,
	LABEL_OK,
	LABEL_CH_1,
	LABEL_CH_2,
	LABEL_C1PLUS2,
	LABEL_UPARROW,
	LABEL_DOWNARROW,
	LABEL_MINUS,
	LABEL_PLUS,
	LABEL_SEL,
	LABEL_LOCK,
	LABEL_UNLOCK,
	LABEL_MORE,
	LABEL_EDIT,
	LABEL_FAST,
	LABEL_PREV,
	LABEL_NEXT,
	LABEL_SLOW,
	LABEL_YES,
	LABEL_NO,


	LABEL_CNT
};

#define UP_ARROW "\x18"
#define DOWN_ARROW  "\x19"
#define RIGHT_ARROW "\xC4\x1A"
#define LEFT_ARROW  "\x10\xC4"

/* Forward decl */
struct Bitmap;

typedef struct MenuBar
{
	struct Bitmap *bitmap;
	const_iptr_t  *labels;
	int            num_labels;
} MenuBar;

void mbar_init(
		struct MenuBar *mb,
		struct Bitmap *bmp,
		const_iptr_t *labels,
		int num_labels);
void mbar_draw(const struct MenuBar *mb);

#endif /* GUI_MENUBAR_H */
