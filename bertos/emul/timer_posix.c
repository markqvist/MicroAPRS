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
 * Copyright 2005, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Low-level timer module for Qt emulator (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
//#include <cfg/compiler.h> // hptime.t
#include <os/hptime.h>
#include <kern/irq.h>     // irq_register()

#if !CONFIG_KERN_IRQ
#include <signal.h>       // sigaction()
#include <string.h>       // memset()
#endif
#include <sys/time.h>     // setitimer()


// Forward declaration for the user interrupt server routine.
void timer_isr(int);

/// HW dependent timer initialization.
static void timer_hw_init(void)
{
	#if CONFIG_KERN_IRQ
		irq_register(SIGALRM, (void (*)(void))timer_isr);
	#else // ! CONFIG_KERN_IRQ
		struct sigaction sa;
		memset(&sa, 0, sizeof(sa));

		// Setup interrupt callback
		sa.sa_handler = timer_isr;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGALRM);
		sa.sa_flags = SA_RESTART;
		sigaction(SIGALRM, &sa, NULL);
	#endif // CONFIG_KERN_IRQ

	// Setup POSIX realtime timer to interrupt every 1/TIMER_TICKS_PER_SEC.
	static const struct itimerval itv =
	{
		{ 0, 1000000 / TIMER_TICKS_PER_SEC }, /* it_interval */
		{ 0, 1000000 / TIMER_TICKS_PER_SEC }  /* it_value */
	};
	setitimer(ITIMER_REAL, &itv, NULL);
}

static void timer_hw_cleanup(void)
{
	static const struct itimerval itv =
	{
		{ 0, 0 }, /* it_interval */
		{ 0, 0 }  /* it_value */
	};
	setitimer(ITIMER_REAL, &itv, NULL);
	signal(SIGALRM, SIG_DFL);
}

INLINE hptime_t timer_hw_hpread(void)
{
	return hptime_get();
}

#define timer_hw_triggered() (true)
