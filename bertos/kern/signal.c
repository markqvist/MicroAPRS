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
 * \brief IPC signals implementation.
 *
 * Signals are a low-level IPC primitive.  A process receives a signal
 * when some external event has happened.  Like interrupt requests,
 * signals do not carry any additional information.  If processing a
 * specific event requires additional data, the process must obtain it
 * through some other mechanism.
 *
 * Despite the name, one shouldn't confuse these signals with POSIX
 * signals.  POSIX signals are usually executed synchronously, like
 * software interrupts.
 *
 * Signals are very low overhead.  Using them exclusively to wait
 * for multiple asynchronous events results in very simple dispatch
 * logic with low processor and resource usage.
 *
 * The "event" module is a higher-level interface that can optionally
 * deliver signals to processes.  Messages provide even higher-level
 * IPC services built on signals.  Semaphore arbitration is also
 * implemented using signals.
 *
 * In this implementation, each process has a limited set of signal
 * bits (usually 32) and can wait for multiple signals at the same
 * time using sig_wait().  Signals can also be polled using sig_check(),
 * but a process spinning on its signals usually defeats their purpose
 * of providing a multitasking-friendly infrastructure for event-driven
 * applications.
 *
 * Signals are like flags: they are either active or inactive.  After an
 * external event has delivered a particular signal, it remains raised until
 * the process acknowledges it using either sig_wait() or sig_check().
 * Counting signals is not a reliable way to count how many times a
 * particular event has occurred, because the same signal may be
 * delivered twice before the process can notice.
 *
 * Signals can be delivered synchronously via sig_send() or asynchronously via
 * sig_post().
 *
 * In the synchronous case the process is awakened if it was waiting for any
 * signal and immediately dispatched for execution via a direct context switch,
 * if its priority is greater than the running process.
 *
 * <pre>
 * - Synchronous-signal delivery:
 *
 *     [P1]____sig_send()____proc_wakeup()____[P2]
 * </pre>
 *
 * In the asynchronous case, the process is scheduled for execution as a
 * consequence of the delivery, but it will be dispatched by the scheduler as
 * usual, according to the scheduling policy.
 *
 * <pre>
 * - Asynchronous-signal delivery:
 *
 *     [P1]____sig_post()____[P1]____proc_schedule()____[P2]
 * </pre>
 *
 * In this way, any execution context, including an interrupt handler, can
 * deliver a signal to a process. However, synchronous signal delivery from a
 * non-sleepable context (like an interrupt handler) is forbidden in order to
 * avoid potential deadlock conditions. Instead, sig_post() can be used from
 * any context, expecially from interrupt context or when the preemption is
 * disabled.
 *
 * Multiple independent signals may be delivered at once with a single
 * invocation of sig_send() or sig_post(), although this is rarely useful.
 *
 * \section signal_allocation Signal Allocation
 *
 * There's no hardcoded mapping of specific events to signal bits.
 * The meaning of a particular signal bit is defined by an agreement
 * between the delivering entity and the receiving process.
 * For instance, a terminal driver may be designed to deliver
 * a signal bit called SIG_INT when it reads the CTRL-C sequence
 * from the keyboard, and a process may react to it by quitting.
 *
 * \section sig_single SIG_SINGLE
 *
 * The SIG_SINGLE bit is reserved as a convenient shortcut in those
 * simple scenarios where a process needs to wait on just one event
 * synchronously.  By using SIG_SINGLE, there's no need to allocate
 * a specific signal from the free pool.  The constraints for safely
 * accessing SIG_SINGLE are:
 *  - The process MUST sig_wait() exclusively on SIG_SINGLE
 *  - SIG_SIGNAL MUST NOT be left pending after use (sig_wait() will reset
 *	  it automatically)
 *  - Do not sleep between starting the asynchronous task that will fire
 *    SIG_SINGLE, and the call to  sig_wait().
 *  - Do not call system functions that may implicitly sleep, such as
 *    timer_delayTicks().
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "signal.h"

#include "cfg/cfg_timer.h"
#include <cfg/debug.h>
#include <cfg/depend.h>

#include <cpu/irq.h>
#include <kern/proc.h>
#include <kern/proc_p.h>


#if CONFIG_KERN_SIGNALS

// Check config dependencies
CONFIG_DEPEND(CONFIG_KERN_SIGNALS, CONFIG_KERN);

sigmask_t sig_waitSignal(Signal *s, sigmask_t sigs)
{
	sigmask_t result;

	/* Sleeping with IRQs disabled or preemption forbidden is illegal */
	IRQ_ASSERT_ENABLED();
	ASSERT(proc_preemptAllowed());

	/*
	 * This is subtle: there's a race condition where a concurrent process
	 * or an interrupt may call sig_send()/sig_post() to set a bit in
	 * Process.sig_recv just after we have checked for it, but before we've
	 * set Process.sig_wait to let them know we want to be awaken.
	 *
	 * In this case, we'd deadlock with the signal bit already set and the
	 * process never being reinserted into the ready list.
	 */
	IRQ_DISABLE;

	/* Loop until we get at least one of the signals */
	while (!(result = s->recv & sigs))
	{
		/*
		 * Tell "them" that we want to be awaken when any of these
		 * signals arrives.
		 */
		s->wait = sigs;

		/* Go to sleep and proc_switch() to another process. */
		proc_switch();
		/*
		 * When we come back here, the wait mask must have been
		 * cleared by someone through sig_send()/sig_post(), and at
		 * least one of the signals we were expecting must have been
		 * delivered to us.
		 */
		ASSERT(!s->wait);
		ASSERT(s->recv & sigs);
	}

	/* Signals found: clear them and return */
	s->recv &= ~sigs;

	IRQ_ENABLE;
	return result;
}

#if CONFIG_TIMER_EVENTS

#include <drv/timer.h>

sigmask_t sig_waitTimeoutSignal(Signal *s, sigmask_t sigs, ticks_t timeout,
				Hook func, iptr_t data)
{
	Timer t;
	sigmask_t res;
	cpu_flags_t flags;

	ASSERT(!sig_checkSignal(s, SIG_TIMEOUT));
	ASSERT(!(sigs & SIG_TIMEOUT));
	/* IRQ are needed to run timer */
	ASSERT(IRQ_ENABLED());

	if (func)
		timer_setSoftint(&t, func, data);
	else
		timer_set_event_signal(&t, proc_current(), SIG_TIMEOUT);
	timer_setDelay(&t, timeout);
	timer_add(&t);
	res = sig_waitSignal(s, SIG_TIMEOUT | sigs);

	IRQ_SAVE_DISABLE(flags);
	/* Remove timer if sigs occur before timer signal */
	if (!(res & SIG_TIMEOUT) && !sig_checkSignal(s, SIG_TIMEOUT))
		timer_abort(&t);
	IRQ_RESTORE(flags);
	return res;
}

#endif // CONFIG_TIMER_EVENTS

INLINE void __sig_signal(Signal *s, Process *proc, sigmask_t sigs, bool wakeup)
{
	cpu_flags_t flags;

	IRQ_SAVE_DISABLE(flags);

	/* Set the signals */
	s->recv |= sigs;

	/* Check if process needs to be awoken */
	if (s->recv & s->wait)
	{
		ASSERT(proc != current_process);

		s->wait = 0;
		if (wakeup)
			proc_wakeup(proc);
		else
			SCHED_ENQUEUE_HEAD(proc);
	}
	IRQ_RESTORE(flags);
}

void sig_sendSignal(Signal *s, Process *proc, sigmask_t sigs)
{
	ASSERT_USER_CONTEXT();
	IRQ_ASSERT_ENABLED();
	ASSERT(proc_preemptAllowed());

	__sig_signal(s, proc, sigs, true);
}

void sig_postSignal(Signal *s, Process *proc, sigmask_t sigs)
{
	__sig_signal(s, proc, sigs, false);
}

#endif /* CONFIG_KERN_SIGNALS */
