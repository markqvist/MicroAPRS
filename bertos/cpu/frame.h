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
 * Copyright 2004, 2005, 2006, 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief CPU-specific stack frame handling macros.
 *
 * These are mainly used by the portable part of the scheduler
 * to work with the process stack frames.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_FRAME_H
#define CPU_FRAME_H

#include <cpu/detect.h>

#include "cfg/cfg_arch.h"      /* ARCH_EMUL */
#include <cfg/compiler.h>      /* for uintXX_t */

#if CPU_X86
	#if CPU_X86_32
		#define CPU_SAVED_REGS_CNT      2
	#elif CPU_X86_64
		#define CPU_SAVED_REGS_CNT      8
	#else
		#error "unknown CPU"
	#endif
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0

#elif CPU_ARM

	#define CPU_SAVED_REGS_CNT     8
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   0

#elif CPU_CM3

	#define CPU_SAVED_REGS_CNT     8
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   0

#elif CPU_PPC

	#define CPU_SAVED_REGS_CNT     1
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   1

#elif CPU_DSP56K

	#define CPU_SAVED_REGS_CNT      8
	#define CPU_STACK_GROWS_UPWARD  1
	#define CPU_SP_ON_EMPTY_SLOT	0

#elif CPU_AVR

	#define CPU_SAVED_REGS_CNT     18
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT    1

#elif CPU_MSP430

	#define CPU_SAVED_REGS_CNT     16
	#define CPU_STACK_GROWS_UPWARD  1
	#define CPU_SP_ON_EMPTY_SLOT    0

#else
	#error No CPU_... defined.
#endif

#ifndef CPU_STACK_GROWS_UPWARD
	#error CPU_STACK_GROWS_UPWARD should have been defined to either 0 or 1
#endif

#ifndef CPU_SP_ON_EMPTY_SLOT
	#error CPU_SP_ON_EMPTY_SLOT should have been defined to either 0 or 1
#endif

/// Default for macro not defined in the right arch section
#ifndef CPU_REG_INIT_VALUE
	#define CPU_REG_INIT_VALUE(reg)     (reg)
#endif

/*
 * Support stack handling peculiarities of a few CPUs.
 *
 * Most processors let their stack grow downward and
 * keep SP pointing at the last pushed value.
 */
#if !CPU_STACK_GROWS_UPWARD
	#if !CPU_SP_ON_EMPTY_SLOT
		/* Most microprocessors (x86, m68k...) */
		#define CPU_PUSH_WORD(sp, data) \
			do { *--(sp) = (data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*(sp)++)
	#else
		/* AVR insanity */
		#define CPU_PUSH_WORD(sp, data) \
			do { *(sp)-- = (data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*++(sp))
	#endif

#else /* CPU_STACK_GROWS_UPWARD */

	#if !CPU_SP_ON_EMPTY_SLOT
		/* DSP56K and other weirdos */
		#define CPU_PUSH_WORD(sp, data) \
			do { *++(sp) = (cpu_stack_t)(data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*(sp)--)
	#else
		#error I bet you cannot find a CPU like this
	#endif
#endif


#if CPU_DSP56K
	/*
	 * DSP56k pushes both PC and SR to the stack in the JSR instruction, but
	 * RTS discards SR while returning (it does not restore it). So we push
	 * 0 to fake the same context.
	 */
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (func)); \
			CPU_PUSH_WORD((sp), 0x100); \
		} while (0);

#elif CPU_CM3

	#if CONFIG_KERN_PREEMPT
		INLINE void cm3_preempt_switch_context(cpu_stack_t **new_sp, cpu_stack_t **old_sp)
		{
			register cpu_stack_t **__new_sp asm ("r0") = new_sp;
			register cpu_stack_t **__old_sp asm ("r1") = old_sp;

			asm volatile ("svc #0"
				: : "r"(__new_sp), "r"(__old_sp) : "memory", "cc");
		}
		#define asm_switch_context cm3_preempt_switch_context

		#define CPU_CREATE_NEW_STACK(stack) \
			do { \
				size_t i; \
				/* Initialize process stack frame */ \
				CPU_PUSH_WORD((stack), 0x01000000);		/* xPSR    */	\
				CPU_PUSH_WORD((stack), (cpu_stack_t)proc_entry);	/* pc      */	\
				CPU_PUSH_WORD((stack), 0);				/* lr      */	\
				CPU_PUSH_WORD((stack), 0);				/* ip	   */	\
				CPU_PUSH_WORD((stack), 0);				/* r3      */	\
				CPU_PUSH_WORD((stack), 0);				/* r2      */	\
				CPU_PUSH_WORD((stack), 0);				/* r1      */	\
				CPU_PUSH_WORD((stack), 0);				/* r0      */	\
				CPU_PUSH_WORD((stack), 0xfffffffd);		/* lr_exc  */	\
				/* Push a clean set of CPU registers for asm_switch_context() */ \
				for (i = 0; i < CPU_SAVED_REGS_CNT; i++) \
					CPU_PUSH_WORD(stack, CPU_REG_INIT_VALUE(i)); \
				CPU_PUSH_WORD(stack, IRQ_PRIO_DISABLED); \
			} while (0)

	#endif /* CONFIG_KERN_PREEMPT */

#elif CPU_AVR
	/*
	 * On AVR, addresses are pushed into the stack as little-endian, while
	 * memory accesses are big-endian (actually, it's a 8-bit CPU, so there is
	 * no natural endianess).
	 */
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			uint16_t funcaddr = (uint16_t)(func); \
			CPU_PUSH_WORD((sp), funcaddr); \
			CPU_PUSH_WORD((sp), funcaddr>>8); \
		} while (0)

	/*
	 * If the kernel is in idle-spinning, the processor executes:
	 *
	 * IRQ_ENABLE;
	 * CPU_IDLE;
	 * IRQ_DISABLE;
	 *
	 * IRQ_ENABLE is translated in asm as "sei" and IRQ_DISABLE as "cli".
	 * We could define CPU_IDLE to expand to none, so the resulting
	 * asm code would be:
	 *
	 * sei;
	 * cli;
	 *
	 * But Atmel datasheet states:
	 * "When using the SEI instruction to enable interrupts,
	 * the instruction following SEI will be executed *before*
	 * any pending interrupts", so "cli" is executed before any
	 * pending interrupt with the result that IRQs will *NOT*
	 * be enabled!
	 * To ensure that IRQ will run a NOP is required.
	 */
	#define CPU_IDLE NOP

#elif CPU_PPC

	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (cpu_stack_t)(func)); /* LR -> 8(SP) */ \
			CPU_PUSH_WORD((sp), 0);                  /* CR -> 4(SP) */ \
		} while (0)

#endif

#ifndef CPU_PUSH_CALL_FRAME
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		CPU_PUSH_WORD((sp), (cpu_stack_t)(func))
#endif

/**
 * \def CPU_IDLE
 *
 * \brief Invoked by the scheduler to stop the CPU when idle.
 *
 * This hook can be redefined to put the CPU in low-power mode, or to
 * profile system load with an external strobe, or to save CPU cycles
 * in hosted environments such as emulators.
 */
#ifndef CPU_IDLE
	#define CPU_IDLE PAUSE
#endif /* !CPU_IDLE */

/**
 * Default macro for creating a new Process stack
 */
#ifndef CPU_CREATE_NEW_STACK

	#define CPU_CREATE_NEW_STACK(stack) \
		do { \
			size_t i; \
			/* Initialize process stack frame */ \
			CPU_PUSH_CALL_FRAME(stack, proc_entry); \
			/* Push a clean set of CPU registers for asm_switch_context() */ \
			for (i = 0; i < CPU_SAVED_REGS_CNT; i++) \
				CPU_PUSH_WORD(stack, CPU_REG_INIT_VALUE(i)); \
		} while (0)
#endif

#endif /* CPU_ATTR_H */
