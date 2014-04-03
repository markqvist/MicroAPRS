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
 * \brief Edit bool widget (interface).
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef DT_EDITBOOL_H
#define DT_EDITBOOL_H

#include <dt/dwidget.h>
#include <dt/dtag.h>

typedef struct DEditBool
{
	DWidget widget;
	bool *value;
	const char *true_string;
	const char *false_string;
	void (*draw)(struct DEditBool *);
} DEditBool;

void editbool_init(DEditBool *e, dpos_t pos, dpos_t size, dcontext_t *context, bool *val, const char *true_str, const char *false_str);
void editbool_update(DEditBool *e, dtag_t tag, dval_t val);
void editbool_draw(DEditBool *e);


#endif /* DT_EDITBOOL_H */
