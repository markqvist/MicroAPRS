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
 * Copyright 2003, 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "cfg/cfg_debug.h"
#include <cfg/macros.h> /* for BV() */
#include <cfg/debug.h>
#include <cfg/os.h>

#include <cpu/attr.h>
#include <cpu/types.h>

#include <mware/formatwr.h> /* for _formatted_write() */
#include <cpu/pgm.h>

#ifdef _DEBUG

#if CPU_HARVARD && !defined(_PROGMEM)
	#error This module build correctly only in program memory!
#endif


#if OS_HOSTED
	#include <unistd.h> // write()

	#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
	#define KDBG_WRITE_CHAR(c)     do { char __c = (c); write(STDERR_FILENO, &__c, sizeof(__c)); } while(0)
	#define KDBG_MASK_IRQ(old)     do { (void)(old); } while(0)
	#define KDBG_RESTORE_IRQ(old)  do { /*nop*/ } while(0)
	typedef char kdbg_irqsave_t; /* unused */

	#define	kdbg_hw_init() do {} while (0) ///< Not needed

	#if CONFIG_KDEBUG_PORT == 666
		#error BITBANG debug console missing for this platform
	#endif
#else
	#include CPU_CSOURCE(kdebug)
#endif


void kdbg_init(void)
{
	/* Init debug hw */
	kdbg_hw_init();
	kputs("\n\n*** BeRTOS DBG START ***\n");
}


/**
 * Output one character to the debug console
 */
static void __kputchar(char c, UNUSED_ARG(void *, unused))
{
	/* Poll while serial buffer is still busy */
	KDBG_WAIT_READY();

	/* Send '\n' as '\r\n' for dumb terminals */
	if (c == '\n')
	{
		KDBG_WRITE_CHAR('\r');
		KDBG_WAIT_READY();
	}

	KDBG_WRITE_CHAR(c);
}


void kputchar(char c)
{
	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	__kputchar(c, 0);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
}


static void PGM_FUNC(kvprintf)(const char * PGM_ATTR fmt, va_list ap)
{
#if CONFIG_PRINTF
	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	PGM_FUNC(_formatted_write)(fmt, __kputchar, 0, ap);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
#else
	/* A better than nothing printf() surrogate. */
	PGM_FUNC(kputs)(fmt);
#endif /* CONFIG_PRINTF */
}

void PGM_FUNC(kprintf)(const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	PGM_FUNC(kvprintf)(fmt, ap);
	va_end(ap);
}

void PGM_FUNC(kputs)(const char * PGM_ATTR str)
{
	char c;

	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	while ((c = PGM_READ_CHAR(str++)))
		__kputchar(c, 0);

	KDBG_RESTORE_IRQ(irqsave);
}


/**
 * Cheap function to print small integers without using printf().
 */
int kputnum(int num)
{
	int output_len = 0;
	int divisor = 10000;
	int digit;

	do
	{
		digit = num / divisor;
		num %= divisor;

		if (digit || output_len || divisor == 1)
		{
			kputchar(digit + '0');
			++output_len;
		}
	}
	while (divisor /= 10);

	return output_len;
}


static void klocation(const char * PGM_ATTR file, int line)
{
	PGM_FUNC(kputs)(file);
	kputchar(':');
	kputnum(line);
	PGM_FUNC(kputs)(PGM_STR(": "));
}

int PGM_FUNC(__bassert)(const char * PGM_ATTR cond, const char * PGM_ATTR file, int line)
{
	klocation(file, line);
	PGM_FUNC(kputs)(PGM_STR("Assertion failed: "));
	PGM_FUNC(kputs)(cond);
	kputchar('\n');
	BREAKPOINT;
	return 1;
}

/*
 * Unfortunately, there's no way to get __func__ in
 * program memory, so we waste quite a lot of RAM in
 * AVR and other Harvard processors.
 */
void PGM_FUNC(__trace)(const char *name)
{
	PGM_FUNC(kprintf)(PGM_STR("%s()\n"), name);
}

void PGM_FUNC(__tracemsg)(const char *name, const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	PGM_FUNC(kprintf)(PGM_STR("%s(): "), name);
	va_start(ap, fmt);
	PGM_FUNC(kvprintf)(fmt, ap);
	va_end(ap);
	kputchar('\n');
}

int PGM_FUNC(__invalid_ptr)(void *value, const char * PGM_ATTR name, const char * PGM_ATTR file, int line)
{
	klocation(file, line);
	PGM_FUNC(kputs)(PGM_STR("Invalid ptr: "));
	PGM_FUNC(kputs)(name);
	#if CONFIG_PRINTF
		PGM_FUNC(kprintf)(PGM_STR(" = 0x%p\n"), value);
	#else
		(void)value;
		kputchar('\n');
	#endif
	return 1;
}


void __init_wall(long *wall, int size)
{
	while(size--)
		*wall++ = WALL_VALUE;
}


int PGM_FUNC(__check_wall)(long *wall, int size, const char * PGM_ATTR name, const char * PGM_ATTR file, int line)
{
	int i, fail = 0;

	for (i = 0; i < size; i++)
	{
		if (wall[i] != WALL_VALUE)
		{
			klocation(file, line);
			PGM_FUNC(kputs)(PGM_STR("Wall broken: "));
			PGM_FUNC(kputs)(name);
			#if CONFIG_PRINTF
				PGM_FUNC(kprintf)(PGM_STR("[%d] (0x%p) = 0x%lx\n"), i, wall + i, wall[i]);
			#else
				kputchar('\n');
			#endif
			fail = 1;
		}
	}

	return fail;
}


#if CONFIG_PRINTF

/**
 * Dump binary data in hex
 */
void kdump(const void *_buf, size_t len)
{
	const unsigned char *buf = (const unsigned char *)_buf;

	kprintf("Dumping buffer at addr [%p], %zu bytes", buf, len);
	size_t i=0;
	while (len--)
	{
		if ((i++ % 16) == 0)
			kputs("\n");
		kprintf("%02X ", *buf++);
	}
	kputchar('\n');
}

#endif /* CONFIG_PRINTF */

#endif /* _DEBUG */
