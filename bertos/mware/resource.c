
#include "resource.h"
#include <mware/observer.h>

/**
 * Internal structure for building a priority queue
 * of processes waiting for the resource to become free.
 */
typedef struct ResourceWaiter
{
	PriNode link;
	struct Observer *owner;

} ResourceWaiter;


bool ResMan_Alloc(Resource *res, int pri, ResMan_time_t timeout, struct Observer *releaseRequest)
{
	bool success = false;

	ASSERT(releaseRequest);

	sem_obtain(&res->lock);

	if (res->owner == releaseRequest)
	{
		// Already ours
		res->pri = pri;
		success = true;
	}
	else if (!res->owner)
	{
		// Trivial acquire: nobody was owning the resource
		res->pri = pri;
		res->owner = releaseRequest;
		success = true;
	}
	else
	{
		ResourceWaiter waiter;

		// Setup waiter structure and enqueue it to resource
		waiter.owner = releaseRequest;
		waiter.link.pri = pri;
		LIST_ENQUEUE(&res->queue, &waiter.link);

		// Resource busy: are we eligible for preemption?
		if ((res->pri < pri) && res->owner->event)
			res->owner->event(EVENT_RELEASE, res);

		// Wait in the queue until the timeout occurs.
		do
		{
			sem_release(&res->lock);
			// TODO: use a semaphore here instead
			ResMan_sleep();
			sem_obtain(&res->lock);

			// Check for ownership
			if (res->owner == releaseRequest)
			{
				success = true;
				break;
			}
		}
		while (timeout--);

		// Remove pending waiter
		if (!success)
			REMOVE(&waiter.link.link);
	}

	sem_release(&res->lock);
	return success;
}

void ResMan_Free(Resource *res)
{
	ResourceWaiter *waiter;

	sem_obtain(&res->lock);


	ASSERT(res->owner);
	//TODO: check for real owner calling free

	// Check for new owner candidates.
	if ((waiter = (ResourceWaiter *)list_remHead(&res->queue)))
	{
		// Transfer ownership of the resource
		res->owner = waiter->owner;
		res->pri = waiter->link.pri;
		//ResMan_wakeup(waiter);
	}
	else
	{
		// Nobody waiting, free the resource
		res->owner = NULL;
		res->pri = -1;
	}

	sem_release(&res->lock);
}

void ResMan_Init(Resource *res)
{
	res->owner = NULL;
	res->pri = -1;

	sem_init(&res->lock);
	LIST_INIT(&res->queue);
}

