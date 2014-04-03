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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \defgroup kern_signal Kernel signals
 * \ingroup kern
 * \{
 *
 * \brief Signal module for IPC.
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "signal"
 * $WIZ$ module_depends = "kernel", "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_signal.h"
 */

#ifndef KERN_SIGNAL_H
#define KERN_SIGNAL_H

#include <cfg/compiler.h>
#include <cfg/macros.h>    // BV()

#include <cpu/irq.h>

#include <kern/proc.h>

#if CONFIG_KERN_SIGNALS

INLINE sigmask_t __sig_checkSignal(Signal *s, sigmask_t sigs)
{
	sigmask_t result;

	result = s->recv & sigs;
	s->recv &= ~sigs;

	return result;
}

/**
 * Check if any of the signals in \a sigs has occurred and clear them.
 *
 * \return the signals that have occurred.
 */
INLINE sigmask_t sig_checkSignal(Signal *s, sigmask_t sigs)
{
	cpu_flags_t flags;
	sigmask_t result;

	IRQ_SAVE_DISABLE(flags);
	result = __sig_checkSignal(s, sigs);
	IRQ_RESTORE(flags);

	return result;
}

/**
 * Check if any of the signals in \a sigs has occurred and clear them.
 *
 * \return the signals that have occurred.
 */
INLINE sigmask_t sig_check(sigmask_t sigs)
{
	Process *proc = proc_current();
	return sig_checkSignal(&proc->sig, sigs);
}

void sig_sendSignal(Signal *s, Process *proc, sigmask_t sig);

/**
 * Send the signals \a sigs to the process \a proc and immeditaly dispatch it
 * for execution.
 *
 * The process will be awoken if it was waiting for any of them and immediately
 * dispatched for execution.
 *
 * \note This function can't be called from IRQ context, use sig_post()
 * instead.
 */
INLINE void sig_send(Process *proc, sigmask_t sig)
{
	sig_sendSignal(&proc->sig, proc, sig);
}

void sig_postSignal(Signal *s, Process *proc, sigmask_t sig);

/**
 * Send the signals \a sigs to the process \a proc.
 * The process will be awoken if it was waiting for any of them.
 *
 * \note This call is interrupt safe.
 */
INLINE void sig_post(Process *proc, sigmask_t sig)
{
	sig_postSignal(&proc->sig, proc, sig);
}

/*
 * XXX: this is provided for backword compatibility, consider to make this
 * deprecated for the future.
 */
INLINE void sig_signal(Process *proc, sigmask_t sig)
{
	sig_postSignal(&proc->sig, proc, sig);
}

sigmask_t sig_waitSignal(Signal *s, sigmask_t sigs);

/**
 * Sleep until any of the signals in \a sigs occurs.
 *
 * \return the signal(s) that have awoken the process.
 */
INLINE sigmask_t sig_wait(sigmask_t sigs)
{
	Process *proc = proc_current();
	return sig_waitSignal(&proc->sig, sigs);
}

sigmask_t sig_waitTimeoutSignal(Signal *s, sigmask_t sigs, ticks_t timeout,
				Hook func, iptr_t data);

/**
 * Sleep until any of the signals in \a sigs or \a timeout ticks elapse.
 * If the timeout elapse a SIG_TIMEOUT is added to the received signal(s).
 * \return the signal(s) that have awoken the process.
 * \note Caller must check return value to check which signal awoke the process.
 */
INLINE sigmask_t sig_waitTimeout(sigmask_t sigs, ticks_t timeout)
{
	Process *proc = proc_current();
	return sig_waitTimeoutSignal(&proc->sig, sigs, timeout,
			NULL, NULL);
}

#endif /* CONFIG_KERN_SIGNALS */

int signal_testRun(void);
int signal_testSetup(void);
int signal_testTearDown(void);

/**
 * \name Signal definitions
 * \{
 */
#define SIG_USER0    BV(0)  /**< Free for user usage */
#define SIG_USER1    BV(1)  /**< Free for user usage */
#define SIG_USER2    BV(2)  /**< Free for user usage */
#define SIG_USER3    BV(3)  /**< Free for user usage */
#define SIG_SINGLE   BV(4)  /**< Used to wait for a single event */
#define SIG_SYSTEM5  BV(5)  /**< Reserved for internal system use */
#define SIG_SYSTEM6  BV(6)  /**< Reserved for internal system use */
#define SIG_TIMEOUT  BV(7)  /**< Reserved for timeout use */

/**
 * Max number of signals that can be used by drivers or user applications.
 */
#define SIG_USER_MAX SIG_SINGLE
/*\}*/

/* \} */ //defgroup kern_signal

#endif /* KERN_SIGNAL_H */
