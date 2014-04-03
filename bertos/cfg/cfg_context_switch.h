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
 * \brief Configuration file for the context switch benchmark.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_CONTEXT_SWITCH_H
#define CFG_CONTEXT_SWITCH_H

/**
 * Use hp timer for the bechmark.
 *
 * $WIZ$ type = "boolean"
 * $WIZ$ module= "boolean"
 * $WIZ$ conditional_deps = "ser"
 */
#define CONFIG_USE_HP_TIMER       1


/**
 * Use board led for benchmark.
 *
 * $WIZ$ type = "boolean"
 */
#define CONFIG_USE_LED       1


/**
 * Debug console port.
 * $WIZ$ type = "int"; min = 0
 */
#define CONFIG_CTX_DEBUG_PORT 0

/**
 * Baudrate for the debug console.
 * $WIZ$ type = "int"; min = 300
 */
#define CONFIG_CTX_DEBUG_BAUDRATE  115200UL

#endif /* CFG_CONTEXT_SWITCH_H */
