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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Semaphore test.
 *
 * For testing priority inversion (avoidance) a set of processes
 * interacting among each others by means of two semaphores are
 * disturbed by an unrelated process, i.e., a process not using
 * any semaphore at all.
 *
 * In case of priority inversion, high priority processes
 * are affected (delayed!) by such process, even it has lower
 * priority, because of semaphores. On the other hand, when priority
 * inheritance is enabled, non interacting low priority processes
 * can't affect the execution of high priority ones.
 *
 * It all can be seen looking at the finishing time of the various
 * processes involved in sem_inv_test (logged).
 *
 * Notice that priority inheritance makes sense iff priorities
 * exist, so the whole test depends on CONFIG_KERN_PRI.
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Dario Faggioli <raistlin@linux.it>
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PRI" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PRI 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PRI_INHERIT" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PRI_INHERIT 1" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_sem.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SEMAPHORES" >> $cfgdir/cfg_sem.h
 * $test$: echo "#define CONFIG_KERN_SEMAPHORES 1" >> $cfgdir/cfg_sem.h
 */

#include <cfg/debug.h>
#include <cfg/test.h>

#include <kern/sem.h>
#include <kern/proc.h>
#include <kern/irq.h>

#include <drv/timer.h>

// Global settings for the serialization test.
#define MAX_GLOBAL_COUNT             1024
#define TEST_TIME_OUT_MS             6000
#define DELAY                           5

// Settings for the test processes (serialization test).
//Process 1
#define INC_PROC_T1                     1
#define DELAY_PROC_T1   INC_PROC_T1*DELAY
//Process 2
#define INC_PROC_T2                     3
#define DELAY_PROC_T2   INC_PROC_T2*DELAY
//Process 3
#define INC_PROC_T3                     5
#define DELAY_PROC_T3   INC_PROC_T3*DELAY
//Process 4
#define INC_PROC_T4                     7
#define DELAY_PROC_T4   INC_PROC_T4*DELAY
//Process 5
#define INC_PROC_T5                    11
#define DELAY_PROC_T5   INC_PROC_T5*DELAY
//Process 6
#define INC_PROC_T6                    13
#define DELAY_PROC_T6   INC_PROC_T6*DELAY
//Process 7
#define INC_PROC_T7                    17
#define DELAY_PROC_T7   INC_PROC_T7*DELAY
//Process 8
#define INC_PROC_T8                    19
#define DELAY_PROC_T8   INC_PROC_T8*DELAY

Semaphore sem;
unsigned int global_count = 0;

/*
 * These macros generate the code needed to create the test process functions.
 */
#define PROC_TEST(num) static void proc_semTest##num(void) \
{ \
	unsigned int local_count = 0; \
	\
	for (int i = 0; i < INC_PROC_T##num; ++i) \
	{ \
		sem_obtain(&sem); \
		kprintf("> test%d: Obtain semaphore.\n", num); \
		local_count = global_count; \
		kprintf("> test%d: Read global count [%d]\n", num, local_count); \
		timer_delay(DELAY_PROC_T##num); \
		local_count += INC_PROC_T##num; \
		global_count = local_count; \
		kprintf("> test%d: Update count g[%d] l[%d]\n", num, global_count, local_count); \
		sem_release(&sem); \
		kprintf("> test%d: Relase semaphore.\n", num); \
	} \
} \

#define PROC_TEST_STACK(num)  PROC_DEFINE_STACK(proc_sem_test##num##_stack, KERN_MINSTACKSIZE * 2)
#define PROC_TEST_INIT(num)   proc_new(proc_semTest##num, NULL, sizeof(proc_sem_test##num##_stack), proc_sem_test##num##_stack);

// Define processes for the serialization test.
PROC_TEST(1)
PROC_TEST(2)
PROC_TEST(3)
PROC_TEST(4)
PROC_TEST(5)
PROC_TEST(6)
PROC_TEST(7)
PROC_TEST(8)

#if CONFIG_KERN_PRI

// Global settings for the priority inversion test.
// 0.5 secs, enough for seeing the effects
#define BASETIME 500

Semaphore s1, s2;
unsigned int loops = 0;	// For counting iterations
int finishing_time[8];

typedef enum ProcType {NONE, S1, S2, S1S2} ProcType;
/*
 * Macros for the processes of the priority inversion test.
 */
#define PROC_INV_TEST(num) static void proc_semInvTest##num(void) \
{ \
	ProcType p_type = (ProcType)((ssize_t) proc_currentUserData()); \
	int mult = p_type == NONE ? 5 : 1; \
	unsigned int i, local_count = 0; \
	ticks_t start; \
	\
	kprintf("> test%d(%d): Start.\n", num, proc_current()->link.pri); \
	finishing_time[num-1] = timer_clock(); \
	\
	if (p_type == S1 || p_type == S1S2) { \
		kprintf("> test%d(prio=%d): Obtain %p..\n", num, \
				proc_current()->link.pri, &s1); \
		sem_obtain(&s1); \
		kprintf("> test%d(prio=%d): Obtained %p.\n", num, \
				proc_current()->link.pri, &s1); \
	} \
	if (p_type == S2 || p_type == S1S2) { \
		kprintf("> test%d(prio=%d): Obtain %p..\n", num, \
				proc_current()->link.pri, &s2); \
		sem_obtain(&s2); \
		kprintf("> test%d(prio=%d): Obtained %p.\n", num, \
				proc_current()->link.pri, &s2); \
	} \
	\
	start = timer_clock(); \
	for (i = 0; i < loops * mult && (((unsigned)timer_clock()-start) <= loops*mult); i++) { \
		local_count++; \
	} \
	\
	sem_obtain(&sem); \
	global_count += local_count; \
	kprintf("> test%d(prio=%d): global_count=%u..\n", num, \
			proc_current()->link.pri, global_count); \
	sem_release(&sem); \
	\
	if (p_type == S2 || p_type == S1S2) { \
		kprintf("> test%d(prio=%d): Release %p..\n", num, \
				proc_current()->link.pri, &s2); \
		sem_release(&s2); \
		kprintf("> test%d(prio=%d): %p Released.\n", num, \
				proc_current()->link.pri, &s2); \
	} \
	if (p_type == S1 || p_type == S1S2) { \
		kprintf("> test%d(prio=%d): Release %p..\n", num, \
				proc_current()->link.pri, &s1); \
		sem_release(&s1); \
		kprintf("> test%d(prio=%d): %p Released.\n", num, \
				proc_current()->link.pri, &s1); \
	} \
	\
	finishing_time[num-1] = timer_clock() - finishing_time[num-1]; \
	kprintf("> test%d(prio=%d): Exit.\n", num, proc_current()->link.pri); \
} \

#define PROC_INV_TEST_INIT(num, pri, type) \
do { \
	struct Process *p; \
	\
	timer_delay(10); \
	p = proc_new(proc_semInvTest##num, \
			((void*)type), sizeof(proc_sem_test##num##_stack), \
			proc_sem_test##num##_stack); \
	proc_setPri(p, pri); \
} while (0) \

// Define processes for the priority inversion test.
PROC_INV_TEST(1)
PROC_INV_TEST(2)
PROC_INV_TEST(3)
PROC_INV_TEST(4)
PROC_INV_TEST(5)
PROC_INV_TEST(6)
PROC_INV_TEST(7)
PROC_INV_TEST(8)

#endif /* CONFIG_KERN_PRI */

// Define process stacks for both of the tests.
PROC_TEST_STACK(1)
PROC_TEST_STACK(2)
PROC_TEST_STACK(3)
PROC_TEST_STACK(4)
PROC_TEST_STACK(5)
PROC_TEST_STACK(6)
PROC_TEST_STACK(7)
PROC_TEST_STACK(8)

static int sem_ser_test(void)
{
	ticks_t start_time = timer_clock();

	sem_init(&sem);
	global_count = 0;

	kprintf("Run semaphore serialization test..\n");

	// Initialize the processes.
	PROC_TEST_INIT(1)
	PROC_TEST_INIT(2)
	PROC_TEST_INIT(3)
	PROC_TEST_INIT(4)
	PROC_TEST_INIT(5)
	PROC_TEST_INIT(6)
	PROC_TEST_INIT(7)
	PROC_TEST_INIT(8)
	kputs("> Main: Processes created\n");

	/*
	 * Wait until all processes exit, if something goes wrong we return an
	 * error after timeout_ms.
	 */
	while((timer_clock() - start_time) < ms_to_ticks(TEST_TIME_OUT_MS))
	{
		if (sem_attempt(&sem))
		{
			kputs("> Main: Check if test has finished..\n");
			if(global_count == MAX_GLOBAL_COUNT)
			{
				kputs("> Main: Test Finished..Ok!\n");
				return 0;
			}
			sem_release(&sem);
			kputs("> Main: Test is still running..\n");
		}
		proc_yield();
	}

	kputs("Semaphore serialization test failed..\n");
	return -1;
}

#if CONFIG_KERN_PRI

static int sem_inv_test(void)
{
	int i, orig_pri = proc_current()->link.pri;
	ticks_t fake, start_time;

	sem_init(&sem);
	global_count = 0;
	loops = 0;

	sem_init(&s1);
	sem_init(&s2);

	kputs("> Main: calibration for the busy wait cycle..\n");
	proc_setPri(proc_current(), 10);

	fake = start_time = timer_clock();
	while ((fake - start_time) < ms_to_ticks(BASETIME)) {
		fake = timer_clock();
		loops++;
	}
	kprintf("> Main: calibration done, %dms equals to %u cycles!\n", BASETIME, loops);

	kputs("> Main: Run Priority Inversion test...\n");

	// Will take s2
	PROC_INV_TEST_INIT(1, 2, S2);

	// 2 will block on s2; 3 will take s2 and still block on s2
	PROC_INV_TEST_INIT(2, 3, S2);
	PROC_INV_TEST_INIT(3, 4, S1S2);

	// Will block on s1, nothing happens..
	PROC_INV_TEST_INIT(4, 5, S1);

	// No semaphore, without PI this will delay everyone!
	PROC_INV_TEST_INIT(5, 6, NONE);

	// Will block on s1 and boost
	PROC_INV_TEST_INIT(6, 7, S1);
	PROC_INV_TEST_INIT(7, 8, S1);
	PROC_INV_TEST_INIT(8, 9, S1);

	// All processes created, let them run.
	proc_setPri(proc_current(), orig_pri);
	while ((timer_clock() - start_time) < ms_to_ticks(TEST_TIME_OUT_MS*2)) {
		if (sem_attempt(&sem)) {
			if (global_count >= loops*7 + loops*5) {
				for (i = 0; i < 8; i++)
					kprintf("> Main: I-O latency of %d = %ldms\n", i+1, (long) ms_to_ticks(finishing_time[i]));
				kputs("> Main: Test Finished..Ok!\n");
				return 0;
			}
			sem_release(&sem);
		}
		proc_yield();
	}

	kputs("> Main: Priority Inversion Test failed..\n");
	return -1;
}

#else

void sem_inv_test(void)
{
}

#endif /* CONFIG_KERN_PRI */

/**
 * Run semaphore test
 */
int sem_testRun(void)
{
	/* Start tests */
	sem_ser_test();		// Serialization
	sem_inv_test();		// Priority Inversion

	return 0;
}

int sem_testSetup(void)
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

int sem_testTearDown(void)
{
	kputs("TearDown Semaphore test.\n");
	return 0;
}

TEST_MAIN(sem);
