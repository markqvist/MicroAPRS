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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief System irq handler for Atmel AT91 ARM7 processors (interface).
 */

#ifndef DRV_AT91_SYSIRQ_H
#define DRV_AT91_SYSIRQ_H

#include <cfg/compiler.h>

typedef void (* sysirq_handler_t)(void);   ///< Type for system irq handler.
typedef void (* sysirq_setEnable_t)(bool); ///< Type for system irq enable/disable function.

/**
 * Structure used to define a system interrupt source.
 */
typedef struct SysIrq
{
	bool enabled;                 ///< Getter for irq enable/disable state.
	sysirq_setEnable_t setEnable; ///< Setter for irq enable/disable state.
	sysirq_handler_t handler;     ///< IRQ handler.
} SysIrq;

/**
 * System IRQ ID list.
 */
typedef enum sysirq_t
{
	SYSIRQ_PIT, ///< Periodic Interval Timer
	/* TODO: add all system irqs */
	SYSIRQ_CNT
} sysirq_t;

void sysirq_init(void);
void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler);
void sysirq_setEnable(sysirq_t irq, bool enable);
bool sysirq_enabled(sysirq_t irq);

#endif /* ARCH_ARM_SYSIRQ_H */
