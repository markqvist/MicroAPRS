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
 * \brief Integer edit widget (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "editint.h"

#include <cfg/macros.h>

#include <dt/dwidget.h>
#include <dt/dtag.h>
#include <dt/dnotifier.h>

#include <drv/lcd_text.h>

/**
 * Init.
 */
void editint_init(DEditInt *e, dpos_t pos, dpos_t size, dcontext_t *context, int *value, int min, int max)
{
	// Initialize superclass
	widget_init(&e->widget, pos, size, context);

	// Override superclass methods
	e->widget.notifier.update = (update_func_ptr)editint_update;

	// Init instance
	e->value = value;
	e->min = min;
	e->max = max;
	e->style = EDIS_DEFAULT;
	e->draw = editint_draw;
}

/**
 * Handle the messages (edit the int).
 */
void editint_update(DEditInt *e, dtag_t tag, dval_t _val)
{
	bool changed = false;
	int val = (int)_val;

	switch (tag)
	{
	case TAG_SETVALUE:
		*e->value = MINMAX(e->min, val, e->max);
		changed = true;
		break;

	/* Increments the integer by val */
	case TAG_UP:
		if (e->style & EDIS_WRAP)
		{
			if (*e->value + val > e->max)
				*e->value = (*e->value + val - e->min) % (e->max - e->min + 1) + e->min;
			else
				*e->value += val;
		}
		else
			*e->value = MIN(*e->value + val, e->max);
		changed = true;
		break;
	/* Decrements the integer by val */
	case TAG_DOWN:
		if (e->style & EDIS_WRAP)
		{
			if (*e->value - val < e->min)
				*e->value = e->max - (e->max - (*e->value - val)) % (e->max - e->min + 1);
			else
				*e->value -= val;
		}
		else
			*e->value = MAX(*e->value - val, e->min);
		changed = true;
		break;

	default:
		break;
	}

	if (changed)
	{
		e->draw(e);
		dnotify_targets(&e->widget.notifier, TAG_SETVALUE, (dval_t)*e->value);
	}
}

/**
 * Draw the integer on the context.
 */
void editint_draw(DEditInt *e)
{
	lcd_printf((Layer *)e->widget.context, (lcdpos_t)e->widget.pos, LCD_NORMAL,"%*d", (int)e->widget.size, *e->value);
}
