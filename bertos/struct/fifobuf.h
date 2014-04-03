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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \defgroup fifobuf FIFO buffer
 * \ingroup struct
 * \{
 *
 * \brief General pourpose FIFO buffer implemented with a ring buffer
 *
 * \li \c begin points to the first buffer element;
 * \li \c end points to the last buffer element (unlike the STL convention);
 * \li \c head points to the element to be extracted next;
 * \li \c tail points to the location following the last insertion;
 * \li when any of the pointers advances beyond \c end, it is reset
 *     back to \c begin.
 *
 * \code
 *
 *  +-----------------------------------+
 *  |  empty  |   valid data   |  empty |
 *  +-----------------------------------+
 *  ^         ^                ^        ^
 *  begin    head             tail     end
 *
 * \endcode
 *
 * The buffer is EMPTY when \c head and \c tail point to the same location:
 *		\code head == tail \endcode
 *
 * The buffer is FULL when \c tail points to the location immediately
 * after \c head:
 *		\code tail == head - 1 \endcode
 *
 * The buffer is also FULL when \c tail points to the last buffer
 * location and head points to the first one:
 *		\code head == begin && tail == end \endcode
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef STRUCT_FIFO_H
#define STRUCT_FIFO_H

#include <cpu/types.h>
#include <cpu/irq.h>
#include <cfg/debug.h>

typedef struct FIFOBuffer
{
	unsigned char * volatile head;
	unsigned char * volatile tail;
	unsigned char *begin;
	unsigned char *end;
} FIFOBuffer;


#define ASSERT_VALID_FIFO(fifo) \
	ATOMIC( \
		ASSERT((fifo)->head >= (fifo)->begin); \
		ASSERT((fifo)->head <= (fifo)->end); \
		ASSERT((fifo)->tail >= (fifo)->begin); \
		ASSERT((fifo)->tail <= (fifo)->end); \
	)


/**
 * Check whether the fifo is empty
 *
 * \note Calling fifo_isempty() is safe while a concurrent
 *       execution context is calling fifo_push() or fifo_pop()
 *       only if the CPU can atomically update a pointer
 *       (which the AVR and other 8-bit processors can't do).
 *
 * \sa fifo_isempty_locked
 */
INLINE bool fifo_isempty(const FIFOBuffer *fb)
{
	//ASSERT_VALID_FIFO(fb);
	return fb->head == fb->tail;
}


/**
 * Check whether the fifo is full
 *
 * \note Calling fifo_isfull() is safe while a concurrent
 *       execution context is calling fifo_pop() and the
 *       CPU can update a pointer atomically.
 *       It is NOT safe when the other context calls
 *       fifo_push().
 *       This limitation is not usually problematic in a
 *       consumer/producer scenario because the
 *       fifo_isfull() and fifo_push() are usually called
 *       in the producer context.
 */
INLINE bool fifo_isfull(const FIFOBuffer *fb)
{
	//ASSERT_VALID_FIFO(fb);
	return
		((fb->head == fb->begin) && (fb->tail == fb->end))
		|| (fb->tail == fb->head - 1);
}


/**
 * Push a character on the fifo buffer.
 *
 * \note Calling \c fifo_push() on a full buffer is undefined.
 *       The caller must make sure the buffer has at least
 *       one free slot before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts, unless the CPU can't update
 *       a pointer atomically (which the AVR and other 8-bit
 *       processors can't do).
 *
 * \sa fifo_push_locked
 */
INLINE void fifo_push(FIFOBuffer *fb, unsigned char c)
{
#ifdef __MWERKS__
#pragma interrupt called
#endif
	//ASSERT_VALID_FIFO(fb);

	/* Write at tail position */
	*(fb->tail) = c;

	if (UNLIKELY(fb->tail == fb->end))
		/* wrap tail around */
		fb->tail = fb->begin;
	else
		/* Move tail forward */
		fb->tail++;
}


/**
 * Pop a character from the fifo buffer.
 *
 * \note Calling \c fifo_pop() on an empty buffer is undefined.
 *       The caller must make sure the buffer contains at least
 *       one character before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts.
 */
INLINE unsigned char fifo_pop(FIFOBuffer *fb)
{
#ifdef __MWERKS__
#pragma interrupt called
#endif
	//ASSERT_VALID_FIFO(fb);

	if (UNLIKELY(fb->head == fb->end))
	{
		/* wrap head around */
		fb->head = fb->begin;
		return *(fb->end);
	}
	else
		/* move head forward */
		return *(fb->head++);
}


/**
 * Make the fifo empty, discarding all its current contents.
 */
INLINE void fifo_flush(FIFOBuffer *fb)
{
	//ASSERT_VALID_FIFO(fb);
	fb->head = fb->tail;
}


#if CPU_REG_BITS >= CPU_BITS_PER_PTR

	/*
	 * 16/32bit CPUs that can update a pointer with a single write
	 * operation, no need to disable interrupts.
	 */
	#define fifo_isempty_locked(fb) fifo_isempty((fb))
	#define fifo_push_locked(fb, c) fifo_push((fb), (c))
	#define fifo_pop_locked(fb)     fifo_pop((fb))
	#define fifo_flush_locked(fb)   fifo_flush((fb))

#else /* CPU_REG_BITS < CPU_BITS_PER_PTR */

	/**
	 * Similar to fifo_isempty(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_isempty()
	 */
	INLINE bool fifo_isempty_locked(const FIFOBuffer *fb)
	{
		bool result;
		ATOMIC(result = fifo_isempty(fb));
		return result;
	}


	/**
	 * Similar to fifo_push(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_push()
	 */
	INLINE void fifo_push_locked(FIFOBuffer *fb, unsigned char c)
	{
		ATOMIC(fifo_push(fb, c));
	}

	/* Probably not really needed, but hard to prove. */
	INLINE unsigned char fifo_pop_locked(FIFOBuffer *fb)
	{
		unsigned char c;
		ATOMIC(c = fifo_pop(fb));
		return c;
	}

	/**
	 * Similar to fifo_flush(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_flush()
	 */
	INLINE void fifo_flush_locked(FIFOBuffer *fb)
	{
		ATOMIC(fifo_flush(fb));
	}

#endif /* CPU_REG_BITS < BITS_PER_PTR */


/**
 * Thread safe version of fifo_isfull()
 */
INLINE bool fifo_isfull_locked(const FIFOBuffer *_fb)
{
	bool result;
	ATOMIC(result = fifo_isfull(_fb));
	return result;
}


/**
 * FIFO Initialization.
 */
INLINE void fifo_init(FIFOBuffer *fb, unsigned char *buf, size_t size)
{
	/* FIFO buffers have a known bug with 1-byte buffers. */
	ASSERT(size > 1);

	fb->head = fb->tail = fb->begin = buf;
	fb->end = buf + size - 1;
}

/**
 * \return Lenght of the FIFOBuffer \a fb.
 */
INLINE size_t fifo_len(FIFOBuffer *fb)
{
	return fb->end - fb->begin;
}


#if 0

/*
 * UNTESTED: if uncommented, to be moved in fifobuf.c
 */
void fifo_pushblock(FIFOBuffer *fb, unsigned char *block, size_t len)
{
	size_t freelen;

	/* Se c'e' spazio da tail alla fine del buffer */
	if (fb->tail >= fb->head)
	{
		freelen = fb->end - fb->tail + 1;

		/* C'e' abbastanza spazio per scrivere tutto il blocco? */
		if (freelen < len)
		{
			/* Scrivi quello che entra fino alla fine del buffer */
			memcpy(fb->tail, block, freelen);
			block += freelen;
			len -= freelen;
			fb->tail = fb->begin;
		}
		else
		{
			/* Scrivi tutto il blocco */
			memcpy(fb->tail, block, len);
			fb->tail += len;
			return;
		}
	}

	for(;;)
	{
		while (!(freelen = fb->head - fb->tail - 1))
			Delay(FIFO_POLLDELAY);

		/* C'e' abbastanza spazio per scrivere tutto il blocco? */
		if (freelen < len)
		{
			/* Scrivi quello che entra fino alla fine del buffer */
			memcpy(fb->tail, block, freelen);
			block += freelen;
			len -= freelen;
			fb->tail += freelen;
		}
		else
		{
			/* Scrivi tutto il blocco */
			memcpy(fb->tail, block, len);
			fb->tail += len;
			return;
		}
	}
}
#endif

/** \} */ /* defgroup fifobuf */

#endif /* STRUCT_FIFO_H */
