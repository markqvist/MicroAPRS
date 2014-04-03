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
 * Copyright 2004, 2008, 2011 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \defgroup pool Pool memory allocator
 * \ingroup struct
 * \{
 *
 * \brief Pool macros.
 *
 * The pool module provides the boilerplate code to create a set of objects
 * of the same type.
 * It provides an interface similar to the heap module, with pool_alloc() and
 * pool_free() functions that allocate and free an element respectively.
 * In contrast with the heap module, you can specify exactly the number of
 * items that you want to be in the pool.
 *
 * Items in the pool must be a derived class of <tt>Node *</tt>, which also
 * means that they can be used as-is with list containers, eg. MsgPort.
 *
 * Example code:
 * \code
 * typedef struct MyType
 * {
 *     Node *n;
 *     uint16_t *buf;
 *     // other members here...
 * } MyType;
 *
 * DECLARE_POOL(mypool, MyType, POOL_SIZE);
 *
 * static void elem_init(MyType *e)
 * {
 *     e->buf = NULL;
 *     // other initializations here
 * }
 *
 * int main(void)
 * {
 *     pool_init(&mypool, elem_init);
 *
 *     MyType *foo = pool_alloc(&mypool);
 *     // do stuff with foo
 *     pool_free(&mypool, foo);
 * }
 * \endcode
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#ifndef STRUCT_POOL_H
#define STRUCT_POOL_H

#include <cfg/macros.h>
#include <struct/list.h>

/**
 * \brief Extern pool declaration
 */
#define EXTERN_POOL(name) \
	extern List name

#define DECLARE_POOL_WITH_STORAGE(name, type, num, storage) \
	static type name##_items[num]; \
	storage name; \
	INLINE void name##_init(void (*init_func)(type*)) \
	{ \
		size_t i; \
		LIST_INIT(&name); \
		for (i=0;i<countof(name##_items);++i) \
		{ \
			if (init_func) init_func(&name##_items[i]); \
			ADDTAIL(&name, (Node*)&name##_items[i]); \
		} \
	} \
	INLINE void name##_init(void (*init_func)(type*)) \
	/**/

/**
 * \brief Helper macro to declare a Pool data type.
 *
 * Data type inserted into the pool must be a <tt>Node *</tt>
 * type.
 *
 * \param name Variable name of the pool.
 * \param type Data type held by the pool.
 * \param num Number of elements in pool.
 */
#define DECLARE_POOL(name, type, num) \
	DECLARE_POOL_WITH_STORAGE(name, type, num, List)

/**
 * \brief Static Pool declaration
 *
 * \sa DECLARE_POOL
 */
#define DECLARE_POOL_STATIC(name, type, num) \
	DECLARE_POOL_WITH_STORAGE(name, type, num, static List)

/**
 * Initialize the pool \a name, calling \a init_func on each element.
 *
 * The init function must have the following prototype:
 * \code
 * void init_func(type *)
 * \endcode
 * where \a type is the type of objects held in the pool.
 *
 * \param name Pool to initialize
 * \param init_func Init function to be called on each element
 */
#define pool_init(name, init_func)     (*(name##_init))(init_func)

/**
 * \brief Allocate an element from the pool.
 *
 * The returned element is of type <tt>Node *</tt>, it's safe to
 * cast it to the type contained in the pool.
 *
 * \note If the element was recycled with pool_free(), it will not be reset,
 * so don't assume that the element has specific values.
 *
 * \param name Pointer to pool to alloc from.
 * \return Element of the type present in the pool.
 */
#define pool_alloc(name)               list_remHead(name)

/**
 * \brief Recycle an element into the pool
 *
 * \note Element fields are not reset to its original values, keep that in
 * mind when you alloc nodes.
 *
 * \param name Pointer to pool where the node should be recycled.
 * \param elem Element to be recycled.
 */
#define pool_free(name, elem)          ADDHEAD(name, (Node*)elem)

/**
 * \brief Test if the pool is empty
 *
 * \param name Pointer to pool.
 * \return True if the pool is empty, false otherwise.
 */
#define pool_empty(name)               LIST_EMPTY(name)

 /** \} */ /* defgroup pool */

#endif /* STRUCT_POOL_H */
