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
 * \brief IRQ management for the Cortex-M3 processor.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef DRV_CORTEX_M3_SYSIRQ_H
#define DRV_CORTEX_M3_SYSIRQ_H

#include <cfg/compiler.h>

#if CPU_CM3_LM3S
	#include <io/lm3s.h>
#elif CPU_CM3_STM32
	#include <io/stm32.h>
#elif CPU_CM3_SAM3
	#include <io/sam3.h>
/*#elif  Add other families here */
#else
	#error Unknown CPU
#endif

typedef void (*sysirq_handler_t)(void);
typedef unsigned int sysirq_t;

void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler);
void sysirq_setPriority(sysirq_t irq, int prio);
void sysirq_freeHandler(sysirq_t irq);

void sysirq_init(void);

#endif /* DRV_CORTEX_M3_SYSIRQ_H */
