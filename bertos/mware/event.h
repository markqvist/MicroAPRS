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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001, 2003 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \defgroup event_handling Event handling module
 * \ingroup core
 * \{
 *
 * \brief Events handling
 *
 * This module implements a common system for executing
 * a user defined action calling a hook function.
 *
 *
 *  Device drivers often need to wait the completion of some event, usually to
 *  allow the hardware to accomplish some asynchronous task.
 *
 *  A common approach is to place a busy wait with a cpu_relax() loop that invokes
 *  the architecture-specific instructions to say that we're not doing much with
 *  the processor.
 *
 *  Although technically correct, the busy loop degrades the overall system
 *  performance in presence of multiple processes and power consumption.
 *
 *  With the kernel the natural way to implement such wait/complete mechanism is to
 *  use signals via sig_wait() and sig_post()/sig_send().
 *
 *  However, signals in BeRTOS are only available in presence of the kernel (that
 *  is just a compile-time option). This means that each device driver must provide
 *  two different interfaces to implement the wait/complete semantic: one with the
 *  kernel and another without the kernel.
 *
 *  The purpose of the completion events is to provide a generic interface to
 *  implement a synchronization mechanism to block the execution of code until a
 *  specific event happens.
 *
 *  This interface does not depend on the presence of the kernel and it
 *  automatically uses the appropriate event backend to provide the same
 *  behaviour with or without the kernel.
 *
 *  Example usage (wait for a generic device driver initialization):
 *  \code
 *  static Event e;
 *
 *  static void irq_handler(void)
 *  {
 *      // Completion event has happened, resume the execution of init()
 *      event_do(&e);
 *  }
 *
 *  static void init(void)
 *  {
 *      // Declare the generic completion event
 *      event_initGeneric(&e);
 *      // Submit the hardware initialization request
 *      async_hw_init();
 *      // Wait for the completion of the event
 *      event_wait(&e);
 *  }
 *  \endcode
 *
 * Example usage: wait multiple generic events via event_select()
 * \code
 * Event ev1;
 * Event ev2;
 *
 * void event_notifier(void)
 * {
 *      Event *evs[] = { &ev1, &ev2 };
 *
 *      event_initGeneric(&ev1);
 *      event_initGeneric(&ev2);
 *
 *      while (1)
 *      {
 *              int id = event_select(evs, countof(evs),
 *                                      ms_to_ticks(100));
 *              if (id < 0)
 *              {
 *                      kprintf("no IRQ\n");
 *                      continue;
 *              }
 *              kprintf("IRQ %d happened\n", id);
 *      }
 * }
 *
 * void irq1_handler(void)
 * {
 *      // do something
 *      ...
 *
 *      // notify the completion of event 1
 *      event_do(&ev1);
 * }
 *
 * void irq2_handler(void)
 * {
 *      // do something
 *      ...
 *
 *      // notify the completion of event 2
 *      event_do(&ev2);
 * }
 * \endcode
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "event"
 */

#ifndef KERN_EVENT_H
#define KERN_EVENT_H

#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"
#include "cfg/cfg_timer.h"
#include <cfg/compiler.h>

#include <cpu/power.h> /* cpu_relax() */

#if CONFIG_KERN && CONFIG_KERN_SIGNALS
#include <kern/signal.h>
/* Forward decl */
struct Process;
#endif

typedef struct Event
{
	void (*action)(struct Event *);
	union
	{
#if CONFIG_KERN && CONFIG_KERN_SIGNALS
		struct
		{
			struct Process *sig_proc;  /* Process to be signalled */
			sigbit_t        sig_bit;   /* Signal to send */
			Signal          sig;       /* Local signal structure (used by generic event) */
		} Sig;
#endif
		struct
		{
			Hook  func;         /* Pointer to softint hook */
			void *user_data;    /* Data to be passed back to user hook */
		} Int;

		struct
		{
			bool completed;             /* Generic event completion */
		} Gen;
	} Ev;
} Event;

void event_hook_ignore(Event *event);
void event_hook_signal(Event *event);
void event_hook_softint(Event *event);
void event_hook_generic(Event *event);
void event_hook_generic_signal(Event *event);

/** Initialize the event \a e as a no-op */
#define event_initNone(e) \
	((e)->action = event_hook_ignore)

/** Same as event_initNone(), but returns the initialized event */
INLINE Event event_createNone(void)
{
	Event e;
	e.action = event_hook_ignore;
	return e;
}

/** Initialize the event \a e with a software interrupt (call function \a f, with parameter \a u) */
#define event_initSoftint(e,f,u) \
	((e)->action = event_hook_softint,(e)->Ev.Int.func = (f), (e)->Ev.Int.user_data = (u))

/** Same as event_initSoftint(), but returns the initialized event */
INLINE Event event_createSoftint(Hook func, void *user_data)
{
	Event e;
	e.action = event_hook_softint;
	e.Ev.Int.func = func;
	e.Ev.Int.user_data = user_data;
	return e;
}

#if CONFIG_KERN && CONFIG_KERN_SIGNALS
/** Initialize the event \a e with a signal (send signal \a s to process \a p) */
#define event_initSignal(e,p,s) \
	((e)->action = event_hook_signal,(e)->Ev.Sig.sig_proc = (p), (e)->Ev.Sig.sig_bit = (s))

/** Same as event_initSignal(), but returns the initialized event */
INLINE Event event_createSignal(struct Process *proc, sigbit_t bit)
{
	Event e;
	e.action = event_hook_signal;
	e.Ev.Sig.sig_proc = proc;
	e.Ev.Sig.sig_bit = bit;
	return e;
}

/**
 * Signal used to implement generic events.
 */
#define EVENT_GENERIC_SIGNAL	SIG_SYSTEM6

/** Initialize the generic sleepable event \a e */
#define event_initGeneric(e)					\
	((e)->action = event_hook_generic_signal,		\
		(e)->Ev.Sig.sig_proc = proc_current(),		\
		(e)->Ev.Sig.sig_bit = EVENT_GENERIC_SIGNAL,	\
		(e)->Ev.Sig.sig.wait = 0, (e)->Ev.Sig.sig.recv = 0)
#else
#define event_initGeneric(e) \
	((e)->action = event_hook_generic, (e)->Ev.Gen.completed = false)
#endif

/**
 * Create a generic sleepable event.
 *
 * \return the properly initialized generic event structure.
 */
INLINE Event event_createGeneric(void)
{
	Event e;
	event_initGeneric(&e);
	return e;
}

/**
 * Wait the completion of event \a e.
 *
 * This function releases the CPU the application is configured to use
 * the kernel, otherwise it's just a busy wait.
 * \note It's forbidden to use this function inside irq handling functions.
 */
INLINE void event_wait(Event *e)
{
#if CONFIG_KERN_SIGNALS
	e->Ev.Sig.sig_proc = proc_current();
	sig_waitSignal(&e->Ev.Sig.sig, EVENT_GENERIC_SIGNAL);
#else
	while (ACCESS_SAFE(e->Ev.Gen.completed) == false)
		cpu_relax();
	e->Ev.Gen.completed = false;
	MEMORY_BARRIER;
#endif
}

/**
 * Wait for multiple events
 *
 * On success return the offset in the \a evs vector of the Event that
 * happened, -1 if the timeout expires.
 *
 * NOTE: timeout == 0 means no timeout.
 *
 * \attention The API is work in progress and may change in future versions.
 */
int event_select(Event **evs, int n, ticks_t timeout);

/**
 * Wait the completion of event \a e or \a timeout elapses.
 *
 * \note It's forbidden to use this function inside irq handling functions.
 */
bool event_waitTimeout(Event *e, ticks_t timeout);

/**
 * Trigger an event.
 *
 * Execute the callback function associated with event \a e.
 *
 * This function can be used also in interrupt routines, but only if the
 * event was created as a signal or generic event.
 */
INLINE void event_do(struct Event *e)
{
	e->action(e);
}

/** \} */

#endif /* KERN_EVENT_H */
