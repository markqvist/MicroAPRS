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
 * Copyright 2005, 2006, 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Support for reading program memory on Harvard architectures.
 *
 * Support is currently provided for AVR microcontrollers only.
 *
 * These macros allow building code twice, with and without
 * pgm support (e.g.: strcpy() and strcpy_P()).
 *
 * Set the _PROGMEM predefine to compile in conditional
 * program-memory support.
 *
 *
 * \note This module contains code ripped out from avr-libc,
 *       which is distributed under a 3-clause BSD license.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef MWARE_PGM_H
#define MWARE_PGM_H

#include <cfg/compiler.h> /* For intXX_t */
#include <cpu/detect.h>
#include <cpu/attr.h>     /* For CPU_HARVARD */
#include <cpu/types.h>    /* For SIZEOF_INT */

#if CPU_AVR

	#ifdef __AVR_ENHANCED__
		#define pgm_read8(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint8_t __result; \
			__asm__ \
			( \
				"lpm %0, Z" "\n\t" \
				: "=r" (__result) \
				: "z" (__addr16) \
			); \
			__result; \
		})
		#define pgm_read16(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"lpm %A0, Z+"   "\n\t" \
				"lpm %B0, Z"    "\n\t" \
				: "=r" (__result), "=z" (__addr16) \
				: "1" (__addr16) \
			); \
			__result; \
		})


	#else /* !__AVR_ENHANCED__ */

		#define pgm_read8(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint8_t __result; \
			__asm__ \
			( \
				"lpm" "\n\t" \
				"mov %0, r0" "\n\t" \
				: "=r" (__result) \
				: "z" (__addr16) \
				: "r0" \
			); \
			__result; \
		})
		#define pgm_read16(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"lpm"           "\n\t" \
				"mov %A0, r0"   "\n\t" \
				"adiw r30, 1"   "\n\t" \
				"lpm"           "\n\t" \
				"mov %B0, r0"   "\n\t" \
				: "=r" (__result), "=z" (__addr16) \
				: "1" (__addr16) \
				: "r0" \
			); \
			__result; \
		})

	#endif /* !__AVR_ENHANCED__ */

	#define pgm_read32(addr)	((uint32_t)(pgm_read16(addr) | (((uint32_t)pgm_read16(((const uint8_t *)(addr)) + 2)) << 16)))
	#ifndef PROGMEM
	#define PROGMEM  __attribute__((__progmem__))
	#endif
	#ifndef PSTR
	#define PSTR(s) ({ static const char __c[] PROGMEM = (s); &__c[0]; })
	#endif
	#ifndef PFUNC
	#define PFUNC(x)      x ## _P
	#endif

#elif CPU_HARVARD
	#error Missing CPU support
#endif


#if !CPU_HARVARD
	#define pgm_read8(a)     (*(const uint8_t  *)(a))
	#define pgm_read16(a)    (*(const uint16_t *)(a))
	#define pgm_read32(a)    (*(const uint32_t *)(a))
#endif

#define pgm_read_char(a)        pgm_read8(a)
#define pgm_read_uint16_t(addr) pgm_read16(addr)


#if SIZEOF_INT == 2
	#define pgm_read_int(addr) ((int)pgm_read16(addr))
#elif SIZEOF_INT == 4
	#define pgm_read_int(addr) ((int)pgm_read32(addr))
#else
	#error Missing support for CPU word size!
#endif

#ifndef PSTR
#define PSTR            /* nothing */
#endif

#ifndef PFUNC
#define PFUNC(x) x
#endif

#ifndef PROGMEM
#define PROGMEM         /* nothing */
#endif

/**
 * \name Types for variables stored in program memory (harvard processors).
 * \{
 */
typedef PROGMEM char pgm_char;
typedef PROGMEM int8_t pgm_int8_t;
typedef PROGMEM uint8_t pgm_uint8_t;
typedef PROGMEM int16_t pgm_int16_t;
typedef PROGMEM uint16_t pgm_uint16_t;
typedef PROGMEM int32_t pgm_int32_t;
typedef PROGMEM uint32_t pgm_uint32_t;
/*\}*/

/**
 * \name PGM support macros.
 *
 * These macros enable dual compilation of code for both program
 * and data memory.
 *
 * Such a function may be defined like this:
 *
 * \code
 *	void PGM_FUNC(lcd_puts)(PGM_ATTR const char *str)
 *	{
 *		char c;
 *		while ((c = PGM_READ_CHAR(str++))
 *			lcd_putchar(c);
 *	}
 * \endcode
 *
 * The above code can be compiled twice: once with the _PROGMEM preprocessor
 * symbol defined, and once without.  The two object modules can then be
 * linked in the same application for use by client code:
 *
 * \code
 *	lcd_puts("Hello, world!");
 *	lcd_puts_P(PSTR("Hello, world!"));
 *
 *	// To be used when invoking inside other PGM_FUNC functions:
 *	PGM_FUNC(lcd_puts)(some_string);
 * \endcode
 *
 * \{
 */
#ifdef _PROGMEM
	#define PGM_READ8(a)     pgm_read8(a)
	#define PGM_READ16(a)    pgm_read16(a)
	#define PGM_READ32(a)    pgm_read32(a)
	#define PGM_FUNC(x)      PFUNC(x)
	#define PGM_STR(x)       PSTR(x)
	#define PGM_ATTR         PROGMEM
#else
	#define PGM_READ8(a)     (*(const uint8_t  *)(a))
	#define PGM_READ16(a)    (*(const uint16_t *)(a))
	#define PGM_READ32(a)    (*(const uint32_t *)(a))
	#define PGM_FUNC(x)      x
	#define PGM_STR(x)       x
	#define PGM_ATTR         /* nothing */
#endif

#define PGM_READ_CHAR(addr)      PGM_READ8(addr)

/* \} */


#endif /* MWARE_PGM_H */
