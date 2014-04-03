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
 * \brief Integer edit (interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/10 15:46:09  batt
 *#* Add EDIS_WRAP style that wrap around min and max.
 *#*
 *#* Revision 1.2  2005/05/26 14:44:10  batt
 *#* Abstract widget from layer: use context.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#ifndef DT_EDITINT_H
#define DT_EDITINT_H

#include <dt/dwidget.h>
#include <dt/dtag.h>
#include <drv/lcd_text.h>

#include <cfg/compiler.h>
#include <cfg/macros.h>


#define EDIS_DEFAULT        0
#define EDIS_CURSORLEFT     BV(0)
#define EDIS_WRAP           BV(1)

/** Type for edit style */
typedef uint16_t dstyle_t;

typedef struct DEditInt
{
	DWidget widget;

	int *value;
	int min;
	int max;
	dstyle_t style;
	void (*draw)(struct DEditInt *);

} DEditInt;

void editint_init(DEditInt *e, dpos_t pos, dpos_t size, dcontext_t *context, int *value, int min, int max);
void editint_update(DEditInt *e, dtag_t tag, dval_t val);
void editint_draw(DEditInt *e);

#endif
