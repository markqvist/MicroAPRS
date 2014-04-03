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
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Signals test.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_signal.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SIGNALS" >> $cfgdir/cfg_signal.h
 * $test$: echo "#define CONFIG_KERN_SIGNALS 1" >> $cfgdir/cfg_signal.h
 */

#include "cfg/cfg_timer.h"
#include <cfg/debug.h>
#include <cfg/test.h>

#include <kern/signal.h>
#include <kern/proc.h>
#include <kern/irq.h>

#include <drv/timer.h>

// Set mask with all signal that we want to test.
int test_signal[] = {
	SIG_USER0,
	SIG_USER1,
	SIG_USER2,
	SIG_USER3,
	SIG_TIMEOUT,
	SIG_SYSTEM5,
	SIG_SYSTEM6,
	SIG_SINGLE
};

// Current signal to send
int count = 0;

sigmask_t sig_to_master;
sigmask_t sig_to_slave;

/*
 * These macros generate the code needed to create the test process functions.
 */
#define PROC_TEST_SLAVE(index, signal) \
static void proc_signalTest##index(void) \
{ \
	kputs("> Slave [" #index "]: Wait signal [" #signal "]\n"); \
	sig_wait(signal); \
	kputs("> Slave [" #index "]: send signal [" #signal "]\n"); \
	sig_send(proc_currentUserData(), signal); \
}

#define MAIN_CHECK_SIGNAL(index, slave) \
	do { \
		kprintf("> Main: send signal [%d]\n", test_signal[index]); \
		sig_send(slave, test_signal[index]); \
		kprintf("> Main: wait signal [%d]\n", test_signal[index]); \
		sig_wait(test_signal[index]); \
		count++; \
	} while(0) \

#if CONFIG_KERN_HEAP

#define PROC_TEST_SLAVE_INIT(index, master_process) proc_new(proc_signalTest##index, master_process, KERN_MINSTACKSIZE * 2, NULL)

#else

#define PROC_TEST_SLAVE_STACK(index) PROC_DEFINE_STACK(proc_signal_test##index##_stack, KERN_MINSTACKSIZE * 2);
#define PROC_TEST_SLAVE_INIT(index, master_process) proc_new(proc_signalTest##index, master_process, sizeof(proc_signal_test##index##_stack), proc_signal_test##index##_stack)

PROC_TEST_SLAVE_STACK(0)
PROC_TEST_SLAVE_STACK(1)
PROC_TEST_SLAVE_STACK(2)
PROC_TEST_SLAVE_STACK(3)
PROC_TEST_SLAVE_STACK(4)
PROC_TEST_SLAVE_STACK(5)
PROC_TEST_SLAVE_STACK(6)
PROC_TEST_SLAVE_STACK(7)

#endif

// Generate the code for signal test.
PROC_TEST_SLAVE(0, SIG_USER0)
PROC_TEST_SLAVE(1, SIG_USER1)
PROC_TEST_SLAVE(2, SIG_USER2)
PROC_TEST_SLAVE(3, SIG_USER3)
PROC_TEST_SLAVE(4, SIG_TIMEOUT)
PROC_TEST_SLAVE(5, SIG_SYSTEM5)
PROC_TEST_SLAVE(6, SIG_SYSTEM6)
PROC_TEST_SLAVE(7, SIG_SINGLE)

/**
 * Run signal test
 */
int signal_testRun(void)
{
	struct Process *main_process = proc_current();
	struct Process *slave_0;
	struct Process *slave_1;
	struct Process *slave_2;
	struct Process *slave_3;
	struct Process *slave_4;
	struct Process *slave_5;
	struct Process *slave_6;
	struct Process *slave_7;

	kprintf("Run Signal test..\n");
	slave_0 = PROC_TEST_SLAVE_INIT(0, main_process);
	slave_1 = PROC_TEST_SLAVE_INIT(1, main_process);
	slave_2 = PROC_TEST_SLAVE_INIT(2, main_process);
	slave_3 = PROC_TEST_SLAVE_INIT(3, main_process);
	slave_4 = PROC_TEST_SLAVE_INIT(4, main_process);
	slave_5 = PROC_TEST_SLAVE_INIT(5, main_process);
	slave_6 = PROC_TEST_SLAVE_INIT(6, main_process);
	slave_7 = PROC_TEST_SLAVE_INIT(7, main_process);

	MAIN_CHECK_SIGNAL(0, slave_0);
	MAIN_CHECK_SIGNAL(1, slave_1);
	MAIN_CHECK_SIGNAL(2, slave_2);
	MAIN_CHECK_SIGNAL(3, slave_3);
	MAIN_CHECK_SIGNAL(4, slave_4);
	MAIN_CHECK_SIGNAL(5, slave_5);
	MAIN_CHECK_SIGNAL(6, slave_6);
	MAIN_CHECK_SIGNAL(7, slave_7);

	if(count == countof(test_signal))
	{
		kprintf("Signal test finished..ok!\n");
		return 0;
	}

	kprintf("Signal test finished..fail!\n");
	return -1;
}

int signal_testSetup(void)
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

int signal_testTearDown(void)
{
	kputs("TearDown Signal test.\n");
	return 0;
}

TEST_MAIN(signal);
