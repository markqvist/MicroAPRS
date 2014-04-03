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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Debug macros for inter-module dependency checking.
 *
 * These macros expand to nothing in release builds.  In debug
 * builds, they perform run-time dependency checks for modules.
 *
 * The usage pattern looks like this:
 *
 * \code
 * MOD_DEFINE(phaser)
 *
 * void phaser_init(void)
 * {
 *     MOD_CHECK(computer);
 *     MOD_CHECK(warp_core);
 *
 *    [...charge weapons...]
 *
 *    MOD_INIT(phaser);
 * }
 *
 * void phaser_cleanup(void)
 * {
 *    MOD_CLEANUP(phaser);
 *
 *    [...disarm phaser...]
 * }
 * \endcode
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef CFG_MODULE_H
#define CFG_MODULE_H

#include <cfg/debug.h>

/**
 * Declare a global variable for module dependency check.
 *
 * \see MOD_INIT(), MOD_CHECK()
 */
#define MOD_DEFINE(module)   DB(extern bool module ## _initialized; bool module ## _initialized;)

/**
 * Check that \a module was already initialized.
 *
 * Put this check just before accessing any facility
 * provided by a module that requires prior initialization.
 *
 * \see MOD_INIT()
 */

#define MOD_CHECK(module) \
do { \
	DB(extern bool module ## _initialized;) \
	ASSERT(module ## _initialized); \
} while (0)

/**
 * Mark module as initialized.
 *
 * Marking initialization requires the global data
 * previously defined by MOD_DEFINE().
 *
 * To prevent double initialization bugs, an initialized
 * module must first be cleaned up with MOD_CLEANUP() before
 * calling MOD_INIT() another time.
 *
 * \see MOD_CLEANUP(), MOD_CHECK(), MOD_DEFINE()
 */
#define MOD_INIT(module) \
do { \
	ASSERT(!module ## _initialized); \
	DB(module ## _initialized = true;) \
} while (0)

/**
 * Mark module as being no longer initialized.
 *
 * Marking initialization requires the global data
 * previously defined by MOD_DEFINE().
 *
 * \see MOD_INIT(), MOD_CHECK(), MOD_DEFINE()
 */
#define MOD_CLEANUP(module) \
do { \
	ASSERT(module ## _initialized); \
	DB(module ## _initialized = false;) \
} while (0)

#endif /* CFG_MODULE_H */

