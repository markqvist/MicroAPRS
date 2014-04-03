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
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Andrea Righi <arighi@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Kernel footprint benchmark.
 */

#include "kernel_footprint.h"

#include <cpu/irq.h>

#include <kern/proc.h>
#include <kern/signal.h>
#include <kern/msg.h>
#include <kern/sem.h>

MsgPort in_port;

static void init(void)
{
    IRQ_ENABLE;
    proc_init();
}

static PROC_DEFINE_STACK(proc1_stack, KERN_MINSTACKSIZE);

static NORETURN void proc1_main(void)
{
	int i;
	for(;;)
	{
		i++;
	}
}

void kernel_footprint(void)
{
	init();

	// generate code for process
	struct Process *p =
		proc_new(proc1_main, 0, sizeof(proc1_stack), proc1_stack);
	proc_setPri(p, 5);
	proc_yield();

	// generate code for msg
	Msg msg;
	msg_initPort(&in_port, event_createSignal(p, SIG_USER1));
	msg_put(&in_port, &msg);
	msg_peek(&in_port);
	Msg *msg_re = msg_get(&in_port);
	msg_reply(msg_re);

	// generate code for signals
	sig_send(p, SIG_USER0);

	// generate code for msg
	Semaphore sem;
	sem_init(&sem);
	sem_obtain(&sem);
	sem_release(&sem);

	sig_wait(SIG_USER0);
}
