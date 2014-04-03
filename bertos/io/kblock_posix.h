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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief KBlock interface on POSIX file.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "kfile_posix"
 * $WIZ$ module_depends = "kblock"
 */

#ifndef KBLOCK_POSIX_H
#define KBLOCK_POSIX_H

#include "kblock.h"

#include <stdio.h>

typedef struct KBlockPosix
{
	KBlock b;
	FILE *fp;
} KBlockPosix;

#define KBT_KBLOCKPOSIX MAKE_ID('K', 'B', 'F', 'L')


INLINE KBlockPosix *KBLOCKPOSIX_CAST(KBlock *b)
{
	ASSERT(b->priv.type == KBT_KBLOCKPOSIX);
	return (KBlockPosix *)b;
}

void kblockposix_init(KBlockPosix *f, FILE *fp, bool hwbuf, void *buf, size_t block_size, block_idx_t block_count);

#endif /* KBLOCK_POSIX_H */
