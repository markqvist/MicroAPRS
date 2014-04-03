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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Heap subsystem (public interface).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "heap.h"

#include <cfg/debug.h> // ASSERT()
#include <string.h>    // memset()

#define FREE_FILL_CODE     0xDEAD
#define ALLOC_FILL_CODE    0xBEEF


/*
 * This function prototype is deprecated, will change in:
 * void heap_init(struct Heap* h, heap_buf_t* memory, size_t size)
 * in the next BeRTOS release.
 */
void heap_init(struct Heap* h, void* memory, size_t size)
{
	#ifdef _DEBUG
	memset(memory, FREE_FILL_CODE, size);
	#endif

	ASSERT2(((size_t)memory % alignof(heap_buf_t)) == 0,
	"memory buffer is unaligned, please use the HEAP_DEFINE_BUF() macro to declare heap buffers!\n");

	/* Initialize heap with a single big chunk */
	h->FreeList = (MemChunk *)memory;
	h->FreeList->next = NULL;
	h->FreeList->size = size;
}


void *heap_allocmem(struct Heap* h, size_t size)
{
	MemChunk *chunk, *prev;

	/* Round size up to the allocation granularity */
	size = ROUND_UP2(size, sizeof(MemChunk));

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(MemChunk);

	/* Walk on the free list looking for any chunk big enough to
	 * fit the requested block size.
	 */
	for (prev = (MemChunk *)&h->FreeList, chunk = h->FreeList;
		chunk;
		prev = chunk, chunk = chunk->next)
	{
		if (chunk->size >= size)
		{
			if (chunk->size == size)
			{
				/* Just remove this chunk from the free list */
				prev->next = chunk->next;
				#ifdef _DEBUG
					memset(chunk, ALLOC_FILL_CODE, size);
				#endif
				return (void *)chunk;
			}
			else
			{
				/* Allocate from the END of an existing chunk */
				chunk->size -= size;
				#ifdef _DEBUG
					memset((uint8_t *)chunk + chunk->size, ALLOC_FILL_CODE, size);
				#endif
				return (void *)((uint8_t *)chunk + chunk->size);
			}
		}
	}

	return NULL; /* fail */
}


void heap_freemem(struct Heap* h, void *mem, size_t size)
{
	MemChunk *prev;
	ASSERT(mem);

#ifdef _DEBUG
	memset(mem, FREE_FILL_CODE, size);
#endif

	/* Round size up to the allocation granularity */
	size = ROUND_UP2(size, sizeof(MemChunk));

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(MemChunk);

	/* Special cases: first chunk in the free list or memory completely full */
	ASSERT((uint8_t*)mem != (uint8_t*)h->FreeList);
	if (((uint8_t *)mem) < ((uint8_t *)h->FreeList) || !h->FreeList)
	{
		/* Insert memory block before the current free list head */
		prev = (MemChunk *)mem;
		prev->next = h->FreeList;
		prev->size = size;
		h->FreeList = prev;
	}
	else /* Normal case: not the first chunk in the free list */
	{
		/*
		 * Walk on the free list. Stop at the insertion point (when mem
		 * is between prev and prev->next)
		 */
		prev = h->FreeList;
		while (prev->next < (MemChunk *)mem && prev->next)
			prev = prev->next;

		/* Make sure mem is not *within* prev */
		ASSERT((uint8_t*)mem >= (uint8_t*)prev + prev->size);

		/* Should it be merged with previous block? */
		if (((uint8_t *)prev) + prev->size == ((uint8_t *)mem))
		{
			/* Yes */
			prev->size += size;
		}
		else /* not merged with previous chunk */
		{
			MemChunk *curr = (MemChunk*)mem;

			/* insert it after the previous node
			 * and move the 'prev' pointer forward
			 * for the following operations
			 */
			curr->next = prev->next;
			curr->size = size;
			prev->next = curr;

			/* Adjust for the following test */
			prev = curr;
		}
	}

	/* Also merge with next chunk? */
	if (((uint8_t *)prev) + prev->size == ((uint8_t *)prev->next))
	{
		prev->size += prev->next->size;
		prev->next = prev->next->next;

		/* There should be only one merge opportunity, becuase we always merge on free */
		ASSERT((uint8_t*)prev + prev->size != (uint8_t*)prev->next);
	}
}

/**
 * Returns the number of free bytes in a heap.
 * \param h the heap to check.
 *
 * \note The returned value is the sum of all free memory regions 
 *       in the heap.
 *       Those regions are likely to be *not* contiguous,
 *       so a successive allocation may fail even if the
 *       requested amount of memory is lower than the current free space.
 */
size_t heap_freeSpace(struct Heap *h)
{
	size_t free_mem = 0;
	for (MemChunk *chunk = h->FreeList; chunk; chunk = chunk->next)
		free_mem += chunk->size;

	return free_mem;
}

#if CONFIG_HEAP_MALLOC

/**
 * Standard malloc interface
 */
void *heap_malloc(struct Heap* h, size_t size)
{
	size_t *mem;

	size += sizeof(size_t);
	if ((mem = (size_t*)heap_allocmem(h, size)))
		*mem++ = size;

	return mem;
}

/**
 * Standard calloc interface
 */
void *heap_calloc(struct Heap* h, size_t size)
{
	void *mem;

	if ((mem = heap_malloc(h, size)))
		memset(mem, 0, size);

	return mem;
}

/**
 * Free a block of memory, determining its size automatically.
 *
 * \param h    Heap from which the block was allocated.
 * \param mem  Pointer to a block of memory previously allocated with
 *             either heap_malloc() or heap_calloc().
 *
 * \note If \a mem is a NULL pointer, no operation is performed.
 *
 * \note Freeing the same memory block twice has undefined behavior.
 *
 * \note This function works like the ANSI C free().
 */
void heap_free(struct Heap *h, void *mem)
{
	size_t *_mem = (size_t *)mem;

	if (_mem)
	{
		--_mem;
		heap_freemem(h, _mem, *_mem);
	}
}

#endif /* CONFIG_HEAP_MALLOC */
