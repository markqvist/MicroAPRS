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
 * \defgroup list General purpose lists
 * \ingroup struct
 * \{
 *
 * \brief General pourpose double-linked lists
 *
 * Lists contain nodes. You can put any custom struct into any list as long
 * as it has a Node struct inside it. If you make the Node struct the first
 * member of your data type, you can simply cast it to (Node *) when passing
 * it to list functions.
 *
 * Lists must be initialized before use with LIST_INIT(). You can then add
 * objects using ADDHEAD() and ADDTAIL() macros, and remove them with
 * list_remHead() and list_remTail().
 *
 * You can create lists with priorities by using PriNode instead of Node as
 * the base member struct.
 * Use LIST_ENQUEUE() and LIST_ENQUEUE_HEAD() to insert a priority node into
 * a list.
 *
 * To iterate over a list, use the macros FOREACH_NODE() and REVERSE_FOREACH_NODE()
 * in this way:
 * \code
 * struct Foo
 * {
 *     Node n;
 *     int a;
 * }
 *
 * int main()
 * {
 *        List foo_list;
 *        static Foo foo1, foo2;
 *        Foo *fp;
 *
 *        LIST_INIT(&foo_list);
 *        ADDHEAD(&foo_list, (Node *)&foo1);
 *        INSERT_BEFORE(&foo_list, (Node *)&foo2);
 *        FOREACH_NODE(fp, &foo_list)
 *            fp->a = 10;
 * }
 * \endcode
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef STRUCT_LIST_H
#define STRUCT_LIST_H

#include <cfg/compiler.h> /* INLINE */
#include <cfg/debug.h> /* ASSERT_VALID_PTR() */

/**
 * This structure represents a node for bidirectional lists.
 *
 * Data is usually appended to nodes by making them the first
 * field of another struture, as a poor-man's form of inheritance.
 */
typedef struct _Node
{
	struct _Node *succ;
	struct _Node *pred;
} Node;

/**
 * Head of a doubly-linked list of \c Node structs.
 *
 * Lists must be initialized with LIST_INIT() prior to use.
 *
 * Nodes can be added and removed from either end of the list
 * with O(1) performance.  Iterating over these lists can be
 * tricky: use the FOREACH_NODE() macro instead.
 */
typedef struct _List
{
	Node head;
	Node tail;
} List;

/**
 * Extended node for priority queues.
 */
typedef struct _PriNode
{
	Node link;
	int  pri;
} PriNode;


/**
 * Template for a naked node in a list of \a T structures.
 *
 * To be used as data member in other structures:
 *
 * \code
 *    struct Foo
 *    {
 *        DECLARE_NODE_ANON(struct Foo)
 *        int a;
 *        float b;
 *    }
 *
 *    DECLARE_LIST_TYPE(Foo);
 *
 *    void foo(void)
 *    {
 *        static LIST_TYPE(Foo) foo_list;
 *        static Foo foo1, foo2;
 *        Foo *fp;
 *
 *        LIST_INIT(&foo_list);
 *        ADDHEAD(&foo_list, &foo1);
 *        INSERT_BEFORE(&foo_list, &foo2);
 *        FOREACH_NODE(fp, &foo_list)
 *            fp->a = 10;
 *    }
 *
 * \endcode
 */
#define DECLARE_NODE_ANON(T) \
	T *succ; T *pred;

/** Declare a typesafe node for structures of type \a T. */
#define DECLARE_NODE_TYPE(T) \
	typedef struct T##Node { T *succ; T *pred; } T##Node

/** Template for a list of \a T structures. */
#define DECLARE_LIST_TYPE(T) \
	DECLARE_NODE_TYPE(T); \
	typedef struct T##List { \
		 T##Node head; \
		 T##Node tail; \
	} T##List

#define NODE_TYPE(T) T##Node
#define LIST_TYPE(T) T##List

/**
 * Get a pointer to the first node in a list.
 *
 * If \a l is empty, result points to l->tail.
 */
#define LIST_HEAD(l) ((l)->head.succ)

/**
 * Get a pointer to the last node in a list.
 *
 * If \a l is empty, result points to l->head.
 */
#define LIST_TAIL(l) ((l)->tail.pred)

// TODO: move in compiler.h
#if COMPILER_TYPEOF
	#define TYPEOF_OR_VOIDPTR(type) typeof(type)
#else
	#define TYPEOF_OR_VOIDPTR(type) void *
#endif

/**
 * Iterate over all nodes in a list.
 *
 * This macro generates a "for" statement using the following parameters:
 * \param n   Node pointer to be used in each iteration.
 * \param l   Pointer to list.
 */
#define FOREACH_NODE(n, l) \
	for( \
		(n) = (TYPEOF_OR_VOIDPTR(n))LIST_HEAD(l); \
		((Node *)(n))->succ; \
		(n) = (TYPEOF_OR_VOIDPTR(n))(((Node *)(n))->succ) \
	)

/**
 * Iterate backwards over all nodes in a list.
 *
 * This macro generates a "for" statement using the following parameters:
 * \param n   Node pointer to be used in each iteration.
 * \param l   Pointer to list.
 */
#define REVERSE_FOREACH_NODE(n, l) \
	for( \
		(n) = (TYPEOF_OR_VOIDPTR(n))LIST_TAIL(l); \
		((Node *)(n))->pred; \
		(n) = (TYPEOF_OR_VOIDPTR(n))(((Node *)(n))->pred) \
	)

/**
 * Iterate on the list safely against node removal.
 *
 * This macro generates a "for" statement using the following parameters:
 * \param n   Node pointer to be used in each iteration.
 * \param p   Temporal storage for the iterator.
 * \param l   Pointer to list.
 */
#define FOREACH_NODE_SAFE(n, p, l) \
	for( \
		(n) = (TYPEOF_OR_VOIDPTR(n))LIST_HEAD(l), (p) = ((Node *)(n))->succ; \
		((Node *)(n))->succ; \
		(n) = (p), (p) = (TYPEOF_OR_VOIDPTR(n))(((Node *)(n))->succ) \
	)

/** Initialize a list. */
#define LIST_INIT(l) \
	do { \
		(l)->head.succ = (TYPEOF_OR_VOIDPTR((l)->head.succ)) &(l)->tail; \
		(l)->head.pred = NULL; \
		(l)->tail.succ = NULL; \
		(l)->tail.pred = (TYPEOF_OR_VOIDPTR((l)->tail.pred)) &(l)->head; \
	} while (0)

#ifdef _DEBUG
	/** Make sure that a list is valid (it was initialized and is not corrupted). */
	#define LIST_ASSERT_VALID(l) \
		do { \
			Node *n, *pred; \
			ASSERT((l)->head.succ != NULL); \
			ASSERT((l)->head.pred == NULL); \
			ASSERT((l)->tail.succ == NULL); \
			ASSERT((l)->tail.pred != NULL); \
			pred = &(l)->head; \
			FOREACH_NODE(n, l) \
			{ \
				ASSERT(n->pred == pred); \
				pred = n; \
			} \
			ASSERT(n == &(l)->tail); \
		} while (0)

	/// Checks that a node isn't part of a given list
	#define LIST_ASSERT_NOT_CONTAINS(list,node) \
		do { \
			Node *ln; \
			ASSERT_VALID_PTR(list); \
			ASSERT_VALID_PTR(node); \
			FOREACH_NODE(ln, list) \
				ASSERT(ln != (Node *)(node)); \
		} while (0)

	#define INVALIDATE_NODE(n) ((n)->succ = (n)->pred = NULL)
#else
	#define LIST_ASSERT_VALID(l) do {} while (0)
	#define LIST_ASSERT_NOT_CONTAINS(list,node) do {} while (0)
	#define INVALIDATE_NODE(n) do {} while (0)
#endif

/** Tell whether a list is empty. */
#define LIST_EMPTY(l)  ( (void *)((l)->head.succ) == (void *)(&(l)->tail) )

/** Add node to list head. */
#define ADDHEAD(l,n) \
	do { \
		LIST_ASSERT_NOT_CONTAINS((l),(n)); \
		(n)->succ = (l)->head.succ; \
		(n)->pred = (l)->head.succ->pred; \
		(n)->succ->pred = (n); \
		(n)->pred->succ = (n); \
	} while (0)

/** Add node to list tail. */
#define ADDTAIL(l,n) \
	do { \
		LIST_ASSERT_NOT_CONTAINS((l),(n)); \
		(n)->succ = &(l)->tail; \
		(n)->pred = (l)->tail.pred; \
		(n)->pred->succ = (n); \
		(l)->tail.pred = (n); \
	} while (0)

/**
 * Insert node \a n before node \a ln.
 *
 * \note You can't pass in a list header as \a ln, but
 *       it is safe to pass list-\>head of an empty list.
 */
#define INSERT_BEFORE(n,ln) \
	do { \
		ASSERT_VALID_PTR(n); \
		ASSERT_VALID_PTR(ln); \
		(n)->succ = (ln); \
		(n)->pred = (ln)->pred; \
		(ln)->pred->succ = (n); \
		(ln)->pred = (n); \
	} while (0)

/**
 * Remove \a n from whatever list it is in.
 *
 * \note Removing a node that has not previously been
 *       inserted into a list invokes undefined behavior.
 */
#define REMOVE(n) \
	do { \
		ASSERT_VALID_PTR(n); \
		(n)->pred->succ = (n)->succ; \
		(n)->succ->pred = (n)->pred; \
		INVALIDATE_NODE(n); \
	} while (0)

/**
 * Insert a priority node in a priority queue.
 *
 * The new node is inserted immediately before the first node with the same
 * priority or appended to the tail if no such node exists.
 */
#define LIST_ENQUEUE_HEAD(list, node) \
	do { \
		PriNode *ln; \
		LIST_ASSERT_NOT_CONTAINS((list),(node)); \
		FOREACH_NODE(ln, (list)) \
			if (ln->pri <= (node)->pri) \
				break; \
		INSERT_BEFORE(&(node)->link, &ln->link); \
	} while (0)

/**
 * Insert a priority node in a priority queue.
 *
 * The new node is inserted immediately before the first node with lower
 * priority or appended to the tail if no such node exists.
 */
#define LIST_ENQUEUE(list, node) \
	do { \
		PriNode *ln; \
		LIST_ASSERT_NOT_CONTAINS((list),(node)); \
		FOREACH_NODE(ln, (list)) \
			if (ln->pri < (node)->pri) \
				break; \
		INSERT_BEFORE(&(node)->link, &ln->link); \
	} while (0)


/**
 * Unlink a node from the head of the list \a l.
 *
 * \return Pointer to node, or NULL if the list was empty.
 */
INLINE Node *list_remHead(List *l)
{
	Node *n;

	ASSERT_VALID_PTR(l);

	if (LIST_EMPTY(l))
		return (Node *)0;

	n = l->head.succ; /* Get first node. */
	l->head.succ = n->succ; /* Link list head to second node. */
	n->succ->pred = &l->head; /* Link second node to list head. */

	INVALIDATE_NODE(n);
	return n;
}

/**
 * Unlink a node from the tail of the list \a l.
 *
 * \return Pointer to node, or NULL if the list was empty.
 */
INLINE Node *list_remTail(List *l)
{
	Node *n;

	ASSERT_VALID_PTR(l);

	if (LIST_EMPTY(l))
		return NULL;

	n = l->tail.pred; /* Get last node. */
	l->tail.pred = n->pred; /* Link list tail to second last node. */
	n->pred->succ = &l->tail; /* Link second last node to list tail. */

	INVALIDATE_NODE(n);
	return n;
}

/** \} */ //defgroup list

#endif /* STRUCT_LIST_H */
