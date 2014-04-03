#include "cfg/cfg_lwip.h"

#define LOG_LEVEL  3 //INFO
#define LOG_FORMAT 0 //TERSE
#include <cfg/log.h>

#include <drv/timer.h>

#include <cpu/power.h>
#include <cpu/types.h>

#include <arch/sys_arch.h>
#include <lwip/sys.h>

#include <kern/signal.h>
#include <kern/msg.h>
#include <kern/proc.h>
#include <kern/proc_p.h>

#include <struct/heap.h>

#include <mware/event.h>

/****************************************************************************/

/*
 * Generic mutex (binary semaphore) implementation
 *
 * TODO: move this to a different place (i.e., bertos/kern/sem.c).
 */
INLINE void mutex_verify(struct Mutex *s)
{
	(void)s;
	ASSERT(s);
	LIST_ASSERT_VALID(&s->wait_queue);
	ASSERT((s->count == MUTEX_LOCKED) || (s->count == MUTEX_UNLOCKED));
}

bool mutex_attempt(struct Mutex *s)
{
	return cpu_atomic_xchg(&s->count, MUTEX_LOCKED) == MUTEX_UNLOCKED;
}

static NOINLINE void mutex_slowpath_obtain(struct Mutex *s)
{
	PROC_ATOMIC(
		mutex_verify(s);
		ADDTAIL(&s->wait_queue, (Node *)current_process)
	);
	proc_switch();
}

void mutex_obtain(struct Mutex *s)
{
	if (UNLIKELY(cpu_atomic_xchg(&s->count, MUTEX_LOCKED)) !=
				MUTEX_UNLOCKED)
		mutex_slowpath_obtain(s);
}

void mutex_release(struct Mutex *s)
{
	Process *proc = NULL;

	PROC_ATOMIC(
		mutex_verify(s);
		proc = (Process *)list_remHead(&s->wait_queue);
		if (!proc)
			s->count = 1;
	);
	if (proc)
		ATOMIC(proc_wakeup(proc));
}

void mutex_init(struct Mutex *s)
{
	LIST_INIT(&s->wait_queue);
	s->count = 1;
}

/****************************************************************************/

typedef struct SemNode
{
	Node node;
	Mutex sem;
} SemNode;

#define MAX_SEM_CNT 16

static struct SemNode sem_pool[MAX_SEM_CNT];
static List free_sem;

/**
 * Creates and returns a new semaphore.
 *
 * \param count Specifies the initial state of the semaphore.
 * \return The semaphore or SYS_SEM_NULL on error.
 */
sys_sem_t sys_sem_new(u8_t count)
{
	SemNode *sem;

	PROC_ATOMIC(sem = (SemNode *)list_remHead(&free_sem));
	if (UNLIKELY(!sem))
	{
		LOG_ERR("Out of semaphores!\n");
		return SYS_SEM_NULL;
	}

	mutex_init(&sem->sem);
	// must obtain semaphore depending on the parameter
	// NOTE: count == 1 means that the semaphore is unlocked
	if (count <= 0)
		mutex_obtain(&sem->sem);
	return (sys_sem_t)&sem->sem;
}

/**
 * Frees a semaphore created by sys_sem_new.
 *
 * \param semaphore Mutex to be freed
 */
void sys_sem_free(sys_sem_t semaphore)
{
	SemNode *sem = containerof(semaphore, SemNode, sem);
	PROC_ATOMIC(ADDHEAD(&free_sem, &sem->node));
}

/**
 * Signals (or releases) a semaphore.
 */
void sys_sem_signal(sys_sem_t sem)
{
	mutex_release(sem);
}

/**
 * Blocks the thread while waiting for the semaphore to be signaled.
 *
 * The timeout parameter specifies how many milliseconds the function should block
 * before returning; if the function times out, it should return SYS_ARCH_TIMEOUT.
 * If timeout=0, then the function should block indefinitely.
 * If the function acquires the semaphore, it should return how many milliseconds
 * expired while waiting for the semaphore.
 * The function may return 0 if the semaphore was immediately available.
 */
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
	ticks_t end, start = timer_clock();

	if (timeout == 0)
	{
		mutex_obtain(sem);
		return ticks_to_ms(timer_clock() - start);
	}

	do
	{
		cpu_relax();
		end = timer_clock();
	} while ((end - start < ms_to_ticks(timeout) && !mutex_attempt(sem)));

	return (end - start > ms_to_ticks(timeout)) ?
			SYS_ARCH_TIMEOUT : (u32_t)ticks_to_ms(end - start);
}

/* Mbox functions */

typedef struct IpPort
{
	Node node;
	MsgPort port;
} IpPort;

#define MAX_PORT_CNT 16
static struct IpPort port_pool[MAX_PORT_CNT];
static List free_port;

typedef struct IpMsg
{
	Msg msg;
	void *data;
} IpMsg;

#define MAX_MSG_CNT 32
static struct IpMsg msg_pool[MAX_MSG_CNT];
static List free_msg;

// TODO: allocate memory for 'size' messages
sys_mbox_t sys_mbox_new(UNUSED_ARG(int, size))
{
	IpPort *port;

	PROC_ATOMIC(port = (IpPort *)list_remHead(&free_port));
	if (UNLIKELY(!port))
	{
		LOG_ERR("Out of message ports!\n");
		return SYS_MBOX_NULL;
	}
	msg_initPort(&port->port, event_createGeneric());
	port->port.event.Ev.Sig.sig_proc = NULL;

	return (sys_mbox_t)(&port->port);
}

void sys_mbox_free(sys_mbox_t mbox)
{
	IpPort *port = containerof(mbox, IpPort, port);
	PROC_ATOMIC(ADDHEAD(&free_port, &port->node));
}

void sys_mbox_post(sys_mbox_t mbox, void *data)
{
	sys_mbox_trypost(mbox, data);
}

/*
 * Try to post the "msg" to the mailbox. Returns ERR_MEM if this one
 * is full, else, ERR_OK if the "msg" is posted.
 */
err_t sys_mbox_trypost(sys_mbox_t mbox, void *data)
{
	IpMsg *msg;

	PROC_ATOMIC(msg = (IpMsg *)list_remHead(&free_msg));
	if (UNLIKELY(!msg))
	{
		LOG_ERR("out of messages!\n");
		return ERR_MEM;
	}
	msg->data = data;

	msg_lockPort(mbox);
	ADDTAIL(&mbox->queue, &msg->msg.link);
	msg_unlockPort(mbox);

	if (mbox->event.Ev.Sig.sig_proc)
		event_do(&mbox->event);

	return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **data, u32_t timeout)
{
	/* Blocks the thread until a message arrives in the mailbox, but does
	not block the thread longer than "timeout" milliseconds (similar to
	the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
	be blocked until a message arrives. The "msg" argument is a result
	parameter that is set by the function (i.e., by doing "*msg =
	ptr"). The "msg" parameter maybe NULL to indicate that the message
	should be dropped.

	The return values are the same as for the sys_arch_sem_wait() function:
	Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
	timeout.

	Note that a function with a similar name, sys_mbox_fetch(), is
	implemented by lwIP.
	*/

	Msg *msg;
	ticks_t start = timer_clock();

	while (1)
	{
		/* Fast path */
		msg = msg_get(mbox);
		if (LIKELY(msg))
			break;

		mbox->event.Ev.Sig.sig_proc = proc_current();
		/* Slow path */
		if (!timeout)
			event_wait(&mbox->event);
		else
		{
			if (!event_waitTimeout(&mbox->event,
					ms_to_ticks(timeout)))
			{
				mbox->event.Ev.Sig.sig_proc = NULL;
				return SYS_ARCH_TIMEOUT;
			}
		}
	}
	mbox->event.Ev.Sig.sig_proc = NULL;
	if (data)
		*data = containerof(msg, IpMsg, msg)->data;

	PROC_ATOMIC(ADDHEAD(&free_msg, &msg->link));

	return ticks_to_ms(timer_clock() - start);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **data)
{
	/* This is similar to sys_arch_mbox_fetch, however if a message is not
	present in the mailbox, it immediately returns with the code
	SYS_MBOX_EMPTY. On success 0 is returned.

	To allow for efficient implementations, this can be defined as a
	function-like macro in sys_arch.h instead of a normal function. For
	example, a naive implementation could be:
	#define sys_arch_mbox_tryfetch(mbox,msg) \
	  sys_arch_mbox_fetch(mbox,msg,1)
	although this would introduce unnecessary delays.
	*/

	Msg *msg;

	msg = msg_get(mbox);
	if (UNLIKELY(!msg))
		return SYS_MBOX_EMPTY;
	if (data)
		*data = containerof(msg, IpMsg, msg)->data;
	PROC_ATOMIC(ADDHEAD(&free_msg, &msg->link));

	return 0;
}

typedef struct ThreadNode
{
	Node node;
	struct Process *pid;
	void (*entry)(void *);
	void *arg;
	struct sys_timeouts timeout;
} ThreadNode;

#define MAX_THREAD_CNT 8

static ThreadNode thread_pool[MAX_THREAD_CNT];
static List free_thread;
static List used_thread;

static struct sys_timeouts lwip_system_timeouts; // Default timeouts list for lwIP

struct sys_timeouts *sys_arch_timeouts(void)
{
	ThreadNode *thread_node;
	struct Process *curr_pid = proc_current();

	FOREACH_NODE(thread_node, &used_thread)
	{
		if (thread_node->pid == curr_pid)
			return &(thread_node->timeout);
	}

	return &lwip_system_timeouts;
}

static void thread_trampoline(void)
{
	ThreadNode *thread_node = (ThreadNode *)proc_currentUserData();

	thread_node->entry(thread_node->arg);
}

#if !CONFIG_KERN_HEAP
/*
 * NOTE: threads are never destroyed, consequently these stacks are never
 * deallocated. So, the stack allocator can be implemented as a simple index
 * that is atomically incremented at each allocation.
 */
static cpu_stack_t thread_stack[MAX_THREAD_CNT]
			[DEFAULT_THREAD_STACKSIZE / sizeof(cpu_stack_t)]
				ALIGNED(sizeof(cpu_stack_t));
static int last_stack;
#endif

sys_thread_t sys_thread_new(const char *name, void (* thread)(void *arg),
				void *arg, int stacksize, int prio)
{
	ThreadNode *thread_node;
	cpu_stack_t *stackbase;

	proc_forbid();
	thread_node = (ThreadNode *)list_remHead(&free_thread);
	if (UNLIKELY(!thread_node))
	{
		proc_permit();
		LOG_ERR("Out of threads!\n");
		return NULL;
	}
	ADDHEAD(&used_thread, &thread_node->node);
	proc_permit();

	thread_node->entry = thread;
	thread_node->arg = arg;

	#if !CONFIG_KERN_HEAP
		ASSERT(stacksize <= DEFAULT_THREAD_STACKSIZE);
		PROC_ATOMIC(stackbase = thread_stack[last_stack++]);
	#else
		stackbase = NULL;
	#endif
	thread_node->pid = proc_new_with_name(name, thread_trampoline,
				(void *)thread_node, stacksize, stackbase);
	if (thread_node->pid == NULL)
		return NULL;

	#if CONFIG_KERN_PRI
		proc_setPri(thread_node->pid, prio);
	#else
		/* Avoid warnings when priorities are disabled */
		(void) prio;
	#endif

	return thread_node->pid;
}

void sys_init(void)
{
	LIST_INIT(&free_sem);
	LIST_INIT(&free_port);
	LIST_INIT(&free_msg);
	LIST_INIT(&free_thread);
	LIST_INIT(&used_thread);

	for (int i = 0; i < MAX_SEM_CNT; ++i)
		ADDHEAD(&free_sem, &sem_pool[i].node);

	for (int i = 0; i < MAX_PORT_CNT; ++i)
		ADDHEAD(&free_port, &port_pool[i].node);

	for (int i = 0; i < MAX_MSG_CNT; ++i)
		ADDHEAD(&free_msg, &msg_pool[i].msg.link);

	for (int i = 0; i < MAX_THREAD_CNT; ++i)
		ADDHEAD(&free_thread, &thread_pool[i].node);
}
