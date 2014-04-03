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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief AT91SAM7S256 CRT, adapted from NUt/OS, see license below.
 */

/*
 * Copyright (C) 2005-2007 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

#include <cpu/detect.h>
#include "cfg/cfg_arch.h"


#if CPU_FREQ != 48054857L
	/* Avoid errors on nightly test */
	#if !defined(ARCH_NIGHTTEST) || !(ARCH & ARCH_NIGHTTEST)
		#warning Clock registers set for 48.055MHz operation, revise following code if you want a different clock.
	#endif
#endif


#if CPU_ARM_SAM7S_LARGE || CPU_ARM_SAM7X
	/*
	 * With a 18.432MHz cristal, master clock is:
	 * (((18.432 * (PLL_MUL_VAL + 1)) / PLL_DIV_VAL) / AT91MCK_PRES) = 48.055MHz
	 */
	#define PLL_MUL_VAL  72  /**< Real multiplier value is PLL_MUL_VAL + 1! */
	#define PLL_DIV_VAL  14
	#define AT91MCK_PRES PMC_PRES_CLK_2

	/*
	 * Register I/O adresses.
	 */
	#define MC_BASE             0xFFFFFF00
	#define MC_FMR_OFF          0x00000060
	#define MC_FWS_2R3W         0x00000100

	#define AIC_BASE            0xFFFFF000
	#define AIC_EOICR_OFF       0x00000130
	#define AIC_IDCR_OFF        0x00000124

	#define WDT_BASE            0xFFFFFD40
	#define WDT_MR_OFF          0x00000004
	#define WDT_WDDIS            (1 << 15)

	#define PMC_BASE            0xFFFFFC00
	#define PMC_PCER_OFF        0x00000010
	#define PMC_SR_OFF          0x00000068
	#define PMC_MCKR_OFF        0x00000030
	#define PMC_MOSCS             (1 << 0)
	#define PMC_LOCK              (1 << 2)
	#define PMC_MCKRDY            (1 << 3)
	#define PMC_CSS_MASK        0x00000003
	#define PMC_CSS_PLL_CLK     0x00000003
	#define PMC_PRES_MASK       0x0000001C
	#define PMC_PRES_CLK_2      0x00000004

	#if CPU_ARM_SAM7S_LARGE
		#define PMC_PIO_CLK_EN (1 << 2)
	#elif CPU_ARM_SAM7X
		#define PMC_PIO_CLK_EN ((1 << 2) | (1 << 3))
	#else
		#error CPU not supported
	#endif

	#define CKGR_MOR_OFF        0x00000020
	#define CKGR_PLLR_OFF       0x0000002C
	#define CKGR_MOSCEN           (1 << 0)
	#define CKGR_MUL_SHIFT              16
	#define CKGR_PLLCOUNT_SHIFT          8

	#define RSTC_MR             0xFFFFFD08
	#define RSTC_KEY            0xA5000000
	#define RSTC_URSTEN           (1 << 0)

	#define ARM_MODE_USR              0x10
	#define ARM_MODE_FIQ              0x11
	#define ARM_MODE_IRQ              0x12
	#define ARM_MODE_SVC              0x13
	#define ARM_MODE_ABORT            0x17
	#define ARM_MODE_UNDEF            0x1B
	#define ARM_MODE_SYS              0x1F

#else
	#error No register I/O definition for selected ARM CPU
#endif
/*\}*/

/*
 * Section 0: Vector table and reset entry.
 */
        .section .vectors,"ax",%progbits

        .global __vectors
__vectors:
        ldr     pc, [pc, #24]   /* Reset */
        ldr     pc, [pc, #24]   /* Undefined instruction */
        ldr     pc, [pc, #24]   /* Software interrupt */
        ldr     pc, [pc, #24]   /* Prefetch abort */
        ldr     pc, [pc, #24]   /* Data abort */
        ldr     pc, [pc, #24]   /* Reserved */

        /*
         * On IRQ the PC will be loaded from AIC_IVR, which
         * provides the address previously set in AIC_SVR.
         * The interrupt routine will be called in ARM_MODE_IRQ
         * with IRQ disabled and FIQ unchanged.
         */
        ldr     pc, [pc, #-0xF20]   /* Interrupt request, auto vectoring. */
        ldr     pc, [pc, #-0xF20]   /* Fast interrupt request, auto vectoring. */

        .word   _init
        .word   __undef
        .word   __swi
        .word   __prefetch_abort
        .word   __data_abort

        .weak   __undef
        .set    __undef, __xcpt_dummy_undef
        .weak   __swi
        .set    __swi, __xcpt_dummy_swi
        .weak   __prefetch_abort
        .set    __prefetch_abort, __xcpt_dummy_pref
        .weak   __data_abort
        .set    __data_abort, __xcpt_dummy_dab

/**        .global __xcpt_dummy*/
__xcpt_dummy_undef:
        b       __xcpt_dummy_undef

__xcpt_dummy_swi:
        b       __xcpt_dummy_swi

__xcpt_dummy_pref:
        b       __xcpt_dummy_pref

__xcpt_dummy_dab:
        b       __xcpt_dummy_dab


        .ltorg
/*
 * Hardware initialization.
 */
        .section .init, "ax", %progbits
        .globl	_init
_init:
        /*
         * Use 2 cycles for flash access.
         */
        ldr     r1, =MC_BASE
        mov     r0, #MC_FWS_2R3W
        str     r0, [r1, #MC_FMR_OFF]

        /*
         * Disable all interrupts. Useful for debugging w/o target reset.
         */
        ldr     r1, =AIC_BASE
        mvn     r0, #0
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_IDCR_OFF]

        /*
         * The watchdog is enabled after processor reset. Disable it.
         */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]

        /*
         * Enable the main oscillator. Set startup time of 6 * 8 slow
         * clock cycles and wait until oscillator is stabilized.
         */
        ldr     r1, =PMC_BASE
        mov     r0, #(6 << 8)
        orr     r0, r0, #CKGR_MOSCEN
        str     r0, [r1, #CKGR_MOR_OFF]
wait_moscs:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MOSCS
        beq     wait_moscs

        /*
         * Switch to Slow oscillator clock.
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        and     r0, r0, #~PMC_CSS_MASK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_slowosc:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_slowosc

        /*
         * Switch to prescaler div 1 factor.
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        and     r0, r0, #~PMC_PRES_MASK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_presc:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_presc

        /*
         * Set PLL:
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Wait 28 clock cycles until PLL is locked.
         */
        ldr     r0, =((PLL_MUL_VAL << CKGR_MUL_SHIFT) | (28 << CKGR_PLLCOUNT_SHIFT) | PLL_DIV_VAL)

        str     r0, [r1, #CKGR_PLLR_OFF]
wait_lock:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_LOCK
        beq     wait_lock

        /*
         * Set master clock prescaler.
         */
        mov     r0, #AT91MCK_PRES
        str     r0, [r1, #PMC_MCKR_OFF]
wait_presrdy:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_presrdy

        /*
         * Switch to PLL clock. Trying to set this together with the
         * prescaler fails (see datasheets).
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        orr     r0, r0, #PMC_CSS_PLL_CLK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_pllsel:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_pllsel

        /*
         * Enable external reset key.
         */
        ldr     r0, =(RSTC_KEY | RSTC_URSTEN)
        ldr     r1, =RSTC_MR
        str     r0, [r1, #0]

        /*
         * Set exception stack pointers
         */
        ldr     r0, =__stack_fiq_end
        msr     CPSR_c, #ARM_MODE_FIQ | 0xC0
        mov     r13, r0
        ldr     r0, =__stack_irq_end
        msr     CPSR_c, #ARM_MODE_IRQ | 0xC0
        mov     r13, r0
        ldr     r0, =__stack_abt_end
        msr     CPSR_c, #ARM_MODE_ABORT | 0xC0
        mov     r13, r0
        ldr     r0, =__stack_und_end
        msr     CPSR_c, #ARM_MODE_UNDEF | 0xC0
        mov     r13, r0
        ldr     r0, =__stack_svc_end
        msr     CPSR_c, #ARM_MODE_SVC | 0xC0
        mov     r13, r0

	/*
	 * Clear .bss
	 */
        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0

_40:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     _40

        /*
         * Relocate .data section (Copy from ROM to RAM).
         */
        ldr     r1, =__etext
        ldr     r2, =__data_start
        ldr     r3, =__data_end

_41:
        cmp     r2, r3
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     _41

        /*
         * Initialize user stack pointer.
         */
	/* msr     CPSR_c, #ARM_MODE_SYS | 0xC0 */
	ldr     r13, =__stack_end


	/*
	 * Enable clock for PIO(s)
	 */
        ldr     r1, =PMC_BASE
        mov     r0, #PMC_PIO_CLK_EN
        str     r0, [r1, #PMC_PCER_OFF]


        /*
         * Jump to main
         */
        ldr     r0, =main
        bx      r0

End:
        b       End

        .ltorg
