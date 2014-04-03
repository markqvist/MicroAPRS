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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 *
 * \brief Test kernel preemption.
 *
 * This testcase spawns TASKS parallel threads that runs for TIME seconds. They
 * continuously spin updating a global counter (one counter for each thread).
 *
 * At exit each thread checks if the others have been che chance to update
 * their own counter. If not, it means the preemption didn't occur and the
 * testcase returns an error message.
 *
 * Otherwise, if all the threads have been able to update their own counter it
 * means preemption successfully occurs, since there is no active sleep inside
 * each thread's implementation.
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PRI" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PRI 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PREEMPT" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PREEMPT 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_HEAP" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_HEAP 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_HEAP_SIZE" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_HEAP_SIZE 2097152L" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_monitor.h $cfgdir/
 * $test$: sed -i "s/CONFIG_KERN_MONITOR 0/CONFIG_KERN_MONITOR 1/" $cfgdir/cfg_monitor.h
 * $test$: cp bertos/cfg/cfg_signal.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SIGNALS" >> $cfgdir/cfg_signal.h
 * $test$: echo "#define CONFIG_KERN_SIGNALS 1" >> $cfgdir/cfg_signal.h
 *
 */

#include <stdio.h> // sprintf
#include <string.h> // memset

#include <kern/proc.h>
#include <kern/irq.h>
#include <kern/monitor.h>

#include <drv/timer.h>
#include <cfg/test.h>
#include <cfg/cfg_proc.h>

enum
{
	TEST_OK = 1,
	TEST_FAIL = 2,
};

/* Number of tasks to spawn */
#define TASKS	8

static char name[TASKS][32];

static unsigned int done[TASKS];

static cpu_atomic_t barrier[TASKS];
static cpu_atomic_t main_barrier;

/* Base time delay for processes using timer_delay() */
#define DELAY	5

// Define process stacks for test.
#define WORKER_STACK_SIZE KERN_MINSTACKSIZE * 3

#if CONFIG_KERN_HEAP
#define WORKER_STACK(id)	 NULL
#else /* !CONFIG_KERN_HEAP */
static cpu_stack_t worker_stack[TASKS][(WORKER_STACK_SIZE +
			sizeof(cpu_stack_t) - 1) / sizeof(cpu_stack_t)];
#define WORKER_STACK(id)	 (&worker_stack[id][0])
#endif /* CONFIG_KERN_HEAP */

static int prime_numbers[] =
{
	1, 3, 5, 7, 11, 13, 17, 19,
	23, 29, 31, 37, 41, 43, 47, 53,
};
STATIC_ASSERT(TASKS <= countof(prime_numbers));

#if CONFIG_KERN_PREEMPT
/* Time to run each preemptible thread (in seconds) */
#define TIME	10

static unsigned long preempt_counter[TASKS];
static unsigned int preempt_done[TASKS];
#endif

static void cleanup(void)
{
#if CONFIG_KERN_PREEMPT
	// Clear shared data (this is needed when this testcase is embedded in
	// the demo application).
	memset(preempt_counter, 0, sizeof(preempt_counter));
	memset(preempt_done, 0, sizeof(preempt_done));
#endif /* CONFIG_KERN_PREEMPT */
	memset(done, 0, sizeof(done));
	memset(barrier, 0, sizeof(barrier));
	main_barrier = 0;
}

static void worker(void)
{
	ssize_t pid = (ssize_t)proc_currentUserData();
	long tot = prime_numbers[pid - 1];
	unsigned int my_count = 0;
	int i;

	barrier[pid - 1] = 1;
	/* Synchronize on the main barrier */
	while (!main_barrier)
		proc_yield();
	for (i = 0; i < tot; i++)
	{
		my_count++;
		PROC_ATOMIC(kprintf("> %s[%zd] running\n", __func__, pid));
		timer_delay(tot * DELAY);
	}
	done[pid - 1] = 1;
	PROC_ATOMIC(kprintf("> %s[%zd] completed\n", __func__, pid));
}

static int worker_test(void)
{
	ssize_t i;

	// Init the test processes
	cleanup();
	kputs("Run Proc test..\n");
	for (i = 0; i < TASKS; i++)
	{
		name[i][0] = '\0';
		snprintf(&name[i][0], sizeof(name[i]), "worker_%zd", i + 1);
		name[i][sizeof(name[i]) - 1] = '\0';
		proc_new_with_name(name[i], worker, (iptr_t)(i + 1),
				WORKER_STACK_SIZE, WORKER_STACK(i));
	}
	/* Synchronize on start */
	while (1)
	{
		for (i = 0; i < TASKS; i++)
			if (!barrier[i])
				break;
		if (i == TASKS)
			break;
		proc_yield();
	}
	main_barrier = 1;
	MEMORY_BARRIER;
	kputs("> Main: Processes started\n");
	while (1)
	{
		for (i = 0; i < TASKS; i++)
		{
			if (!done[i])
				break;
		}
		if (i == TASKS)
			break;
		monitor_report();
		timer_delay(93);
	}
	kputs("> Main: process test finished..ok!\n");
	return 0;
}

#if CONFIG_KERN_PREEMPT
static void preempt_worker(void)
{
	ssize_t pid = (ssize_t)proc_currentUserData();
	unsigned long *my_count = &preempt_counter[pid - 1];
	ticks_t start, stop;
	int i;

	barrier[pid - 1] = 1;
	/* Synchronize on the main barrier */
	while (!main_barrier)
		proc_yield();
	PROC_ATOMIC(kprintf("> %s[%zd] running\n", __func__, pid));
	start = timer_clock();
	stop  = ms_to_ticks(TIME * 1000);
	while (timer_clock() - start < stop)
	{
		IRQ_ASSERT_ENABLED();
		(*my_count)++;
		/* be sure to wrap to a value different than 0 */
		if (UNLIKELY(*my_count == (unsigned int)~0))
			*my_count = 1;
	}
	PROC_ATOMIC(kprintf("> %s[%zd] completed: (counter = %lu)\n",
				__func__, pid, *my_count));
	for (i = 0; i < TASKS; i++)
		if (!preempt_counter[i])
		{
			preempt_done[pid - 1] = TEST_FAIL;
			return;
		}
	preempt_done[pid - 1] = TEST_OK;
}

static int preempt_worker_test(void)
{
	unsigned long score = 0;
	ssize_t i;

	// Init the test processes
	cleanup();
	kputs("Run Preemption test..\n");
	for (i = 0; i < TASKS; i++)
	{
		name[i][0] = '\0';
		snprintf(&name[i][0], sizeof(name[i]),
				"preempt_worker_%zd", i + 1);
		name[i][sizeof(name[i]) - 1] = '\0';
		proc_new_with_name(name[i], preempt_worker, (iptr_t)(i + 1),
				WORKER_STACK_SIZE, WORKER_STACK(i));
	}
	kputs("> Main: Processes created\n");
	/* Synchronize on start */
	while (1)
	{
		for (i = 0; i < TASKS; i++)
			if (!barrier[i])
				break;
		if (i == TASKS)
			break;
		proc_yield();
	}
	/* Now all threads have been created, start them all */
	main_barrier = 1;
	MEMORY_BARRIER;
	kputs("> Main: Processes started\n");
	while (1)
	{
		for (i = 0; i < TASKS; i++)
		{
			if (!preempt_done[i])
				break;
			else if (preempt_done[i] == TEST_FAIL)
			{
				kputs("> Main: process test finished..fail!\n");
				return -1;
			}
		}
		if (i == TASKS)
			break;
		monitor_report();
		timer_delay(1000);
	}
	for (i = 0; i < TASKS; i++)
		score += preempt_counter[i];
	kputs("> Main: process test finished..ok!\n");
	kprintf("> Score: %lu\n", score);
	return 0;
}
#endif /* CONFIG_KERN_PREEMPT */

#if CONFIG_KERN_SIGNALS & CONFIG_KERN_PRI

// Define params to test priority
#define PROC_PRI_TEST(num) static void proc_pri_test##num(void) \
{ \
	struct Process *main_proc = (struct Process *) proc_currentUserData(); \
	kputs("> Process: " #num "\n"); \
	sig_send(main_proc, SIG_USER##num); \
}

// Default priority is 0
#define PROC_PRI_TEST_INIT(num, proc)					\
do {									\
	struct Process *p = proc_new(proc_pri_test##num, (proc),	\
					WORKER_STACK_SIZE,		\
					WORKER_STACK(num));		\
	proc_setPri(p, num + 1);					\
} while (0)

PROC_PRI_TEST(0)
PROC_PRI_TEST(1)
PROC_PRI_TEST(2)

static int prio_worker_test(void)
{
	struct Process *curr = proc_current();
	int orig_pri = curr->link.pri;
	int ret = 0;

	// test process priority
	// main process must have the higher priority to check signals received
	proc_setPri(proc_current(), 10);

	kputs("Run Priority test..\n");
	// the order in which the processes are created is important!
	PROC_PRI_TEST_INIT(0, curr);
	PROC_PRI_TEST_INIT(1, curr);
	PROC_PRI_TEST_INIT(2, curr);

	// signals must be: USER2, 1, 0 in order
	sigmask_t signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER2))
	{
		ret = -1;
		goto out;
	}
	signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER1))
	{
		ret = -1;
		goto out;
	}
	signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER0))
	{
		ret = -1;
		goto out;
	}
	// All processes must have quit by now, but just in case...
	signals = sig_waitTimeout(SIG_USER0 | SIG_USER1 | SIG_USER2, 200);
	if (signals & (SIG_USER0 | SIG_USER1 | SIG_USER2))
	{
		ret = -1;
		goto out;
	}
	if (signals & SIG_TIMEOUT)
	{
		kputs("Priority test successfull.\n");
	}
out:
	proc_setPri(proc_current(), orig_pri);
	if (ret != 0)
		kputs("Priority test failed.\n");
	return ret;
}
#endif /* CONFIG_KERN_SIGNALS & CONFIG_KERN_PRI */

/**
 * Process scheduling test
 */
int proc_testRun(void)
{
	/* Start tests */
	worker_test();
#if CONFIG_KERN_PREEMPT
	preempt_worker_test();
#endif /* CONFIG_KERN_PREEMPT */
#if CONFIG_KERN_SIGNALS & CONFIG_KERN_PRI
	prio_worker_test();
#endif /* CONFIG_KERN_SIGNALS & CONFIG_KERN_PRI */
	return 0;
}

int proc_testSetup(void)
{
	kdbg_init();

	kprintf("Init Timer..");
	timer_init();
	kprintf("Done.\n");

	kprintf("Init Process..");
	proc_init();
	kprintf("Done.\n");

	return 0;
}

int proc_testTearDown(void)
{
	kputs("TearDown Process test.\n");
	return 0;
}

TEST_MAIN(proc);
