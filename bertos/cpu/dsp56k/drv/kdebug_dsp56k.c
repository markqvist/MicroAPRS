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
 *
 * -->
 *
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#error Revise me!

/* Debugging go through the JTAG interface. The MSL library already
	implements the console I/O correctly. */
#include <stdio.h>
#define KDBG_WAIT_READY()         do { } while (0)
#define KDBG_WRITE_CHAR(c)        __put_char(c, stdout)
#define KDBG_MASK_IRQ(old)        do { (void)(old); } while (0)
#define KDBG_RESTORE_IRQ(old)     do { (void)(old); } while (0)
typedef uint8_t kdbg_irqsave_t; /* unused */
#if CONFIG_KDEBUG_PORT == 666
	#error BITBANG debug console missing for this platform

#define	kdbg_hw_init() do {} while (0) ///< Not needed
