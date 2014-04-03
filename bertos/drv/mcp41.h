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
 *
 * \brief MCP41xxx digital potentiomenter driver.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "mcp41"
 * $WIZ$ module_depends = "timer", "kfile"
 * $WIZ$ module_hw = "bertos/hw/hw_mcp41.h", "bertos/hw/hw_mcp41.c", "bertos/hw/mcp41_map.h"
 */

#ifndef DRV_MCP41_H
#define DRV_MCP41_H

#include "hw/mcp41_map.h"
#include <cfg/compiler.h>

#include <io/kfile.h>

#define MCP41_WRITE_DATA 0x11
#define MCP41_HW_MAX     255
#define MCP41_MAX        999

/**
 * Type for digital potentiometer resistance.
 */
typedef uint16_t mcp41_res_t;

void mcp41_setResistance(Mcp41Dev dev, mcp41_res_t res);
void mcp41_init(KFile *ch);

#endif /* DRV_MCP41_H */
