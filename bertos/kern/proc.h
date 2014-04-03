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
 * Copyright 1999, 2000, 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \defgroup kern_proc Process (Threads) management
 * \ingroup kern
 * \{
 *
 * \brief BeRTOS Kernel core (Process scheduler).
 *
 * This is the core kernel module. It allows you to create new processes
 * (which are called \b threads in other systems) and set the priority of
 * each process.
 *
 * A process needs a work area (called \b stack) to run. To create a process,
 * you need to declare a stack area, then create the process.
 * You may also pass NULL for the stack area, if you have enabled kernel heap:
 * in this case the stack will be automatically allocated.
 *
 * Example:
 * \code
 * PROC_DEFINE_STACK(stack1, 200);
 *
 * void NORETURN proc1_run(void)
 * {
 *    while (1)
 *    {
 *       LOG_INFO("I'm alive!\n");
 *       timer_delay(1000);
 *    }
 * }
 *
 *
 * int main()
 * {
 *    Process *p1 = proc_new(proc1_run, NULL, stack1, sizeof(stack1));
 *    // here the process is already running
 *    proc_setPri(p1, 2);
 *    // ...
 * }
 * \endcode
 *
 * The Process struct must be regarded as an opaque data type, do not access
 * any of its members directly.
 *
 * The entry point function should be declared as NORETURN, because it will
 * remove a warning and enable compiler optimizations.
 *
 * You can temporarily disable preemption calling proc_forbid(); remember
 * to enable it again calling proc_permit().
 *
 * \note You should hardly need to manually release the CPU; however you
 *       can do it using the cpu_relax() function. It is illegal to release
 *       the CPU with preemption disabled.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "kernel"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_proc.h"
 * $WIZ$ module_depends = "switch_ctx"
 * $WIZ$ module_supports = "not atmega103"
 */

#ifndef KERN_PROC_H
#define KERN_PROC_H

#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"
#include "cfg/cfg_monitor.h"
#include "sem.h"

#include <struct/list.h> // Node, PriNode

#include <cfg/compiler.h>
#include <cfg/debug.h> // ASSERT()

#include <cpu/types.h> // cpu_stack_t
#include <cpu/frame.h> // CPU_SAVED_REGS_CNT

/* The following silents warnings on nightly tests. We need to regenerate
 * all the projects before this can be removed.
 */
#ifndef CONFIG_KERN_PRI_INHERIT
#define CONFIG_KERN_PRI_INHERIT 0
#endif

/*
 * WARNING: struct Process is considered private, so its definition can change any time
 * without notice. DO NOT RELY on any field defined here, use only the interface
 * functions below.
 *
 * You have been warned.
 */
typedef struct Process
{
#if CONFIG_KERN_PRI
	PriNode      link;        /**< Link Process into scheduler lists */
# if CONFIG_KERN_PRI_INHERIT
	PriNode      inh_link;    /**< Link Process into priority inheritance lists */
	List         inh_list;    /**< Priority inheritance list for this Process */
	Semaphore    *inh_blocked_by;  /**< Semaphore blocking this Process */
	int          orig_pri;    /**< Process priority without considering inheritance */
# endif
#else
	Node         link;        /**< Link Process into scheduler lists */
#endif
	cpu_stack_t  *stack;       /**< Per-process SP */
	iptr_t       user_data;   /**< Custom data passed to the process */

#if CONFIG_KERN_SIGNALS
	Signal       sig;
#endif

#if CONFIG_KERN_HEAP
	uint16_t     flags;       /**< Flags */
#endif

#if CONFIG_KERN_HEAP | CONFIG_KERN_MONITOR
	cpu_stack_t  *stack_base;  /**< Base of process stack */
	size_t       stack_size;  /**< Size of process stack */
#endif

	/* The actual process entry point */
	void (*user_entry)(void);

#if CONFIG_KERN_MONITOR
	struct ProcMonitor
	{
		Node        link;
		const char *name;
	} monitor;
#endif

} Process;

/**
 * Initialize the process subsystem (kernel).
 * It must be called before using any process related function.
 */
void proc_init(void);

struct Process *proc_new_with_name(const char *name, void (*entry)(void), iptr_t data, size_t stacksize, cpu_stack_t *stack);

#if !CONFIG_KERN_MONITOR
	/**
	 * Create a new named process and schedules it for execution.
	 *
	 * When defining the stacksize take into account that you may want at least:
	 * \li save all the registers for each nested function call;
	 * \li have memory for the struct Process, which is positioned at the bottom
	 * of the stack;
	 * \li have some memory for temporary variables inside called functions.
	 *
	 * The value given by KERN_MINSTACKSIZE is rather safe to use in the first place.
	 *
	 * \param entry Function that the process will execute.
	 * \param data Pointer to user data.
	 * \param size Length of the stack.
	 * \param stack Pointer to the memory area to be used as a stack.
	 *
	 * \return Process structure of new created process
	 *         if successful, NULL otherwise.
	 */
	#define proc_new(entry,data,size,stack) proc_new_with_name(NULL,(entry),(data),(size),(stack))
#else
	#define proc_new(entry,data,size,stack) proc_new_with_name(#entry,(entry),(data),(size),(stack))
#endif

/**
 * Terminate the execution of the current process.
 */
void proc_exit(void);

/*
 * Public scheduling class methods.
 */
void proc_yield(void);

#if CONFIG_KERN_PREEMPT
bool proc_needPreempt(void);
void proc_preempt(void);
#else
INLINE bool proc_needPreempt(void)
{
	return false;
}

INLINE void proc_preempt(void)
{
}
#endif

void proc_rename(struct Process *proc, const char *name);
const char *proc_name(struct Process *proc);
const char *proc_currentName(void);

/**
 * Return a pointer to the user data of the current process.
 *
 * To obtain user data, just call this function inside the process. Remember to cast
 * the returned pointer to the correct type.
 * \return Pointer to the user data of the current process.
 */
INLINE iptr_t proc_currentUserData(void)
{
	extern struct Process *current_process;
	return current_process->user_data;
}

int proc_testSetup(void);
int proc_testRun(void);
int proc_testTearDown(void);

/**
 * Return the context structure of the currently running process.
 *
 * The details of the Process structure are private to the scheduler.
 * The address returned by this function is an opaque pointer that can
 * be passed as an argument to other process-related functions.
 */
INLINE struct Process *proc_current(void)
{
	extern struct Process *current_process;
	return current_process;
}

#if CONFIG_KERN_PRI
	void proc_setPri(struct Process *proc, int pri);
#else
	INLINE void proc_setPri(UNUSED_ARG(struct Process *,proc), UNUSED_ARG(int, pri))
	{
	}
#endif

#if CONFIG_KERN_PREEMPT

	/**
	 * Disable preemptive task switching.
	 *
	 * The scheduler maintains a global nesting counter.  Task switching is
	 * effectively re-enabled only when the number of calls to proc_permit()
	 * matches the number of calls to proc_forbid().
	 *
	 * \note Calling functions that could sleep while task switching is disabled
	 * is dangerous and unsupported.
	 *
	 * \note proc_permit() expands inline to 1-2 asm instructions, so it's a
	 * very efficient locking primitive in simple but performance-critical
	 * situations.  In all other cases, semaphores offer a more flexible and
	 * fine-grained locking primitive.
	 *
	 * \sa proc_permit()
	 */
	INLINE void proc_forbid(void)
	{
		extern cpu_atomic_t preempt_count;
		/*
		 * We don't need to protect the counter against other processes.
		 * The reason why is a bit subtle.
		 *
		 * If a process gets here, preempt_forbid_cnt can be either 0,
		 * or != 0.  In the latter case, preemption is already disabled
		 * and no concurrency issues can occur.
		 *
		 * In the former case, we could be preempted just after reading the
		 * value 0 from memory, and a concurrent process might, in fact,
		 * bump the value of preempt_forbid_cnt under our nose!
		 *
		 * BUT: if this ever happens, then we won't get another chance to
		 * run until the other process calls proc_permit() to re-enable
		 * preemption.  At this point, the value of preempt_forbid_cnt
		 * must be back to 0, and thus what we had originally read from
		 * memory happens to be valid.
		 *
		 * No matter how hard you think about it, and how complicated you
		 * make your scenario, the above holds true as long as
		 * "preempt_forbid_cnt != 0" means that no task switching is
		 * possible.
		 */
		++preempt_count;

		/*
		 * Make sure preempt_count is flushed to memory so the preemption
		 * softirq will see the correct value from now on.
		 */
		MEMORY_BARRIER;
	}

	/**
	 * Re-enable preemptive task switching.
	 *
	 * \sa proc_forbid()
	 */
	INLINE void proc_permit(void)
	{
		extern cpu_atomic_t preempt_count;

		/*
		 * This is to ensure any global state changed by the process gets
		 * flushed to memory before task switching is re-enabled.
		 */
		MEMORY_BARRIER;
		/* No need to protect against interrupts here. */
		ASSERT(preempt_count > 0);
		--preempt_count;
		/*
		 * This ensures preempt_count is flushed to memory immediately so the
		 * preemption interrupt sees the correct value.
		 */
		MEMORY_BARRIER;
	}

	/**
	 * \return true if preemptive task switching is allowed.
	 * \note This accessor is needed because preempt_count
	 *       must be absoultely private.
	 */
	INLINE bool proc_preemptAllowed(void)
	{
		extern cpu_atomic_t preempt_count;
		return (preempt_count == 0);
	}
#else /* CONFIG_KERN_PREEMPT */
	#define proc_forbid() /* NOP */
	#define proc_permit() /* NOP */
	#define proc_preemptAllowed() (true)
#endif /* CONFIG_KERN_PREEMPT */

/** Deprecated, use the proc_preemptAllowed() macro. */
#define proc_allowed() proc_preemptAllowed()

/**
 * Execute a block of \a CODE atomically with respect to task scheduling.
 */
#define PROC_ATOMIC(CODE) \
	do { \
		proc_forbid(); \
		CODE; \
		proc_permit(); \
	} while(0)

/**
 * Default stack size for each thread, in bytes.
 *
 * The goal here is to allow a minimal task to save all of its
 * registers twice, plus push a maximum of 32 variables on the
 * stack. We add also struct Process size since we save it into the process'
 * stack.
 *
 * The actual size computed by the default formula greatly depends on what
 * options are active and on the architecture.
 *
 * Note that on most 16bit architectures, interrupts will also
 * run on the stack of the currently running process.  Nested
 * interrupts will greatly increases the amount of stack space
 * required per process.  Use irqmanager to minimize stack
 * usage.
 */

#if (ARCH & ARCH_EMUL)
	/* We need a large stack because system libraries are bloated */
	#define KERN_MINSTACKSIZE 65536
#else
	#if CONFIG_KERN_PREEMPT
		/*
		 * A preemptible kernel needs a larger stack compared to the
		 * cooperative case. A task can be interrupted anytime in each
		 * node of the call graph, at any level of depth. This may
		 * result in a higher stack consumption, to call the ISR, save
		 * the current user context and to execute the kernel
		 * preemption routines implemented as ISR prologue and
		 * epilogue. All these calls are nested into the process stack.
		 *
		 * So, to reduce the risk of stack overflow/underflow problems
		 * add a x2 to the portion stack reserved to the user process.
		 */
		#define KERN_MINSTACKSIZE \
			(sizeof(Process) + CPU_SAVED_REGS_CNT * 2 * sizeof(cpu_stack_t) \
			+ 32 * sizeof(int) * 2)
	#else
		#define KERN_MINSTACKSIZE \
			(sizeof(Process) + CPU_SAVED_REGS_CNT * 2 * sizeof(cpu_stack_t) \
			+ 32 * sizeof(int))
	#endif /* CONFIG_KERN_PREEMPT */

#endif

#ifndef CONFIG_KERN_MINSTACKSIZE
	/* For backward compatibility */
	#define CONFIG_KERN_MINSTACKSIZE KERN_MINSTACKSIZE
#else
	#warning FIXME: This macro is deprecated, use KERN_MINSTACKSIZE instead
#endif

/**
 * Utility macro to allocate a stack of size \a size.
 *
 * This macro define a static stack for one process and do
 * check if given stack size is enough to run process.
 * \note If you plan to use kprintf() and similar functions, you will need
 * at least KERN_MINSTACKSIZE * 2 bytes.
 *
 * \param name Variable name for the stack.
 * \param size Stack size in bytes. It must be at least KERN_MINSTACKSIZE.
 */
#define PROC_DEFINE_STACK(name, size) \
	cpu_stack_t name[((size) + sizeof(cpu_stack_t) - 1) / sizeof(cpu_stack_t)]; \
	STATIC_ASSERT((size) >= KERN_MINSTACKSIZE);

/* Memory fill codes to help debugging */
#if CONFIG_KERN_MONITOR
	#include <cpu/types.h>
	#if (SIZEOF_CPUSTACK_T == 1)
		/* 8bit cpu_stack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5
		#define CONFIG_KERN_MEMFILLCODE    0xDB
	#elif (SIZEOF_CPUSTACK_T == 2)
		/* 16bit cpu_stack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5
		#define CONFIG_KERN_MEMFILLCODE    0xDBDB
	#elif (SIZEOF_CPUSTACK_T == 4)
		/* 32bit cpu_stack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5UL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBUL
	#elif (SIZEOF_CPUSTACK_T == 8)
		/* 64bit cpu_stack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5A5A5A5A5ULL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBDBDBDBDBULL
	#else
		#error No cpu_stack_t size supported!
	#endif
#endif
/** \} */ //defgroup kern_proc

#endif /* KERN_PROC_H */
