/*!
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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief AVR context switch
 *
 */

#include <avr/io.h>

/* void asm_switch_context(void **new_sp [r24:r25], void **save_sp [r22:r23]) */
.globl asm_switch_context
asm_switch_context:

;	r0 is the TEMP REG and can be used freely.
;	r1 is the ZERO REG and must always contain 0.
;
;	Stack frame is 18 byte, remember to update
;	CPU_SAVED_REGS_CNT if you change pushed regs.

	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17

	push	r28
	push	r29

	in	r18,SPL-__SFR_OFFSET	; r18:r19 = SP
	in	r19,SPH-__SFR_OFFSET
	movw	r26,r22			; X = save_sp
	st	X+,r18			; *save_sp = SP
	st	X,r19
	movw	r26,r24			; X = new_sp
	ld	r18,X+
	ld	r19,X

;FIXME: We probably need to safe the RAMP registers for some XMEGA devices / setups

; 	Set new stack pointer.
;	AVR is an 8 bit processor so
;	care must be taken when updating
;	SP that is a 16 bit reg.
;	Two instructions are required to update SP
;	so an IRQ can sneak in between them.
;	So IRQ *MUST* be disabled and then restored.
	in	r0, SREG-__SFR_OFFSET
	cli				; Disable interrupt
	out	SPL-__SFR_OFFSET,r18	; SP = *new_sp
	out	SPH-__SFR_OFFSET,r19
	out	SREG-__SFR_OFFSET,r0	; Restore previous IRQ state

	pop	r29
	pop	r28

	pop	r17
	pop	r16
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2

	ret
