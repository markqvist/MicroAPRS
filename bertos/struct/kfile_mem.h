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
 * \brief KFile interface over a memory buffer.
 *
 * Convenient way to access a memory region using the KFile interface.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "kfilemem"
 * $WIZ$ module_depends = "kfile"
 */

#ifndef STRUCT_KFILE_MEM
#define STRUCT_KFILE_MEM

#include <io/kfile.h>

/**
 * Context for KFile over memory buffer.
 */
typedef struct KFileMem
{
	KFile fd;  ///< KFile base class
	void *mem; ///< Pointer to the memory buffer used.
} KFileMem;

/**
 * ID for KFile Mem.
 */
#define KFT_KFILEMEM MAKE_ID('M', 'E', 'M', '0')

/**
 * Convert + ASSERT from generic KFile to KFileMem.
 */
INLINE KFileMem * KFILEMEM_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_KFILEMEM);
	return (KFileMem *)fd;
}

/**
 * Initialize KFileMem struct.
 *
 * \param km Interface to initialize.
 * \param mem Pointer to the memory buffer to operate on.
 * \param len Size of the buffer
 */
void kfilemem_init(KFileMem *km, void *mem, size_t len);

#endif /* STRUCT_KFILE_MEM */
