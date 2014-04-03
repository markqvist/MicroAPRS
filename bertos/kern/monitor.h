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
 * \brief Monitor to check for stack overflows
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * $WIZ$ module_name = "monitor"
 * $WIZ$ module_depends = "kernel"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_monitor.h"
 */

#ifndef KERN_MONITOR_H
#define KERN_MONITOR_H

#include "cfg/cfg_monitor.h"

#include <cpu/types.h>

/**
 * Start the kernel monitor. It is a special process which checks every second the stacks of the
 * running processes trying to detect stack overflows.
 *
 * \param stacksize Size of stack in chars
 * \param stack Pointer to the stack that will be used by the monitor
 *
 * \note The stack is provided by the caller so that there is no wasted space if the monitor
 * is not used.
 */
void monitor_start(size_t stacksize, cpu_stack_t *stack);


/**
 * Manually check if a given stack has overflown. This is used to check for stacks
 * of processes handled externally form the kernel, or for other stacks (for instance
 * the interrupt supervisor stack).
 *
 * \note For this function to work, the stack must have been filled at startup with
 * CONFIG_KERN_STACKFILLCODE.
 */
size_t monitor_checkStack(cpu_stack_t *stack_base, size_t stack_size);


/** Print a report of the stack status through kdebug */
void monitor_report(void);

#endif /* KERN_MONITOR_H */
