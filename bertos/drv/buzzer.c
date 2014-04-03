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
 * Copyright 1999, 2003 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Buzzer driver (implementation)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "buzzer.h"

#include "hw/hw_buzzer.h"
#include <drv/timer.h>

#include <mware/event.h>

#include <cfg/debug.h>
#include <cfg/module.h>


/* Local vars */
static Timer buz_timer;
static bool buz_timer_running;
static mtime_t buz_repeat_interval;
static mtime_t buz_repeat_duration;


/**
 * Turn off buzzer, called by software timer
 */
static void buz_softint(void)
{
	if (IS_BUZZER_ON)
	{
		BUZZER_OFF;
		if (buz_repeat_interval)
		{
			/* Wait for interval time */
			timer_setDelay(&buz_timer, ms_to_ticks(buz_repeat_interval));
			timer_add(&buz_timer);
		}
		else
			buz_timer_running = false;
	}
	else if (buz_repeat_interval)
	{
		/* Wait for beep time */
		BUZZER_ON;
		timer_setDelay(&buz_timer, ms_to_ticks(buz_repeat_duration));
		timer_add(&buz_timer);
	}
	else
		buz_timer_running = false;
}


/**
 * Beep for the specified ms time
 */
void buz_beep(mtime_t time)
{
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Remove the software interrupt if it was already queued */
	if (buz_timer_running)
		timer_abort(&buz_timer);

	/* Turn on buzzer */
	BUZZER_ON;

	/* Add software interrupt to turn the buzzer off later */
	buz_timer_running = true;
	timer_setDelay(&buz_timer, ms_to_ticks(time));
	timer_add(&buz_timer);

	IRQ_RESTORE(flags);
}


/**
 * Start buzzer repetition
 */
void buz_repeat_start(mtime_t duration, mtime_t interval)
{
	buz_repeat_interval = interval;
	buz_repeat_duration = duration;
	buz_beep(duration);
}


/**
 * Stop buzzer repetition
 */
void buz_repeat_stop(void)
{
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Remove the software interrupt if it was already queued */
	if (buz_timer_running)
	{
		timer_abort(&buz_timer);
		buz_timer_running = false;
	}

	buz_repeat_interval = 0;
	BUZZER_OFF;

	IRQ_RESTORE(flags);
}

MOD_DEFINE(buzzer)

/**
 * Initialize buzzer.
 */
void buz_init(void)
{
	MOD_CHECK(timer);

	BUZZER_HW_INIT;

	/* Init software interrupt. */
	timer_setSoftint(&buz_timer, (Hook)buz_softint, 0);

	MOD_INIT(buzzer);
}
