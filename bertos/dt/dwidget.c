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
 * -->
 *
 *
 * \brief Widget (implementation).
 * A widget is typically a graphic object on a device.
 * Its proproperties are the position, the size and a context on which the widget is drawn.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@ðeveler.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.2  2005/05/26 14:44:10  batt
 *#* Abstract widget from layer: use context.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#include <dt/dwidget.h>
#include <dt/dnotifier.h>

/**
 * Init the widget of \a pos and \a size on the drawing \a context.
 */
void widget_init(DWidget *w, dpos_t pos, dpos_t size, dcontext_t *context)
{
	// Init superclass
	notifier_init(&w->notifier);

	// Init instance
	w->pos = pos;
	w->size = size;
	w->context = context;
}
