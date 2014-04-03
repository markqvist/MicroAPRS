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
 * Copyright 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief CPU-specific type definitions.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_TYPES_H
#define CPU_TYPES_H

#include "detect.h"
#include "attr.h"
#include <limits.h>
#include <cfg/compiler.h> /* for uintXX_t */

#if CPU_I196

	typedef uint16_t cpu_flags_t; // FIXME
	typedef unsigned int cpu_stack_t;
	typedef cpu_stack_t cpu_aligned_stack_t;
	typedef unsigned int cpu_atomic_t;
	#warning Verify following constant
	#define SIZEOF_CPUSTACK_T 2
	#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T

#elif CPU_X86

	/* Get cpu_flags_t definition from the hosting environment. */
	#include <cfg/os.h>
	#if OS_EMBEDDED
		typedef uint32_t cpu_flags_t; // FIXME
	#endif /* OS_EMBEDDED */

	typedef uint32_t cpu_atomic_t;

	#if CPU_X86_64
		typedef uint64_t cpu_stack_t;
		typedef cpu_stack_t cpu_aligned_stack_t;
		#define SIZEOF_CPUSTACK_T 8
		#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T
	#else
		typedef uint32_t cpu_stack_t;
		typedef cpu_stack_t cpu_aligned_stack_t;
		#define SIZEOF_CPUSTACK_T 4
		#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T
	#endif

#elif CPU_ARM || CPU_CM3

	typedef uint32_t cpu_flags_t;
	typedef uint32_t cpu_atomic_t;
	typedef uint32_t cpu_stack_t;
	#define SIZEOF_CPUSTACK_T 4

	typedef uint64_t cpu_aligned_stack_t;
	#define SIZEOF_CPUALIGNED_T 8

#elif CPU_PPC

	/* Get cpu_flags_t definition from the hosting environment. */
	#include <cfg/os.h>
	#if OS_EMBEDDED
		typedef uint32_t cpu_flags_t;
	#endif

	typedef uint32_t cpu_atomic_t;
	typedef uint32_t cpu_stack_t;
	typedef  cpu_stack_t cpu_aligned_stack_t;
	#define SIZEOF_CPUSTACK_T 4
	#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T

#elif CPU_DSP56K

	typedef uint16_t cpu_flags_t;
	typedef uint16_t cpu_atomic_t;
	typedef unsigned int cpu_stack_t;
	typedef cpu_stack_t cpu_aligned_stack_t;
	#warning Verify following costant
	#define SIZEOF_CPUSTACK_T 2
	#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T

#elif CPU_AVR

	typedef uint8_t cpu_flags_t;
	typedef uint8_t cpu_atomic_t;
	typedef uint8_t cpu_stack_t;
	typedef cpu_stack_t cpu_aligned_stack_t;
	#define SIZEOF_CPUSTACK_T 1
	#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T

#elif CPU_MSP430

	typedef uint16_t cpu_flags_t;
	typedef uint16_t cpu_stack_t;
	typedef cpu_stack_t cpu_aligned_stack_t;
	#define SIZEOF_CPUSTACK_T 2
	#define SIZEOF_CPUALIGNED_T SIZEOF_CPUSTACK_T

#else
	#error No CPU_... defined.
#endif

/**
 * \name Default type sizes.
 *
 * These defaults are reasonable for most 16/32bit machines.
 * Some of these macros may be overridden by CPU-specific code above.
 *
 * ANSI C requires that the following equations be true:
 * \code
 *   sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long)
 *   sizeof(float) <= sizeof(double)
 *   CPU_BITS_PER_CHAR  >= 8
 *   CPU_BITS_PER_SHORT >= 8
 *   CPU_BITS_PER_INT   >= 16
 *   CPU_BITS_PER_LONG  >= 32
 * \endcode
 * \{
 */
#ifndef SIZEOF_CHAR
#define SIZEOF_CHAR  1
#endif

#ifndef SIZEOF_SHORT
#define SIZEOF_SHORT  2
#endif

#ifndef SIZEOF_INT
#if CPU_REG_BITS < 32
	#define SIZEOF_INT  2
#else
	#define SIZEOF_INT  4
#endif
#endif /* !SIZEOF_INT */

#ifndef SIZEOF_LONG
#if CPU_REG_BITS > 32
	#define SIZEOF_LONG  8
#else
	#define SIZEOF_LONG  4
#endif
#endif

#ifndef SIZEOF_PTR
#if CPU_REG_BITS < 32
	#define SIZEOF_PTR   2
#elif CPU_REG_BITS == 32
	#define SIZEOF_PTR   4
#else /* CPU_REG_BITS > 32 */
	#define SIZEOF_PTR   8
#endif
#endif

#ifndef SIZEOF_SIZE_T
#if CPU_REG_BITS < 32
	#define SIZEOF_SIZE_T   2
#elif CPU_REG_BITS == 32
	#define SIZEOF_SIZE_T   4
#else /* CPU_REG_BITS > 32 */
	#define SIZEOF_SIZE_T   8
#endif
#endif

#ifndef CPU_BITS_PER_CHAR
#define CPU_BITS_PER_CHAR   (SIZEOF_CHAR * 8)
#endif

#ifndef CPU_BITS_PER_SHORT
#define CPU_BITS_PER_SHORT  (SIZEOF_SHORT * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_INT
#define CPU_BITS_PER_INT    (SIZEOF_INT * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_LONG
#define CPU_BITS_PER_LONG   (SIZEOF_LONG * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_PTR
#define CPU_BITS_PER_PTR    (SIZEOF_PTR * CPU_BITS_PER_CHAR)
#endif


/*\}*/

#ifndef INT_MAX
	#define INT_MAX	((int)((unsigned int)~0 >> 1))
	#define INT_MIN	(-INT_MAX - 1)
#endif

/* Sanity checks for the above definitions */
STATIC_ASSERT(sizeof(char) == SIZEOF_CHAR);
STATIC_ASSERT(sizeof(short) == SIZEOF_SHORT);
STATIC_ASSERT(sizeof(long) == SIZEOF_LONG);
STATIC_ASSERT(sizeof(int) == SIZEOF_INT);
STATIC_ASSERT(sizeof(void *) == SIZEOF_PTR);
STATIC_ASSERT(sizeof(int8_t) * CPU_BITS_PER_CHAR == 8);
STATIC_ASSERT(sizeof(uint8_t) * CPU_BITS_PER_CHAR == 8);
STATIC_ASSERT(sizeof(int16_t) * CPU_BITS_PER_CHAR == 16);
STATIC_ASSERT(sizeof(uint16_t) * CPU_BITS_PER_CHAR == 16);
STATIC_ASSERT(sizeof(int32_t) * CPU_BITS_PER_CHAR == 32);
STATIC_ASSERT(sizeof(uint32_t) * CPU_BITS_PER_CHAR == 32);
#ifdef __HAS_INT64_T__
STATIC_ASSERT(sizeof(int64_t) * CPU_BITS_PER_CHAR == 64);
STATIC_ASSERT(sizeof(uint64_t) * CPU_BITS_PER_CHAR == 64);
#endif
STATIC_ASSERT(sizeof(cpu_stack_t) == SIZEOF_CPUSTACK_T);
STATIC_ASSERT(sizeof(cpu_aligned_stack_t) == SIZEOF_CPUALIGNED_T);
STATIC_ASSERT(sizeof(size_t) == SIZEOF_SIZE_T);


/**
 * Macros for hardware access, both direct and via the bit-band region.
 */
/*\{*/
#define HWREG(x)   (*((reg32_t *)(x)))
#define HWREGH(x)  (*((reg16_t *)(x)))
#define HWREGB(x)  (*((reg8_t *)(x)))
/*\}*/

#endif /* CPU_TYPES_H */
