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
 * \brief Cortex-M3 architecture's entry point
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "cfg/cfg_proc.h" /* CONFIG_KERN_PREEMPT */
#include "switch_ctx_cm3.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <cpu/attr.h> /* PAUSE */
#include <cpu/irq.h> /* IRQ_DISABLE */
#include <cpu/types.h>

#include <drv/irq_cm3.h>
#include <drv/clock_cm3.h>

#include <kern/proc_p.h>

#include <io/cm3.h>

#ifndef __IAR_SYSTEMS_ICC__
extern size_t __text_end, __data_start, __data_end, __bss_start, __bss_end;
#endif

extern void __init2(void);

/* Architecture's entry point */
void __init2(void)
{
	/*
	 * The main application expects IRQs disabled.
	 */
	IRQ_DISABLE;

	/* Set the appropriate clocking configuration */
	clock_init();

	/* Initialize IRQ vector table in RAM */
	sysirq_init();

#if (CONFIG_KERN && CONFIG_KERN_PREEMPT)
	/*
	 * Voluntary context switch handler.
	 *
	 * This software interrupt can always be triggered and must be
	 * dispatched as soon as possible, thus we just disable IRQ priority
	 * for it.
	 */
	sysirq_setHandler(FAULT_SVCALL, svcall_handler);
	sysirq_setPriority(FAULT_SVCALL, IRQ_PRIO_MAX);
	/*
	 * Preemptible context switch handler
	 *
	 * The priority of this IRQ must be the lowest priority in the system
	 * in order to run last in the interrupt service routines' chain.
	 */
	sysirq_setHandler(FAULT_PENDSV, pendsv_handler);
	sysirq_setPriority(FAULT_PENDSV, IRQ_PRIO_MIN);
#endif
}
