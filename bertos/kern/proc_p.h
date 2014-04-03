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
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Internal scheduler structures and definitions for processes.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef KERN_PROC_P_H
#define KERN_PROC_P_H

#include "cfg/cfg_proc.h"
#include "cfg/cfg_monitor.h"

#include <cfg/compiler.h>

#include <cpu/types.h>        /* for cpu_stack_t */
#include <cpu/irq.h>          // IRQ_ASSERT_DISABLED()

#include <kern/proc.h>   // struct Process

#ifndef asm_switch_context
/**
 * CPU dependent context switching routines.
 *
 * Saving and restoring the context on the stack is done by a CPU-dependent
 * support routine which usually needs to be written in assembly.
 */
EXTERN_C void asm_switch_context(cpu_stack_t **new_sp, cpu_stack_t **save_sp);
#endif

/**
 * \name Flags for Process.flags.
 * \{
 */
#define PF_FREESTACK  BV(0)  /**< Free the stack when process dies */
/*\}*/


/** Track running processes. */
extern REGISTER Process	*current_process;

/**
 * Track ready processes.
 *
 * Access to this list must be performed with interrupts disabled
 */
extern REGISTER List     proc_ready_list;

#if CONFIG_KERN_PRI
# if CONFIG_KERN_PRI_INHERIT
	#define __prio_orig(proc) (proc->orig_pri)
	#define __prio_inh(proc) (LIST_EMPTY(&(proc)->inh_list) ? INT_MIN : \
					((PriNode *)LIST_HEAD(&proc->inh_list))->pri)
	#define __prio_proc(proc) (__prio_inh(proc) > __prio_orig(proc) ? \
					__prio_inh(proc) : __prio_orig(proc))
# endif
	#define prio_next()	(LIST_EMPTY(&proc_ready_list) ? INT_MIN : \
					((PriNode *)LIST_HEAD(&proc_ready_list))->pri)
	#define prio_proc(proc)	(proc->link.pri)
	#define prio_curr()	prio_proc(current_process)

	#define SCHED_ENQUEUE_INTERNAL(proc) \
			LIST_ENQUEUE(&proc_ready_list, &(proc)->link)
	#define SCHED_ENQUEUE_HEAD_INTERNAL(proc) \
			LIST_ENQUEUE_HEAD(&proc_ready_list, &(proc)->link)
#else
	#define prio_next()	0
	#define prio_proc(proc)	0
	#define prio_curr()	0

	#define SCHED_ENQUEUE_INTERNAL(proc) ADDTAIL(&proc_ready_list, &(proc)->link)
	#define SCHED_ENQUEUE_HEAD_INTERNAL(proc) ADDHEAD(&proc_ready_list, &(proc)->link)
#endif

/**
 * Enqueue a process in the ready list.
 *
 * Always use this macro to instert a process in the ready list, as its
 * might vary to implement a different scheduling algorithms.
 *
 * \note Access to the scheduler ready list must be performed with
 *       interrupts disabled.
 */
#define SCHED_ENQUEUE(proc)  do { \
		IRQ_ASSERT_DISABLED(); \
		LIST_ASSERT_VALID(&proc_ready_list); \
		SCHED_ENQUEUE_INTERNAL(proc); \
	} while (0)

#define SCHED_ENQUEUE_HEAD(proc)  do { \
		IRQ_ASSERT_DISABLED(); \
		LIST_ASSERT_VALID(&proc_ready_list); \
		SCHED_ENQUEUE_HEAD_INTERNAL(proc); \
	} while (0)


#if CONFIG_KERN_PRI
/**
 * Changes the priority of an already enqueued process.
 *
 * Searches and removes the process from the ready list, then uses LIST_ENQUEUE(()
 * to insert again to fix priority.
 *
 * No action is performed for processes that aren't in the ready list, eg. in semaphore queues.
 *
 * \note Performance could be improved with a different implementation of priority list.
 */
INLINE void sched_reenqueue(struct Process *proc)
{
	IRQ_ASSERT_DISABLED();
	LIST_ASSERT_VALID(&proc_ready_list);
	Node *n;
	PriNode *pos = NULL;
	FOREACH_NODE(n, &proc_ready_list)
	{
		if (n == &proc->link.link)
		{
			pos = (PriNode *)n;
			break;
		}
	}

	// only remove and enqueue again if process is already in the ready list
	// otherwise leave it alone
	if (pos)
	{
		REMOVE(&proc->link.link);
		LIST_ENQUEUE(&proc_ready_list, &proc->link);
	}
}
#endif //CONFIG_KERN_PRI

/* Process trampoline */
void proc_entry(void);

/* Schedule another process *without* adding the current one to the ready list. */
void proc_switch(void);

/* Immediately schedule a particular process bypassing the scheduler. */
void proc_wakeup(Process *proc);

/* Initialize a scheduler class. */
void proc_schedInit(void);

#if CONFIG_KERN_MONITOR
	/** Initialize the monitor */
	void monitor_init(void);

	/** Register a process into the monitor */
	void monitor_add(Process *proc, const char *name);

	/** Unregister a process from the monitor */
	void monitor_remove(Process *proc);

	/** Rename a process */
	void monitor_rename(Process *proc, const char *name);
#endif /* CONFIG_KERN_MONITOR */

/*
 * Quantum related macros are used in the
 * timer module and must be empty when
 * kernel is disabled.
 */
#if (CONFIG_KERN && CONFIG_KERN_PREEMPT)
INLINE int preempt_quantum(void)
{
	extern int _proc_quantum;
	return _proc_quantum;
}

INLINE void proc_decQuantum(void)
{
	extern int _proc_quantum;
	if (_proc_quantum > 0)
		_proc_quantum--;
}

INLINE void preempt_reset_quantum(void)
{
	extern int _proc_quantum;
	_proc_quantum = CONFIG_KERN_QUANTUM;
}
#else /* !(CONFIG_KERN && CONFIG_KERN_PREEMPT) */
INLINE int preempt_quantum(void)
{
	return 0;
}

INLINE void proc_decQuantum(void)
{
}

INLINE void preempt_reset_quantum(void)
{
}
#endif /* (CONFIG_KERN && CONFIG_KERN_PREEMPT) */

#endif /* KERN_PROC_P_H */
