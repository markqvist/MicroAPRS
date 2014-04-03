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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Functions to convert integers to/from host byte-order.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef MWARE_BYTEORDER_H
#define MWARE_BYTEORDER_H

#include <cfg/compiler.h>
#include <cpu/attr.h>
#include <cpu/detect.h>
#include <cpu/types.h>
#include <cfg/macros.h>

/**
 * Swap upper and lower bytes in a 16-bit value.
 */
#define SWAB16(x) ((uint16_t)(ROTR((x), 8) + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint16_t))))

/*
 * On Cortex-M3, GCC 4.4 builtin implementation is slower than our own
 * rot-based implementation.
 */
#if GNUC_PREREQ(4, 3) && !CPU_CM3
#define SWAB32(x) ((uint32_t)(__builtin_bswap32((x) + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint32_t)))))
#else
/**
 * Reverse bytes in a 32-bit value (e.g.: 0x12345678 -> 0x78563412).
 */
#define SWAB32(x) ((uint32_t)(( \
	(ROTR(x, 8) & 0xFF00FF00) | \
	(ROTL(x, 8) & 0x00FF00FF))) + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint32_t)))
#endif

#if GNUC_PREREQ(4, 3)
#define SWAB64(x) ((uint64_t)(__builtin_bswap64((x) + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint64_t)))))
#else
/**
 * Reverse bytes in a 64-bit value.
 */
#define SWAB64(x) ((uint64_t)(						\
	(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) |	\
	(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) |	\
	(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) |	\
	(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) |	\
	(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) |	\
	(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) |	\
	(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) |	\
	(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) +	\
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint64_t))))
#endif

#if CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN
#define cpu_to_le16(x) ((uint16_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint16_t))))
#define cpu_to_le32(x) ((uint32_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint32_t))))
#define cpu_to_le64(x) ((uint64_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint64_t))))
#define cpu_to_be16(x) SWAB16(x)
#define cpu_to_be32(x) SWAB32(x)
#define cpu_to_be64(x) SWAB64(x)
#elif CPU_BYTE_ORDER == CPU_BIG_ENDIAN
#define cpu_to_le16(x) SWAB16(x)
#define cpu_to_le32(x) SWAB32(x)
#define cpu_to_le64(x) SWAB64(x)
#define cpu_to_be16(x) ((uint16_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint16_t))))
#define cpu_to_be32(x) ((uint32_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint32_t))))
#define cpu_to_be64(x) ((uint64_t)(x + \
		STATIC_ASSERT_EXPR(sizeof(x) == sizeof(uint64_t))))
#else
#error "unrecognized CPU endianness"
#endif

#define be16_to_cpu(x)		cpu_to_be16(x)
#define le16_to_cpu(x)		cpu_to_le16(x)
#define be32_to_cpu(x)		cpu_to_be32(x)
#define le32_to_cpu(x)		cpu_to_le32(x)
#define be64_to_cpu(x)		cpu_to_be64(x)
#define le64_to_cpu(x)		cpu_to_le64(x)

#define host_to_net16(x)	cpu_to_be16(x)
#define net_to_host16(x)	be16_to_cpu(x)
#define host_to_net32(x)	cpu_to_be32(x)
#define net_to_host32(x)	be32_to_cpu(x)
#define host_to_net64(x)	cpu_to_be64(x)
#define net_to_host64(x)	be64_to_cpu(x)

/**
 * Reverse bytes in a float value.
 */
INLINE float swab_float(float x)
{
	/* Avoid breaking strict aliasing rules.  */
	char *cx = (char *)(&x);
	STATIC_ASSERT(sizeof(float) == 4);
	#define BYTEORDER_SWAP(a, b) do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while(0)
	BYTEORDER_SWAP(cx[0], cx[3]);
	BYTEORDER_SWAP(cx[1], cx[2]);
	#undef BYTEORDER_SWAP
	return x;
}

INLINE float cpu_to_be_float(float x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab_float(x) : x;
}

INLINE float cpu_to_le_float(float x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab_float(x) : x;
}

INLINE float be_float_to_cpu(float x)
{
	return cpu_to_be_float(x);
}

INLINE float le_float_to_cpu(float x)
{
	return cpu_to_le_float(x);
}

INLINE float host_to_net_float(float x)
{
	return cpu_to_be_float(x);
}

INLINE float net_to_host_float(float x)
{
	return be_float_to_cpu(x);
}

#if CPU_ARM
INLINE cpu_atomic_t
cpu_atomic_xchg(volatile cpu_atomic_t *ptr, cpu_atomic_t val)
{
	cpu_atomic_t ret;

	asm volatile(
		"swp     %0, %1, [%2]"

		: "=&r" (ret)
		: "r" (val), "r" (ptr)
		: "memory", "cc");

	return ret;
}
#else /* CPU_ARM */
#include <cpu/irq.h>

INLINE cpu_atomic_t
cpu_atomic_xchg(volatile cpu_atomic_t *ptr, cpu_atomic_t val)
{
	cpu_atomic_t ret;

	ATOMIC(
		ret = *ptr;
		*ptr = val;
	);
	return ret;
}
#endif /* CPU_ARM */

#ifdef __cplusplus

/// Type generic byte swapping.
template<typename T>
INLINE T swab(T x);

template<> INLINE uint16_t swab(uint16_t x) { return SWAB16(x); }
template<> INLINE uint32_t swab(uint32_t x) { return SWAB32(x); }
template<> INLINE uint64_t swab(uint64_t x) { return SWAB64(x); }
template<> INLINE int16_t  swab(int16_t x)  { return static_cast<int16_t>(SWAB16(static_cast<uint16_t>(x))); }
template<> INLINE int32_t  swab(int32_t x)  { return static_cast<int32_t>(SWAB32(static_cast<uint32_t>(x))); }
template<> INLINE int64_t  swab(int64_t x)  { return static_cast<int64_t>(SWAB64(static_cast<uint64_t>(x))); }
template<> INLINE float    swab(float x)    { return swab_float(x); }

/// Type generic conversion from CPU byte order to big-endian byte order.
template<typename T>
INLINE T cpu_to_be(T x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab(x) : x;
}

/// Type generic conversion from CPU byte-order to little-endian.
template<typename T>
INLINE T cpu_to_le(T x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab(x) : x;
}

/// Type generic conversion from big endian byte-order to CPU byte order.
template<typename T>
INLINE T be_to_cpu(T x)
{
	return cpu_to_be(x);
}

/// Type generic conversion from little-endian byte order to CPU byte order.
template<typename T>
INLINE T le_to_cpu(T x)
{
	return cpu_to_le(x);
}

/// Type generic conversion from network byte order to host byte order.
template<typename T>
INLINE T net_to_host(T x)
{
	return be_to_cpu(x);
}

/// Type generic conversion from host byte order to network byte order.
template<typename T>
INLINE T host_to_net(T x)
{
	return net_to_host(x);
}

#endif /* __cplusplus */

#endif /* MWARE_BYTEORDER_H */
