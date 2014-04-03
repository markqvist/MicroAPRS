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
 * \defgroup macros General purpose macros
 * \ingroup core
 * \{
 *
 * \brief Common and handy function macros
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Giovanni Bajo <rasky@develer.com>
 */
#ifndef CFG_MACROS_H
#define CFG_MACROS_H

#include <cfg/compiler.h>

/* avr-gcc does not seem to support libstdc++ */
#if defined(__cplusplus) && !CPU_AVR
	/* Type-generic macros implemented with template functions. */
	#include <algorithm>

	template<class T> inline T ABS(T n) { return n >= 0 ? n : -n; }
	#define MIN(a,b)   std::min(a, b)
	#define MAX(a,b)   std::max(a, b)
	#define SWAP(a,b)  std::swap(a, b)
#elif (COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF)
	/* Type-generic macros implemented with statement expressions. */
	#define ABS(n) ({ \
		typeof(n) _n = (n); \
		(_n < 0) ? -_n : _n; \
	})
	#define MIN(a,b) ({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		ASSERT_TYPE_EQUAL(_a, _b); \
		/** \
		 * The (typeof(_a)) cast in necessary: \
		 * result type of conditional expressions is \
		 * *NOT* the type of the value returned but \
		 * the type that would be produced if _a and _b \
		 * were mixed in an expression. \
		 * Even in _a and _b are of the same type, \
		 * if mixed in an expression the type will be \
		 * (at least) promoted to int! \
		 */ \
		((typeof(_a))((_a < _b) ? _a : _b)); \
	})
	#define MAX(a,b) ({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		ASSERT_TYPE_EQUAL(_a, _b); \
		/** \
		 * The (typeof(_a)) cast in necessary: \
		 * result type of conditional expressions is \
		 * *NOT* the type of the value returned but \
		 * the type that would be produced if _a and _b \
		 * were mixed in an expression. \
		 * Even in _a and _b are of the same type, \
		 * if mixed in an expression the type will be \
		 * (at least) promoted to int! \
		 */ \
		((typeof(_a))((_a > _b) ? _a : _b)); \
	})
#else /* !(COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF) */
	/* Buggy macros for inferior compilers.  */
	#define ABS(a)		(((a) < 0) ? -(a) : (a))
	#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
	#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif /* !(COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF) */

/** Align \p value to the next \p align boundary */
#define ALIGN_UP(value, align)	(((value) & ((align) - 1)) ? \
				(((value) + ((align) - 1)) & ~((align) - 1)) : \
				(value))

/** Bound \a x between \a min and \a max. */
#define MINMAX(min,x,max)  (MIN(MAX(min, x), max))

#ifdef __cplusplus
	/* Use standard implementation from <algorithm> */
	#define SWAP(a,b)  std::swap(a, b)
#elif COMPILER_TYPEOF
	/**
	 * Type-generic macro to swap \a a with \a b.
	 *
	 * \note Arguments are evaluated multiple times.
	 */
	#define SWAP(a, b) \
		do { \
			typeof(a) tmp; \
			ASSERT_TYPE_EQUAL(a, b); \
			tmp = (a); \
			(a) = (b); \
			(b) = tmp; \
		} while (0)
#else /* !COMPILER_TYPEOF */
	/* Sub-optimal implementation that only works with integral types. */
	#define SWAP(a, b) \
		do { \
			(a) ^= (b); \
			(b) ^= (a); \
			(a) ^= (b); \
		} while (0)

#endif /* COMPILER_TYPEOF */

/**
 * Shuffle the content of \a array that counts \a len elements.
 */
#define SHUFFLE(array, len) \
	do { \
		int i, j; \
		for (i = (len) - 1; i > 0; i--) \
		{ \
			j = ((i + 1) * (rand() / (RAND_MAX + 1.0))); \
			SWAP((array)[i], (array)[j]); \
		} \
	} while (0)

/**
 * Macro to swap \a a with \a b, with explicit type \a T for dumb C89 compilers.
 *
 * \note Arguments are evaluated multiple times.
 */
#define SWAP_T(a, b, T) \
	do { \
		T tmp; \
		ASSERT_TYPE_IS(a, T); \
		ASSERT_TYPE_IS(b, T); \
		tmp = (a); \
		(a) = (b); \
		(b) = tmp; \
	} while (0)

/**
 * Reverse the bits contained in b (LSB becomes the MSB and so on).
 * \note \a b is evaluated twice
 */
#define REVERSE_UINT8(b) \
	((uint8_t)((((b) * 0x0802UL & 0x22110UL) | ((b) * 0x8020UL & 0x88440UL)) * 0x10101UL >> 16))

#ifndef BV
	/** Convert a bit value to a binary flag. */
	#define BV(x)  (1<<(x))
#endif

/** Same as BV() but with 32 bit result */
#define BV32(x)  ((uint32_t)1<<(x))

/** Same as BV() but with 16 bit result */
#define BV16(x)  ((uint16_t)1<<(x))

/** Same as BV() but with 8 bit result */
#define BV8(x)  ((uint8_t)1<<(x))

/**
 * Perform an integer division rounding the result to the nearest int value.
 * \note \a divisor should preferibly be a costant, otherwise this macro generates
 * 2 division. Also divisor is evaluated twice.
 */
#define DIV_ROUND(dividend, divisor)  (((dividend) + (divisor) / 2) / (divisor))

/**
 * Perform an integer division rounding the result to the upper int value.
 * \note \a divisor is evaluated twice.
 */
#define DIV_ROUNDUP(dividend, divisor)  (((dividend) + (divisor) - 1) / (divisor))


/**
 * Perform a multiply between the integer \a a and the float constant \a f.
 *
 * This macro can be used in order to avoid floating point arithmetics
 * in expressions like this:
 * \code
 * int a, b;
 * a = b * 0.5579652750;
 * \endcode
 *
 * This macro rounds the floating point constant to a fraction,
 * usign (2 ^ prec) as the denominator.
 * For instance, with prec = 8, the constant 0.5579652750 will be rounded to:
 * (143 / 256) = 0.55859375
 * So, the former code will be transformed to:
 * \code
 * a = b * 143 / 256;
 * \endcode
 *
 * Since the denominator is a power of 2, we rely on the compiler to optimize
 * this to a right shift.
 * So, when you have to multiply an integer by a float constant, this macro
 * will not use the floating point arithmentics.
 * The operation will be converted to a mul + shift, with a huge performance boost.
 *
 * \note \a f MUST be a constant in order gain performance benefits.
 *
 * \param a integer you want to multiply
 * \param f floating point constant which you want to multply with \a a
 * \param prec conversion precision, ranges from 1 to the number of bits in a long.
 *             The higher, the better the approximation of the float constant will be.
 */
#define INT_MULT(a, f, prec) (((a) * (long)((f) * (1 << (prec)) + 0.5)) >> (prec))


/** Round up \a x to an even multiple of the 2's power \a pad. */
#define ROUND_UP2(x, pad) (((x) + ((pad) - 1)) & ~((pad) - 1))

/**
 * \name Integer round macros.
 *
 * Round \a x to a multiple of \a base.
 * \note If \a x is signed these macros generate a lot of code.
 * \{
 */
#define ROUND_DOWN(x, base)    ( (x) - ((x) % (base)) )
#define ROUND_UP(x, base)      ( ((x) + (base) - 1) - (((x) + (base) - 1) % (base)) )
#define ROUND_NEAREST(x, base) ( ((x) + (base) / 2) - (((x) + (base) / 2) % (base)) )
/* \} */

/** Check if \a x is an integer power of 2. */
#define IS_POW2(x)     (!(bool)((x) & ((x)-1)))

/** Calculate a compile-time log2 for a uint8_t */
#define UINT8_LOG2(x) \
	((x) < 2 ? 0 : \
	 ((x) < 4 ? 1 : \
	  ((x) < 8 ? 2 : \
	   ((x) < 16 ? 3 : \
	    ((x) < 32 ? 4 : \
	     ((x) < 64 ? 5 : \
	      ((x) < 128 ? 6 : 7)))))))

/** Calculate a compile-time log2 for a uint16_t */
#define UINT16_LOG2(x) \
	((x < 256) ? UINT8_LOG2(x) : UINT8_LOG2((x) >> 8) + 8)

/** Calculate a compile-time log2 for a uint32_t */
#define UINT32_LOG2(x) \
	((x < 65536UL) ? UINT16_LOG2(x) : UINT16_LOG2((x) >> 16) + 16)

#if COMPILER_VARIADIC_MACROS
	/** Count the number of arguments (up to 16). */
	#define PP_COUNT(...) \
		PP_COUNT__(__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
	#define PP_COUNT__(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,count,...) \
		count
#endif

#if COMPILER_VARIADIC_MACROS
	/**
	 * \def BIT_CHANGE(reg, (mask, value), ...)
	 *
	 * This macro allows for efficient and compact bit toggling in a hardware
	 * register. It is meant to replace hand-coded cruft which toggles bits
	 * in sequence.
	 *
	 * It is possible to specify an unlimited pair of (mask, value) parameters.
	 * For instance:
	 *
	 * \code
	 * void set_timer(bool start)
	 * {
	 *     BIT_CHANGE(REG_CTRL_TIMER,
	 *        (TIMER_MODE, MODE_COUNT),
	 *        (OVL_IRQ, 1),
	 *        (CMP_IRQ, 1),
	 *        (START, start)
	 *     );
	 * }
	 * \endcode
	 *
	 * The macro expansion will be roughly the following:
	 *
	 * \code
	 * REG_CTRL_TIMER = (REG_CTRL_TIMER & ~(TIMER_MODE|OVL_IRQ|CMP_IRQ|START)
	 *                  | (MODE_COUNT|OVL_IRQ|CMP_IRQ|(start ? START : 0));
	 * \endcode
	 *
	 * It is up to the compiler to produce the optimal code. We checked that GCC produces
	 * the best code in most cases. We preferred this expansion over the use of a block
	 * with a local variable because CodeWarrior 6.1 was not able to remove completely the
	 * allocation of the local from the stack.
	 *
	 * \note This macro is available only in C99 because it makes use of variadic macros.
	 * It would be possible to make up an implementation with a slightly different syntax
	 * for use with C90 compilers, through Boost Preprocessor.
	 */

	/**
	 * \def BIT_CHANGE_BV(reg, (bit, value), ...)
	 *
	 * Similar to BIT_CHANGE(), but get bits instead of masks (and applies BV() to convert
	 * them to masks).
	 */

	#define BIT_EXTRACT_FLAG_0(bit, value)  bit
	#define BIT_EXTRACT_FLAG_1(bit, value)  BV(bit)
	#define BIT_EXTRACT_VALUE__(bit, value) value

	#define BIT_MASK_SINGLE__(use_bv, index, max, arg) \
		((index < max) ? (PP_CAT(BIT_EXTRACT_FLAG_, use_bv) arg) : 0) \
		/* */

	#define BIT_MASK_IF_SINGLE__(use_bv, index, max, arg) \
		(((index < max) && (BIT_EXTRACT_VALUE__ arg)) ? (PP_CAT(BIT_EXTRACT_FLAG_, use_bv) arg) : 0) \
		/* */

	#define BIT_ITER__2(macro, use_bv, max, a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15, ...) \
		(macro(use_bv, 0, max, a0) | \
		macro(use_bv, 1, max, a1) | \
		macro(use_bv, 2, max, a2) | \
		macro(use_bv, 3, max, a3) | \
		macro(use_bv, 4, max, a4) | \
		macro(use_bv, 5, max, a5) | \
		macro(use_bv, 6, max, a6) | \
		macro(use_bv, 7, max, a7) | \
		macro(use_bv, 8, max, a8) | \
		macro(use_bv, 9, max, a9) | \
		macro(use_bv, 10, max, a10) | \
		macro(use_bv, 11, max, a11) | \
		macro(use_bv, 12, max, a12) | \
		macro(use_bv, 13, max, a13) | \
		macro(use_bv, 14, max, a14) | \
		macro(use_bv, 15, max, a15)) \
		/* */

	#define BIT_ITER__(macro, use_bv, ...) \
		BIT_ITER__2(macro, use_bv, PP_COUNT(__VA_ARGS__), __VA_ARGS__, (0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1)) \
		/* */

	#define BIT_MASKS__(use_bv, ...) \
		BIT_ITER__(BIT_MASK_SINGLE__, use_bv, __VA_ARGS__)
		/* */

	#define BIT_MASKS_CONDITIONAL__(use_bv, ...) \
		BIT_ITER__(BIT_MASK_IF_SINGLE__, use_bv, __VA_ARGS__)
		/* */

	#define BIT_CHANGE__(reg, use_bv, ...) \
		((reg) = ((reg) & ~BIT_MASKS__(use_bv, __VA_ARGS__)) | BIT_MASKS_CONDITIONAL__(use_bv, __VA_ARGS__)) \
		/* */

	#define BIT_CHANGE(reg, ...)        BIT_CHANGE__(reg, 0, __VA_ARGS__)
	#define BIT_CHANGE_BV(reg, ...)     BIT_CHANGE__(reg, 1, __VA_ARGS__)

#endif /* COMPILER_VARIADIC_MACROS */

/**
 * Macro for rotating bit left or right.
 * \{
 */
#define ROTR(var, rot) (((var) >> (rot)) | ((var) << ((sizeof(var) * 8) - (rot))))
#define ROTL(var, rot) (((var) << (rot)) | ((var) >> ((sizeof(var) * 8) - (rot))))
/*\}*/

/**
 * Make an id from 4 letters, useful for
 * file formats and kfile ids.
 */
#define MAKE_ID(a,b,c,d) \
 	( ((uint32_t)(a) << 24) \
 	| ((uint32_t)(b) << 16) \
 	| ((uint32_t)(c) <<  8) \
 	| ((uint32_t)(d) <<  0) )

/**
 * Type for id generated by MAKE_ID().
 */
typedef uint32_t id_t;

/**
 * Check if a pointer is aligned to a certain power-of-2 size
 */
INLINE bool is_aligned(const void *addr, size_t size)
{
	return ((size_t)addr & (size - 1)) == 0;
}

/** \} */ //defgroup macros

#endif /* MACROS_H */

