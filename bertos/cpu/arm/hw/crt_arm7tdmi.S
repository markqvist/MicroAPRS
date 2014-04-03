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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief ARM7TDMI CRT.
 */

#define ARM_MODE_USR   0x10
#define ARM_MODE_FIQ   0x11
#define ARM_MODE_IRQ   0x12
#define ARM_MODE_SVC   0x13
#define ARM_MODE_ABORT 0x17
#define ARM_MODE_UNDEF 0x1B
#define ARM_MODE_SYS   0x1F

#define IRQ_BIT        0x80
#define FIQ_BIT        0x40


/*
 * Hardware initialization.
 */
        .section .init, "ax", %progbits
__init0:
        /*
         * Set stack pointers
         */
        ldr     r0, =__stack_fiq_end
        msr     CPSR_c, #ARM_MODE_FIQ | IRQ_BIT | FIQ_BIT
        mov     r13, r0
        ldr     r0, =__stack_irq_end
        msr     CPSR_c, #ARM_MODE_IRQ | IRQ_BIT | FIQ_BIT
        mov     r13, r0
        ldr     r0, =__stack_abt_end
        msr     CPSR_c, #ARM_MODE_ABORT | IRQ_BIT | FIQ_BIT
        mov     r13, r0
        ldr     r0, =__stack_und_end
        msr     CPSR_c, #ARM_MODE_UNDEF | IRQ_BIT | FIQ_BIT
        mov     r13, r0
        ldr     r0, =__stack_svc_end
        msr     CPSR_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT
        mov     r13, r0

	/*
	 * Early hw initialization #1.
	 * Called before clearing .bss and
	 * loading .data segments.
	 */
	bl	__init1

	/*
	 * Clear .bss
	 */
        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0

bss_loop:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     bss_loop

        /*
         * Relocate .data section (Copy from ROM to RAM).
         */
        ldr     r1, =__etext
        ldr     r2, =__data_start
        ldr     r3, =__data_end

data_loop:
        cmp     r2, r3
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     data_loop

	/*
	 * Early hw initialization #2.
	 * Called after setting up .bss and .data segments
	 * but before calling main().
	 */
	bl	__init2

        /*
         * Jump to main
         */
        bl	main

end:
        b       end

__dummy_init:
	mov	pc, lr

	/*
	 * Redefine your own __init() in order to supply
	 * a completely custom initialization routine.
	 */
        .weak   __init
        .set    __init, __init0

	/*
	 * Redefine your own __init1() in order to supply
	 * an hardware initialization routine.
	 * Remember that __init1() is called *before*
	 * clearing .bss and loading .data sections.
	 */
        .weak   __init1
        .set    __init1, __dummy_init

	/*
	 * Redefine your own __init2() in order to supply
	 * an hardware initialization routine.
	 * Remember that __init2() is called *after*
	 * clearing .bss and loading .data sections, just
	 * before calling main().
	 */
        .weak   __init2
        .set    __init2, __dummy_init

	/*
	 * Redefine your own __undef() in order to supply
	 * a custom handler for undefined instruction exception.
	 */
        .weak   __undef
        .set    __undef, __xcpt_dummy_undef

	/*
	 * Redefine your own __swi() in order to supply
	 * a custom handler for software interrupt exception.
	 */
        .weak   __swi
        .set    __swi, __xcpt_dummy_swi

	/*
	 * Redefine your own __prefetch_abort() in order to supply
	 * a custom handler for prefetch abort exception.
	 */
        .weak   __prefetch_abort
        .set    __prefetch_abort, __xcpt_dummy_pref

	/*
	 * Redefine your own __data_abort() in order to supply
	 * a custom handler for data abort exception.
	 */
        .weak   __data_abort
        .set    __data_abort, __xcpt_dummy_dab

        .ltorg

	.section .exceptions, "ax", %progbits

__xcpt_dummy_undef:
        b       __xcpt_dummy_undef

__xcpt_dummy_swi:
        b       __xcpt_dummy_swi

__xcpt_dummy_pref:
        b       __xcpt_dummy_pref

__xcpt_dummy_dab:
        b       __xcpt_dummy_dab
