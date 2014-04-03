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
 * Copyright 1999, 2003 Bernie Innocenti
 *
 * -->
 *
 * \brief Keyboard driver (implementation)
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "hw/hw_kbd.h"

#include "cfg/cfg_kbd.h"
#include <cfg/debug.h>
#include <cfg/module.h>

#include <drv/timer.h>
#include <mware/event.h>
#include <drv/kbd.h>


/* Configuration sanity checks */
#if !defined(CONFIG_KBD_POLL) || (CONFIG_KBD_POLL != KBD_POLL_SOFTINT)
	#error CONFIG_KBD_POLL must be defined to either KBD_POLL_SOFTINT
#endif
#if !defined(CONFIG_KBD_BEEP) || (CONFIG_KBD_BEEP != 0 && CONFIG_KBD_BEEP != 1)
	#error CONFIG_KBD_BEEP must be defined to either 0 or 1
#endif
#if !defined(CONFIG_KBD_OBSERVER) || (CONFIG_KBD_OBSERVER != 0 && CONFIG_KBD_OBSERVER != 1)
	#error CONFIG_KBD_OBSERVER must be defined to either 0 or 1
#endif
#if !defined(CONFIG_KBD_LONGPRESS) || (CONFIG_KBD_LONGPRESS != 0 && CONFIG_KBD_LONGPRESS != 1)
	#error CONFIG_KBD_LONGPRESS must be defined to either 0 or 1
#endif

#if CONFIG_KBD_BEEP
	#include <drv/buzzer.h>
#endif

#define KBD_CHECK_INTERVAL  10  /**< (ms) Timing for kbd softint */
#define KBD_DEBOUNCE_TIME   30  /**< (ms) Debounce time */
#define KBD_BEEP_TIME        5  /**< (ms) Duration of keybeep */

#define KBD_REPEAT_DELAY   400  /**< (ms) Keyboard repeat delay for first character */
#define KBD_REPEAT_RATE    100  /**< (ms) Initial interchar delay for keyboard repeat */
#define KBD_REPEAT_MAXRATE  20  /**< (ms) Minimum delay for keyboard repeat */
#define KBD_REPEAT_ACCEL     5  /**< (ms) Keyboard repeat speed increase */

#define KBD_LNG_DELAY     1000  /**< (ms) Keyboard long pression keys delay */


/** Status for keyboard repeat state machine */
static enum { KS_IDLE, KS_REPDELAY, KS_REPEAT } kbd_rptStatus;

/** Used to notify the occurrence of a key pressed event */
static Event key_pressed;

static volatile keymask_t kbd_buf; /**< Single entry keyboard buffer */
static volatile keymask_t kbd_cnt; /**< Number of keypress events in \c kbd_buf */
static keymask_t kbd_rpt_mask;     /**< Mask of repeatable keys. */

#if CONFIG_KBD_POLL == KBD_POLL_SOFTINT
static Timer kbd_timer;            /**< Keyboard softtimer */
#endif

static List kbd_rawHandlers;       /**< Raw keyboard handlers */
static List kbd_handlers;          /**< Cooked keyboard handlers */

static KbdHandler kbd_defHandler;  /**< The default keyboard handler */
static KbdHandler kbd_debHandler;  /**< The debounce keyboard handler */
static KbdHandler kbd_rptHandler;  /**< Auto-repeat keyboard handler */

#if CONFIG_KBD_LONGPRESS
static KbdHandler kbd_lngHandler;  /**< Long pression keys handler */
#endif

#if CONFIG_KBD_OBSERVER
	#include <mware/observer.h>
	Subject kbd_subject;
#endif


/**
 * Poll keyboard and dispatch keys to handlers.
 *
 * Read the key states and invoke all keyboard
 * handlers to process the new state.
 *
 * Call this function periodically using a software
 * timer, an interrupt or a process.
 */
static void kbd_poll(void)
{
	/** Currently depressed key */
	static keymask_t current_key;

	struct KbdHandler *handler;
	keymask_t key = kbd_readkeys();

	/* Call raw input handlers */
	FOREACH_NODE(handler, &kbd_rawHandlers)
		key = handler->hook(key);

	/* If this key was not previously pressed */
	if (key != current_key)
	{
		/* Remember last key */
		current_key = key;

		/* Call cooked input handlers */
		FOREACH_NODE(handler, &kbd_handlers)
			key = handler->hook(key);
	}
}

#if CONFIG_KBD_POLL == KBD_POLL_SOFTINT

/**
 * Keyboard soft-irq handler.
 */
static void kbd_softint(UNUSED_ARG(iptr_t, arg))
{
	kbd_poll();
	timer_add(&kbd_timer);
}

#else
	#error "Define keyboard poll method"

#endif /* CONFIG_KBD_POLL */

/**
 * \brief Read a key from the keyboard buffer.
 *
 * When a key is kept depressed between calls of this function a value
 * is returned only after the time specified with KBD_REPAT_DELAY to
 * avoid too fast keyboard repeat.
 *
 * \note Calls \c schedule() internally.
 *
 * \note This function is \b not interrupt safe!
 *
 * \return The mask of depressed keys or 0 if no keys are depressed.
 *
 */
keymask_t kbd_peek(void)
{
	keymask_t key = 0;

#if CONFIG_KBD_SCHED
	/* Let other tasks run for a while */
	extern void schedule(void);
	schedule();
#endif

	/* Extract an event from the keyboard buffer */
	IRQ_DISABLE;
	if (kbd_cnt)
	{
		--kbd_cnt;
		key = kbd_buf;
	}
	IRQ_ENABLE;

	return key;
}

/**
 * Wait for a keypress and return the mask of depressed keys.
 *
 * \note This function is \b not interrupt safe!
 */
keymask_t kbd_get(void)
{
	keymask_t key;

	#if CONFIG_KBD_POLL == KBD_POLL_SOFTINT
		event_wait(&key_pressed);
		key = kbd_peek();
	#else
		while (!(key = kbd_peek()))
			cpu_relax();
	#endif

	return key;
}


/**
 * Wait up to \c timeout ms for a keypress
 * and return the mask of depressed keys, or K_TIMEOUT
 * if the timeout was reacked.
 */
keymask_t kbd_get_timeout(mtime_t timeout)
{
	if (event_waitTimeout(&key_pressed, timeout))
		return kbd_peek();
	else
		return K_TIMEOUT;
}


void kbd_addHandler(struct KbdHandler *handler)
{
	KbdHandler *node;
	List *list;

	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Choose between raw and coocked handlers list */
	list = (handler->flags & KHF_RAWKEYS) ?
		&kbd_rawHandlers : &kbd_handlers;

	/*
	 * Search for the first node whose priority
	 * is lower than the timer we want to add.
	 */
	FOREACH_NODE(node,list)
		if (node->pri < handler->pri)
			break;

	/* Enqueue handler in the handlers chain */
	INSERT_BEFORE(&handler->link, &node->link);

	IRQ_RESTORE(flags);
}


void kbd_remHandler(struct KbdHandler *handler)
{
	/* Remove the handler */
	ATOMIC(REMOVE(&handler->link));
}


/**
 * This is the default key handler, called after
 * all other handlers have had their chance to
 * do their special processing. This handler
 * pushes all input in the keyboard FIFO buffer.
 */
static keymask_t kbd_defHandlerFunc(keymask_t key)
{
	if (key)
	{
		/* Force a single event in kbd buffer */
		kbd_buf = key;
		kbd_cnt = 1;
		#if CONFIG_KBD_POLL == KBD_POLL_SOFTINT
			event_do(&key_pressed);
		#endif

		#if CONFIG_KBD_OBSERVER
			observer_notify(&kbd_subject, KBD_EVENT_KEY, &key);
		#endif

		#if CONFIG_KBD_BEEP
			if (!(key & K_REPEAT))
				buz_beep(KBD_BEEP_TIME);
		#endif
	}

	/* Eat all input */
	return 0;
}

/**
 * Handle keyboard debounce
 */
static keymask_t kbd_debHandlerFunc(keymask_t key)
{
	/** Buffer for debounce */
	static keymask_t debounce_key;

	/** Timer for keyboard debounce */
	static ticks_t debounce_time;

	/** Key aquired after debounce */
	static keymask_t new_key;


	ticks_t now = timer_clock();

	if (key != debounce_key)
	{
		/* Reset debounce timer */
		debounce_key = key;
		debounce_time = now;
	}
	else if ((new_key != debounce_key)
		&& (now - debounce_time > ms_to_ticks(KBD_DEBOUNCE_TIME)))
	{
		new_key = debounce_key;
		debounce_time = now;
	}

	return new_key;
}

#if CONFIG_KBD_LONGPRESS
/**
 * Handle long pression keys.
 */
static keymask_t kbd_lngHandlerFunc(keymask_t key)
{
	static ticks_t start;
	ticks_t now = timer_clock();

	if (key & K_LNG_MASK)
	{
		if (now - start > ms_to_ticks(KBD_LNG_DELAY))
			key |= K_LONG;
	}
	else
		start = now;
	return key;
}
#endif

/**
 * Set current mask of repeatable keys.
 */
keymask_t kbd_setRepeatMask(keymask_t mask)
{
	keymask_t oldmask = kbd_rpt_mask;
	ATOMIC(kbd_rpt_mask = mask);
	return oldmask;
}

/**
 * Handle keyboard repeat
 */
static keymask_t kbd_rptHandlerFunc(keymask_t key)
{
	/* Timer for keyboard repeat events. */
	static ticks_t repeat_time;

	/* Current repeat rate (for acceleration). */
	static ticks_t repeat_rate; /** Current repeat rate (for acceleration) */

	ticks_t now = timer_clock();

	switch (kbd_rptStatus)
	{
		case KS_IDLE:
			if (key & kbd_rpt_mask)
			{
				repeat_time = now;
				kbd_rptStatus = KS_REPDELAY;
			}
			break;

		case KS_REPDELAY:
			if (key & kbd_rpt_mask)
			{
				if (now - repeat_time > ms_to_ticks(KBD_REPEAT_DELAY))
				{
					key = (key & kbd_rpt_mask) | K_REPEAT;
					repeat_time = now;
					repeat_rate = ms_to_ticks(KBD_REPEAT_RATE);
					kbd_rptStatus = KS_REPEAT;
				}
				else
					key = 0;
			}
			else
				kbd_rptStatus = KS_IDLE;
			break;

		case KS_REPEAT:
			if (key & kbd_rpt_mask)
			{
				if (now - repeat_time > repeat_rate)
				{
					/* Enqueue a new event in the buffer */
					key = (key & kbd_rpt_mask) | K_REPEAT;
					repeat_time = now;

					/* Repeat rate acceleration */
					if (repeat_rate > ms_to_ticks(KBD_REPEAT_MAXRATE))
						repeat_rate -= ms_to_ticks(KBD_REPEAT_ACCEL);
				}
				else
					key = 0;
			}
			else
				kbd_rptStatus = KS_IDLE;

			break;
	}

	return key;
}


MOD_DEFINE(kbd)

/**
 * Initialize keyboard ports and softtimer
 */
void kbd_init(void)
{
#if CONFIG_KBD_BEEP
	MOD_CHECK(buzzer);
#endif

	KBD_HW_INIT;

	/* Init handlers lists */
	LIST_INIT(&kbd_handlers);
	LIST_INIT(&kbd_rawHandlers);

	/* Add debounce keyboard handler */
	kbd_debHandler.hook = kbd_debHandlerFunc;
	kbd_debHandler.pri = 100; /* high priority */
	kbd_debHandler.flags = KHF_RAWKEYS;
	kbd_addHandler(&kbd_debHandler);

	#if CONFIG_KBD_LONGPRESS
	/* Add long pression keyboard handler */
	kbd_lngHandler.hook = kbd_lngHandlerFunc;
	kbd_lngHandler.pri = 90; /* high priority */
	kbd_lngHandler.flags = KHF_RAWKEYS;
	kbd_addHandler(&kbd_lngHandler);
	#endif

	/* Add repeat keyboard handler */
	kbd_rptHandler.hook = kbd_rptHandlerFunc;
	kbd_rptHandler.pri = 80; /* high priority */
	kbd_rptHandler.flags = KHF_RAWKEYS;
	kbd_addHandler(&kbd_rptHandler);

	/* Add default keyboard handler */
	kbd_defHandler.hook = kbd_defHandlerFunc;
	kbd_defHandler.pri = -128; /* lowest priority */
	kbd_addHandler(&kbd_defHandler);

#if CONFIG_KBD_OBSERVER
	observer_InitSubject(&kbd_subject);
#endif

#if CONFIG_KBD_POLL == KBD_POLL_SOFTINT

	MOD_CHECK(timer);
	#if CONFIG_KERN
		MOD_CHECK(proc);
	#endif

	/* Initialize the keyboard event (key pressed) */
	event_initGeneric(&key_pressed);

	/* Add kbd handler to soft timers list */
	event_initSoftint(&kbd_timer.expire, kbd_softint, NULL);
	timer_setDelay(&kbd_timer, ms_to_ticks(KBD_CHECK_INTERVAL));
	timer_add(&kbd_timer);

#else
	#error "Define keyboard poll method"

#endif

	MOD_INIT(kbd);
}
