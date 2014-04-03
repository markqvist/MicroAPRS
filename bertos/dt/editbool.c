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
 * \brief Edit bool widget (implementation).
 * This widget handles boolean editing.
 * The boolean value will be displayed using two strings:
 * one when the bool is false and one when it's true.
 *
 * \author Francesco Sacchi <batt@ðeveler.com>
 */

#include "editbool.h"
#include <dt/dtag.h>

#include <drv/lcd_text.h>

/**
 * Init widget.
 */
void editbool_init(DEditBool *e, dpos_t pos, dpos_t size, dcontext_t *context, bool *value, const char *true_string, const char *false_string)
{
	// Initialize superclass
	widget_init(&e->widget, pos, size, context);

	// Override superclass methods
	e->widget.notifier.update = (update_func_ptr)editbool_update;

	// Init instance
	e->value = value;
	e->true_string = true_string;
	e->false_string = false_string;
	e->draw = editbool_draw;
}

/**
 * Handle the messages (edit the bool).
 */
void editbool_update(DEditBool *e, dtag_t tag, dval_t _val)
{
	bool changed = false;

	switch (tag)
	{
	case TAG_SETVALUE:
		*e->value = (bool)_val;
		changed = true;
		break;

	case TAG_TOGGLE:
		*e->value = !*e->value;
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
 * Draw the string on the context.
 */
void editbool_draw(DEditBool *e)
{
	lcd_printf((Layer *)e->widget.context, (lcdpos_t)e->widget.pos, LCD_NORMAL, "%*s", (int)e->widget.size , *e->value? e->true_string: e->false_string);
}
