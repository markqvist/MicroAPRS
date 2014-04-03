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
 * \brief Low-level timer driver (SysTick) for LM3S1968.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "timer_cm3.h"

#include <cfg/debug.h>

#include <cpu/irq.h>

#include <drv/irq_cm3.h>

INLINE void timer_hw_setPeriod(unsigned long period)
{
	ASSERT(period < (1 << 24));
	NVIC_ST_RELOAD_R = period - 1;
}

static void timer_hw_enable(void)
{
	NVIC_ST_CTRL_R |=
		NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN;
}

static void timer_hw_disable(void)
{
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN);
}

void timer_hw_init(void)
{
	timer_hw_setPeriod(CPU_FREQ / TIMER_TICKS_PER_SEC);
	sysirq_setHandler(FAULT_SYSTICK, timer_handler);
	timer_hw_enable();
}

void timer_hw_exit(void)
{
	timer_hw_disable();
	sysirq_freeHandler(FAULT_SYSTICK);
}
