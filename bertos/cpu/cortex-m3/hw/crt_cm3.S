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
 * \brief Cortex-M3 architecture's entry point
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#define CONTROL_UNPRIVILEGED	1
#define CONTROL_PSP		2

.syntax unified
.thumb
.section .init, "ax", %progbits

.weak __init
.set __init, __init0

.weak __init1
.set __init1, __dummy_init

.weak __init2
.set __init2, __dummy_init

.balign 2
.thumb_func
__dummy_init:
	bx lr

/*
 * Reset handler.
 */
.balign 2
.thumb_func
__init0:
	/* Disable IRQs */
	cpsid i
	/*
	 * Initialize process stack pointer.
	 */
	ldr     r0, =__psp_end
	msr     psp, r0

	/* Call the early hardware initialization routine */
	bl __init1

	/* Copy the data segment initializers from flash to SRAM */
	ldr r0, =__text_end
	ldr r1, =__data_start
	ldr r2, =__data_end
data_loop:
	cmp r1, r2
	ittt lo
	ldrlo r3, [r0], #4
	strlo r3, [r1], #4
	blo data_loop

	/* Zero fill the bss segment */
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	mov r0, #0
bss_loop:
	cmp r1, r2
	itt lo
	strlo r0, [r1], #4
	blo bss_loop

	/* Switch to the process stack */
	movs r0, CONTROL_PSP
	msr control, r0
	isb

	/* Call the hardware initialization routine */
	bl __init2

	/* Call the application's entry point */
	cpsie i
	mov r0, #0
	mov r1, #0
	bl main
end:
	wfi
	b end
