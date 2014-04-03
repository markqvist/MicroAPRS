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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Monitor to check for stack overflows
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */


#include "monitor.h"

#if CONFIG_KERN_MONITOR

#include "proc_p.h"
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <struct/list.h>

#include <drv/timer.h>

#include <kern/proc.h>

#include <cpu/frame.h> /* CPU_STACK_GROWS_UPWARD */

/* Access to this list must be protected against the scheduler */
static List MonitorProcs;

void monitor_init(void)
{
	LIST_INIT(&MonitorProcs);
}


void monitor_add(Process *proc, const char *name)
{
	proc->monitor.name = name;

	PROC_ATOMIC(ADDTAIL(&MonitorProcs, &proc->monitor.link));
}


void monitor_remove(Process *proc)
{
	PROC_ATOMIC(REMOVE(&proc->monitor.link));
}

void monitor_rename(Process *proc, const char *name)
{
	proc->monitor.name = name;
}

size_t monitor_checkStack(cpu_stack_t *stack_base, size_t stack_size)
{
	cpu_stack_t *beg;
	cpu_stack_t *cur;
	cpu_stack_t *end;
	int inc;
	size_t sp_free;


	beg = stack_base;
	end = stack_base + stack_size / sizeof(cpu_stack_t);
	inc = +1;

	if (CPU_STACK_GROWS_UPWARD)
	{
		SWAP(beg, end);
		inc = -1;
	}

	cur = beg;
	while (cur != end)
	{
		if (*cur != CONFIG_KERN_STACKFILLCODE)
			break;

		cur += inc;
	}

	sp_free = ABS(cur - beg) * sizeof(cpu_stack_t);
	return sp_free;
}


void monitor_report(void)
{
	Node *node;
	int i;

	proc_forbid();
	kprintf("%-9s%-9s%-9s%-9s%s\n", "TCB", "SPbase", "SPsize", "SPfree", "Name");
	for (i = 0; i < 56; i++)
		kputchar('-');
	kputchar('\n');

	FOREACH_NODE(node, &MonitorProcs)
	{
		Process *p = containerof(node, Process, monitor.link);
		size_t free = monitor_checkStack(p->stack_base, p->stack_size);
		kprintf("%-9p%-9p%-9zu%-9zu%s\n",
			p, p->stack_base, p->stack_size, free, p->monitor.name);
	}
	proc_permit();
}


static void NORETURN monitor(void)
{
	Node *node;

	for (;;)
	{
		proc_forbid();
		FOREACH_NODE(node, &MonitorProcs)
		{
			Process *p = containerof(node, Process, monitor.link);
			size_t free = monitor_checkStack(p->stack_base, p->stack_size);

			if (p->stack_base && free < 0x20)
				kprintf("MONITOR: Free stack of process '%s' is only %u chars\n",
						p->monitor.name, (unsigned int)free);
		}
		proc_permit();

		/* Give some rest to the system */
		timer_delay(500);
	}
}

void monitor_start(size_t stacksize, cpu_stack_t *stack)
{
	struct Process *p = proc_new(monitor, NULL, stacksize, stack);
	proc_setPri(p, -10);
}

#endif /* CONFIG_KERN_MONITOR */
