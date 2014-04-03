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
 *
 * -->
 *
 * \defgroup debug Debugging facilities and macros
 * \ingroup core
 * \{
 *
 * \brief Simple debug facilities for hosted and embedded C/C++ applications.
 *
 * Debug output goes to stderr in hosted applications.
 * Freestanding (AKA embedded) applications use \c drv/kdebug.c to output
 * diagnostic messages to a serial terminal or a JTAG debugger.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "debug"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_debug.h"
 * $WIZ$ DEBUG = 1
 * $WIZ$ module_depends = "formatwr"
 * $WIZ$ module_supports = "not atmega103"
 */

#ifndef BERTOS_DEBUG_H
#define BERTOS_DEBUG_H

#include <cfg/os.h>
#include <cfg/compiler.h>

#include "cfg/cfg_debug.h"   /* CONFIG_KDEBUG_* */

/*
 * Defaults for rarely used config stuff.
 */
#ifndef CONFIG_KDEBUG_TRACE
#define CONFIG_KDEBUG_TRACE  1
#endif

#ifndef CONFIG_KDEBUG_VERBOSE_ASSERT
#define CONFIG_KDEBUG_VERBOSE_ASSERT  1
#endif

#ifndef CONFIG_KDEBUG_WALLS
#define CONFIG_KDEBUG_WALLS  1
#endif

#if defined(__doxygen__)
	/**
	 * Preprocessor symbol defined only for debug builds.
	 *
	 * The build infrastructure must arrange for _DEBUG to
	 * be predefined for all the source files being compiled.
	 *
	 * This is compatible with the MSVC convention for the
	 * default Debug and Release project targets.
	 */
	#define _DEBUG 1
#endif

#ifdef _DEBUG
	// STLport specific: enable extra checks
	#define __STL_DEBUG 1

	// MSVC specific: Enable memory allocation debug
	#if defined(_MSC_VER)
		#include <crtdbg.h>
	#endif

	/*
	 * On UNIX systems the extabilished practice is to define
	 * NDEBUG for release builds and nothing for debug builds.
	 */
	#ifdef NDEBUG
	#undef NDEBUG
	#endif

	/**
	 * This macro duplicates the old MSVC trick of redefining
	 * THIS_FILE locally to avoid the overhead of many duplicate
	 * strings in ASSERT().
	 */
	#ifndef THIS_FILE
		#define THIS_FILE  __FILE__
	#endif

	/**
	 * This macro can be used to conditionally exclude one or more
	 * statements conditioned on \c _DEBUG, avoiding the clutter
	 * of ifdef/endif pairs.
	 *
	 * \code
	 *     struct FooBar
	 *     {
	 *         int foo;
	 *         bool bar;
	 *         DB(int ref_count;) // Track number of users
	 *
	 *         void release()
	 *         {
	 *             DB(--ref_count;)
	 *         }
	 *     };
	 * \endcode
	 */
	#define DB(x) x

	#include <cpu/attr.h>        /* CPU_HARVARD */

	/* These are implemented in drv/kdebug.c */
	void kdbg_init(void);
	void kputchar(char c);
	int kputnum(int num);
	void kdump(const void *buf, size_t len);
	void __init_wall(long *wall, int size);

	#if CPU_HARVARD
		#include <cpu/pgm.h>
		void kputs_P(const char *PROGMEM str);
		void kprintf_P(const char *PROGMEM fmt, ...) FORMAT(__printf__, 1, 2);
		int __bassert_P(const char *PROGMEM cond, const char *PROGMEM file, int line);
		void __trace_P(const char *func);
		void __tracemsg_P(const char *func, const char *PROGMEM fmt, ...);
		int __invalid_ptr_P(void *p, const char *PROGMEM name, const char *PROGMEM file, int line);
		int __check_wall_P(long *wall, int size, const char * PGM_ATTR name, const char * PGM_ATTR file, int line);
		#define kputs(str)  kputs_P(PSTR(str))
		#define kprintf(fmt, ...)  kprintf_P(PSTR(fmt) ,## __VA_ARGS__)
		#define __bassert(cond, file, line)  __bassert_P(PSTR(cond), PSTR(file), (line))
		#define __trace(func)  __trace_P(func)
		#define __tracemsg(func, fmt, ...)  __tracemsg_P(func, PSTR(fmt), ## __VA_ARGS__)
		#define __invalid_ptr(p, name, file, line)  __invalid_ptr_P((p), PSTR(name), PSTR(file), (line))
		#define __check_wall(wall, size, name, file, line)  __check_wall_P(wall, size, PSTR(name), PSTR(file), (line))
	#else /* !CPU_HARVARD */
		void kputs(const char *str);
		void kprintf(const char *fmt, ...) FORMAT(__printf__, 1, 2);
		int __bassert(const char *cond, const char *file, int line);
		void __trace(const char *func);
		void __tracemsg(const char *func, const char *fmt, ...) FORMAT(__printf__, 2, 3);
		int __invalid_ptr(void *p, const char *name, const char *file, int line);
		int __check_wall(long *wall, int size, const char *name, const char *file, int line);
	#endif /* !CPU_HARVARD */

	#if CONFIG_KDEBUG_VERBOSE_ASSERT
		/**
		 * Assert a pre-condition on code.
		 */
		#define ASSERT(x)         ((void)(LIKELY(x) ? 0 : __bassert(#x, THIS_FILE, __LINE__)))
		/**
		 * Assert a pre-condition and give explanation message when assert fails
		 */
		#define ASSERT2(x, help)  ((void)(LIKELY(x) ? 0 : __bassert(help " (" #x ")", THIS_FILE, __LINE__)))
	#else
		#define ASSERT(x)         ((void)(LIKELY(x) ? 0 : __bassert("", THIS_FILE, __LINE__)))
		#define ASSERT2(x, help)  ((void)ASSERT(x))
	#endif

	#define IS_VALID_PTR(p) (LIKELY((void *)(p) >= (void *)CPU_RAM_START))
	/**
	 * Check that the given pointer is either NULL or pointing to valid memory.
	 *
	 * The assumption here is that valid pointers never point to low
	 * memory regions.  This helps catching pointers taken from
	 * struct/class memebers when the struct pointer was NULL.
	 *
	 * \see ASSERT_VALID_PTR_OR_NULL()
	 */
	#define ASSERT_VALID_PTR(p) (IS_VALID_PTR(p) \
		? 0 : __invalid_ptr(p, #p, THIS_FILE, __LINE__))

	/**
	 * Check that the given pointer is not pointing to invalid memory.
	 *
	 * \note The check for invalid memory is architecture specific and
	 *       conservative.  The current implementation only checks against
	 *       a lower bound.
	 *
	 * \see ASSERT_VALID_PTR()
	 */
	#define ASSERT_VALID_PTR_OR_NULL(p) ((void)(LIKELY((p == NULL) \
		|| ((void *)(p) >= (void *)CPU_RAM_START)) \
		? 0 : __invalid_ptr((p), #p, THIS_FILE, __LINE__)))

	#if CONFIG_KDEBUG_TRACE
		#define TRACE  __trace(__func__)
		#define TRACEMSG(msg,...) __tracemsg(__func__, msg, ## __VA_ARGS__)
	#else
		#define TRACE  do {} while(0)
		#define TRACEMSG(...)  do {} while(0)
	#endif

	/**
	 * Check that the given pointer actually points to an object
	 * of the specified type.
	 */
	#define ASSERT_VALID_OBJ(_t, _o) do { \
		ASSERT_VALID_PTR((_o)); \
		ASSERT(dynamic_cast<_t>((_o)) != NULL); \
	}

	/**
	 * \name Debug object creation and destruction.
	 *
	 * These macros help track some kinds of leaks in C++ programs.
	 * Usage is as follows:
	 *
	 * \code
	 *   class Foo
	 *   {
	 *       DECLARE_INSTANCE_TRACKING(Foo)
	 *
	 *       Foo()
	 *       {
	 *           NEW_INSTANCE(Foo);
	 *           // ...
	 *       }
	 *
	 *       ~Foo()
	 *       {
	 *           DELETE_INSTANCE(Foo);
	 *           // ...
	 *       }
	 *   };
	 *
	 *   // Put this in the implementation file of the class
	 *   IMPLEMENT_INSTANCE_TRACKING(Foo)
	 *
	 *   // Client code
	 *   int main(void)
	 *   {
	 *        Foo *foo = new Foo;
	 *        cout << GET_INSTANCE_COUNT(Foo) << endl; // prints "1"
	 *        delete foo;
	 *        ASSERT_ZERO_INSTANCES(Foo); // OK
	 *   }
	 * \endcode
	 * \{
	 */
	#define NEW_INSTANCE(CLASS)                do { ++CLASS::__instances } while (0)
	#define DELETE_INSTANCE(CLASS)             do { --CLASS::__instances } while (0)
	#define ASSERT_ZERO_INSTANCES(CLASS)       ASSERT(CLASS::__instances == 0)
	#define GET_INSTANCE_COUNT(CLASS)          (CLASS::__instances)
	#define DECLARE_INSTANCE_TRACKING(CLASS)   static int __instances
	#define IMPLEMENT_INSTANCE_TRACKING(CLASS) int CLASS::__instances = 0
	/*\}*/

#else /* !_DEBUG */

	/*
	 * On UNIX systems the extabilished practice is to define
	 * NDEBUG for release builds and nothing for debug builds.
	 */
	#ifndef NDEBUG
	#define NDEBUG 1
	#endif

	#define DB(x)  /* nothing */
	#ifndef ASSERT
		#define ASSERT(x)  ((void)0)
	#endif /* ASSERT */
	#define ASSERT2(x, help)             ((void)0)
	#define IS_VALID_PTR(p)              (1)
	#define ASSERT_VALID_PTR(p)          ((void)0)
	#define ASSERT_VALID_PTR_OR_NULL(p)  ((void)0)
	#define ASSERT_VALID_OBJ(_t, _o)     ((void)0)
	#define TRACE                        do {} while (0)
	#if COMPILER_VARIADIC_MACROS
		#define TRACEMSG(x, ...)         do {} while (0)
	#else
		INLINE void TRACEMSG(UNUSED_ARG(const char *, msg), ...)
		{
			/* NOP */
		}
	#endif

	#define NEW_INSTANCE(CLASS)                do {} while (0)
	#define DELETE_INSTANCE(CLASS)             do {} while (0)
	#define ASSERT_ZERO_INSTANCES(CLASS)       do {} while (0)
	#define GET_INSTANCE_COUNT(CLASS)          ERROR_ONLY_FOR_DEBUG
	#define DECLARE_INSTANCE_TRACKING(CLASS)
	#define IMPLEMENT_INSTANCE_TRACKING(CLASS)

	INLINE void kdbg_init(void) { /* nop */ }
	INLINE void kputchar(UNUSED_ARG(char, c)) { /* nop */ }
	INLINE int kputnum(UNUSED_ARG(int, num)) { return 0; }
	INLINE void kputs(UNUSED_ARG(const char *, str)) { /* nop */ }
	INLINE void kdump(UNUSED_ARG(const void *, buf), UNUSED_ARG(size_t, len)) { /* nop */ }

	#if defined(__cplusplus) && COMPILER_VARIADIC_MACROS
		/* G++ can't inline functions with variable arguments... */
		#define kprintf(fmt,...) do { (void)(fmt); } while(0)
	#else
		/* ...but GCC can. */
		INLINE void kprintf(UNUSED_ARG(const char *, fmt), ...) { /* nop */ }
	#endif

#endif /* _DEBUG */

#if CONFIG_KDEBUG_WALLS
	/**
	 * \name Walls to detect data corruption
	 * \{
	 */
	#define WALL_SIZE                    8
	#define WALL_VALUE                   (long)0xABADCAFEL
	#define DECLARE_WALL(name,size)      long name[(size) / sizeof(long)];
	#define FWD_DECLARE_WALL(name,size)  extern long name[(size) / sizeof(long)];
	#define INIT_WALL(name)              __init_wall((name), countof(name))
	#define CHECK_WALL(name)             __check_wall((name), countof(name), #name, THIS_FILE, __LINE__)
	/*\}*/
#else
	#define DECLARE_WALL(name, size)     /* nothing */
	#define FWD_DECLARE_WALL(name, size) /* nothing */
	#define INIT_WALL(name)              do {} while (0)
	#define CHECK_WALL(name)             do {} while (0)
#endif

/** \} */ // defgroup debug

#endif /* BERTOS_DEBUG_H */
