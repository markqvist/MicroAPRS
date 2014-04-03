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
 * \brief Simple preemptive multitasking scheduler.
 *
 * Preemption is explicitly regulated at the exit of each interrupt service
 * routine (ISR). Each task obtains a time quantum as soon as it is scheduled
 * on the CPU and its quantum is decremented at each clock tick. The frequency
 * of the timer determines the system tick granularity and CONFIG_KERN_QUANTUM
 * the time sharing interval.
 *
 * When the quantum expires the handler proc_needPreempt() checks if the
 * preemption is enabled and in this case proc_schedule() is called, that
 * possibly replaces the current running thread with a different one.
 *
 * The preemption can be disabled or enabled via proc_forbid() and
 * proc_permit() primitives. This is implemented using a global atomic counter.
 * When the counter is greater than 0 the task cannot be preempted; only when
 * the counter reaches 0 the task can be preempted again.
 *
 * Preemption-disabled sections may be nested. The preemption will be
 * re-enabled when the outermost preemption-disabled section completes.
 *
 * The voluntary preemption still happens via proc_switch() or proc_yield().
 * The first one assumes the current process has been already added to a
 * private wait queue (e.g., on a semaphore or a signal), while the second one
 * takes care of adding the process into the ready queue.
 *
 * Context switch is done by CPU-dependent support routines. In case of a
 * voluntary preemption the context switch routine must take care of
 * saving/restoring only the callee-save registers (the voluntary-preemption is
 * actually a function call). The kernel-preemption always happens inside a
 * signal/interrupt context and it must take care of saving all registers. For
 * this, in the entry point of each ISR the caller-save registers must be
 * saved. In the ISR exit point, if the context switch must happen, we switch
 * to user-context and call the same voluntary context switch routine that take
 * care of saving/restoring also the callee-save registers. On resume from the
 * switch, the interrupt exit point moves back to interrupt-context, resumes
 * the caller-save registers (saved in the ISR entry point) and return from the
 * interrupt-context.
 *
 * \note Thread priority (if enabled by CONFIG_KERN_PRI) defines the order in
 * the \p proc_ready_list and the capability to deschedule a running process. A
 * low-priority thread can't preempt a high-priority thread.
 *
 * A high-priority process can preempt a low-priority process immediately (it
 * will be descheduled and replaced in the interrupt exit point). Processes
 * running at the same priority can be descheduled when they expire the time
 * quantum.
 *
 * \note Sleeping while preemption is disabled fallbacks to a busy-wait sleep.
 * Voluntary preemption when preemption is disabled raises a kernel bug.
 *
 * -->
 *
 * \brief Simple cooperative and preemptive multitasking scheduler.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Andrea Righi <arighi@develer.com>
 */

#include "proc_p.h"
#include "proc.h"

#include "cfg/cfg_proc.h"
#define LOG_LEVEL KERN_LOG_LEVEL
#define LOG_FORMAT KERN_LOG_FORMAT
#include <cfg/log.h>

#include "cfg/cfg_monitor.h"
#include <cfg/macros.h>    // ROUND_UP2
#include <cfg/module.h>
#include <cfg/depend.h>    // CONFIG_DEPEND()

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/attr.h>
#include <cpu/frame.h>

#if CONFIG_KERN_HEAP
	#include <struct/heap.h>
#endif

#include <string.h>           /* memset() */

#define PROC_SIZE_WORDS (ROUND_UP2(sizeof(Process), sizeof(cpu_stack_t)) / sizeof(cpu_stack_t))

/*
 * The scheduer tracks ready processes by enqueuing them in the
 * ready list.
 *
 * \note Access to the list must occur while interrupts are disabled.
 */
REGISTER List proc_ready_list;

/*
 * Holds a pointer to the TCB of the currently running process.
 *
 * \note User applications should use proc_current() to retrieve this value.
 */
REGISTER Process *current_process;

/** The main process (the one that executes main()). */
static struct Process main_process;

#if CONFIG_KERN_HEAP

/**
 * Local heap dedicated to allocate the memory used by the processes.
 */
static HEAP_DEFINE_BUF(heap_buf, CONFIG_KERN_HEAP_SIZE);
static Heap proc_heap;

/*
 * Keep track of zombie processes (processes that are exiting and need to
 * release some resources).
 *
 * \note Access to the list must occur while kernel preemption is disabled.
 */
static List zombie_list;

#endif /* CONFIG_KERN_HEAP */

/*
 * Check if the process context switch can be performed directly by the
 * architecture-dependent asm_switch_context() or if it must be delayed
 * because we're in the middle of an ISR.
 *
 * Return true if asm_switch_context() can be executed, false
 * otherwise.
 *
 * NOTE: if an architecture does not implement IRQ_RUNNING() this function
 * always returns true.
 */
#define CONTEXT_SWITCH_FROM_ISR()	(!IRQ_RUNNING())

/*
 * Save context of old process and switch to new process.
  */
static void proc_context_switch(Process *next, Process *prev)
{
	cpu_stack_t *dummy;

	if (UNLIKELY(next == prev))
		return;
	/*
	 * If there is no old process, we save the old stack pointer into a
	 * dummy variable that we ignore.  In fact, this happens only when the
	 * old process has just exited.
	 */
	asm_switch_context(&next->stack, prev ? &prev->stack : &dummy);
}

static void proc_initStruct(Process *proc)
{
	/* Avoid warning for unused argument. */
	(void)proc;

#if CONFIG_KERN_SIGNALS
	proc->sig.recv = 0;
	proc->sig.wait = 0;
#endif

#if CONFIG_KERN_HEAP
	proc->flags = 0;
#endif

#if CONFIG_KERN_PRI
	proc->link.pri = 0;

# if CONFIG_KERN_PRI_INHERIT
	proc->orig_pri = proc->inh_link.pri = proc->link.pri;
	proc->inh_blocked_by = NULL;
	LIST_INIT(&proc->inh_list);
# endif
#endif
}

MOD_DEFINE(proc);

void proc_init(void)
{
	LIST_INIT(&proc_ready_list);

#if CONFIG_KERN_HEAP
	LIST_INIT(&zombie_list);
	heap_init(&proc_heap, heap_buf, sizeof(heap_buf));
#endif
	/*
	 * We "promote" the current context into a real process. The only thing we have
	 * to do is create a PCB and make it current. We don't need to setup the stack
	 * pointer because it will be written the first time we switch to another process.
	 */
	proc_initStruct(&main_process);
	current_process = &main_process;

#if CONFIG_KERN_MONITOR
	monitor_init();
	monitor_add(current_process, "main");
#endif
	MOD_INIT(proc);
}


#if CONFIG_KERN_HEAP

/**
 * Free all the resources of all zombie processes previously added to the zombie
 * list.
 */
static void proc_freeZombies(void)
{
	Process *proc;

	while (1)
	{
		PROC_ATOMIC(proc = (Process *)list_remHead(&zombie_list));
		if (proc == NULL)
			return;

		if (proc->flags & PF_FREESTACK)
		{
			PROC_ATOMIC(heap_freemem(&proc_heap, proc->stack_base,
				proc->stack_size + PROC_SIZE_WORDS * sizeof(cpu_stack_t)));
		}
	}
}

/**
 * Enqueue a process in the zombie list.
 */
static void proc_addZombie(Process *proc)
{
	Node *node;
#if CONFIG_KERN_PREEMPT
	ASSERT(!proc_preemptAllowed());
#endif

#if CONFIG_KERN_PRI
	node = &(proc)->link.link;
#else
	node = &(proc)->link;
#endif
	LIST_ASSERT_VALID(&zombie_list);
	ADDTAIL(&zombie_list, node);
}

#endif /* CONFIG_KERN_HEAP */

/**
 * Create a new process, starting at the provided entry point.
 *
 *
 * \note The function
 * \code
 * proc_new(entry, data, stacksize, stack)
 * \endcode
 * is a more convenient way to create a process, as you don't have to specify
 * the name.
 *
 * \return Process structure of new created process
 *         if successful, NULL otherwise.
 */
struct Process *proc_new_with_name(UNUSED_ARG(const char *, name), void (*entry)(void), iptr_t data, size_t stack_size, cpu_stack_t *stack_base)
{
	Process *proc;
	LOG_INFO("name=%s", name);
#if CONFIG_KERN_HEAP
	bool free_stack = false;

	/*
	 * Free up resources of a zombie process.
	 *
	 * We're implementing a kind of lazy garbage collector here for
	 * efficiency reasons: we can avoid to introduce overhead into another
	 * kernel task dedicated to free up resources (e.g., idle) and we're
	 * not introducing any overhead into the scheduler after a context
	 * switch (that would be *very* bad, because the scheduler runs with
	 * IRQ disabled).
	 *
	 * In this way we are able to release the memory of the zombie tasks
	 * without disabling IRQs and without introducing any significant
	 * overhead in any other kernel task.
	 */
	proc_freeZombies();

	/* Did the caller provide a stack for us? */
	if (!stack_base)
	{
		/* Did the caller specify the desired stack size? */
		if (!stack_size)
			stack_size = KERN_MINSTACKSIZE;

		/* Allocate stack dinamically */
		PROC_ATOMIC(stack_base =
			(cpu_stack_t *)heap_allocmem(&proc_heap, stack_size));
		if (stack_base == NULL)
			return NULL;

		free_stack = true;
	}

#else // CONFIG_KERN_HEAP

	/* Stack must have been provided by the user */
	ASSERT2(IS_VALID_PTR(stack_base), "Invalid stack pointer. Did you forget to \
		enable CONFIG_KERN_HEAP?");
	ASSERT2(stack_size, "Stack size cannot be 0.");

#endif // CONFIG_KERN_HEAP

#if CONFIG_KERN_MONITOR
	/*
	 * Fill-in the stack with a special marker to help debugging.
	 * On 64bit platforms, CONFIG_KERN_STACKFILLCODE is larger
	 * than an int, so the (int) cast is required to silence the
	 * warning for truncating its size.
	 */
	memset(stack_base, (int)CONFIG_KERN_STACKFILLCODE, stack_size);
#endif

	/* Initialize the process control block */
	if (CPU_STACK_GROWS_UPWARD)
	{
		proc = (Process *)stack_base;
		proc->stack = stack_base + PROC_SIZE_WORDS;
		// On some architecture stack should be aligned, so we do it.
		proc->stack = (cpu_stack_t *)((uintptr_t)proc->stack + (sizeof(cpu_aligned_stack_t) - ((uintptr_t)proc->stack % sizeof(cpu_aligned_stack_t))));
		if (CPU_SP_ON_EMPTY_SLOT)
			proc->stack++;
	}
	else
	{
		proc = (Process *)(stack_base + stack_size / sizeof(cpu_stack_t) - PROC_SIZE_WORDS);
		// On some architecture stack should be aligned, so we do it.
		proc->stack = (cpu_stack_t *)((uintptr_t)proc - ((uintptr_t)proc % sizeof(cpu_aligned_stack_t)));
		if (CPU_SP_ON_EMPTY_SLOT)
			proc->stack--;
	}
	/* Ensure stack is aligned */
	ASSERT((uintptr_t)proc->stack % sizeof(cpu_aligned_stack_t) == 0);

	stack_size -= PROC_SIZE_WORDS * sizeof(cpu_stack_t);
	proc_initStruct(proc);
	proc->user_data = data;

#if CONFIG_KERN_HEAP | CONFIG_KERN_MONITOR
	proc->stack_base = stack_base;
	proc->stack_size = stack_size;
	#if CONFIG_KERN_HEAP
	if (free_stack)
		proc->flags |= PF_FREESTACK;
	#endif
#endif
	proc->user_entry = entry;
	CPU_CREATE_NEW_STACK(proc->stack);

#if CONFIG_KERN_MONITOR
	monitor_add(proc, name);
#endif

	/* Add to ready list */
	ATOMIC(SCHED_ENQUEUE(proc));

	return proc;
}

/**
 * Return the name of the specified process.
 *
 * NULL is a legal argument and will return the name "<NULL>".
 */
const char *proc_name(struct Process *proc)
{
#if CONFIG_KERN_MONITOR
	return proc ? proc->monitor.name : "<NULL>";
#else
	(void)proc;
	return "---";
#endif
}

/// Return the name of the currently running process
const char *proc_currentName(void)
{
	return proc_name(proc_current());
}

/// Rename a process
void proc_rename(struct Process *proc, const char *name)
{
#if CONFIG_KERN_MONITOR
	monitor_rename(proc, name);
#else
	(void)proc; (void)name;
#endif
}


#if CONFIG_KERN_PRI
/**
 * Change the scheduling priority of a process.
 *
 * Process piorities are signed ints, whereas a larger integer value means
 * higher scheduling priority.  The default priority for new processes is 0.
 * The idle process runs with the lowest possible priority: INT_MIN.
 *
 * A process with a higher priority always preempts lower priority processes.
 * Processes of equal priority share the CPU time according to a simple
 * round-robin policy.
 *
 * As a general rule to maximize responsiveness, compute-bound processes
 * should be assigned negative priorities and tight, interactive processes
 * should be assigned positive priorities.
 *
 * To avoid interfering with system background activities such as input
 * processing, application processes should remain within the range -10
 * and +10.
 */
void proc_setPri(struct Process *proc, int pri)
{
#if CONFIG_KERN_PRI_INHERIT
	int new_pri;

	/*
	 * Whatever it will happen below, this is the new
	 * original priority of the process, i.e., the priority
	 * it has without taking inheritance under account.
	 */
	proc->orig_pri = pri;

	/* If not changing anything we can just leave */
	if ((new_pri = __prio_proc(proc)) == proc->link.pri)
		return;

	/*
	 * Actual process priority is the highest among its
	 * own priority and the one of the top-priority
	 * process that it is blocking (returned by
	 * __prio_proc()).
	 */
	proc->link.pri = new_pri;
#else
	if (proc->link.pri == pri)
		return;

	proc->link.pri = pri;
#endif // CONFIG_KERN_PRI_INHERIT

	if (proc != current_process)
		ATOMIC(sched_reenqueue(proc));
}
#endif // CONFIG_KERN_PRI

INLINE void proc_run(void)
{
	void (*entry)(void) = current_process->user_entry;

	LOG_INFO("New process starting at %p", entry);
	entry();
}

/**
 * Entry point for all the processes.
 */
void proc_entry(void)
{
	/*
	 * Return from a context switch assumes interrupts are disabled, so
	 * we need to explicitly re-enable them as soon as possible.
	 */
	IRQ_ENABLE;
	/* Call the actual process's entry point */
	proc_run();
	proc_exit();
}

/**
 * Terminate the current process
 */
void proc_exit(void)
{
	LOG_INFO("%p:%s", current_process, proc_currentName());

#if CONFIG_KERN_MONITOR
	monitor_remove(current_process);
#endif

	proc_forbid();
#if CONFIG_KERN_HEAP
	/*
	 * Set the task as zombie, its resources will be freed in proc_new() in
	 * a lazy way, when another process will be created.
	 */
	proc_addZombie(current_process);
#endif
	current_process = NULL;
	proc_permit();

	proc_switch();

	/* never reached */
	ASSERT(0);
}

/**
 * Call the scheduler and eventually replace the current running process.
 */
static void proc_schedule(void)
{
	Process *old_process = current_process;

	IRQ_ASSERT_DISABLED();

	/* Poll on the ready queue for the first ready process */
	LIST_ASSERT_VALID(&proc_ready_list);
	while (!(current_process = (struct Process *)list_remHead(&proc_ready_list)))
	{
		/*
		 * Make sure we physically reenable interrupts here, no matter what
		 * the current task status is. This is important because if we
		 * are idle-spinning, we must allow interrupts, otherwise no
		 * process will ever wake up.
		 *
		 * During idle-spinning, an interrupt can occur and it may
		 * modify \p proc_ready_list. To ensure that compiler reload this
		 * variable every while cycle we call CPU_MEMORY_BARRIER.
		 * The memory barrier ensure that all variables used in this context
		 * are reloaded.
		 * \todo If there was a way to write sig_wait() so that it does not
		 * disable interrupts while waiting, there would not be any
		 * reason to do this.
		 */
		IRQ_ENABLE;
		CPU_IDLE;
		MEMORY_BARRIER;
		IRQ_DISABLE;
	}
	if (CONTEXT_SWITCH_FROM_ISR())
		proc_context_switch(current_process, old_process);
	/* This RET resumes the execution on the new process */
	LOG_INFO("resuming %p:%s\n", current_process, proc_currentName());
}

#if CONFIG_KERN_PREEMPT
/* Global preemption nesting counter */
cpu_atomic_t preempt_count;

/*
 * The time sharing interval: when a process is scheduled on a CPU it gets an
 * amount of CONFIG_KERN_QUANTUM clock ticks. When these ticks expires and
 * preemption is enabled a new process is selected to run.
 */
int _proc_quantum;

/**
 * Check if we need to schedule another task
 */
bool proc_needPreempt(void)
{
	if (UNLIKELY(current_process == NULL))
		return false;
	if (!proc_preemptAllowed())
		return false;
	if (LIST_EMPTY(&proc_ready_list))
		return false;
	return preempt_quantum() ? prio_next() > prio_curr() :
			prio_next() >= prio_curr();
}

/**
 * Preempt the current task.
 */
void proc_preempt(void)
{
	IRQ_ASSERT_DISABLED();
	ASSERT(current_process);

	/* Perform the kernel preemption */
	LOG_INFO("preempting %p:%s\n", current_process, proc_currentName());
	/* We are inside a IRQ context, so ATOMIC is not needed here */
	SCHED_ENQUEUE(current_process);
	preempt_reset_quantum();
	proc_schedule();
}
#endif /* CONFIG_KERN_PREEMPT */

/* Immediately switch to a particular process */
static void proc_switchTo(Process *proc)
{
	Process *old_process = current_process;

	SCHED_ENQUEUE(current_process);
	preempt_reset_quantum();
	current_process = proc;
	proc_context_switch(current_process, old_process);
}

/**
 * Give the control of the CPU to another process.
 *
 * \note Assume the current process has been already added to a wait queue.
 *
 * \warning This should be considered an internal kernel function, even if it
 * is allowed, usage from application code is strongly discouraged.
 */
void proc_switch(void)
{
	ASSERT(proc_preemptAllowed());
	ATOMIC(
		preempt_reset_quantum();
		proc_schedule();
	);
}

/**
 * Immediately wakeup a process, dispatching it to the CPU.
 */
void proc_wakeup(Process *proc)
{
	ASSERT(proc_preemptAllowed());
	ASSERT(current_process);
	IRQ_ASSERT_DISABLED();

	if (prio_proc(proc) >= prio_curr())
		proc_switchTo(proc);
	else
		SCHED_ENQUEUE_HEAD(proc);
}

/**
 * Voluntarily release the CPU.
 */
void proc_yield(void)
{
	Process *proc;

	/*
	 * Voluntary preemption while preemption is disabled is considered
	 * illegal, as not very useful in practice.
	 *
	 * ASSERT if it happens.
	 */
	ASSERT(proc_preemptAllowed());
	IRQ_ASSERT_ENABLED();

	IRQ_DISABLE;
	proc = (struct Process *)list_remHead(&proc_ready_list);
	if (proc)
		proc_switchTo(proc);
	IRQ_ENABLE;
}
