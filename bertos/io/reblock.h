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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief KBlock block size reducer
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $WIZ$ module_name = "reblock"
 * $WIZ$ module_depends = "kblock"
 */

#ifndef REBLOCK_H
#define REBLOCK_H

#include "kblock.h"


typedef struct Reblock
{
	KBlock  fd;
	KBlock *native_fd;
} Reblock;

#define KBT_REBLOCK MAKE_ID('R', 'E', 'B', 'L')


INLINE Reblock *REBLOCK_CAST(KBlock *b)
{
	ASSERT(b->priv.type == KBT_REBLOCK);
	return (Reblock *)b;
}

void reblock_init(Reblock *rbl, KBlock *native_fd, size_t native_blk_size);

#endif /* REBLOCK_H */
