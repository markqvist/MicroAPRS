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
 * Copyright 2001,2004,2005,2006,2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \defgroup kern_sem Mutually exclusive semaphores
 * \ingroup kern
 * \{
 * \brief Mutually exclusive semaphores.
 *        Shared locking not supported in this implementation.
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "semaphores"
 * $WIZ$ module_depends = "kernel"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_sem.h"
 */


#ifndef KERN_SEM_H
#define KERN_SEM_H

#include <cfg/compiler.h>
#include <struct/list.h>

/* Fwd decl */
struct Process;


typedef struct Semaphore
{
	struct Process *owner;
	List            wait_queue;
	int             nest_count;
} Semaphore;

/**
 * \name Process synchronization services
 * \{
 */
void sem_init(struct Semaphore *s);
bool sem_attempt(struct Semaphore *s);
void sem_obtain(struct Semaphore *s);
void sem_release(struct Semaphore *s);
/* \} */
/* \} */ //defgroup kern_sem

int sem_testRun(void);
int sem_testSetup(void);
int sem_testTearDown(void);

#endif /* KERN_SEM_H */
