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
 * Copyright 2000, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \defgroup drv_timers Timer module
 * \ingroup core
 * \{
 *
 * \brief Hardware independent timer driver.
 *
 * All timer related functions are implemented in this module. You have several options to use timers:
 * \li simple delay: just use timer_delay() if you want to wait for a few milliseconds;
 * \li delay with callback: create a timer structure and use timer_setDelay() and timer_setSoftint() to set the callback;
 * \li delay with signal: same as above but use timer_setSignal() to set specify which signal to send.
 * \li simple synchronous timer based scheduler: use synctimer_add() to schedule an event in a user provided queue.
 *
 * Whenever a timer expires you need to explicitly arm it again with timer_add(). If you want to abort a timer, use timer_abort().
 * You can use conversion macros when using msecs to specify the delay.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_timer.h"
 * $WIZ$ module_depends = "event", "sysirq"
 * $WIZ$ module_supports = "not atmega103 and not atmega8"
 */

#ifndef DRV_TIMER_H
#define DRV_TIMER_H

#include <cfg/os.h>
#include <cfg/macros.h>

#include <cpu/attr.h>
#include <cpu/irq.h>


/*
 * Include platform-specific binding header if we're hosted.
 * Try the CPU specific one for bare-metal environments.
 */
#if OS_HOSTED
	//#include OS_HEADER(timer)
	#include <emul/timer_posix.h>
#else
	#include CPU_HEADER(timer)
#endif

STATIC_ASSERT(sizeof(hptime_t) == SIZEOF_HPTIME_T);

#include "cfg/cfg_timer.h"
#include <cfg/debug.h>
#include <cfg/compiler.h>

#include <struct/list.h>

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_TIMER_EVENTS) || ((CONFIG_TIMER_EVENTS != 0) && CONFIG_TIMER_EVENTS != 1)
	#error CONFIG_TIMER_EVENTS must be set to either 0 or 1 in cfg_timer.h
#endif
#if !defined(CONFIG_TIMER_UDELAY) || ((CONFIG_TIMER_UDELAY != 0) && CONFIG_TIMER_EVENTS != 1)
	#error CONFIG_TIMER_UDELAY must be set to either 0 or 1 in cfg_timer.h
#endif
#if defined(CONFIG_TIMER_DISABLE_UDELAY)
	#error Obosolete config option CONFIG_TIMER_DISABLE_UDELAY.  Use CONFIG_TIMER_UDELAY
#endif
#if defined(CONFIG_TIMER_DISABLE_EVENTS)
	#error Obosolete config option CONFIG_TIMER_DISABLE_EVENTS.  Use CONFIG_TIMER_EVENTS
#endif

extern volatile ticks_t _clock;

#define TIMER_AFTER(x, y) ((long)(y) - (long)(x) < 0)
#define TIMER_BEFORE(x, y) TIMER_AFTER(y, x)

/**
 * \brief Return the system tick counter (expressed in ticks)
 *
 * The result is guaranteed to increment monotonically,
 * but client code must be tolerant with respect to overflows.
 *
 * The following code is safe:
 *
 * \code
 *   drop_teabag();
 *   ticks_t tea_start_time = timer_clock();
 *
 *   for (;;)
 *   {
 *       if (timer_clock() - tea_start_time > TEAPOT_DELAY)
 *       {
 *           printf("Your tea, Sir.\n");
 *           break;
 *       }
 *       patience();
 *   }
 * \endcode
 *
 * \note This function must disable interrupts on 8/16bit CPUs because the
 * clock variable is larger than the processor word size and can't
 * be copied atomically.
 * \sa timer_delay()
 */
INLINE ticks_t timer_clock(void)
{
	ticks_t result;

	ATOMIC(result = _clock);

	return result;
}

/**
 * Faster version of timer_clock(), to be called only when the timer
 * interrupt is disabled (DISABLE_INTS) or overridden by a
 * higher-priority or non-nesting interrupt.
 *
 * \sa timer_clock
 */
INLINE ticks_t timer_clock_unlocked(void)
{
	return _clock;
}


/** Convert \a ms [ms] to ticks. */
INLINE ticks_t ms_to_ticks(mtime_t ms)
{
#if TIMER_TICKS_PER_SEC < 1000
	/* Slow timer: avoid rounding down too much. */
	return (ms * TIMER_TICKS_PER_SEC) / 1000;
#else
	/* Fast timer: don't overflow ticks_t. */
	return ms * DIV_ROUND(TIMER_TICKS_PER_SEC, 1000);
#endif
}

/** Convert \a us [us] to ticks. */
INLINE ticks_t us_to_ticks(utime_t us)
{
#if TIMER_TICKS_PER_SEC < 1000
	/* Slow timer: avoid rounding down too much. */
	return ((us / 1000) * TIMER_TICKS_PER_SEC) / 1000;
#else
	/* Fast timer: don't overflow ticks_t. */
	return (us * DIV_ROUND(TIMER_TICKS_PER_SEC, 1000)) / 1000;
#endif
}

/** Convert \a ticks [ticks] to ms. */
INLINE mtime_t ticks_to_ms(ticks_t ticks)
{
#if TIMER_TICKS_PER_SEC < 1000
	/* Slow timer: avoid rounding down too much. */
	return (ticks * 1000) / TIMER_TICKS_PER_SEC;
#else
	/* Fast timer: avoid overflowing ticks_t. */
	return ticks / (TIMER_TICKS_PER_SEC / 1000);
#endif
}

/** Convert \a ticks [ticks] to us. */
INLINE utime_t ticks_to_us(ticks_t ticks)
{
#if TIMER_TICKS_PER_SEC < 1000
	/* Slow timer: avoid rounding down too much. */
	return ((ticks * 1000) / TIMER_TICKS_PER_SEC) * 1000;
#else
	/* Fast timer: avoid overflowing ticks_t. */
	return (ticks / (TIMER_TICKS_PER_SEC / 1000)) * 1000;
#endif
}

/** Convert \a us [us] to hpticks */
INLINE hptime_t us_to_hptime(utime_t us)
{
#if TIMER_HW_HPTICKS_PER_SEC > 10000000UL
	return us * DIV_ROUND(TIMER_HW_HPTICKS_PER_SEC, 1000000UL);
#else
	return (us * ((TIMER_HW_HPTICKS_PER_SEC + 500) / 1000UL) + 500) / 1000UL;
#endif
}

/** Convert \a hpticks [hptime] to usec */
INLINE utime_t hptime_to_us(hptime_t hpticks)
{
#if TIMER_HW_HPTICKS_PER_SEC < 100000UL
	return hpticks * DIV_ROUND(1000000UL, TIMER_HW_HPTICKS_PER_SEC);
#else
	return (hpticks * 1000UL) / DIV_ROUND(TIMER_HW_HPTICKS_PER_SEC, 1000UL);
#endif /* TIMER_HW_HPTICKS_PER_SEC < 100000UL */
}

void timer_delayTicks(ticks_t delay);
/**
 * Wait some time [ms].
 *
 * \note CPU is released while waiting so you don't have to call cpu_relax() explicitly.
 * \param delay Time to wait [ms].
 */
INLINE void timer_delay(mtime_t delay)
{
	timer_delayTicks(ms_to_ticks(delay));
}

void timer_init(void);
void timer_cleanup(void);

int timer_testSetup(void);
int timer_testRun(void);
int timer_testTearDown(void);

#if CONFIG_TIMER_UDELAY
void timer_busyWait(hptime_t delay);
void timer_delayHp(hptime_t delay);
INLINE void timer_udelay(utime_t delay)
{
	timer_delayHp(us_to_hptime(delay));
}
#endif

#if CONFIG_TIMER_EVENTS

#include <mware/event.h>

/**
 * The timer driver supports multiple synchronous timers
 * that can trigger an event when they expire.
 *
 * \sa timer_add()
 * \sa timer_abort()
 */
typedef struct Timer
{
	Node    link;     /**< Link into timers queue */
	ticks_t _delay;   /**< [ticks] Timer delay */
	ticks_t tick;     /**< [ticks] When this timer will expire */
	Event   expire;   /**< Event to execute when the timer expires */
	DB(uint16_t magic;)
} Timer;

/* Timer is active when Timer.magic contains this value (for debugging purposes). */
#define TIMER_MAGIC_ACTIVE    0xABBA
#define TIMER_MAGIC_INACTIVE  0xBAAB

void timer_add(Timer *timer);
Timer *timer_abort(Timer *timer);

/**
 * Set the timer so that it calls an user hook when it expires
 *
 * Sometimes you may want to use the same callback for different events, so you must have
 * different data to operate on. The user_data parameter is such data.
 *
 * \param timer Timer struct to set the callback to
 * \param func  Function that will be called when the timer expires
 * \param user_data Additional data you may want to pass to the callback
 */
INLINE void timer_setSoftint(Timer *timer, Hook func, iptr_t user_data)
{
	event_initSoftint(&timer->expire, func, user_data);
}

/**
 * Set the timer delay (the time before the event will be triggered)
 *
 * \note It's illegal to change the delay of the timer when it's
 * still running.
 */
INLINE void timer_setDelay(Timer *timer, ticks_t delay)
{
	timer->_delay = delay;
}


void synctimer_add(Timer *timer, List* q);

/** \sa timer_abort */
#define synctimer_abort(t) timer_abort(t)

void synctimer_poll(List* q);


#endif /* CONFIG_TIMER_EVENTS */

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS

/** Set the timer so that it sends a event notification when it expires */
INLINE void timer_setEvent(Timer *timer)
{
	event_initGeneric(&timer->expire);
}

/** Wait until the timer expires */
INLINE void timer_waitEvent(Timer *timer)
{
	event_wait(&timer->expire);
}

/** Set the timer so that it sends a signal when it expires */
INLINE void timer_setSignal(Timer *timer, struct Process *proc, sigmask_t sigs)
{
	event_initSignal(&timer->expire, proc, sigs);
}

#define timer_set_event_signal timer_setSignal

#endif /* CONFIG_KERN_SIGNALS */

/** \} */ //defgroup drv_timers

#endif /* DRV_TIMER_H */
