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
 * \brief NXP LPC2xxx interrupt vectors.
 */

#include <cpu/detect.h>
#include "cfg/cfg_arch.h"
#if defined(ARCH_NIGHTTEST) && (ARCH & ARCH_NIGHTTEST)
	/* Avoid errors during nigthly test */
	#define __vectors __vectors_lpc2
#endif

/*
 * Section 0: Vector table and reset entry.
 */
        .section .vectors,"ax",%progbits

        .global __vectors
__vectors:
        ldr     pc, _init            /* Reset */
        ldr     pc, _undef           /* Undefined instruction */
        ldr     pc, _swi             /* Software interrupt */
        ldr     pc, _prefetch_abort  /* Prefetch abort */
        ldr     pc, _data_abort      /* Data abort */
#if CPU_ARM_LPC2378
        .word	0xb9206e58           /* In LPX2xxx, this location holds the checksum of the previous vectors */
        ldr     pc, [pc, #-0x120]    /* Use VIC */
#else
        .word	0xb9205f88           /* In LPX2xxx, this location holds the checksum of the previous vectors */
	#if CPU_ARM_LPC2
		#warning Check correct VICAddress register for this CPU, default set to 0xFFFFF030
	#endif
        ldr     pc, [pc, #-0xFF0]    /* Use VIC */
#endif
        ldr     pc, _fiq	     /* Fast interrupt request */
_init:
        .word   __init
_undef:
        .word   __undef
_swi:
        .word   __swi
_prefetch_abort:
        .word   __prefetch_abort
_data_abort:
        .word   __data_abort
_fiq:
	.word	__fiq

	.weak	__fiq
__fiq:
	b	__fiq

        .ltorg
