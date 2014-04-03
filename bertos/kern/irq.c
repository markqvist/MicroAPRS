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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Process scheduler (public interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * Still in development, disable nightly test for now
 * notest: avr
 * notest: arm
 */
#include "irq.h"

#include <cfg/module.h>
#include <kern/proc_p.h>
#include <kern/proc.h>

#include "cfg/cfg_proc.h"

#include <unistd.h> // FIXME: move POSIX stuff to irq_posix.h

MOD_DEFINE(irq)

// FIXME
static void (*irq_handlers[100])(void);

/* signal handler */
void irq_entry(int signum)
{
	irq_handlers[signum]();
}

void irq_register(int irq, void (*callback)(void))
{
	irq_handlers[irq] = callback;
}

void irq_init(void)
{
	struct sigaction act;

	act.sa_handler = irq_entry;
	sigemptyset(&act.sa_mask);
	//sigaddset(&act.sa_mask, irq);
	act.sa_flags = SA_RESTART; // | SA_SIGINFO;

	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	MOD_INIT(irq);
}
