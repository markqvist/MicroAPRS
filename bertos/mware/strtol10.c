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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Poor man's hex arrays (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "strtol10.h"

/**
 * Convert a formatted base-10 ASCII number to unsigned long binary representation.
 *
 * Unlike the standard strtoul(), this function has an interface
 * that makes it better suited for protocol parsers.  It's also
 * much simpler and smaller than a full featured strtoul().
 *
 * \param first  Pointer to first byte of input range (STL-style).
 * \param last   Pointer to end of input range (STL-style).
 *               Pass NULL to parse up to the first \\0.
 * \param val    Pointer to converted value.
 *
 * \return true for success, false for failure.
 *
 * \see strtol10()
 */
bool strtoul10(const char *first, const char *last, unsigned long *val)
{
	// Check for no input
	if (*first == '\0')
		return false;

	*val = 0;
	for(/*nop*/; first != last && *first != '\0'; ++first)
	{
		if ((*first < '0') || (*first > '9'))
			return false;

		*val = (*val * 10L) + (*first - '0');
	}

	return true;
}


/**
 * Convert a formatted base-10 ASCII number to signed long binary representation.
 *
 * \see strtoul10()
 */
bool strtol10(const char *first, const char *last, long *val)
{
	bool negative = false;

	if (*first == '+')
		++first; /* skip unary plus sign */
	else if (*first == '-')
	{
		negative = true;
		++first;
	}

	bool result = strtoul10(first, last, (unsigned long *)val);

	if (negative)
		*val = - *val;

	return result;
}

