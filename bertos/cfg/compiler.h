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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Additional support macros for compiler independance
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef BERTOS_COMPILER_H
#define BERTOS_COMPILER_H

#include <cpu/detect.h>


#if defined __GNUC__ && defined __GNUC_MINOR__
	#define GNUC_PREREQ(maj, min) \
		((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
	#define GNUC_PREREQ(maj, min) 0
#endif

/* Some CW versions do not allow enabling C99 from the settings panel. */
#if defined(__MWERKS__)
	#pragma c99 on
#endif

#if defined(__STDC_VERSION__)
    #if (__STDC_VERSION__ == 199409L) // IAR
	    #define COMPILER_C99      1  // not true, because partial C99, avoid miscompilation
    #elif (__STDC_VERSION__ >= 199901L) // GCC
	    #define COMPILER_C99      1
    #else
	    #define COMPILER_C99      0
    #endif
#endif


/** Concatenate two different preprocessor tokens (allowing macros to expand) */
#define PP_CAT(x,y)         PP_CAT__(x,y)
#define PP_CAT__(x,y)       x ## y
#define PP_CAT3(x,y,z)      PP_CAT(PP_CAT(x,y),z)
#define PP_CAT4(x,y,z,w)    PP_CAT(PP_CAT3(x,y,z),w)
#define PP_CAT5(x,y,z,w,j)  PP_CAT(PP_CAT4(x,y,z,w),j)

/** String-ize a token (allowing macros to expand) */
#define PP_STRINGIZE(x)     PP_STRINGIZE__(x)
#define PP_STRINGIZE__(x)   #x


/**
 */
#if COMPILER_C99
	#define COUNT_PARMS2(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _, ...) _
	#define COUNT_PARMS(...) \
			COUNT_PARMS2(11 , ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

	/**
	 * usage:
	 * \code
	 * #define foo_init(...) PP_CAT(foo_init_, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	 * \endcode
	 */

#else
	#define COUNT_PARMS2(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _, ...) _
	#define COUNT_PARMS(args...) \
			COUNT_PARMS2(11 , ## args, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

	/**
	 * usage:
	 * \code
	 * #define foo_init(args...) PP_CAT(foo_init_, COUNT_PARMS(args)) (args)
	 * \endcode
	 */

#endif

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)

	#pragma language=extended

	/* IAR iccarm specific functions */
	#include <intrinsics.h>
	#pragma diag_suppress=Pe940
	#pragma inline = forced

	#define MEMORY_BARRIER		asm("")

	#if CPU_ARM || CPU_CM3

		#define COMPILER_VARIADIC_MACROS 1

		#define INTERRUPT(x)  __irq __arm void x (void)
		#define INLINE        static inline
		#define NAKED

		/* Include some standard C89/C99 stuff */
		#include <stddef.h>
		#include <stdint.h>
		#include <stdbool.h>

	#elif CPU_I196

		// IAR has size_t as built-in type, but does not define this symbol.
		#define _SIZE_T_DEFINED

		#define INTERRUPT(x)  interrupt [x]
		#define REGISTER      shortad
		#define INLINE        /* unsupported */

		/*
		 * Imported from <longjmp.h>. Unfortunately, we can't just include
		 * this header because it typedefs jmp_buf to be an array of chars.
		 * This would allow the compiler to place the buffer on an odd address.
		 * The CPU _should_ be able to perform word accesses to
		 * unaligned data, but there are *BUGS* in the 80196KC with
		 * some combinations of opcodes and addressing modes. One of
		 * these, "ST SP,[?GR]+" is used in the longjmp() implementation
		 * provided by the IAR compiler ANSI C library. When ?GR contains
		 * an odd address, surprisingly the CPU will copy the high order
		 * byte of the source operand (SP) in the low order byte of the
		 * destination operand (the memory location pointed to by ?GR).
		 *
		 * We also need to replace the library setjmp()/longjmp() with
		 * our own versions because the IAR implementation "forgets" to
		 * save the contents of local registers (?LR).
		 */
		struct _JMP_BUF
		{
			void *sp;           /* Stack pointer */
			void *return_addr;  /* Return address */
			int lr[6];          /* 6 local registers */
		};

		typedef struct _JMP_BUF jmp_buf[1];

		int setjmp(jmp_buf env);
		void longjmp(jmp_buf env, int val);

		/* Fake bool support */
		#define true (1==1)
		#define false (1!=1)
		typedef unsigned char bool;

	#else
		#error Unsupported CPU
	#endif

#elif defined(_MSC_VER) /* Win32 emulation support */

	/* MSVC doesn't provide <stdbool.h>. */
	#ifndef __cplusplus
		#define true (1==1)
		#define false (1!=1)
		typedef int bool;
	#endif /* !__cplusplus */

	/* These C99 functions are oddly named in MSVCRT32.lib */
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf

	/* MSVC doesn't support C99's __func__, but has a similar extension. */
	#define __func__ __FUNCTION__

	/* MSVC doesn't support C99's inline keyword */
	#ifndef __cplusplus
		#define INLINE __inline
	#endif

#elif defined(__GNUC__)

	/* Compiler features */
	#define COMPILER_VARIADIC_MACROS 1 /* Even in C++ */
	#define COMPILER_TYPEOF 1
	#define COMPILER_STATEMENT_EXPRESSIONS 1

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  __attribute__((__format__(type, fmt, first)))
	#define NORETURN                __attribute__((__noreturn__))
	#define UNUSED_ARG(type,arg)    __attribute__((__unused__)) type arg
	#define UNUSED_VAR(type,name)	__attribute__((__unused__)) type name
	#define USED_VAR(type,name)     __attribute__((__used__)) type name
	#define INLINE                  static inline __attribute__((__always_inline__))
	#define NOINLINE                __attribute__((noinline))
	#define LIKELY(x)               __builtin_expect(!!(x), 1)
	#define UNLIKELY(x)             __builtin_expect(!!(x), 0)
	#define PURE_FUNC               __attribute__((pure))
	#define CONST_FUNC              __attribute__((const))
	#define UNUSED_FUNC             __attribute__((unused))
	#define USED_FUNC               __attribute__((__used__))
	#define RESTRICT                __restrict__
	#define MUST_CHECK              __attribute__((warn_unused_result))
	#define PACKED                  __attribute__((packed))
	#define ALIGNED(x)              __attribute__ ((__aligned__(x)))
	#if CPU_ARM | CPU_CM3
		#define NAKED		__attribute__((naked))
	#else
		#define NAKED
	#endif

	/**
	 * Force compiler to reload context variable.
	 */
	#define MEMORY_BARRIER           asm volatile ("" : : : "memory")

	#if GNUC_PREREQ(3,1)
		#define DEPRECATED  __attribute__((__deprecated__))
	#endif

	#if GNUC_PREREQ(4,5)
		#define UNREACHABLE() __builtin_unreachable()
	#endif

	#ifndef __cplusplus
		#define ASSERT_TYPE_EQUAL(var1, var2) \
			STATIC_ASSERT(__builtin_types_compatible_p(typeof(var1), typeof(var2)))
		#define ASSERT_TYPE_IS(var, type) \
			STATIC_ASSERT(__builtin_types_compatible_p(typeof(var), type))
	#endif

	/* Include some standard C89/C99 stuff */
	#include <stddef.h>
	#include <stdint.h>
	#include <stdbool.h>
	#if !CPU_AVR
	#include <sys/types.h> /* for ssize_t */
	#endif

	#ifndef __cplusplus
		/*
		 * Disallow some C++ keywords as identifiers in C programs,
		 * for improved portability.
		 */
		#pragma GCC poison new delete class template typename
		#pragma GCC poison private protected public operator
		#pragma GCC poison friend mutable using namespace
		#pragma GCC poison cin cout cerr clog
	#endif



#elif defined(__MWERKS__)

	/* Compiler features */
	#define COMPILER_VARIADIC_MACROS 1
	#define COMPILER_TYPEOF 1
	#define COMPILER_STATEMENT_EXPRESSIONS 1

	#define typeof __typeof__

	#define UNUSED_ARG(type,arg)    type

	#include <stddef.h>
	#include <stdint.h>
	#include <stdbool.h>

	// CodeWarrior has size_t as built-in type, but does not define this symbol.
	#define _SIZE_T_DEFINED

#else
	#error unknown compiler
#endif


/* Defaults for compiler extensions. */

/**
 * \def COMPILER_VARIADIC_MACROS
 * Support for macros with variable arguments.
 */
#ifndef COMPILER_VARIADIC_MACROS
#define COMPILER_VARIADIC_MACROS (COMPILER_C99 != 0)
#endif

/**
 * \def COMPILER_TYPEOF
 * Support for dynamic type identification.
 */
#ifndef COMPILER_TYPEOF
#define COMPILER_TYPEOF 0
#endif

/**
 * \def COMPILER_STATEMENT_EXPRESSIONS
 * Support for statement expressions.
 */
#ifndef COMPILER_STATEMENT_EXPRESSIONS
#define COMPILER_STATEMENT_EXPRESSIONS 0
#endif

/* A few defaults for missing compiler features. */
#ifndef INLINE
#define INLINE                 static inline
#endif
#ifndef NOINLINE
#define NOINLINE               /* nothing */
#endif
#ifndef NORETURN
#define NORETURN               /* nothing */
#endif
#ifndef FORMAT
#define FORMAT(type,fmt,first) /* nothing */
#endif
#ifndef DEPRECATED
#define DEPRECATED             /* nothing */
#endif
#ifndef UNUSED_ARG
#define UNUSED_ARG(type,arg)   type arg
#endif
#ifndef UNUSED_VAR
#define UNUSED_VAR(type,name)  type name
#endif
#ifndef USED_VAR
#define USED_VAR(type,name)    type name
#endif
#ifndef REGISTER
#define REGISTER               /* nothing */
#endif
#ifndef LIKELY
#define LIKELY(x)              x
#endif
#ifndef UNLIKELY
#define UNLIKELY(x)            x
#endif
#ifndef PURE_FUNC
#define PURE_FUNC              /* nothing */
#endif
#ifndef CONST_FUNC
#define CONST_FUNC             /* nothing */
#endif
#ifndef UNUSED_FUNC
#define UNUSED_FUNC            /* nothing */
#endif
#ifndef USED_FUNC
#define USED_FUNC              /* nothing */
#endif
#ifndef RESTRICT
#define RESTRICT               /* nothing */
#endif
#ifndef MUST_CHECK
#define MUST_CHECK             /* nothing */
#endif
#ifndef PACKED
#define PACKED                 /* nothing */
#endif
#ifndef ALIGNED
#define ALIGNED                /* nothing */
#endif
#ifndef MEMORY_BARRIER
#define MEMORY_BARRIER         /* nothing */
#warning No memory barrier defined for select compiler. If you use the kernel check it.
#endif
#ifndef UNREACHABLE
#define UNREACHABLE() for (;;)
#endif


/* Misc definitions */
#ifndef NULL
#define NULL  (void *)0
#endif
#ifndef EOF
#define	EOF   (-1)
#endif

/* Support for hybrid C/C++ applications. */
#ifdef __cplusplus
	#define EXTERN_C        extern "C"
	#define EXTERN_C_BEGIN  extern "C" {
	#define EXTERN_C_END    }
	#define EXTERN_CONST    extern const
	#define CONST_CAST(TYPE,EXPR)   (const_cast<TYPE>(EXPR))
#else
	#define EXTERN_C        extern
	#define EXTERN_C_BEGIN  /* nothing */
	#define EXTERN_C_END    /* nothing */
	#define EXTERN_CONST    const
	#define CONST_CAST(TYPE,EXPR)   ((TYPE)(EXPR)) /* FIXME: How can we suppress the warning in C? */
#endif


#if defined(_MSC_VER) \
	|| ((defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)) && CPU_I196)
	/**
	 * \name ISO C99 fixed-size types
	 *
	 * These should be in <stdint.h>, but a few compilers lack them.
	 * \{
	 */
	typedef signed char         int8_t;
	typedef unsigned char       uint8_t;
	typedef short int           int16_t;
	typedef unsigned short int  uint16_t;
	typedef long int            int32_t; /* _WIN64 safe */
	typedef unsigned long int   uint32_t; /* _WIN64 safe */

	#ifdef _MSC_VER
		typedef __int64              int64_t;
		typedef unsigned __int64     uint64_t;
	#else
		typedef long long            int64_t;
		typedef unsigned long long   uint64_t;
	#endif
	/* \} */
#else
	/* This is the standard location. */
	#include <stdint.h>
#endif

#if CPU_AVR_ATMEGA8
	/*
	 * The ATmega8 has a very small Flash, so we can't afford
	 * to link in support routines for 32bit integer arithmetic.
	 */
	typedef int16_t ticks_t;  /**< Type for time expressed in ticks. */
	typedef int16_t mtime_t;  /**< Type for time expressed in milliseconds. */
	typedef int16_t utime_t;  /**< Type for time expressed in microseconds. */
	#define SIZEOF_MTIME_T (16 / CPU_BITS_PER_CHAR)
	#define SIZEOF_UTIME_T (16 / CPU_BITS_PER_CHAR)
	#define MTIME_INFINITE 0x7FFFL
#else
	typedef int32_t ticks_t;  /**< Type for time expressed in ticks. */

	typedef int32_t utime_t;  /**< Type for time expressed in microseconds. */
	#define SIZEOF_UTIME_T (32 / CPU_BITS_PER_CHAR)

	#ifndef DEVLIB_MTIME_DEFINED
		#define DEVLIB_MTIME_DEFINED 1 /* Resolve conflict with <os/hptime.h> */
		typedef int32_t mtime_t;  /**< Type for time expressed in milliseconds. */
		#define SIZEOF_MTIME_T (32 / CPU_BITS_PER_CHAR)
		#define MTIME_INFINITE 0x7FFFFFFFL
	#endif
#endif

/** User defined callback type */
typedef void (*Hook)(void *);

/** Bulk storage large enough for both pointers or integers. */
typedef void * iptr_t;

/** Bulk storage large enough for both pointers to constants or integers. */
typedef const void * const_iptr_t;

typedef unsigned char sigbit_t;  /**< Type for signal bits. */
typedef unsigned char sigmask_t; /**< Type for signal masks. */

/**
 * Signal structure
 */
typedef struct Signal
{
	sigmask_t    wait;    /**< Signals the process is waiting for */
	sigmask_t    recv;    /**< Received signals */
} Signal;

/**
 * \name Standard type definitions.
 *
 * These should be in <sys/types.h> or <stddef.h>, but many compilers
 * and C libraries lack them.
 *
 * We check for some common definitions to avoid redefinitions:
 *
 *    glibc, avr-libc: _SIZE_T_DEFINED, __ssize_t_defined
 *    Darwin libc:     _BSD_SIZE_T_DEFINED_, _SIZE_T
 *    IAR ARM:         _SIZE_T
 *
 * \{
 */
#if !(defined(size_t) || defined(_SIZE_T_DEFINED) || defined(_BSD_SIZE_T_DEFINED_) \
	|| defined(_SIZE_T))
	#if CPU_X86
		/* 32bit or 64bit (32bit for _WIN64). */
		typedef unsigned long size_t;
	#else
		#error Unknown CPU
	#endif
#endif

#if !(defined(ssize_t) || defined(_SSIZE_T) || defined(__ssize_t_defined))
	#if CPU_X86
		/* 32bit or 64bit (32bit for _WIN64). */
		typedef long ssize_t;
	#elif CPU_ARM || CPU_CM3
		typedef int ssize_t;
	#elif CPU_AVR || CPU_MSP430
		/* 16bit (missing in avr-/msp430-libc's sys/types.h). */
		typedef int ssize_t;
	#else
		#error Unknown CPU
	#endif
#endif
/*\}*/


/**
 * \name Types for hardware registers.
 *
 * Only use these types for registers whose contents can
 * be changed asynchronously by external hardware.
 *
 * \{
 */
#if CPU_DSP56K
	/* Registers can be accessed only through 16-bit pointers */
	typedef volatile uint16_t  reg16_t;
#else
	typedef volatile uint8_t   reg8_t;
	typedef volatile uint16_t  reg16_t;
	typedef volatile uint32_t  reg32_t;
#endif
/*\}*/


/* Quasi-ANSI macros */
#ifndef offsetof
	/**
	 * Return the byte offset of the member \a m in struct \a s.
	 *
	 * \note This macro should be defined in "stddef.h" and is sometimes
	 *       compiler-specific (g++ has a builtin for it).
	 */
	#define offsetof(s,m)  (size_t)&(((s *)0)->m)
#endif
#ifndef countof
	/**
	 * Count the number of elements in the static array \a a.
	 *
	 * \note This macro is non-standard, but implements a very common idiom
	 */
	#if defined(__GNUC__) && !defined(__cplusplus)
		/*
		 * Perform a compile time type checking: countof() can only
		 * work with static arrays, so throw a compile time error if a
		 * pointer is passed as argument.
		 *
		 * NOTE: the construct __builtin_types_compatible_p() is only
		 * available for C.
		 */
		#define countof(a) (sizeof(a) / sizeof(*(a)) +		\
				STATIC_ASSERT_EXPR(			\
					!__builtin_types_compatible_p(	\
						typeof(a), typeof(&a[0]))))
	#else
		#define countof(a)  (sizeof(a) / sizeof(*(a)))
	#endif
#endif
#ifndef alignof
	/**
	 * Return the alignment in memory of a generic data type.
	 *
	 * \note We need to worry about alignment when allocating memory that
	 * will be used later by unknown objects (e.g., malloc()) or, more
	 * generally, whenever creating generic container types.
	 */
	#define alignof(type) offsetof(struct { char c; type member; }, member)
#endif

/**
 * Cast a member of a structure out to the containing structure.
 *
 * \param ptr     the pointer to the member.
 * \param type    the type of the container struct this is embedded in.
 * \param member  the name of the member within the struct.
 */
#if COMPILER_TYPEOF && COMPILER_STATEMENT_EXPRESSIONS
	#define containerof(ptr, type, member) ({ \
		typeof( ((type *)0)->member ) *_mptr = (ptr); /* type check */ \
		(type *)(void *)((char *)_mptr - offsetof(type, member)); \
	})
#else
	#define containerof(ptr, type, member) \
		( (type *)(void *)((char *)(ptr) - offsetof(type, member)) )
#endif

/** Issue a compilation error if the \a condition is false */
#define STATIC_ASSERT(condition)  \
	UNUSED_VAR(extern char, STATIC_ASSERTION_FAILED__[(condition) ? 1 : -1])

/**
 * Issue a compilation error if \a __cond is false (this can be used inside an
 * expression).
 */
#define STATIC_ASSERT_EXPR(__cond) \
	(sizeof(struct { int STATIC_ASSERTION_FAILED__:!!(__cond); }) * 0)

#ifndef ASSERT_TYPE_EQUAL
	/** Ensure two variables have the same type. */
	#define ASSERT_TYPE_EQUAL(var1, var2)  \
			do { (void)(&(var1) == &(var2)); } while(0)
#endif

#ifndef ASSERT_TYPE_IS
	/** Ensure variable is of specified type. */
	#define ASSERT_TYPE_IS(var, type)  \
			do { (void)(&(var) == (type *)0); } while(0)
#endif

/**
 * Prevent the compiler from optimizing access to the variable \a x, enforcing
 * a refetch from memory. This also forbid from reordering successing instances
 * of ACCESS_SAFE().
 */
#ifdef __ICCARM__
#define ACCESS_SAFE(x) x
#else
#define ACCESS_SAFE(x) (*(volatile typeof(x) *)&(x))
#endif

#endif /* BERTOS_COMPILER_H */
