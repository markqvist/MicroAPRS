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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Linker script for Atmel SAM3N4 Cortex M3 processor.
 *
 */

/*
 * Define stack size here
 * Variable used in the cortex-m3_rom.ld file
 */
STACK_SIZE = 0x1000;

/*
 * Memory configuration for SAM3N4.
 */
MEMORY
{
        rom(rx) : org = 0x0, len = 256k
        ram(rwx) : org = 0x20000000, len = 24k
}

INCLUDE "bertos/cpu/cortex-m3/scripts/cortex-m3_rom.ld"

