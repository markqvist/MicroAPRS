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
 * \brief System IRQ handler for Atmel AT91 ARM7 processors.
 *
 * In Atmel AT91 ARM7TDMI processors, there are various
 * peripheral interrupt sources.
 * In general, every source has its own interrupt vector, so it
 * is possible to assign a specific handler for each interrupt
 * independently.
 * However, there are a few sources called "system sources" that
 * share a common IRQ line and vector, called "system IRQ".
 * So a unique system IRQ dispatcher is implemented here.
 * This module also contains an interface to manage every source
 * independently. It is possible to assign to every system IRQ
 * a specific IRQ handler.
 *
 * \see sysirq_setHandler
 * \see sysirq_setEnable
 */

#include "sysirq_at91.h"
#include <io/arm.h>
#include <cpu/irq.h>
#include <cpu/types.h>
#include <cfg/module.h>
#include <cfg/macros.h>

/**
 * Enable/disable the Periodic Interrupt Timer
 * interrupt.
 */
INLINE void pit_setEnable(bool enable)
{
	if (enable)
		PIT_MR |= BV(PITIEN);
	else
		PIT_MR &= ~BV(PITIEN);
}

/**
 * Table containing all system irqs.
 */
static SysIrq sysirq_tab[] =
{
	/* PIT, Periodic Interval Timer (System timer)*/
	{
		.enabled = false,
		.setEnable = pit_setEnable,
		.handler = NULL,
	},
	/* TODO: add other system sources here */
};

STATIC_ASSERT(countof(sysirq_tab) == SYSIRQ_CNT);

/**
 * System IRQ dispatcher.
 * This is the entry point for all system IRQs in AT91.
 * This function checks for interrupt enable state of
 * various sources (system timer, etc..) and calls
 * the corresponding handler.
 *
 * \note On AT91SAM7, all system IRQs (timer included) are handled
 * by the sysirq_dispatcher, so we can't differentiate between
 * context-switch and non-context-switch ISR inside this
 * class of IRQs.
 */
static DECLARE_ISR_CONTEXT_SWITCH(sysirq_dispatcher)
{
	unsigned int i;

	for (i = 0; i < countof(sysirq_tab); i++)
	{
		if (sysirq_tab[i].enabled
		 && sysirq_tab[i].handler)
			sysirq_tab[i].handler();
	}

	/* Inform hw that we have served the IRQ */
	AIC_EOICR = 0;
}

#define SYSIRQ_PRIORITY 0 ///< default priority for system irqs.


MOD_DEFINE(sysirq);

/**
 * Init system IRQ handling.
 * \note all system interrupts are disabled.
 */
void sysirq_init(void)
{
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Disable all system interrupts */
	for (unsigned i = 0; i < countof(sysirq_tab); i++)
		sysirq_tab[i].setEnable(false);

	/* Set the vector. */
	AIC_SVR(SYSC_ID) = sysirq_dispatcher;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(SYSC_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED | SYSIRQ_PRIORITY;
	/* Clear pending interrupt */
	AIC_ICCR = BV(SYSC_ID);
	/* Enable the system IRQ */
	AIC_IECR = BV(SYSC_ID);

	IRQ_RESTORE(flags);
	MOD_INIT(sysirq);
}


/**
 * Helper function used to set handler for system IRQ \a irq.
 */
void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler)
{
	ASSERT(irq < SYSIRQ_CNT);
	sysirq_tab[irq].handler = handler;
}

/**
 * Helper function used to enable/disable system IRQ \a irq.
 */
void sysirq_setEnable(sysirq_t irq, bool enable)
{
	ASSERT(irq < SYSIRQ_CNT);

	sysirq_tab[irq].setEnable(enable);
	sysirq_tab[irq].enabled = enable;
}

/**
 * Helper function used to get system IRQ \a irq state.
 */
bool sysirq_enabled(sysirq_t irq)
{
	ASSERT(irq < SYSIRQ_CNT);

	return sysirq_tab[irq].enabled;
}
