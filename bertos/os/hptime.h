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
 * \brief Portable abstraction for high-resolution time handling (interface)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef HPTIME_H
#define HPTIME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32

	/** our type for "high precision absolute time" */
	typedef __int64 hptime_t;
	#define SIZEOF_HPTIME_T 8

	#define HPTIME_TICKS_PER_SECOND		(10000000I64)
	#define HPTIME_TICKS_PER_MILLISEC	(10000I64)
	#define HPTIME_TICKS_PER_MICRO		(10I64)

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

	#include <stdint.h> /* int64_t */

	#ifndef DEVLIB_MTIME_DEFINED
		#define DEVLIB_MTIME_DEFINED 1 /* Resolve conflict with <cfg/compiler.h> */
		typedef int32_t mtime_t;
		#define SIZEOF_MTIME_T (32 / CPU_BITS_PER_CHAR)
		#define MTIME_INFINITE 0x7FFFFFFFL
	#endif

	/** Type for "high precision absolute time". */
	typedef int64_t hptime_t;
	#define SIZEOF_HPTIME_T 8

	#define HPTIME_TICKS_PER_SECOND		(1000000LL)
	#define HPTIME_TICKS_PER_MILLISEC	(1000LL)
	#define HPTIME_TICKS_PER_MICRO		(1LL)

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */

/**
 * Return the current time with the maximum precision made available from the hosting OS
 */
extern hptime_t hptime_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HPTIME_H */
