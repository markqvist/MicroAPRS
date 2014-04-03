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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * \brief Compiler defines for Emulation Layer for lwIP
 *      - Architecture environment, some compiler specific, some
 *        environment specific (probably should move env stuff
 *        to sys_arch.h.)
 *
 */

#ifndef LWIP_CC_H
#define LWIP_CC_H

#include <cfg/compiler.h>
#include <cpu/attr.h>
#include <lwip/arch.h>
#ifndef BYTE_ORDER
	#if CPU_BYTE_ORDER == CPU_BIG_ENDIAN
		#define BYTE_ORDER   BIG_ENDIAN
	#elif CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN
		#define BYTE_ORDER   LITTLE_ENDIAN
	#endif
#endif

#include <cfg/debug.h>

#include <sys/time.h>
// Unix error codes required by lwip
#include <errno.h>
// memset required by lwip
#include <string.h>

typedef uint8_t u8_t;
typedef int8_t s8_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef int mem_ptr_t;


/* Define (sn)printf formatters for these lwIP types */
#if CPU_ARM_AT91 || CPU_CM3_SAM3 || (ARCH & ARCH_EMUL)
	#define U16_F "hu"
	#define S16_F "d"
	#define X16_F "x"
	#define U32_F "lu"
	#define S32_F "ld"
	#define X32_F "lx"
#elif CPU_AVR
	#define U16_F "u"
	#define S16_F "d"
	#define X16_F "x"
	#define U32_F "lu"
	#define S32_F "ld"
	#define X32_F "lx"
#else
	#error This CPU is currently unsupported by lwip
#endif

/**
 * Compiler hints for packing lwip's structures
 */
#define PACK_STRUCT_STRUCT    PACKED

/*
 * Platform specific diagnostic output
 */
// not fatal, print a message
#define LWIP_PLATFORM_DIAG(y)   kprintf y

// fatal, print message and abandon execution.
#define LWIP_PLATFORM_ASSERT(y) \
	do { \
		kprintf(y); \
		ASSERT(0); \
	} while(0)

/*
 * "lightweight" synchronization mechanisms
 *
 * SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */

// TODO: if lwip is not used within multiple processes or interrupts, it's ok
// not to define them
/* #define SYS_ARCH_DECL_PROTECT(x) - declare a protection state variable.
    SYS_ARCH_PROTECT(x)      - enter protection mode.
    SYS_ARCH_UNPROTECT(x)    - leave protection mode. */

#define SYS_ARCH_DECL_PROTECT(x)
#define SYS_ARCH_PROTECT(x)		proc_forbid()
#define SYS_ARCH_UNPROTECT(x)		proc_permit()

#endif
