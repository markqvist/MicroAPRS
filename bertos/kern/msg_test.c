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
 * \brief Message test.
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
#include <cfg/compiler.h>

#include <kern/msg.h>
#include <kern/proc.h>
#include <kern/signal.h>

#include <mware/event.h>

#include <drv/timer.h>

/*
 * In the nightly build test, signals are disables, so this
 * code won't compile.
 * Since this code is used when we run "make check" it will be
 * compiled and therefor tested there.
 */
#if CONFIG_KERN_SIGNALS


// Global settings for the test.
#define MAX_GLOBAL_COUNT            11040
#define TEST_TIME_OUT_MS             5000
#define DELAY                           5

// Settings for the test message.
//Process 0
#define INC_PROC_T0                     1
#define DELAY_PROC_T0   INC_PROC_T0*DELAY
//Process 1
#define INC_PROC_T1                     3
#define DELAY_PROC_T1   INC_PROC_T1*DELAY
//Process 2
#define INC_PROC_T2                     5
#define DELAY_PROC_T2   INC_PROC_T2*DELAY
//Process 3
#define INC_PROC_T3                     7
#define DELAY_PROC_T3   INC_PROC_T3*DELAY
//Process 4
#define INC_PROC_T4                    11
#define DELAY_PROC_T4   INC_PROC_T4*DELAY
//Process 5
#define INC_PROC_T5                    13
#define DELAY_PROC_T5   INC_PROC_T5*DELAY

/*
 * These macros generate the code needed to create the test process functions.
 */
#define RECV_PROC(num, sig) \
static NORETURN void receiver_proc##num(void) \
{ \
	TestMsg *rec_msg; \
	for(;;) \
	{ \
		sig_wait(sig); \
		kprintf("Proc[%d]..get message\n", num); \
		rec_msg = containerof(msg_get(&test_port##num), TestMsg, msg); \
		timer_delay(rec_msg->delay); \
		rec_msg->result += rec_msg->val; \
		kprintf("Proc[%d]..process message val[%d],delay[%d],res[%d]\n", num, rec_msg->val, rec_msg->delay, rec_msg->result); \
		msg_reply(&rec_msg->msg); \
		process_num++; \
		kprintf("Proc[%d] reply\n", num); \
	} \
}

#define SEND_MSG(num) \
	do { \
		kprintf("Main send message to proc[%d]\n", num); \
		msg##num.msg.replyPort = &test_portMain; \
		msg_put(&test_port##num, &msg##num.msg); \
	} while(0)

#define RECV_STACK(num) PROC_DEFINE_STACK(receiver_stack##num, KERN_MINSTACKSIZE * 2)
#define RECV_INIT_PROC(num) proc_new(receiver_proc##num, NULL, sizeof(receiver_stack##num), receiver_stack##num)
#define RECV_INIT_MSG(num, proc, sig) msg_initPort(&test_port##num, event_createSignal(proc, sig))

// A test message with the parameters and a result.
typedef struct
{
	Msg msg;

	int val;
	int delay;
	int result;
} TestMsg;

// Global count to check if the test is going ok.
static int count = 0;
static int process_num;

// Our message port.
static MsgPort test_port0;
static MsgPort test_port1;
static MsgPort test_port2;
static MsgPort test_port3;
static MsgPort test_port4;
static MsgPort test_port5;

/*
 * Generate the process to test message.
 */
RECV_PROC(0, SIG_USER0)
RECV_PROC(1, SIG_USER1)
RECV_PROC(2, SIG_USER2)
RECV_PROC(3, SIG_USER3)
RECV_PROC(4, SIG_SYSTEM5)
RECV_PROC(5, SIG_SYSTEM6)
/*
 * These signal are already use from
 * main process and the sig_waitWithTimeout functions, so we don't
 * use it.
 *
 * RECV_PROC(6, SIG_SINGLE)
 * RECV_PROC(7, SIG_TIMEOUT)
 */

RECV_STACK(0);
RECV_STACK(1);
RECV_STACK(2);
RECV_STACK(3);
RECV_STACK(4);
RECV_STACK(5);

/*
 * Help function to fill the message to send
 */
static void fill_msg(TestMsg *msg, int val, int delay, int res)
{
	msg->val = val;
	msg->delay = delay;
	msg->result = res;
}

/**
 * Run signal test
 */
int msg_testRun(void)
{
	MsgPort test_portMain;
	TestMsg msg0;
	TestMsg msg1;
	TestMsg msg2;
	TestMsg msg3;
	TestMsg msg4;
	TestMsg msg5;
	TestMsg *reply;

	// Allocate and start the test process
    struct Process *recv0 = RECV_INIT_PROC(0);
    struct Process *recv1 = RECV_INIT_PROC(1);
    struct Process *recv2 = RECV_INIT_PROC(2);
    struct Process *recv3 = RECV_INIT_PROC(3);
    struct Process *recv4 = RECV_INIT_PROC(4);
    struct Process *recv5 = RECV_INIT_PROC(5);

	kprintf("Run Message test..\n");

	// Init port and message
    RECV_INIT_MSG(Main, proc_current(), SIG_SINGLE);
    RECV_INIT_MSG(0, recv0, SIG_USER0);
    RECV_INIT_MSG(1, recv1, SIG_USER1);
    RECV_INIT_MSG(2, recv2, SIG_USER2);
    RECV_INIT_MSG(3, recv3, SIG_USER3);
    RECV_INIT_MSG(4, recv4, SIG_SYSTEM5);
    RECV_INIT_MSG(5, recv5, SIG_SYSTEM6);

	// Fill-in first message and send it out.
	fill_msg(&msg0, INC_PROC_T0, DELAY_PROC_T0, 0);
	fill_msg(&msg1, INC_PROC_T1, DELAY_PROC_T1, 0);
	fill_msg(&msg2, INC_PROC_T2, DELAY_PROC_T2, 0);
	fill_msg(&msg3, INC_PROC_T3, DELAY_PROC_T3, 0);
	fill_msg(&msg4, INC_PROC_T4, DELAY_PROC_T4, 0);
	fill_msg(&msg5, INC_PROC_T5, DELAY_PROC_T5, 0);


	// Send and wait the message
	for (int i = 0; i < 23; ++i)
    {
		process_num = 0;
		SEND_MSG(0);
		SEND_MSG(1);
		SEND_MSG(2);
		SEND_MSG(3);
		SEND_MSG(4);
		SEND_MSG(5);
		while(1)
		{
			sigmask_t sigs = sig_waitTimeout(SIG_SINGLE, ms_to_ticks(TEST_TIME_OUT_MS));
			if (sigs & SIG_SINGLE)
			{
				// Wait for a reply...
				while ((reply = (TestMsg *)msg_get(&test_portMain)))
				{
					count += reply->result;
					kprintf("Main recv[%d] count[%d]\n", reply->result, count);
				}
			}
			
			if (process_num == 6)
				break;

			if (sigs & SIG_TIMEOUT)
			{
				kputs("Main: sig timeout\n");
				goto error;
			}
		}
    }

	if(count == MAX_GLOBAL_COUNT)
	{
		kprintf("Message test finished..ok!\n");
		return 0;
	}
	
error:
	kprintf("Message test finished..fail!\n");
	return -1;
}

int msg_testSetup(void)
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

int msg_testTearDown(void)
{
	kputs("TearDown Message test.\n");
	return 0;
}

TEST_MAIN(msg);

#endif /* CONFIG_KERN_SIGNALS */
