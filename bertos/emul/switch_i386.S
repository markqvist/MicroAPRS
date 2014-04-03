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
 * \brief i386 context switch
 *
 * x86 function calling convention:
 * --------------------------------
 *  arguments         | callee-saved        | extra caller-saved | return
 * [callee-clobbered] |                     | [callee-clobbered] |
 * -------------------------------------------------------------------------
 * eax edx ecx        | ebx edi esi ebp [*] | <none>             | eax, edx
 *
 *  [*]  In the frame-pointers case rbp must hold a base address for the
 *       current stack frame.
 *
 * asm_switch_context() can be considered as a normal function call, so we need
 * to save all the callee-clobbered registers minus the return registers.
 */

#ifdef __APPLE__
    // This workaround is necessary to compile under OS X assembler.
    #define SWITCH_CONTEXT _asm_switch_context
#else
    #define SWITCH_CONTEXT asm_switch_context
#endif

/* void asm_switch_context(void ** new_sp [sp+4], void ** save_sp [sp+8]) */
.globl SWITCH_CONTEXT
SWITCH_CONTEXT:
	pushl	%ebp
	pushl	%ecx
	movl	0x10(%esp),%ebp         /* ebp = save_sp */
	movl	%esp,(%ebp)             /* *save_sp = esp */
	movl	0x0c(%esp),%ebp         /* ebp = new_sp */
	movl	(%ebp),%esp             /* esp = *new_sp */
	popl	%ecx
	popl	%ebp
	ret
