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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief PowerPC context switch
 */

#ifdef __APPLE__
    //This workaround is necessary to compile under OS X assembler.
    #define SWITCH_CONTEXT _asm_switch_context
#else
    #define SWITCH_CONTEXT asm_switch_context
#endif

.balign 4

/* void asm_switch_context(void ** new_sp, void ** save_sp) */
/*                                 r3              r4       */
.globl SWITCH_CONTEXT
SWITCH_CONTEXT:
	mflr	0		/* r0 = lr */
	stw	0,8(1)		/* store lr at *(sp+8) */
	stw	1,0(4)		/* *save_sp = sp */
	lwz	1,0(3)		/* sp = *new_sp */
	lwz	0,8(1)		/* retrieve lr from *(sp+8) */
	mtlr	0		/* lr = r0 */
	blr			/* return */
