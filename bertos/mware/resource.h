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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief TODO:
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef MWARE_RESOURCE_H
#define MWARE_RESOURCE_H

#include <drv/timer.h> // time_t
#include <kern/sem.h>

#warning FIXME:Revise me!

/*
 * Abstract locking primitives used by host OS.
 */
typedef Semaphore ResourceLock;
#define ResMan_sleep()         timer_delay(1)
#define ResMan_time_t          mtime_t



// Forward decl
struct Observer;

/**
 * Hold context information for a resource such as an audio channel.
 *
 * Each driver registers one or more Resource instances with the
 * ResMan using ResMan_Register().
 *
 * Clients can then allocate the resource through ResMan_Alloc()
 * providing a desired priority and an Observer for asynchronous
 * notification.
 *
 * Allocated resources can be stolen by other clients asking for a
 * higher priority.  ResMan notifies a preemption request by invoking
 * the Observer of the current owner.
 *
 * The Observer callback must take whatever action is needed to
 * release the resource as soon as possible to avoid blocking the
 * new owner.
 */
typedef struct Resource
{
//Private
	/// Control access to fields below.
	Semaphore lock;

	/// Pointer to current owner's observer.  NULL if resource is free.
	struct Observer *owner;

	/// Priority of current owner (higher values mean higher priority).
	int pri;

	/// Queue of processes waiting to obtain the resource.
	List queue;
} Resource;

/// Event sent by ResMan to owners when to request resource release.
enum { EVENT_RELEASE = 1 };

/// Try to allocate a resource \a res with priority \a pri for at most \a timeout ticks.
bool ResMan_Alloc(Resource *res, int pri, ResMan_time_t timeout, struct Observer *releaseRequest);

/// Free resource \a res.  Will eventually wake-up other queued owners.
void ResMan_Free(Resource *res);

void ResMan_Init(Resource *res);

#endif /* MWARE_RESOURCE_H */
