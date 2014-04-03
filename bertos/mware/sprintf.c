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
 * Copyright 2002, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief sprintf() implementation based on _formatted_write()
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "sprintf"
 * $WIZ$ module_depends = "formatwr"
 * $WIZ$ module_harvard = "both"
 */

#include <mware/formatwr.h>
#include <cpu/pgm.h>
#include <cfg/compiler.h>

#include <stdio.h>


static void __str_put_char(char c, void *ptr)
{
	/*
	 * This Does not work on Code Warrior. Hmm...
	 *	*(*((char **)ptr))++ = c;
	 */

	**((char **)ptr) = c;
	(*((char **)ptr))++;
}

static void __null_put_char(UNUSED_ARG(char, c), UNUSED_ARG(void *, ptr))
{
	/* nop */
}


int PGM_FUNC(vsprintf)(char *str, const char * PGM_ATTR fmt, va_list ap)
{
	int result;

	if (str)
	{
		result = PGM_FUNC(_formatted_write)(fmt, __str_put_char, &str, ap);

		/* Terminate string */
		*str = '\0';
	}
	else
		result = PGM_FUNC(_formatted_write)(fmt, __null_put_char, 0, ap);


	return result;
}


int PGM_FUNC(sprintf)(char *str, const char * fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = PGM_FUNC(vsprintf)(str, fmt, ap);
	va_end(ap);

	return result;
}

/**
 * State information for __sn_put_char()
 */
struct __sn_state
{
	char *str;
	size_t len;
};

/**
 * formatted_write() callback used [v]snprintf().
 */
static void __sn_put_char(char c, void *ptr)
{
	struct __sn_state *state = (struct __sn_state *)ptr;

	if (state->len)
	{
		--state->len;
		*state->str++ = c;
	}
}


int PGM_FUNC(vsnprintf)(char *str, size_t size, const char * PGM_ATTR fmt, va_list ap)
{
	int result = 0;

	/* Make room for traling '\0'. */
	if (size--)
	{
		if (str)
		{
			struct __sn_state state;
			state.str = str;
			state.len = size;

			result = PGM_FUNC(_formatted_write)(fmt, __sn_put_char, &state, ap);

			/* Terminate string. */
			*state.str = '\0';
		}
		else
			result = PGM_FUNC(_formatted_write)(fmt, __null_put_char, 0, ap);
	}

	return result;
}


int PGM_FUNC(snprintf)(char *str, size_t size, const char * fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = PGM_FUNC(vsnprintf)(str, size, fmt, ap);
	va_end(ap);

	return result;
}
