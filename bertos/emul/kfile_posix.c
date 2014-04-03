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

#include <emul/kfile_posix.h>
#include <string.h>

static size_t kfile_posix_read(struct KFile *_fd, void *buf, size_t size)
{
	KFilePosix *fd = KFILEPOSIX_CAST(_fd);
	size_t len = fread(buf, sizeof(uint8_t), size, fd->fp);
	fd->fd.seek_pos += len;
	return len;
}

static size_t kfile_posix_write(struct KFile *_fd, const void *buf, size_t size)
{
	KFilePosix *fd = KFILEPOSIX_CAST(_fd);
	size_t len = fwrite(buf, sizeof(uint8_t), size, fd->fp);
	fd->fd.seek_pos += len;
	fd->fd.size = MAX(fd->fd.size, fd->fd.seek_pos);
	return len;
}

static kfile_off_t kfile_posix_seek(struct KFile *_fd, kfile_off_t offset, KSeekMode whence)
{
	KFilePosix *fd = KFILEPOSIX_CAST(_fd);
	int std_whence;
	switch (whence)
	{
		case KSM_SEEK_CUR:
			std_whence = SEEK_CUR;
			break;
		case KSM_SEEK_END:
			std_whence = SEEK_END;
			break;
		case KSM_SEEK_SET:
			std_whence = SEEK_SET;
			break;
		default:
			ASSERT(0);
			return EOF;
	}
	int err = fseek(fd->fp, offset, std_whence);
	if (err)
		return err;

	fd->fd.seek_pos = ftell(fd->fp);
	return fd->fd.seek_pos;
}

static int kfile_posix_close(struct KFile *_fd)
{
	KFilePosix *fd = KFILEPOSIX_CAST(_fd);
	return fclose(fd->fp);
}

static int kfile_posix_flush(struct KFile *_fd)
{
	KFilePosix *fd = KFILEPOSIX_CAST(_fd);
	return fflush(fd->fp);
}

FILE *kfile_posix_init(KFilePosix *file, const char *filename, const char *mode)
{
	memset(file, 0, sizeof(*file));
	DB(file->fd._type = KFT_KFILEPOSIX);
	file->fd.read = kfile_posix_read;
	file->fd.write = kfile_posix_write;
	file->fd.close = kfile_posix_close;
	file->fd.seek = kfile_posix_seek;
	file->fd.flush = kfile_posix_flush;

	file->fp = fopen(filename, mode);
	fseek(file->fp, 0, SEEK_END);
	file->fd.size = ftell(file->fp);
	fseek(file->fp, 0, SEEK_SET);
	return file->fp;
}
