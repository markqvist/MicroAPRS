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
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Hardware independent timer driver (implementation)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "timer.h"
#include "hw/hw_timer.h"

#include "cfg/cfg_timer.h"
#include "cfg/cfg_wdt.h"
#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"
#include <cfg/os.h>
#include <cfg/debug.h>
#include <cfg/module.h>

#include <cpu/attr.h>
#include <cpu/types.h>
#include <cpu/irq.h>
#include <cpu/power.h> // cpu_relax()

#include <kern/proc_p.h> // proc_decQuantun()

/*
 * Include platform-specific binding code if we're hosted.
 * Try the CPU specific one for bare-metal environments.
 */
#if OS_HOSTED
	//#include OS_CSOURCE(timer)
	#include <emul/timer_posix.c>
#else
	#ifndef WIZ_AUTOGEN
		#warning Deprecated: now you should include timer_<cpu> directly in the makefile. Remove this line and the following once done.
		#include CPU_CSOURCE(timer)
	#endif
#endif

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_KERN) || ((CONFIG_KERN != 0) && CONFIG_KERN != 1)
	#error CONFIG_KERN must be set to either 0 or 1 in config.h
#endif
#if !defined(CONFIG_WATCHDOG) || ((CONFIG_WATCHDOG != 0) && CONFIG_WATCHDOG != 1)
	#error CONFIG_WATCHDOG must be set to either 0 or 1 in config.h
#endif

#if CONFIG_WATCHDOG
	#include <drv/wdt.h>
#endif

#if defined (CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	#include <kern/signal.h> /* sig_wait(), sig_check() */
	#include <kern/proc.h>   /* proc_current() */
	#include <cfg/macros.h>  /* BV() */
#endif


/**
 * \def CONFIG_TIMER_STROBE
 *
 * This is a debug facility that can be used to
 * monitor timer interrupt activity on an external pin.
 *
 * To use strobes, redefine the macros TIMER_STROBE_ON,
 * TIMER_STROBE_OFF and TIMER_STROBE_INIT and set
 * CONFIG_TIMER_STROBE to 1.
 */
#if !defined(CONFIG_TIMER_STROBE) || !CONFIG_TIMER_STROBE
	#define TIMER_STROBE_ON    do {/*nop*/} while(0)
	#define TIMER_STROBE_OFF   do {/*nop*/} while(0)
	#define TIMER_STROBE_INIT  do {/*nop*/} while(0)
#endif


/// Master system clock (1 tick accuracy)
volatile ticks_t _clock;


#if CONFIG_TIMER_EVENTS

/**
 * List of active asynchronous timers.
 */
REGISTER static List timers_queue;

/**
 * This function really does the job. It adds \a timer to \a queue.
 * \see timer_add for details.
 */
INLINE void timer_addToList(Timer *timer, List *queue)
{
	/* Inserting timers twice causes mayhem. */
	ASSERT(timer->magic != TIMER_MAGIC_ACTIVE);
	DB(timer->magic = TIMER_MAGIC_ACTIVE;)


	/* Calculate expiration time for this timer */
	timer->tick = _clock + timer->_delay;

	/*
	 * Search for the first node whose expiration time is
	 * greater than the timer we want to add.
	 */
	Timer *node = (Timer *)LIST_HEAD(queue);
	while (node->link.succ)
	{
		/*
		 * Stop just after the insertion point.
		 * (this fancy compare takes care of wrap-arounds).
		 */
		if (node->tick - timer->tick > 0)
			break;

		/* Go to next node */
		node = (Timer *)node->link.succ;
	}

	/* Enqueue timer request into the list */
	INSERT_BEFORE(&timer->link, &node->link);
}

/**
 * Add the specified timer to the software timer service queue.
 * When the delay indicated by the timer expires, the timer
 * device will execute the event associated with it.
 *
 * You should not call this function on an already running timer.
 *
 * \note Interrupt safe
 */
void timer_add(Timer *timer)
{
	ATOMIC(timer_addToList(timer, &timers_queue));
}

/**
 * Remove a timer from the timers queue before it has expired.
 *
 * \note Attempting to remove a timer already expired cause
 *       undefined behaviour.
 */
Timer *timer_abort(Timer *timer)
{
	ATOMIC(REMOVE(&timer->link));
	DB(timer->magic = TIMER_MAGIC_INACTIVE;)

	return timer;
}


INLINE void timer_poll(List *queue)
{
	Timer *timer;

	/*
	 * Check the first timer request in the list and process
	 * it when it has expired. Repeat this check until the
	 * first node has not yet expired. Since the list is sorted
	 * by expiry time, all the following requests are guaranteed
	 * to expire later.
	 */
	while ((timer = (Timer *)LIST_HEAD(queue))->link.succ)
	{
		/* This request in list has not yet expired? */
		if (timer_clock() - timer->tick < 0)
			break;

		/* Retreat the expired timer */
		REMOVE(&timer->link);
		DB(timer->magic = TIMER_MAGIC_INACTIVE;)

		/* Execute the associated event */
		event_do(&timer->expire);
	}
}

/**
 * Add \a timer to \a queue.
 * \see synctimer_poll() for details.
 */
void synctimer_add(Timer *timer, List *queue)
{
	timer_addToList(timer, queue);
}

/**
 * Simple synchronous timer based scheduler polling routine.
 *
 * Sometimes you would like to have a proper scheduler,
 * but you can't afford it due to memory constraints.
 *
 * This is a simple replacement: you can create events and call
 * them periodically at specific time intervals.
 * All you have to do is to set up normal timers, and call synctimer_add()
 * instead of timer_add() to add the events to your specific queue.
 * Then, in the main loop or wherever you want, you can call
 * synctimer_poll() to process expired events. The associated callbacks will be
 * executed.
 * As this is done synchronously you don't have to worry about race conditions.
 * You can kill an event by simply calling synctimer_abort().
 *
 */
void synctimer_poll(List *queue)
{
	timer_poll(queue);
}

#endif /* CONFIG_TIMER_EVENTS */


/**
 * Wait for the specified amount of timer ticks.
 *
 * \note Sleeping while preemption is disabled fallbacks to a busy wait sleep.
 */
void timer_delayTicks(ticks_t delay)
{
	/* We shouldn't sleep with interrupts disabled */
	IRQ_ASSERT_ENABLED();

#if CONFIG_KERN_SIGNALS
	Timer t;
	DB(t.magic = TIMER_MAGIC_INACTIVE;)
	if (proc_preemptAllowed())
	{
		timer_setEvent(&t);
		timer_setDelay(&t, delay);
		timer_add(&t);
		timer_waitEvent(&t);
	}
	else
#endif /* !CONFIG_KERN_SIGNALS */
	{
		ticks_t start = timer_clock();

		/* Busy wait */
		while (timer_clock() - start < delay)
			cpu_relax();
	}
}


#if CONFIG_TIMER_UDELAY

/**
 * Busy wait until the specified amount of high-precision ticks have elapsed.
 *
 * \note This function is interrupt safe, the only
 *       requirement is a running hardware timer.
 */
void timer_busyWait(hptime_t delay)
{
	hptime_t now, prev = timer_hw_hpread();
	hptime_t delta;

	for (;;)
	{
		now = timer_hw_hpread();
		/*
		 * The timer counter may wrap here and "prev" can become
		 * greater than "now". So, be sure to always evaluate a
		 * coherent timer difference:
		 *
		 * 0     prev            now   TIMER_HW_CNT
		 * |_____|_______________|_____|
		 *        ^^^^^^^^^^^^^^^
		 * delta = now - prev
		 *
		 * 0     now             prev  TIMER_HW_CNT
		 * |_____|_______________|_____|
		 *  ^^^^^                 ^^^^^
		 * delta = (TIMER_HW_CNT - prev) + now
		 *
		 * NOTE: TIMER_HW_CNT can be any value, not necessarily a power
		 * of 2. For this reason the "%" operator is not suitable for
		 * the generic case.
		 */
		delta = (now < prev) ? ((hptime_t)TIMER_HW_CNT - prev + now) :
						(now - prev);
		if (delta >= delay)
			break;
		delay -= delta;
		prev = now;
	}
}

/**
 * Wait for the specified amount of time (expressed in microseconds).
 *
 * \bug In AVR arch the maximum amount of time that can be used as
 *      delay could be very limited, depending on the hardware timer
 *      used. Check timer_avr.h, and what register is used as hptime_t.
 */
void timer_delayHp(hptime_t delay)
{
	if (UNLIKELY(delay > us_to_hptime(1000)))
	{
		timer_delayTicks(delay / (TIMER_HW_HPTICKS_PER_SEC / TIMER_TICKS_PER_SEC));
		delay %= (TIMER_HW_HPTICKS_PER_SEC / TIMER_TICKS_PER_SEC);
	}

	timer_busyWait(delay);
}
#endif /* CONFIG_TIMER_UDELAY */

/**
 * Timer interrupt handler. Find soft timers expired and
 * trigger corresponding events.
 */
DEFINE_TIMER_ISR
{
	/*
	 * With the Metrowerks compiler, the only way to force the compiler generate
	 * an interrupt service routine is to put a pragma directive within the function
	 * body.
	 */
	#ifdef __MWERKS__
	#pragma interrupt saveall
	#endif

	/*
	 * On systems sharing IRQ line and vector, this check is needed
	 * to ensure that IRQ is generated by timer source.
	 */
	if (!timer_hw_triggered())
		return;

	TIMER_STROBE_ON;

	/* Update the master ms counter */
	++_clock;

	/* Update the current task's quantum (if enabled). */
	proc_decQuantum();

	#if CONFIG_TIMER_EVENTS
		timer_poll(&timers_queue);
	#endif

	/* Perform hw IRQ handling */
	timer_hw_irq();

	TIMER_STROBE_OFF;
}

MOD_DEFINE(timer)

/**
 * Initialize timer
 */
void timer_init(void)
{
	#if CONFIG_KERN_IRQ
		MOD_CHECK(irq);
	#endif

	#if CONFIG_TIMER_EVENTS
		LIST_INIT(&timers_queue);
	#endif

	TIMER_STROBE_INIT;

	_clock = 0;

	timer_hw_init();

	MOD_INIT(timer);
}


#if (ARCH & ARCH_EMUL)
/**
 * Stop timer (only used by emulator)
 */
void timer_cleanup(void)
{
	MOD_CLEANUP(timer);

	timer_hw_cleanup();

	// Hmmm... apparently, the demo app does not cleanup properly
	//ASSERT(LIST_EMPTY(&timers_queue));
}
#endif /* ARCH_EMUL */
