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
 * Copyright 2003, 2004, 2005, 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti
 * All Rights Reserved.
 * -->
 *
 * \brief Keyboard hardware-specific definitions
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Stefano Fedrigo <a@develer.com>
 */

#ifndef HW_KBD_H
#define HW_KBD_H

#include "hw/kbd_map.h"

#include <cfg/macros.h>

#warning TODO:This is an example implementation, you must implement it!

#define K_RPT_MASK (K_UP | K_DOWN | K_OK | K_CANCEL)

#define KBD_HW_INIT \
	do { \
			/* Put here code to init hw */ \
	} while (0)

EXTERN_C int emul_kbdReadCols(void);

/**
 * Read the keyboard ports and return the mask of
 * depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	/* Implement me! */

	//Only for test remove when implement this function
	return 0;
}

#endif /* HW_KBD_H */
