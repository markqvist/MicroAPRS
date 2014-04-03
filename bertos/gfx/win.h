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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Very simple hierarchical windowing system.
 *
 * All functions in this module are to be intended as methods
 * of the Window class.  Please see its documentation
 * for a module-wise introduction.
 *
 * \see struct Window
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.2  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#ifndef GFX_WIN_H
#define GFX_WIN_H

#include <struct/list.h> /* Node, List */
#include <gfx/gfx.h>    /* coord_t */


EXTERN_C_BEGIN

/**
 * Window handle and context structure.
 *
 * A window is a small rectangular area on the
 * screen backed by its own bitmap where you
 * can draw.
 *
 * A window can contain any number of children
 * sub-windows that can be depth arranged with
 * respect to their siblings.
 *
 * At any time, a window and all its children
 * can be drawn into another bitmap to display
 * a complete screen, taking depth and
 * overlapping into account.
 *
 * This rendering model is commonly referred to as
 * screen composition, and is quite popular among
 * modern windowing systems.
 */
typedef struct Window
{
	Node    link;      /**< Link us with other siblings into our parent.  */
	struct Window *parent;  /**< Our parent window.  NULL for the root window. */

	Bitmap *bitmap;    /**< Pixel storage for window contents. */
	Rect    geom;      /**< [px] Window size and position relative to parent. */

	/**
	 * List of child windows, arranged by depth (front to back).
	 *
	 * Child top/left coordinates are relative to us.
	 */
	List    children;

} Window;

/*
 * Public function prototypes
 */
void win_compose(Window *w);
void win_open(Window *w, Window *parent);
void win_close(Window *w);
void win_raise(Window *w);
void win_setGeometry(Window *w, const Rect *new_geom);
void win_move(Window *w, coord_t left, coord_t top);
void win_resize(Window *w, coord_t width, coord_t height);
void win_create(Window *w, Bitmap *bm);

EXTERN_C_END

#endif /* GFX_WIN_H */

