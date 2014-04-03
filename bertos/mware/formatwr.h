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
 *
 * \brief Basic "printf", "sprintf" and "fprintf" formatter.
 *
 * $WIZ$ module_name = "formatwr"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_formatwr.h"
 * $WIZ$ module_depends = "hex"
 * $WIZ$ module_harvard = "both"
 */

#ifndef MWARE_FORMATWR_H
#define MWARE_FORMATWR_H

#include "cfg/cfg_formatwr.h"

#include <cpu/attr.h>    /* CPU_HARVARD */

#include <stdarg.h>      /* va_list */

/**
 * \name _formatted_write() configuration
 * $WIZ$ printf_list = "PRINTF_DISABLED", "PRINTF_NOMODIFIERS", "PRINTF_REDUCED", "PRINTF_NOFLOAT", "PRINTF_FULL"
 * \{
 */
#define PRINTF_DISABLED    0
#define PRINTF_NOMODIFIERS 1
#define PRINTF_REDUCED     2
#define PRINTF_NOFLOAT     3
#define PRINTF_FULL        4
/* \} */

#ifndef CONFIG_PRINTF_RETURN_COUNT
	/** Enable/disable _formatted_write return value */
	#define CONFIG_PRINTF_RETURN_COUNT 1
#endif

int
_formatted_write(
	const char *format,
	void put_char_func(char c, void *user_data),
	void *user_data,
	va_list ap);

#if CPU_HARVARD
	#include <cpu/pgm.h>
	int _formatted_write_P(
		const char * PROGMEM format,
		void put_char_func(char c, void *user_data),
		void *user_data,
		va_list ap);
#endif /* CPU_HARVARD */

int sprintf_testSetup(void);
int sprintf_testRun(void);
int sprintf_testTearDown(void);

#endif /* MWARE_FORMATWR_H */

