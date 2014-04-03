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
 * \brief Cortex-M3 IRQ management.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "irq_cm3.h"

#include <cfg/debug.h> /* ASSERT() */
#include <cfg/log.h> /* LOG_ERR() */
#include <cpu/irq.h>


#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment=0x400
static void (*irq_table[NUM_INTERRUPTS])(void);
#else
static void (*irq_table[NUM_INTERRUPTS])(void)
			__attribute__((section("vtable")));
#endif

/* Priority register / IRQ number table */
static const uint32_t nvic_prio_reg[] =
{
	/* System exception registers */
	0, NVIC_SYS_PRI1, NVIC_SYS_PRI2, NVIC_SYS_PRI3,

	/* External interrupts registers */
	NVIC_PRI0, NVIC_PRI1, NVIC_PRI2, NVIC_PRI3,
	NVIC_PRI4, NVIC_PRI5, NVIC_PRI6, NVIC_PRI7,
	NVIC_PRI8, NVIC_PRI9, NVIC_PRI10, NVIC_PRI11,
	NVIC_PRI12, NVIC_PRI13
};

/* Unhandled IRQ */
static NAKED NORETURN void unhandled_isr(void)
{
	register uint32_t reg;

#ifdef __IAR_SYSTEMS_ICC__
	reg = CPU_READ_IPSR();
#else
	asm volatile ("mrs %0, ipsr" : "=r"(reg));
#endif
	LOG_ERR("unhandled IRQ %lu\n", reg);
	while (1)
		PAUSE;
}

void sysirq_setPriority(sysirq_t irq, int prio)
{
	uint32_t pos = (irq & 3) * 8;
	reg32_t reg = nvic_prio_reg[irq >> 2];
	uint32_t val;

	val = HWREG(reg);
	val &= ~(0xff << pos);
	val |= prio << pos;
	HWREG(reg) = val;
}

static void sysirq_enable(sysirq_t irq)
{
	/* Enable the IRQ line (only for generic IRQs) */
	if (irq >= 16 && irq < 48)
		NVIC_EN0_R = 1 << (irq - 16);
	else if (irq >= 48)
		NVIC_EN1_R = 1 << (irq - 48);
}

void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler)
{
	cpu_flags_t flags;

	ASSERT(irq < NUM_INTERRUPTS);

	IRQ_SAVE_DISABLE(flags);
	irq_table[irq] = handler;
	sysirq_setPriority(irq, IRQ_PRIO);
	sysirq_enable(irq);
	IRQ_RESTORE(flags);
}

void sysirq_freeHandler(sysirq_t irq)
{
	cpu_flags_t flags;

	ASSERT(irq < NUM_INTERRUPTS);

	IRQ_SAVE_DISABLE(flags);
	irq_table[irq] = unhandled_isr;
	IRQ_RESTORE(flags);
}

void sysirq_init(void)
{
	cpu_flags_t flags;
	int i;

	IRQ_SAVE_DISABLE(flags);
	for (i = 0; i < NUM_INTERRUPTS; i++)
		irq_table[i] = unhandled_isr;

	/* Update NVIC to point to the new vector table */
	NVIC_VTABLE_R = (size_t)irq_table;
	IRQ_RESTORE(flags);
}
