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
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief ARM context switch
 *
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Andrea Righi <arighi@develer.com>
 */

#include "cfg/cfg_proc.h"

/* void asm_switch_context(void **new_sp [r0], void **save_sp [r1]) */
.globl asm_switch_context
asm_switch_context:
	/* Save registers */
	stmfd	sp!, {r4 - r11, lr}
	/* Save old stack pointer */
	str     sp, [r1]
	/* Load new stack pointer */
	ldr	sp, [r0]
	/* Load new registers */
	ldmfd	sp!, {r4 - r11, pc}

#if CONFIG_KERN_PREEMPT

/* ARM interrupt mode with IRQ and FIQ disabled */
#define ARM_IRQ_MODE 0xD2
/* ARM supervisor mode with IRQ and FIQ disabled */
#define ARM_SVC_MODE 0xD3

.globl asm_irq_switch_context
asm_irq_switch_context:
	/* Return if preemption is not needed */
	bl	proc_needPreempt
	cmp	r0, #0
	ldmeqfd	sp!, {r0 - r3, ip, pc}^

	/* Otherwise restore regs used by the ISR */
	ldmfd   sp!, {r0 - r3, ip, lr}

	/* Save current process context */
	msr     cpsr_c, #ARM_SVC_MODE
	stmfd   sp!, {r0 - r3, ip, lr}

	/* Save lr_irq and spsr_irq in process stack */
	msr     cpsr_c, #ARM_IRQ_MODE
	mov     r0, lr
	mrs     r1, spsr
	msr     cpsr_c, #ARM_SVC_MODE
	stmfd   sp!, {r0, r1}

	/* Perform the context switch */
	bl	proc_preempt

	/* Restore lr_irq and spsr_irq from process stack */
	ldmfd   sp!, {r0, r1}
	msr     cpsr_c, #ARM_IRQ_MODE
	mov     lr, r0
	msr     spsr_cxsf, r1

	/* Restore process regs */
	msr     cpsr_c, #ARM_SVC_MODE
	ldmfd   sp!, {r0 - r3, ip, lr}

	/* Exit from ISR */
	msr     cpsr_c, #ARM_IRQ_MODE
	movs	pc, lr
#endif /* CONFIG_KERN_PREEMPT */
