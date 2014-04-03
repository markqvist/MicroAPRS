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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Events handling implementation
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */


#include "event.h"
#include "cfg/cfg_signal.h"
#include "cfg/cfg_timer.h"

#include <drv/timer.h> /* timer_clock() */

void event_hook_ignore(UNUSED_ARG(Event *, e))
{
}

void event_hook_softint(Event *e)
{
	e->Ev.Int.func(e->Ev.Int.user_data);
}

void event_hook_generic(Event *e)
{
	e->Ev.Gen.completed = true;
	MEMORY_BARRIER;
}

#if CONFIG_KERN && CONFIG_KERN_SIGNALS
void event_hook_signal(Event *e)
{
	sig_post((e)->Ev.Sig.sig_proc, (e)->Ev.Sig.sig_bit);
}

void event_hook_generic_signal(Event *e)
{
	sig_postSignal(&e->Ev.Sig.sig, e->Ev.Sig.sig_proc, e->Ev.Sig.sig_bit);
}

/*
 * Custom event hook to notify the completion of a event monitored via
 * event_select().
 */
static void event_hook_generic_multiple_signal(Event *e)
{
	sig_post(e->Ev.Sig.sig_proc, e->Ev.Sig.sig_bit);
}

/*
 * Custom timer hook to notify the timeout of a event_waitTimeout().
 */
static void event_hook_generic_timeout_signal(void *arg)
{
	Event *e = (Event *)arg;

	sig_postSignal(&e->Ev.Sig.sig, e->Ev.Sig.sig_proc, SIG_TIMEOUT);
}

/*
 * event_waitTimeout() slow path: this function put the current process to
 * sleep until the event is notified. The timeout is managed using the custom
 * timer hook event_hook_generic_timeout_signal(): if the timeout expires the
 * signal SIG_TIMEOUT is notified via sig_post() to the sigmask embedded in the
 * event.
 *
 * The custom timer hook is required because the default timer's behaviour is
 * to use the process's sigmask to notify the completion of an event, that is
 * not suitable for this case, because we're sleeping on the event's sigmask
 * instead.
 */
static NOINLINE bool event_waitTimeoutSlowPath(Event *e, ticks_t timeout)
{
	bool ret;

	e->Ev.Sig.sig_proc = proc_current();
	ret = (sig_waitTimeoutSignal(&e->Ev.Sig.sig,
			EVENT_GENERIC_SIGNAL, timeout,
			event_hook_generic_timeout_signal, e) & SIG_TIMEOUT) ?
			false : true;
	return ret;
}

bool event_waitTimeout(Event *e, ticks_t timeout)
{
	/*
	 * Fast path: check if the event already happened and return
	 * immediately in this case.
	 */
	if (sig_checkSignal(&e->Ev.Sig.sig,
			EVENT_GENERIC_SIGNAL) == EVENT_GENERIC_SIGNAL)
		return true;
	return event_waitTimeoutSlowPath(e, timeout);
}

/*
 * event_select() slow path: this function handles the case when any event was
 * not yet notified, so it takes care of making the current process to sleep on
 * the list of events, mapping them to a different signal bit and issuing a
 * call to sig_waitTimeout() using the process's sigmask.
 */
static NOINLINE int event_selectSlowPath(Event **evs, int n, ticks_t timeout)
{
	sigmask_t mask = (1 << n) - 1;
	int i;

	for (i = 0; i < n; i++)
	{
		Event *e = evs[i];

		/* Map each event to a distinct signal bit */
		e->Ev.Sig.sig_proc = proc_current();
		e->Ev.Sig.sig_bit = 1 << i;
		e->action = event_hook_generic_multiple_signal;
	}
	IRQ_ENABLE;

	mask = timeout ? sig_waitTimeout(mask, timeout) : sig_wait(mask);
	if (mask & SIG_TIMEOUT)
		return -1;
	return UINT8_LOG2(mask);
}

int event_select(Event **evs, int n, ticks_t timeout)
{
	int i;

	ASSERT(n <= SIG_USER_MAX);

	IRQ_DISABLE;
	/* Fast path: check if one of the event already happened */
	for (i = 0; i < n; i++)
	{
		Event *e = evs[i];

		if (__sig_checkSignal(&e->Ev.Sig.sig,
				EVENT_GENERIC_SIGNAL) == EVENT_GENERIC_SIGNAL)
		{
			IRQ_ENABLE;
			return i;
		}
	}
	/* Otherwise, fallback to the slow path */
	return event_selectSlowPath(evs, n, timeout);
}
#else /* !(CONFIG_KERN && CONFIG_KERN_SIGNALS) */
bool event_waitTimeout(Event *e, ticks_t timeout)
{
	ticks_t end = timer_clock() + timeout;
	bool ret;

	while ((ACCESS_SAFE(e->Ev.Gen.completed) == false) ||
			TIMER_AFTER(timer_clock(), end))
		cpu_relax();
	ret = e->Ev.Gen.completed;
	e->Ev.Gen.completed = false;
	MEMORY_BARRIER;

	return ret;
}

int event_select(Event **evs, int n, ticks_t timeout)
{
	ticks_t end = timer_clock() + timeout;
	int i;

	while (1)
	{
		for (i = 0; i < n; i++)
		{
			Event *e = evs[i];
			if (ACCESS_SAFE(e->Ev.Gen.completed) == true)
			{
				e->Ev.Gen.completed = false;
				MEMORY_BARRIER;
				return i;
			}
		}
		if (timeout && TIMER_AFTER(timer_clock(), end))
			break;
		cpu_relax();
	}
	return -1;
}
#endif /* CONFIG_KERN && CONFIG_KERN_SIGNALS */
