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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief x86_64 context switch
 *
 * x86_64 function call convention:
 * --------------------------------
 *  arguments           |  callee-saved      | extra caller-saved | return
 * [callee-clobbered]   |                    | [callee-clobbered] |
 * -------------------------------------------------------------------------
 * rdi rsi rdx rcx r8-9 | rbx rbp [*] r12-15 | r10-11             | rax, rdx
 *
 *  [*]  In the frame-pointers case rbp must hold a base address for the
 *       current stack frame.
 *
 * asm_switch_context() can be considered as a normal function call, so we need
 * to save all the callee-clobbered registers minus the return registers.
 */

/* void asm_switch_context(void **new_sp [%rdi], void **save_sp [%rsi]) */
.globl asm_switch_context
asm_switch_context:
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rcx
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	movq	%rsp,(%rsi)             /* *save_sp = rsp */
	movq	(%rdi),%rsp             /* rsp = *new_sp */
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rcx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
