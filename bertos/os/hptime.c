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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Portable abstraction for high-resolution time handling (implementation)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "hptime.h"

#if defined(_WIN32)

#include <windows.h>

hptime_t hptime_get(void)
{
	FILETIME ft;

	/*
	 * La precisione dei FileTime sarebbe 100ns, ma il
	 * valore viene ottenuto convertendo una struttura
	 * SYSTEMTIME, che ha precisione di 1ms. Il numero
	 * che otteniamo e' quindi sempre un multiplo di
	 * 100000.
	 */
	GetSystemTimeAsFileTime(&ft);

	/* Copy the upper/lower into a quadword. */
	return (((hptime_t)ft.dwHighDateTime) << 32) + (hptime_t)ft.dwLowDateTime;
}

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#include <sys/time.h> /* for gettimeofday() */
#include <stddef.h> /* for NULL */

hptime_t hptime_get(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (hptime_t)tv.tv_sec * HPTIME_TICKS_PER_SECOND
		+ (hptime_t)tv.tv_usec;
}

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */

