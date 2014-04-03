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
 * -->
 *
 * \brief MCP41 hardware-specific definitions
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_MCP41_H
#define HW_MCP41_H

#include "hw/mcp41_map.h"

#include <cfg/compiler.h>


#warning TODO:This is an example implementation, you must implement it!


INLINE void SET_MCP41_DDR(Mcp41Dev dev)
{
		/* Implement me! */
		//Warning: this funtions is like avr target name,
		//fix it to comply for all target.

		//Only for test remove when implement this function
		(void)dev;
}

INLINE void MCP41_ON(Mcp41Dev i)
{
		/* Implement me! */

		//Only for test remove when implement this function
		(void)i;
}

INLINE void MCP41_OFF(Mcp41Dev i)
{
		/* Implement me! */

		//Only for test remove when implement this function
		(void)i;
}

#endif /* HW_MCP41_H */
