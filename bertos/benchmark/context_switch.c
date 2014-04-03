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
 * \brief Context switch benchmark
 *
 * \author Andrea Righi <arighi@develer.com>
 * \author Daniele Basiele <asterix@develer.com>
 */

#include "context_switch.h"

#include "hw/hw_led.h"

#include "cfg/cfg_context_switch.h"
#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <drv/timer.h>
#if CONFIG_USE_HP_TIMER
#include <drv/ser.h>
static Serial out;
#endif

#include <kern/proc.h>

#define PROC_STACK_SIZE	   KERN_MINSTACKSIZE

static PROC_DEFINE_STACK(hp_stack, PROC_STACK_SIZE);
static PROC_DEFINE_STACK(lp_stack, PROC_STACK_SIZE);

static Process *hp_proc, *lp_proc, *main_proc;
#if CONFIG_USE_HP_TIMER
static hptime_t start, end;
#endif

static void NORETURN hp_process(void)
{
	while (1)
	{
		sig_wait(SIG_USER0);
		#if CONFIG_USE_LED
			LED_ON();
		#endif
		#if CONFIG_USE_HP_TIMER
			end = timer_hw_hpread();
		#endif
		sig_send(main_proc, SIG_USER0);
	}
}

static void NORETURN lp_process(void)
{
	while (1)
	{
		sig_wait(SIG_USER0);
		#if CONFIG_USE_LED
			LED_ON();
			LED_OFF();
		#endif
		#if CONFIG_USE_HP_TIMER
			start = timer_hw_hpread();
		#endif
		sig_send(hp_proc, SIG_USER0);
	}
}


void NORETURN context_switch(void)
{
	IRQ_ENABLE;
	timer_init();
	proc_init();

	#if CONFIG_USE_HP_TIMER
		ser_init(&out, CONFIG_CTX_DEBUG_PORT);
		ser_setbaudrate(&out, CONFIG_CTX_DEBUG_BAUDRATE);
	#endif

	#if CONFIG_USE_LED
		LED_INIT();
	#endif

	proc_forbid();
	hp_proc = proc_new(hp_process, NULL, PROC_STACK_SIZE, hp_stack);
	lp_proc = proc_new(lp_process, NULL, PROC_STACK_SIZE, lp_stack);
	main_proc = proc_current();
	proc_setPri(hp_proc, 2);
	proc_setPri(lp_proc, 1);
	proc_permit();

	while (1)
	{
		timer_delay(100);

		sig_send(lp_proc, SIG_USER0);
		sig_wait(SIG_USER0);

		#if CONFIG_USE_HP_TIMER
			kfile_printf(&out.fd,
				"Switch: %lu.%lu usec\n\r",
				hptime_to_us((end - start)),
				hptime_to_us((end - start) * 1000) % 1000);
		#endif
	}
}
