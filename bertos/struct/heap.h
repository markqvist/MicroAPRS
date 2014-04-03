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
 * \defgroup heap Embedded optimized memory allocator
 * \ingroup core
 * \{
 *
 * \brief Heap subsystem (public interface).
 *
 * \todo Heap memory could be defined as an array of MemChunk, and used
 * in this form also within the implementation. This would probably remove
 * memory alignment problems, and also some aliasing issues.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "heap"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_heap.h"
 */

#ifndef STRUCT_HEAP_H
#define STRUCT_HEAP_H

#include "cfg/cfg_heap.h"
#include <cfg/compiler.h>
#include <cfg/macros.h> // IS_POW2()

/* NOTE: struct size must be a 2's power! */
typedef struct _MemChunk
{
	struct _MemChunk *next;
	size_t size;
} MemChunk;

STATIC_ASSERT(IS_POW2(sizeof(MemChunk)));

typedef MemChunk heap_buf_t;

/// A heap
typedef struct Heap
{
	struct _MemChunk *FreeList;     ///< Head of the free list
} Heap;

/**
 * Utility macro to allocate a heap of size \a size.
 *
 * \param name Variable name for the heap.
 * \param size Heap size in bytes.
 */
#define HEAP_DEFINE_BUF(name, size) \
	heap_buf_t name[((size) + sizeof(heap_buf_t) - 1) / sizeof(heap_buf_t)]

/// Initialize \a heap within the buffer pointed by \a memory which is of \a size bytes
void heap_init(struct Heap* heap, void* memory, size_t size);

/// Allocate a chunk of memory of \a size bytes from the heap
void *heap_allocmem(struct Heap* heap, size_t size);

/// Free a chunk of memory of \a size bytes from the heap
void heap_freemem(struct Heap* heap, void *mem, size_t size);

size_t heap_freeSpace(struct Heap *h);

#define HNEW(heap, type) \
	(type*)heap_allocmem(heap, sizeof(type))

#define HNEWVEC(heap, type, nelem) \
	(type*)heap_allocmem(heap, sizeof(type) * (nelem))

#define HDELETE(heap, type, mem) \
	heap_freemem(heap, mem, sizeof(type))

#define HDELETEVEC(heap, type, nelem, mem) \
	heap_freemem(heap, mem, sizeof(type) * (nelem))


#if CONFIG_HEAP_MALLOC

/**
 * \name Compatibility interface with C standard library
 * \{
 */
void *heap_malloc(struct Heap* heap, size_t size);
void *heap_calloc(struct Heap* heap, size_t size);
void heap_free(struct Heap* heap, void * mem);
/** \} */

#endif

/** \} */ //defgroup heap

int heap_testSetup(void);
int heap_testRun(void);
int heap_testTearDown(void);

#endif /* STRUCT_HEAP_H */
