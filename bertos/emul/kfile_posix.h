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
 * \brief KFile interface implementation in Posix systems.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#ifndef KFILE_POSIX_H
#define KFILE_POSIX_H

#include <io/kfile.h>
#include <stdio.h>

typedef struct KFilePosix
{
  KFile fd;
  FILE *fp;
} KFilePosix;

#define KFT_KFILEPOSIX MAKE_ID('K', 'F', 'T', 'P')

INLINE KFilePosix *KFILEPOSIX_CAST(KFile *fd)
{
  ASSERT(fd->_type == KFT_KFILEPOSIX);
  return (KFilePosix *)fd;
}

FILE *kfile_posix_init(KFilePosix *file, const char *filename, const char *mode);

#endif /* KFILE_POSIX_H */
