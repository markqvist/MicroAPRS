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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for keyboard module.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_KBD_H
#define CFG_KBD_H

/// Keyboard polling method. $WIZ$ supports = "False"
#define CONFIG_KBD_POLL  KBD_POLL_SOFTINT

/// Enable keyboard event delivery to observers. $WIZ$ type = "boolean"
#define CONFIG_KBD_OBSERVER  0

/// Enable key beeps. $WIZ$ type = "boolean"
#define CONFIG_KBD_BEEP  0

/// Enable long pression handler for keys. $WIZ$ type = "boolean"
#define CONFIG_KBD_LONGPRESS  0

/// Enable calling poor man's scheduler to be called inside kbd_peek. $WIZ$ type = "boolean"
#define CONFIG_KBD_SCHED 0

#endif /* CFG_KBD_H */

