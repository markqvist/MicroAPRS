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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief OS-specific definitions
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef CFG_OS_H
#define CFG_OS_H

#include "cfg/cfg_proc.h"

/*
 * OS autodetection (Some systems trigger multiple OS definitions)
 */
#ifdef _WIN32
	#define OS_WIN32  1
	#define OS_ID     win32

	// FIXME: Maybe disable Win32 exceptions?
	typedef int cpu_flags_t;
	#define IRQ_DISABLE                FIXME
	#define IRQ_ENABLE                 FIXME
	#define IRQ_SAVE_DISABLE(old_sigs) FIXME
	#define IRQ_RESTORE(old_sigs)      FIXME

#else
	#define OS_WIN32  0
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	#define OS_UNIX   1
	#define OS_POSIX  1  /* Not strictly UNIX, but no way to autodetect it. */
	#define OS_ID     posix

	/*
	 * The POSIX moral equivalent of disabling IRQs is disabling signals.
	 */
	#include <signal.h>
	typedef sigset_t cpu_flags_t;

	#define SET_ALL_SIGNALS(sigs) \
	do { \
		sigfillset(&sigs); \
		sigdelset(&sigs, SIGINT); \
		sigdelset(&sigs, SIGSTOP); \
		sigdelset(&sigs, SIGCONT); \
	} while(0)

	#define IRQ_DISABLE \
	do { \
		sigset_t sigs; \
		SET_ALL_SIGNALS(sigs); \
		sigprocmask(SIG_BLOCK, &sigs, NULL); \
	} while (0)

	#define IRQ_ENABLE \
	do { \
		sigset_t sigs; \
		SET_ALL_SIGNALS(sigs); \
		sigprocmask(SIG_UNBLOCK, &sigs, NULL); \
	} while (0)

	#define IRQ_SAVE_DISABLE(old_sigs) \
	do { \
		sigset_t sigs; \
		SET_ALL_SIGNALS(sigs); \
		sigprocmask(SIG_BLOCK, &sigs, &old_sigs); \
	} while (0)

	#define IRQ_RESTORE(old_sigs) \
	do { \
		sigprocmask(SIG_SETMASK, &old_sigs, NULL); \
	} while (0)

	#define IRQ_ENABLED() \
	({ \
		sigset_t sigs__; \
		sigprocmask(SIG_SETMASK, NULL, &sigs__); \
		sigismember(&sigs__, SIGALRM) ? false : true; \
	 })

	#if (CONFIG_KERN && CONFIG_KERN_PREEMPT)
		#define DECLARE_ISR_CONTEXT_SWITCH(vect)	\
			void vect(UNUSED_ARG(int, arg));	\
			INLINE void __isr_##vect(void);		\
			void vect(UNUSED_ARG(int, arg))		\
			{					\
				__isr_##vect();			\
				IRQ_PREEMPT_HANDLER();		\
			}					\
			INLINE void __isr_##vect(void)
		/**
		 * With task priorities enabled each ISR is used a point to
		 * check if we need to perform a context switch.
		 *
		 * Instead, without priorities a context switch can occur only
		 * when the running task expires its time quantum. In this last
		 * case, the context switch can only occur in the timer ISR,
		 * that must be always declared with the
		 * DECLARE_ISR_CONTEXT_SWITCH() macro.
		 */
		#if CONFIG_KERN_PRI
			#define DECLARE_ISR(vect) \
				DECLARE_ISR_CONTEXT_SWITCH(vect)
		#endif /* CONFIG_KERN_PRI */
	#endif
	#ifndef DECLARE_ISR
		#define DECLARE_ISR(vect) \
				void vect(UNUSED_ARG(int, arg))
	#endif
	#ifndef DECLARE_ISR_CONTEXT_SWITCH
		#define DECLARE_ISR_CONTEXT_SWITCH(vect) \
				void vect(UNUSED_ARG(int, arg))
	#endif

#else
	#define OS_UNIX   0
	#define OS_POSIX  0
#endif

#ifdef __linux__
	#define OS_LINUX  1
#else
	#define OS_LINUX  0
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#define OS_DARWIN 1
#else
	#define OS_DARWIN 0
#endif


#include "cfg/cfg_arch.h" /* For ARCH_QT */

/*
 * We want Qt and other frameworks to look like OSes because you would
 * tipically want their portable abstractions if you're using one of these.
 */
#if defined(_QT) || (defined(ARCH_QT) && (ARCH & ARCH_QT))
	#define OS_QT 1
	#undef  OS_ID
	#define OS_ID qt
#else
	#define OS_QT 0
#endif

/*
 * Summarize hosted environments as OS_HOSTED and embedded
 * environment with OS_EMBEDDED.
 */
#if OS_WIN32 || OS_UNIX || OS_DARWIN || OS_QT
	#define OS_HOSTED   1
	#define OS_EMBEDDED 0
#else
	#define OS_HOSTED   0
	#define OS_EMBEDDED 1

	/* Embedded environments fall back to CPU-specific code. */
	#define OS_ID       CPU_ID
#endif

/* Self-check for the detection */
#if !defined(OS_ID)
	#error OS_ID not defined
#endif
#if OS_HOSTED && OS_EMBEDDED
	#error Both hosted and embedded OS environment
#endif
#if !OS_HOSTED && !OS_EMBEDDED
	#error Neither hosted nor embedded OS environment
#endif

#if OS_HOSTED

	/// Macro to include OS-specific headers.
	#define OS_HEADER(module)  PP_STRINGIZE(emul/PP_CAT3(module, _, OS_ID).h)

	/// Macro to include OS-specific source files.
	#define OS_CSOURCE(module) PP_STRINGIZE(emul/PP_CAT3(module, _, OS_ID).c)

#else
	// Fallbacks for embedded systems
	#define OS_HEADER(module)  CPU_HEADER(module)
	#define OS_CSOURCE(module) CPU_CSOURCE(module)
#endif

#endif /* CFG_OS_H */
