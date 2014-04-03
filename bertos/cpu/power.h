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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief CPU power management functions
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef CPU_POWER_H
#define CPU_POWER_H

#include "cfg/cfg_proc.h"
#include "cfg/cfg_wdt.h"

#include <cfg/compiler.h>

#if CONFIG_KERN
	#include <kern/proc.h>
#endif

#if CONFIG_WATCHDOG
	#include <drv/wdt.h>
#endif

/**
 * Let the CPU rest in tight busy loops
 *
 * User code that sits in a busy loop should call cpu_relax() every
 * once in a while to perform system-dependent idle processing.
 *
 * Depending on the system configuration, this might perform different
 * actions:
 *
 *  - yield the CPU to other processes
 *  - reset the watchdog timer to avoid it from triggering
 *  - scale the CPU speed down to save power (unimplemented)
 *  - let the event loop of the emulator process a few events
 *
 * \see proc_yield() cpu_pause()
 */
INLINE void cpu_relax(void)
{
	MEMORY_BARRIER;
#if CONFIG_KERN
	if (proc_preemptAllowed())
		proc_yield();
#endif

#if CONFIG_WATCHDOG
	wdt_reset();
#endif
}

/**
 * Stop the processor until the next interrupt occurs.
 *
 * Pausing the CPU effectively reduces power usage, and should be used
 * whenever the program is idle waiting for the next event to occur.
 *
 * To avoid deadlocking, the caller should normally check for the
 * desired condition with interrupts disabled, and enter this function
 * while interrupts are still disabled:
 *
 * \code
 *     IRQ_DISABLE();
 *     while (!event_occurred)
 *         cpu_pause();
 *     IRQ_ENABLE();
 * \endcode
 *
 * \note Some implementations of cpu_pause() may return before any interrupt
 *       has occurred.  Calling code should take this possibility into account.
 *
 * \note This function is currently unimplemented
 *
 * \see cpu_relax() cpu_yield()
 */
INLINE void cpu_pause(void)
{
	//ASSERT_IRQ_DISABLED();
	//IRQ_ENABLE();
	cpu_relax();
	//IRQ_DISABLE();
}

/**
 * Safely call cpu_pause() until the COND predicate becomes true.
 */
#define CPU_PAUSE_ON(COND) ATOMIC(while (!(COND)) { cpu_pause(); })

#endif /* CPU_POWER_H */
