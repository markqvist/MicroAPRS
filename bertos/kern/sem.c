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
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Semaphore based synchronization services.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "sem.h"
#include <cfg/debug.h>

#include <cpu/irq.h> // ASSERT_IRQ_DISABLED()

#include <kern/proc.h>
#include <kern/proc_p.h>
#include <kern/signal.h>

INLINE void sem_verify(struct Semaphore *s)
{
	(void)s;
	ASSERT(s);
	LIST_ASSERT_VALID(&s->wait_queue);
	ASSERT(s->nest_count >= 0);
	ASSERT(s->nest_count < 128);   // heuristic max
}

#if CONFIG_KERN_PRI && CONFIG_KERN_PRI_INHERIT

#define proc_updatePri(proc) (proc_setPri(proc, (proc)->orig_pri))


/**
 * Priority inheritance update algorithm.
 *
 * The algorithm checks and boosts the priority of the semaphore's
 * current owner and also processes in that block the owner, which
 * form a chain of blocking processes.
 *
 * Note that the priority of a process in the chain of blocked
 * processes is always greater or equal than the priority of a process
 * before in the chain. See the diagram below:
 * P1  --. S1 ---> P2 --. S2 ---> P3
 * prio_proc(P2) >= prio_proc(P1) always.
 */
INLINE void pri_inheritBlock(Semaphore *s)
{
	Process *owner = s->owner;

	/*
	 * Enqueue the blocking process in the owner's inheritance
	 * list. Notice that such process might have inherited its
	 * current priority from someone else.
	 */
	current_process->inh_link.pri = __prio_proc(current_process);
	LIST_ENQUEUE(&owner->inh_list, &current_process->inh_link);
	current_process->inh_blocked_by = s;

	/*
	 * As long as a process has the power of boosting the priority
	 * of its lock owner...
	 */
	while (current_process->inh_link.pri > prio_proc(owner)) {
		Process *p = owner;

		 /* Boost the priority of the owner */
		proc_updatePri(p);

		/* If the owner is not blocked, we're done */
		if (!p->inh_blocked_by)
			break;

		/*
		 * Otherwise update the position of the owner
		 * (which is `p' at each round!) in the inheritance
		 * list it lies in and set up `owner' for the
		 * next round.
		 */
		REMOVE(&p->inh_link.link);
		p->inh_link.pri = prio_proc(p);
		owner = p->inh_blocked_by->owner;
		LIST_ENQUEUE(&owner->inh_list, &p->inh_link);
	}
}


/**
 * Priority inheritance unblock algorithm.
 *
 * Pass the priority inheritance list from the current owner to the
 * process that will take ownership of the semaphore next, potentially
 * boosting its priority.
 *
 * \param proc The process that will take ownership of the semaphore.
 */
INLINE void pri_inheritUnblock(Semaphore *s, Process *proc)
{
	Process *owner = s->owner;
	Node *n, *temp;
	Process *p;

	/*
	 * This process has nothing more to do on a priority
	 * inheritance list.
	 */
	REMOVE(&proc->inh_link.link);
	proc->inh_blocked_by = NULL;

	/*
	 * Each process in the former owner's priority inheritance
	 * list that is blocked on 's' needs to be removed from
	 * there and added to the priority inheritance list of
	 * this process, since it's going to be the new owner for
	 * that semaphore.
	 */
	FOREACH_NODE_SAFE(n, temp, &owner->inh_list) {
		p = containerof(n, Process, inh_link.link);
		/* Ensures only the processes blocked on 's' are affected! */
		if (p->inh_blocked_by == s) {
			REMOVE(&p->inh_link.link);
			LIST_ENQUEUE(&proc->inh_list, &p->inh_link);

			/* And again, update the priority of the new owner */
			if (p->inh_link.pri > prio_proc(proc))
				proc_updatePri(proc);
		}
	}

	proc_updatePri(owner);
}
#else
INLINE void pri_inheritBlock(UNUSED_ARG(Semaphore *, s))
{
}

INLINE void pri_inheritUnblock(UNUSED_ARG(Semaphore *, s), UNUSED_ARG(Process *, proc))
{
}
#endif /* CONFIG_KERN_PRI_INHERIT */


/**
 * \brief Initialize a Semaphore structure.
 */
void sem_init(struct Semaphore *s)
{
	LIST_INIT(&s->wait_queue);
	s->owner = NULL;
	s->nest_count = 0;
}


/**
 * \brief Attempt to lock a semaphore without waiting.
 *
 * \return true in case of success, false if the semaphore
 *         was already locked by someone else.
 *
 * \note   each call to sem_attempt() must be matched by a
 *         call to sem_release().
 *
 * \see sem_obtain() sem_release()
 */
bool sem_attempt(struct Semaphore *s)
{
	bool result = false;

	proc_forbid();
	sem_verify(s);
	if ((!s->owner) || (s->owner == current_process))
	{
		s->owner = current_process;
		s->nest_count++;
		result = true;
	}
	proc_permit();

	return result;
}


/**
 * \brief Lock a semaphore.
 *
 * If the semaphore is already owned by another process, the caller
 * process will be enqueued into the waiting list and sleep until
 * the semaphore is available.
 *
 * \note Each call to sem_obtain() must be matched by a
 *       call to sem_release().
 *
 * \note This routine is optimized for highest speed in
 *       the most common case: the semaphore is free or locked
 *       by the calling process itself. Rearranging this code
 *       is probably a bad idea.
 *
 * \sa sem_release() sem_attempt()
 */
void sem_obtain(struct Semaphore *s)
{
	proc_forbid();
	sem_verify(s);

	/* Is the semaphore already locked by another process? */
	if (UNLIKELY(s->owner && (s->owner != current_process)))
	{
		/* Append calling process to the wait queue */
		ADDTAIL(&s->wait_queue, (Node *)current_process);

		/* Trigger priority inheritance logic, if enabled */
		pri_inheritBlock(s);

		/*
		 * We will wake up only when the current owner calls
		 * sem_release(). Then, the semaphore will already
		 * be locked for us.
		 */
		proc_permit();
		proc_switch();
	}
	else
	{
		ASSERT(LIST_EMPTY(&s->wait_queue));

		/* The semaphore was free: lock it */
		s->owner = current_process;
		s->nest_count++;
		proc_permit();
	}
}


/**
 * \brief Release a lock on a previously locked semaphore.
 *
 * If the nesting count of the semaphore reaches zero,
 * the next process waiting for it will be awaken.
 *
 * \note This routine is optimized for highest speed in
 *       the most common case: the semaphore has been locked just
 *       once and nobody else was waiting for it. Rearranging
 *       this code is probably a bad idea.
 *
 * \sa sem_obtain() sem_attempt()
 */
void sem_release(struct Semaphore *s)
{
	Process *proc = NULL;

	proc_forbid();
	sem_verify(s);

	ASSERT(s->owner == current_process);

	/*
	 * Decrement nesting count and check if the semaphore
	 * has been fully unlocked.
	 */
	if (--s->nest_count == 0)
	{
		/* Give semaphore to the first applicant, if any */
		if (UNLIKELY((proc = (Process *)list_remHead(&s->wait_queue))))
		{
			/* Undo the effects of priority inheritance, if enabled */
			pri_inheritUnblock(s, proc);

			s->nest_count = 1;
			s->owner = proc;
		} else {
			/* Disown semaphore */
			s->owner = NULL;
		}
	}
	proc_permit();

	if (proc)
		ATOMIC(proc_wakeup(proc));
}
