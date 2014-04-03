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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2003 Bernie Innocenti
 *
 * -->
 *
 * \defgroup kbd Keyboard handling driver
 * \ingroup drivers
 * \{
 * \brief Keyboard driver.
 *
 * <b>Configuration file</b>: cfg_kbd.h
 *
 * <b>HAL files</b>: hw_kbd.h, kbd_map.h
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "kbd"
 * $WIZ$ module_depends = "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_kbd.h"
 * $WIZ$ module_hw = "bertos/hw/hw_kbd.h", "bertos/hw/kbd_map.h"
 */

#ifndef DRV_KBD_H
#define DRV_KBD_H

#include "hw/kbd_map.h"

#include "cfg/cfg_kbd.h" // CONFIG_KBD_OBSERVER
#include <cfg/compiler.h>

#include <struct/list.h>

/**
 * \name Keyboard polling modes.
 *
 * Define CONFIG_KBD_POLL to one of these.
 *
 * \{
 */
#define KBD_POLL_SOFTINT  1
/* \} */

/**
 * Keyboard handler descriptor
 */
typedef struct KbdHandler
{
	Node link;
	keymask_t (*hook)(keymask_t);   /**< Hook function */
	int8_t pri;                     /**< Priority in input queue */
	uint8_t flags;                  /**< See below for definitions */
} KbdHandler;

#define KHF_RAWKEYS	BV(0)           /**< Handler gets raw key events */


void kbd_init(void);
keymask_t kbd_peek(void);
keymask_t kbd_get(void);
keymask_t kbd_get_timeout(mtime_t timeout);
void kbd_addHandler(struct KbdHandler *handler);
void kbd_remHandler(struct KbdHandler *handler);
keymask_t kbd_setRepeatMask(keymask_t mask);

#if CONFIG_KBD_OBSERVER
	struct Subject;

	/** Subject structure for keyboard observers. */
	extern struct Subject kbd_subject;

	enum
	{
		/* Event for key presses. */
		KBD_EVENT_KEY = 0x100
	};
#endif

/** \} */ //defgroup kbd
#endif /* DRV_KBD_H */
