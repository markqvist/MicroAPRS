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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Cortex-M3 context switch
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/compiler.h>
#include <cfg/cfg_proc.h> /* CONFIG_KERN_PREEMPT */
#include <cpu/irq.h> /* IRQ_PRIO_DISABLED */
#include <cpu/types.h> /* cpu_stack_t */
#include <kern/proc_p.h> /* asm_switch_context() prototype */
#include <kern/proc.h> /* proc_preempt() */
#include "switch_ctx_cm3.h"

#if CONFIG_KERN_PREEMPT
/*
 * Kernel preemption: implementation details.
 *
 * The kernel preemption is implemented using the PendSV IRQ. Inside the
 * SysTick handler when a process needs to be interrupted (expires its time
 * quantum or a high-priority process is awakend) a pending PendSV call is
 * triggered.
 *
 * The PendSV handler is called immediately after the SysTick handler, using
 * the architecture's tail-chaining functionality (an ISR call without the
 * overhead of state saving and restoration between different IRQs). Inside the
 * PendSV handler we perform the stack-switching between the old and new
 * processes.
 *
 * Voluntary context switch is implemented as a soft-interrupt call (SVCall),
 * so any process is always suspended and resumed from an interrupt context.
 *
 * NOTE: interrupts must be disabled or enabled when resuming a process context
 * depending of the type of the previous suspension. If a process was suspended
 * by a voluntary context switch IRQs must be disabled on resume (voluntary
 * context switch always happen with IRQs disabled). Instead, if a process was
 * suspended by the kernel preemption IRQs must be always re-enabled, because
 * the PendSV handler resumes directly the process context. To keep track of
 * this, we save the state of the IRQ priority in register r3 before performing
 * the context switch.
 *
 * If CONFIG_KERN_PREEMPT is not enabled the cooperative implementation
 * fallbacks to the default stack-switching mechanism, performed directly in
 * thread-mode and implemented as a normal function call.
 */

/*
 * Voluntary context switch handler.
 */
void NAKED svcall_handler(void)
{
	asm volatile (
	/* Save context */
		"mrs r3, basepri\n\t"
		"mrs ip, psp\n\t"
		"stmdb ip!, {r3-r11, lr}\n\t"
	/* Stack switch */
		"str ip, [r1]\n\t"
		"ldr ip, [r0]\n\t"
	/* Restore context */
		"ldmia ip!, {r3-r11, lr}\n\t"
		"msr psp, ip\n\t"
		"msr basepri, r3\n\t"
		"bx lr" : : : "memory");
}

/*
 * Preemptible context switch handler.
 */
void NAKED pendsv_handler(void)
{
	register cpu_stack_t *stack asm("ip");

	asm volatile (
		"mrs r3, basepri\n\t"
		"mov %0, %2\n\t"
		"msr basepri, %0\n\t"
		"mrs %0, psp\n\t"
		"stmdb %0!, {r3-r11, lr}\n\t"
		: "=r"(stack)
		: "r"(stack), "i"(IRQ_PRIO_DISABLED)
		: "r3", "memory");
	proc_current()->stack = stack;
	proc_preempt();
	stack = proc_current()->stack;
	asm volatile (
		"ldmia %0!, {r3-r11, lr}\n\t"
		"msr psp, %0\n\t"
		"msr basepri, r3\n\t"
		"bx lr"
		: "=r"(stack) : "r"(stack)
		: "memory");
}
#else /* !CONFIG_KERN_PREEMPT */
#ifdef __IAR_SYSTEMS_ICC__
#else /* __IAR_SYSTEMS_ICC__ */
void NAKED asm_switch_context(cpu_stack_t **new_sp, cpu_stack_t **old_sp)
{
	register cpu_stack_t **_new_sp asm("r0") = new_sp;
	register cpu_stack_t **_old_sp asm("r1") = old_sp;

	asm volatile (
		"mrs ip, psp\n\t"
		/* Save registers */
		"stmdb ip!, {r4-r11, lr}\n\t"
		/* Save old stack pointer */
		"str ip, [%1]\n\t"
		/* Load new stack pointer */
		"ldr ip, [%0]\n\t"
		/* Load new registers */
		"ldmia ip!, {r4-r11, lr}\n\t"
		"msr psp, ip\n\t"
		"bx lr"
		: : "r"(_new_sp), "r"(_old_sp) : "ip", "memory");
}
#endif /* __IAR_SYSTEMS_ICC__ */
#endif /* CONFIG_KERN_PREEMPT */
