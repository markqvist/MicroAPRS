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
 *#* Revision 1.2  2006/08/01 17:26:04  batt
 *#* Update docs.
 *#*
 *#* Revision 1.1  2006/08/01 15:43:01  batt
 *#* Add in board_kd current edited channel visualization.
 *#*
 *#* Revision 1.4  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2006/02/10 12:25:41  bernie
 *#* Add missing header.
 *#*
 *#* Revision 1.2  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#include "win.h"
#include <struct/list.h>

/**
 * Map the contents of all child-windows into the bitmap of \a w.
 *
 * \note Recursively drawing children into their parent
 *       effectively damages the parent buffer.
 */
void win_compose(Window *w)
{
	Window *child;

	/*
	 * Walk over all children, in back to front order and tell them
	 * to compose into us.
	 */
	REVERSE_FOREACH_NODE(child, &w->children)
	{
		/* Recursively compose child first. */
		win_compose(child);

		/* Draw child into our bitmap. */
		if (w->bitmap)
			gfx_blit(w->bitmap, &child->geom, child->bitmap, 0, 0);
	}
}

/**
 * Map window \a w into \a parent.
 *
 * The new window becomes the topmost window.
 *
 * \note Opening a window twice is illegal.
 *
 * \see win_close()
 */
void win_open(Window *w, Window *parent)
{
	ASSERT(!w->parent);
	w->parent = parent;
	ADDHEAD(&parent->children, &w->link);
}

/**
 * Detach window from its parent.
 *
 * Closing a window causes it to become orphan of its
 * parent.  Its content will no longer appear in its
 * parent after the next refresh cycle.
 *
 * \note Closing a window that has not been previously
 *       opened is illegal.
 *
 * \see win_open()
 */
void win_close(Window *w)
{
	ASSERT(w->parent);
	REMOVE(&w->link);
	w->parent = NULL;
}

/**
 * Move window to the topmost position relative to its sibling.
 *
 * \see win_move(), win_resize(), win_setGeometry()
 */
void win_raise(Window *w)
{
	ASSERT(w->parent);
	REMOVE(&w->link);
	ADDHEAD(&w->parent->children, &w->link);
}

/**
 * Set window position and size at the same time.
 *
 * This function is equivalent to subsequent calls to win_move()
 * and win_resize() using the coordinates provided by the
 * \a new_geom rectangle.
 *
 * \note The xmax and ymax members of \a new_geom are non-inclusive,
 *       as usual for the Rect interface.
 *
 * \see win_move()
 * \see win_resize()
 */
void win_setGeometry(Window *w, const Rect *new_geom)
{
	// requires C99?
	// memcpy(&w->geom, new_geom, sizeof(w->geom));
	w->geom = *new_geom;
}

/**
 * Move window to specified position.
 *
 * Move the window top-left corner to the pixel coordinates
 * \a left and \a top, which are relative to the parent window.
 *
 * \note A window can also be moved outside the borders
 *       of its parent, or at negative coordinates.
 *
 * \note It is allowed to move an orphan window.
 */
void win_move(Window *w, coord_t left, coord_t top)
{
	Rect r;

	r.xmin = left;
	r.ymin = top;
	r.xmax = r.xmin + RECT_WIDTH(&w->geom);
	r.ymax = r.ymin + RECT_WIDTH(&w->geom);

	win_setGeometry(w, &r);
}

/**
 * Resize the rectangle of a window.
 *
 * The window shrinks or grows to the specified size.
 *
 * \note Growing a window beyond the size of its
 *       backing bitmap results in unspecified behavior.
 *
 * \note It is allowed to resize an orphan window.
 */
void win_resize(Window *w, coord_t width, coord_t height)
{
	Rect r;

	r.xmin = w->geom.xmin;
	r.ymin = w->geom.ymin;
	r.xmax = r.xmin + width;
	r.ymax = r.ymin + height;

	win_setGeometry(w, &r);
}

/**
 * Initialize a new window structure.
 *
 * The new window initial position is set to (0,0).
 * The size is set to the size of the installed bitmap,
 * or (0,0) if there's no backing store.
 *
 * \arg bm  The bitmap to install as backing store
 *          for drawing into the window, or NULL if
 *          the window is not drawable.
 */
void win_create(Window *w, Bitmap *bm)
{
	w->parent = NULL;
	w->bitmap = bm;
	w->geom.xmin = 0;
	w->geom.ymin = 0;
	if (bm)
	{
		w->geom.xmax = bm->width;
		w->geom.ymax = bm->height;
	}
	LIST_INIT(&w->children);
}

