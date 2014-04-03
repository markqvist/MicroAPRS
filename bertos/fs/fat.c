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
 * \brief FatFS: kfile interface for FatFS module by ChaN.
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 */

#include "fat.h"

static size_t fatfile_read(struct KFile *_fd, void *buf, size_t size)
{
	FatFile *fd = FATFILE_CAST(_fd);
	UINT count;
	fd->error_code = f_read(&fd->fat_file, buf, size, &count);
	return count;
}

static size_t fatfile_write(struct KFile *_fd, const void *buf, size_t size)
{
	FatFile *fd = FATFILE_CAST(_fd);
	UINT count;
	fd->error_code = f_write(&fd->fat_file, buf, size, &count);
	return count;
}

static int fatfile_close(struct KFile *_fd)
{
	FatFile *fd = FATFILE_CAST(_fd);
	fd->error_code = f_close(&fd->fat_file);
	if (fd->error_code)
		return EOF;
	else
		return 0;
}

static kfile_off_t fatfile_seek(struct KFile *_fd, kfile_off_t offset, KSeekMode whence)
{
	/* clip at start-of-file
	 * don't clip at end-of-file when in write mode
	 */
	FatFile *fd = FATFILE_CAST(_fd);
	DWORD lseek_offset = 0;
	switch (whence)
	{
	case KSM_SEEK_SET:
		if (offset > 0)
			lseek_offset = (DWORD) offset;
		break;
	case KSM_SEEK_CUR:
		if (offset > 0)
			lseek_offset = fd->fat_file.fptr + (DWORD) offset;
		else
		{
			if (fd->fat_file.fptr > (DWORD) (-offset))
				lseek_offset = fd->fat_file.fptr - (DWORD)(-offset);
		}
		break;
	case KSM_SEEK_END:
		if (offset > 0)
			lseek_offset = fd->fat_file.fsize + (DWORD) offset;
		else
		{
			if (fd->fat_file.fsize > (DWORD) (-offset))
				lseek_offset = fd->fat_file.fsize + (DWORD) offset;
		}
		break;
	}
	fd->error_code = f_lseek(&fd->fat_file, lseek_offset);
	if ((fd->error_code) || (fd->fat_file.fptr != lseek_offset))
		return EOF;
	else
		/* TODO: this conversion may overflow */
		return (kfile_off_t)fd->fat_file.fptr;
}

static int fatfile_flush(struct KFile *_fd)
{
	FatFile *fd = FATFILE_CAST(_fd);
	fd->error_code = f_sync(&fd->fat_file);
	if (fd->error_code)
		return EOF;
	else
		return 0;
}

static int fatfile_error(struct KFile *_fd)
{
	FatFile *fd = FATFILE_CAST(_fd);
	return (int)fd->error_code;
}

static void fatfile_clearerr(struct KFile *_fd)
{
	FatFile *fd = FATFILE_CAST(_fd);
	fd->error_code = FR_OK;
}

FRESULT fatfile_open(FatFile *file, const char *file_path, BYTE mode)
{
	DB(file->fd._type = KFT_FATFILE);
	file->fd.read = fatfile_read;
	file->fd.write = fatfile_write;
	file->fd.reopen = 0;
	file->fd.close = fatfile_close;
	file->fd.seek = fatfile_seek;
	file->fd.flush = fatfile_flush;
	file->fd.error = fatfile_error;
	file->fd.clearerr = fatfile_clearerr;
	return f_open(&file->fat_file, file_path, mode);
}

