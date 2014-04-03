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
 * Copyright 2004, 2005, 2006, 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * -->
 *
 * \brief CPU-specific attributes.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_ATTR_H
#define CPU_ATTR_H

#include "detect.h"

#include "cfg/cfg_proc.h"      /* CONFIG_KERN_PREEMPT */
#include "cfg/cfg_attr.h"      /* CONFIG_FAST_MEM */


/**
 * \name Macros for determining CPU endianness.
 * \{
 */
#define CPU_BIG_ENDIAN    0x1234
#define CPU_LITTLE_ENDIAN 0x3412 /* Look twice, pal. This is not a bug. */
/*\}*/

/** Macro to include cpu-specific versions of the headers. */
#define CPU_HEADER(module)          PP_STRINGIZE(drv/PP_CAT3(module, _, CPU_ID).h)

/** Macro to include cpu-specific versions of implementation files. */
#define CPU_CSOURCE(module)         PP_STRINGIZE(drv/PP_CAT3(module, _, CPU_ID).c)


#if CPU_I196

	#define NOP                     nop_instruction()

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            16
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD             0

	/// Valid pointers should be >= than this value (used for debug)
	#define CPU_RAM_START		0x100

#elif CPU_X86

	#define CPU_REGS_CNT            7
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD             0

	#if CPU_X86_64
		#define CPU_REG_BITS    64

		#ifdef __WIN64__
			/* WIN64 is an IL32-P64 weirdo. */
			#define SIZEOF_LONG  4
		#endif
	#else
		#define CPU_REG_BITS    32
	#endif

	/// Valid pointers should be >= than this value (used for debug)
	#define CPU_RAM_START      0x1000

	#ifdef __GNUC__
		#define NOP         asm volatile ("nop")
		/* This is a good thing to insert into busy-wait loops. */
		#define PAUSE       asm volatile ("rep; nop" ::: "memory")
		#define BREAKPOINT  asm volatile ("int3" ::)
	#endif

#elif CPU_ARM

	#define CPU_REG_BITS           32
	#define CPU_REGS_CNT           16
	#define CPU_HARVARD            0

	/// Valid pointers should be >= than this value (used for debug)
	#if CPU_ARM_AT91
		#define CPU_RAM_START		0x00200000
	#elif CPU_ARM_LPC2
		#define CPU_RAM_START		0x40000000
	#else
		#warning Fix CPU_RAM_START address for your ARM, default value set to 0x200
		#define CPU_RAM_START		0x200
	#endif

	#ifdef __IAR_SYSTEMS_ICC__
		#warning Check CPU_BYTE_ORDER
		#define CPU_BYTE_ORDER (__BIG_ENDIAN__ ? CPU_BIG_ENDIAN : CPU_LITTLE_ENDIAN)

		#define NOP            __no_operation()

	#else /* GCC and compatibles */

		#if defined(__ARMEB__)
			#define CPU_BYTE_ORDER CPU_BIG_ENDIAN
		#elif defined(__ARMEL__)
			#define CPU_BYTE_ORDER CPU_LITTLE_ENDIAN
		#else
			#error Unable to detect ARM endianness!
  		#endif

		#define NOP            asm volatile ("mov r0,r0" ::)
		#define BREAKPOINT  /* asm("bkpt 0") DOES NOT WORK */

		#if CONFIG_FAST_MEM
			/**
			 * Function attribute for use with performance critical code.
			 *
			 * On the AT91 family, code residing in flash has wait states.
			 * Moving functions to the data section is a quick & dirty way
			 * to get them transparently copied to SRAM for zero-wait-state
			 * operation.
			 */
			#define FAST_FUNC __attribute__((section(".ramfunc")))

			/**
			 * Data attribute to move constant data to fast memory storage.
			 *
			 * \see FAST_FUNC
			 */
			#define FAST_RODATA __attribute__((section(".data")))

		#else // !CONFIG_FAST_MEM
			#define FAST_RODATA /**/
			#define FAST_FUNC /**/
		#endif

		/*
		 * Function attribute to move it into ram memory.
		 */
		#define RAM_FUNC __attribute__((section(".ramfunc")))

	#endif /* !__IAR_SYSTEMS_ICC_ */
#elif CPU_CM3

	#define CPU_REG_BITS           32
	#define CPU_REGS_CNT           16
	#define CPU_HARVARD            0

	/// Valid pointers should be >= than this value (used for debug)
	#if (CPU_CM3_LM3S1968 || CPU_CM3_LM3S8962 || CPU_CM3_STM32 || CPU_CM3_SAM3)
		#define CPU_RAM_START 0x20000000
	#else
		#warning Fix CPU_RAM_START address for your Cortex-M3, default value set to 0x20000000
		#define CPU_RAM_START 0x20000000
	#endif

    #if defined( __ICCARM__)
        #if ((defined __LITTLE_ENDIAN__) && (__LITTLE_ENDIAN__ == 0))
            #define CPU_BYTE_ORDER CPU_BIG_ENDIAN
        #elif ((defined __LITTLE_ENDIAN__) && (__LITTLE_ENDIAN__ == 1))
		    #define CPU_BYTE_ORDER CPU_LITTLE_ENDIAN
        #else
            #error Unable to detect Cortex-M3 endianess!
        #endif

	#define NOP            __no_operation()
    #else
        #if defined(__ARMEB__) // GCC
            #define CPU_BYTE_ORDER CPU_BIG_ENDIAN
        #elif defined(__ARMEL__) // GCC
            #define CPU_BYTE_ORDER CPU_LITTLE_ENDIAN
        #else
            #error Unable to detect Cortex-M3 endianess!
        #endif

	#define NOP         asm volatile ("nop")
	#define PAUSE       asm volatile ("wfi" ::: "memory")
	#define BREAKPOINT  /* asm("bkpt 0") DOES NOT WORK */

	/*
	 * Function attribute to move it into ram memory.
	 */
	#define RAM_FUNC __attribute__((section(".ramfunc")))
    #endif

#elif CPU_PPC

	#define CPU_REG_BITS           (CPU_PPC32 ? 32 : 64)
	#define CPU_REGS_CNT           FIXME
	#define CPU_BYTE_ORDER         (__BIG_ENDIAN__ ? CPU_BIG_ENDIAN : CPU_LITTLE_ENDIAN)
	#define CPU_HARVARD            0

	/// Valid pointers should be >= than this value (used for debug)
	#define CPU_RAM_START          0x1000

	#ifdef __GNUC__
	    #define NOP         asm volatile ("nop" ::)
		#define BREAKPOINT  asm volatile ("twge 2,2" ::)
	#endif

#elif CPU_DSP56K

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            FIXME
	#define CPU_BYTE_ORDER          CPU_BIG_ENDIAN
	#define CPU_HARVARD             1

	/* Memory is word-addessed in the DSP56K */
	#define CPU_BITS_PER_CHAR  16
	#define SIZEOF_SHORT        1
	#define SIZEOF_INT          1
	#define SIZEOF_LONG         2
	#define SIZEOF_PTR          1

	/// Valid pointers should be >= than this value (used for debug)
	#define CPU_RAM_START       0x200

	#define NOP                     asm(nop)
	#define BREAKPOINT              asm(debug)

#elif CPU_AVR

	#define NOP                     asm volatile ("nop" ::)

	#define CPU_REG_BITS            8
	#define CPU_REGS_CNT           33 /* Includes SREG */
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD             1

	/// Valid pointers should be >= than this value (used for debug)
	#if CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA32 || CPU_AVR_ATMEGA103
		#define CPU_RAM_START       0x60
	#elif CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P
		#define CPU_RAM_START       0x100
	#elif CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		#define CPU_RAM_START       0x200
	#elif CPU_AVR_XMEGA_D
		#define CPU_RAM_START		0x2000
	#else
		#warning Fix CPU_RAM_START address for your AVR, default value set to 0x100
		#define CPU_RAM_START       0x100
	#endif

#elif CPU_MSP430

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            12
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD		        0

	/// Valid pointers should be >= than this value (used for debug)
	#define CPU_RAM_START           0x200

	#define NOP                     __asm__ __volatile__ ("nop")

#else
	#error No CPU_... defined.
#endif

#ifndef BREAKPOINT
#define BREAKPOINT /* nop */
#endif

#ifndef FAST_FUNC
	/// Function attribute for use with performance critical code.
	#define FAST_FUNC /* */
#endif

#ifndef FAST_RODATA
	/// Data attribute to move constant data to fast memory storage.
	#define FAST_RODATA /* */
#endif

#ifndef PAUSE
	/// Generic PAUSE implementation.
	#define PAUSE	do {NOP; MEMORY_BARRIER;} while (0)
#endif

#endif /* CPU_ATTR_H */
