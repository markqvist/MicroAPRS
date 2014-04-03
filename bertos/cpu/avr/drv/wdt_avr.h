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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Watchdog interface for AVR architecture.
 *
 * \note The avr-libc already provide an api to manage the watchdog on AVR architecture.
 * In avr-libc are also available several constants used to set the timeout value
 * (see documentation for more detail).
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#ifndef DRV_WDT_AVR_H
#define DRV_WDT_AVR_H

#include "cfg/cfg_wdt.h"

#include <cfg/compiler.h> // INLINE

#include <avr/io.h>
#include <avr/wdt.h>

/**
 * Reset the watchdog timer.
 *
 * This functions is already defind in avr-libc.
 */
// void wdt_reset(void)

/**
 * Start the watchdog timer that fire at the select
 * timeout.
 *
 * \param _timeout you can use the macro that are defineded in
 * avr/wdt.h.
 *
 * (from avr libc documentation)
 * WDTO_15MS
 * WDTO_30MS
 * WDTO_60MS
 * WDTO_120MS
 * WDTO_250MS
 * WDTO_500MS
 * WDTO_1S
 * WDTO_2S
 * WDTO_4S
 * WDTO_8S
 */
INLINE void wdt_start(uint32_t _timeout)
{
	uint8_t timeout = _timeout;

	wdt_enable(timeout);
}

/**
 * Stop watchdog timer.
 */
INLINE void wdt_stop(void)
{
	wdt_disable();
}


#endif /* DRV_WDT_AVR_H */
