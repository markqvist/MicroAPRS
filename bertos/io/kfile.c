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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Virtual KFile I/O interface.
 *
 * This module implements some generic I/O interfaces for kfile.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "kfile.h"

#include "cfg/cfg_kfile.h"
#include <cfg/debug.h>
#include <cfg/log.h>

#include <drv/timer.h>
#include <mware/formatwr.h>

#include <string.h>

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_KFILE_GETS) || ((CONFIG_KFILE_GETS != 0) && CONFIG_KFILE_GETS != 1)
	#error CONFIG_KFILE_GETS must be set to either 0 or 1 in appconfig.h
#endif
#if !defined(CONFIG_PRINTF)
	#error CONFIG_PRINTF missing in appconfig.h
#endif


/**
 * Generic putc() implementation using \a fd->write.
 */
int kfile_putc(int _c, struct KFile *fd)
{
	unsigned char c = (unsigned char)_c;

	if (kfile_write(fd, &c, sizeof(c)) == sizeof(c))
		return (int)((unsigned char)_c);
	else
		return EOF;
}

/**
 * Generic getc() implementation using \a fd->read.
 */
int kfile_getc(struct KFile *fd)
{
	unsigned char c;

	if (kfile_read(fd, &c, sizeof(c)) == sizeof(c))
		return (int)((unsigned char)c);
	else
		return EOF;
}

#if CONFIG_PRINTF
/**
 * Formatted write.
 */
int kfile_printf(struct KFile *fd, const char *format, ...)
{
	va_list ap;
	int len;

	va_start(ap, format);
	len = _formatted_write(format, (void (*)(char, void *))kfile_putc, fd, ap);
	va_end(ap);

	return len;
}
#endif /* CONFIG_PRINTF */

/**
 * Write a string to kfile \a fd.
 * \return 0 if OK, EOF in case of error.
 */
int kfile_print(struct KFile *fd, const char *s)
{
	while (*s)
	{
		if (kfile_putc(*s++, fd) == EOF)
			return EOF;
	}
	return 0;
}

#if CONFIG_KFILE_GETS
/**
 * Read a line long at most as size and put it
 * in buf.
 * \return number of chars read or EOF in case
 *         of error.
 */
int kfile_gets(struct KFile *fd, char *buf, int size)
{
	return kfile_gets_echo(fd, buf, size, false);
}


/**
 * Read a line long at most as size and put it
 * in buf, with optional echo.
 *
 * \return number of chars read, or EOF in case
 *         of error.
 */
int kfile_gets_echo(struct KFile *fd, char *buf, int size, bool echo)
{
	int i = 0;
	int c;

	for (;;)
	{
		if ((c = kfile_getc(fd)) == EOF)
		{
			buf[i] = '\0';
			return -1;
		}

		/* FIXME */
		if (c == '\r' || c == '\n' || i >= size-1)
		{
			buf[i] = '\0';
			if (echo)
				kfile_print(fd, "\r\n");
			break;
		}
		buf[i++] = c;
		if (echo)
			kfile_putc(c, fd);
	}

	return i;
}
#endif /* !CONFIG_KFILE_GETS */


kfile_off_t kfile_copy(KFile *src, KFile *dst, kfile_off_t size)
{
	char buf[32];
	kfile_off_t cp_len = 0;

	while (size)
	{
		size_t len = MIN(sizeof(buf), (size_t)size);
		if (kfile_read(src, buf, len) != len)
			break;

		size_t wr_len = kfile_write(dst, buf, len);
		cp_len += wr_len;
		size -= len;

		if (wr_len != len)
			break;
	}

	return cp_len;
}


/**
 * Move \a fd file seek position of \a offset bytes from \a whence.
 *
 * This is a generic implementation of seek function, you can redefine
 * it in your local module if needed.
 */
kfile_off_t kfile_genericSeek(struct KFile *fd, kfile_off_t offset, KSeekMode whence)
{
	kfile_off_t seek_pos;

	switch (whence)
	{

	case KSM_SEEK_SET:
		seek_pos = 0;
		break;
	case KSM_SEEK_END:
		seek_pos = fd->size;
		break;
	case KSM_SEEK_CUR:
		seek_pos = fd->seek_pos;
		break;
	default:
		ASSERT(0);
		return EOF;
		break;
	}

	/* Bound check */
	if (seek_pos + offset > fd->size)
		LOG_INFO("seek outside EOF\n");

	fd->seek_pos = seek_pos + offset;

	return fd->seek_pos;
}

/**
 * Reopen file \a fd.
 * This is a generic implementation that only flush file
 * and reset seek_pos to 0.
 */
struct KFile * kfile_genericReopen(struct KFile *fd)
{
	kfile_flush(fd);
	kfile_seek(fd, 0, KSM_SEEK_SET);
	return fd;
}

/**
 * Close file \a fd.
 * This is a generic implementation that only flush the file.
 */
int kfile_genericClose(struct KFile *fd)
{
	return kfile_flush(fd);
}


/**
 * Discard input to resynchronize with remote end.
 *
 * Discard incoming data until the kfile_getc stops receiving
 * characters for at least \a delay milliseconds.
 *
 * \note If the timeout occur, we reset the error before to
 * quit.
 */
void kfile_resync(KFile *fd, mtime_t delay)
{
	ticks_t start_time = timer_clock();
	for(;;)
	{
		if(kfile_getc(fd) != EOF)
			start_time = timer_clock();

		if ((timer_clock() - start_time) > ms_to_ticks(delay))
		{
			kfile_clearerr(fd);
			break;
		}

	}
}

/**
 * Stub function that does nothing.
 * This is a generic implementation that only return 0.
 */
static int kfile_generic(UNUSED_ARG(struct KFile *, fd))
{
	return 0;
};


/**
 * Base class KFile constructor.
 */
void kfile_init(struct KFile *fd)
{
	ASSERT(fd);
	memset(fd, 0, sizeof(*fd));
	fd->clearerr = (ClearErrFunc_t)kfile_generic;
	fd->close =  kfile_genericClose;
	fd->error = kfile_generic;
	fd->flush = kfile_generic;
	fd->read = (ReadFunc_t)kfile_generic;
	fd->reopen = kfile_genericReopen;
	fd->seek = kfile_genericSeek;
	fd->write = (WriteFunc_t)kfile_generic;
}

