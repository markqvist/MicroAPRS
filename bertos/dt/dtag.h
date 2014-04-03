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
 * \brief Tags interface.
 * This module contains the base message definitions and the list of all available tags.
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
 *#* Revision 1.8  2005/06/07 15:22:29  batt
 *#* Add const_dval_t.
 *#*
 *#* Revision 1.7  2005/06/06 17:42:23  batt
 *#* Add error tag TAG_ERROR.
 *#*
 *#* Revision 1.6  2005/06/06 12:45:33  batt
 *#* Add TAG_NONE tag.
 *#*
 *#* Revision 1.5  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.4  2005/05/31 11:09:52  batt
 *#* Add some tags.
 *#*
 *#* Revision 1.3  2005/05/26 14:55:12  batt
 *#* Add form_processTime; change form_kbdProcess to form_processKey.
 *#*
 *#* Revision 1.2  2005/05/26 14:43:33  batt
 *#* Add new message filter interface.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#ifndef DT_DTAG_H
#define DT_DTAG_H

#include <cfg/macros.h>

/** Type for values associated with tags. */
typedef iptr_t dval_t;

/** Type for constant values associated with tags. */
typedef const_iptr_t const_dval_t;

/** Type for tag */
typedef enum dtag_t
{
	TAG_END = 0,
	TAG_NONE,
	TAG_ANY,
	TAG_SETVALUE,
	TAG_UP,
	TAG_DOWN,
	TAG_START,
	TAG_STOP,
	TAG_TOGGLE,
	TAG_KEY,
	TAG_TIME,
	TAG_ERROR,
} dtag_t;

/** This is the basic message structure used by all dnotifiers. */
typedef struct DTagItem
{
	dtag_t tag;
	dval_t val;
} DTagItem;

/** Marker to indicate the end of a map for message filtering/translating */
#define TAG_END_MARKER {{TAG_END, 0}, {TAG_END, 0}}
#endif
