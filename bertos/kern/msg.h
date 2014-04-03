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
 * Copyright 1999,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \defgroup kern_msg Message box IPC
 * \ingroup kern
 * \{
 *
 * \brief Simple inter-process messaging system
 *
 * This module implements a common system for executing
 * a user defined action calling a hook function.
 *
 * A message port is an abstraction used to exchange information
 * asynchronously between processes or other entities such as
 * interrupts and call-back functions.
 *
 * This form of IPC is higher-level than bare signals and
 * semaphores, because it sets a policy for exchanging
 * structured data with well-defined synchronization and
 * ownership semantics.
 *
 * Before using it, a message port must be initialized by
 * calling msg_initPort(), which associates the port with
 * an Event object, which can be setup to signal a process
 * or invoke a call-back hook.
 *
 * A process or interrupt routine can deliver messages to any
 * message port by calling msg_put().  By sending a message,
 * the sender temporarly or permanently transfers ownership
 * of its associated data to the receiver.
 *
 * Queuing a message to a port automatically triggers the
 * associated Event to notify the receiver.  When the
 * receiver wakes up, it usually invokes msg_get() to pick
 * the next message from the port.
 *
 * \note
 * When you put a message into a port, such message becomes
 * unavailable until you retrieve it using msg_get(), eg.
 * you must not delete it or put it into another port.
 *
 * Message ports can hold any number of pending messages,
 * and receivers usually process them in FIFO order.
 * Other scheduling policies are possible, but not implemented
 * in this API.
 *
 * After the receiver has done processing a message, it replies
 * it back to the sender with msg_reply(), which transfer
 * ownership back to the original sender.  Replies are delivered
 * to a reply port, which is nothing more than another MsgPort
 * structure designated by the sender.
 *
 * Returning messages to senders is not mandatory, but it provides
 * a convenient way to provide some kind of result and simplify
 * the resource allocation scheme at the same time.
 *
 * When using signals to receive messages in a process, you
 * call sig_wait() in an event-loop to wake up when messages
 * are delivered to any of your ports.  When your process
 * wakes up with the port signal active, multiple messages
 * may already have queued up at the message port, and the
 * process must process them all before returning to sleep.
 * Signals don't keep a nesting count.
 *
 * A simple message loop works like this:
 *
 * \code
 *	// Our message port.
 *	static MsgPort test_port;
 *
 *	// A test message with two parameters and a result.
 *	typedef struct
 *	{
 *		Msg msg;
 *
 *		int x, y;
 *		int result;
 *	} TestMsg;
 *
 *
 *  PROC_DEFINE_STACK(sender_stack, KERN_MINSTACKSIZE);
 *
 *	// A process that sends two messages and waits for replies.
 *	static void sender_proc(void)
 *	{
 *		MsgPort test_reply_port;
 *		TestMsg msg1;
 *		TestMsg msg2;
 *		Msg *reply;
 *
 *		msg_initPort(&test_reply_port,
 *			event_createSignal(proc_current(), SIG_SINGLE);
 *
 *		// Fill-in first message and send it out.
 *		msg1.x = 3;
 *		msg1.y = 2;
 *		msg1.msg.replyPort = &test_reply_port;
 *		msg_put(&test_port, &msg1.msg);
 *
 *		// Fill-in second message and send it out too.
 *		msg2.x = 5;
 *		msg2.y = 4;
 *		msg2.msg.replyPort = &test_reply_port;
 *		msg_put(&test_port, &msg2.msg);
 *
 *		// Wait for a reply...
 *		sig_wait(SIG_SINGLE);
 *
        reply = containerof(msg_get(&test_reply_port), TestMsg, msg);
 *		ASSERT(reply != NULL);
 *		ASSERT(reply->result == 5);
 *
 *		// Get reply to second message.
 *		while (!(reply = containerof(msg_get(&test_reply_port), TestMsg, msg)))
 *		{
 *			// Not yet, be patient and wait some more.
 *			sig_wait(SIG_SINGLE);
 *		}
 *
 *		ASSERT(reply->result == 9);
 *	}
 *
 *
 *	// Receive messages and do something boring with them.
 *	static void receiver_proc(void)
 *	{
 *		msg_initPort(&test_port,
 *			event_createSignal(proc_current(), SIG_EXAMPLE);
 *
 *		proc_new(sender_proc, NULL,sizeof(sender_stack), sender_stack);
 *
 *		for (;;)
 *		{
 *			sigmask_t sigs = sig_wait(SIG_EXAMPLE | more_signals);
 *
 *			if (sigs & SIG_EXAMPLE)
 *			{
 *				TestMsg *emsg;
 *				while((emsg = containerof(msg_get(&test_port), TestMsg, msg)))
 *				{
 *					// Do something with the message
 *					emsg->result = emsg->x + emsg->y;
 *					msg_reply(emsg->msg);
 *				}
 *			}
 *		}
 *	}
 * \endcode
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "msg"
 * $WIZ$ module_depends = "event", "signal", "kernel"
 */


#ifndef KERN_MSG_H
#define KERN_MSG_H

#include <mware/event.h>
#include <struct/list.h>
#include <kern/proc.h>

typedef struct MsgPort
{
	List  queue;   /**< Messages queued at this port. */
	Event event;   /**< Event to trigger when a message arrives. */
} MsgPort;


typedef struct Msg
{
	Node     link;      /**< Link into message port queue. */
	MsgPort *replyPort; /**< Port to which the msg is to be replied. */
	/* User data may follow */
} Msg;


/**
 * Lock a message port.
 *
 * This is required before reading or manipulating
 * any field of the MsgPort structure.
 *
 * \note Ports may be locked multiple times and each
 *       call to msg_lockPort() must be paired with
 *       a corresponding call to msg_unlockPort().
 *
 * \todo Add a configurable policy for locking against
 *       interrupts and locking with semaphorse.
 *
 * \see msg_unlockPort()
 */
INLINE void msg_lockPort(UNUSED_ARG(MsgPort *, port))
{
	proc_forbid();
}

/**
 * Unlock a message port.
 *
 * \see msg_lockPort()
 */
INLINE void msg_unlockPort(UNUSED_ARG(MsgPort *, port))
{
	proc_permit();
}


/** Initialize a message port */
INLINE void msg_initPort(MsgPort *port, Event event)
{
	LIST_INIT(&port->queue);
	port->event = event;
}

/** Queue \a msg into \a port, triggering the associated event */
INLINE void msg_put(MsgPort *port, Msg *msg)
{
	msg_lockPort(port);
	ADDTAIL(&port->queue, &msg->link);
	msg_unlockPort(port);

	event_do(&port->event);
}

/**
 * Get the first message from the queue of \a port.
 *
 * \return Pointer to the message or NULL if the port was empty.
 */
INLINE Msg *msg_get(MsgPort *port)
{
	Msg *msg;

	msg_lockPort(port);
	msg = (Msg *)list_remHead(&port->queue);
	msg_unlockPort(port);

	return msg;
}

/** Peek the first message in the queue of \a port, or NULL if the port is empty. */
INLINE Msg *msg_peek(MsgPort *port)
{
	Msg *msg;

	msg_lockPort(port);
	msg = (Msg *)port->queue.head.succ;
	if (LIST_EMPTY(&port->queue))
		msg = NULL;
	msg_unlockPort(port);

	return msg;
}

/** Send back (reply) \a msg to its sender. */
INLINE void msg_reply(Msg *msg)
{
	msg_put(msg->replyPort, msg);
}

/** \} */ //defgroup kern_msg

int msg_testRun(void);
int msg_testSetup(void);
int msg_testTearDown(void);

#endif /* KERN_MSG_H */
