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
 * Copyright 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Generic editor for (volume/gain/contrast/...) setting.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */
#ifndef GUI_LEVELEDIT_H
#define GUI_LEVELEDIT_H

//#include <gui/levelbar.h>

/* Type for level_init */
#define LEVELEDIT_NOBAR  0  /**< Edit number only, without bar nor units */
#define LEVELEDIT_SINGLE 1  /**< Single channel editing */
#define LEVELEDIT_DOUBLE 2  /**< Double channel editing */



/* Fwd decl */
struct Bitmap;
struct LevelEdit;

/** Type for callback used to set meter levels */
typedef void level_set_callback(void);

/** Type for callback used to customize display of units */
typedef void display_callback(struct LevelEdit *);

/**
 * State of a level meter
 */
typedef struct LevelEdit {
	int type;           /*<! Type of level edititing mode (see prev defines) */
	const char *title;  /*<! Title on top of screen */
	const char *unit;   /*<! Unit of quantity changed by this LevelEdit */
	int min;            /*<! Minimum level */
	int max;            /*<! Maximum level */
	int step;           /*<! Value of a single increment/decrement */

	level_set_callback *set_hook;     /*<! Callback called when a value is changed  */
	display_callback   *display_hook; /*<! Callback for complex unit display */
	int *ch1_val;                     /*<! (left) Value edited by this leveledit */
	int *ch2_val;                     /*<! Right channel edited */

	struct Bitmap   *bitmap;  /*<! Bitmap where the whole thing is rendered */
} LevelEdit;


void level_init(struct LevelEdit *lev,
		int type,
		struct Bitmap *bmp, const char *title, const char *unit,
		int min, int max, int step,
		int *ch1_val, int *ch2_val,
		level_set_callback *change_hook, display_callback *display_hook);
void level_edit(struct LevelEdit *lev);

#endif /* GUI_LEVELEDIT_H */
