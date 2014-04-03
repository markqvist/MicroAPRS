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
 * \brief ADC hardware-specific implementation
 *
 * This ADC module should be use both whit kernel or none.
 * If you are using a kernel, the adc drive does not wait the finish of
 * conversion but use a singal every time a required conversion are
 * ended. This signal wake up a process that return a result of
 * conversion. Otherwise, if you not use a kernl, this module wait
 * whit a loop the finishing of conversion.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "adc_lm3s.h"

#include <cpu/irq.h>

#include "cfg/cfg_adc.h"
#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>

// Define log settings for cfg/log.h.
#define LOG_LEVEL         ADC_LOG_LEVEL
#define LOG_FORMAT        ADC_LOG_FORMAT
#include <cfg/log.h>

#include <drv/adc.h>
#include <drv/timer.h>
#include <drv/clock_lm3s.h>

#include <io/lm3s.h>

/* Select witch ADC use */
#if CPU_CM3_LM3S1968 || CPU_CM3_LM3S8962
	#define ADC_BASE            ADC0_BASE
	#define SYSCTL_RCGC_R       SYSCTL_RCGC0_R
	#define SYSCTL_RCGC_ADC     SYSCTL_RCGC0_ADC0
#else
	#error Unknow ADC register for select cpu core
#endif


#if CONFIG_KERN
	#include <cfg/module.h>

	#include <kern/proc.h>
	#include <kern/signal.h>

	#include <drv/irq_cm3.h>


	#if !CONFIG_KERN_SIGNALS
		#error Signals must be active to use ADC with kernel
	#endif

	/* Signal adc convertion end */
	#define SIG_ADC_COMPLETE SIG_USER0

	/* ADC waiting process */
	static struct Process *adc_process;

	/**
	 * ADC ISR.
	 * Simply signal the adc process that convertion is complete.
	 */
	static DECLARE_ISR(adc_conversion_end_irq)
	{
		sig_post(adc_process, SIG_ADC_COMPLETE);

		/* Clear the status bit */
		HWREG(ADC_BASE + ADC_O_ISC) |= ADC_ISC_IN3;

	}

	static void adc_enable_irq(void)
	{
		/* Clear all pending irq */
		HWREG(ADC_BASE + ADC_O_ISC) = 0;
		/* Register the IRQ handler */
		sysirq_setHandler(INT_ADC3, adc_conversion_end_irq);
		/* Enable IRQ */
		HWREG(ADC_BASE + ADC_O_SSCTL3) |= ADC_SSCTL3_IE0;
		HWREG(ADC_BASE + ADC_O_IM) |= ADC_IM_MASK3;
	}

#endif /* CONFIG_KERN */

/**
 * Select mux channel \a ch.
 * Generally the stm32 cpu family allow us to program the order
 * of adc channel that we want to read.
 * In this driver implementation we put as fist channel to read the
 * select ones.
 */
void adc_hw_select_ch(uint8_t ch)
{
	/* Select channel that we want read */
	HWREG(ADC_BASE + ADC_O_SSMUX3) = ch;
	/* Make single acquisition */
	HWREG(ADC_BASE + ADC_O_SSCTL3) |= ADC_SSCTL3_END0;
	/* Enable sequence S03 (single sample on select channel) */
	HWREG(ADC_BASE + ADC_O_ACTSS) |= ADC_ACTSS_ASEN3;
}

/**
 * Start an ADC convertion.
 * If a kernel is present, preempt until convertion is complete, otherwise
 * a busy wait on ADC_DRDY bit is done.
 */
uint16_t adc_hw_read(void)
{
	#if CONFIG_KERN
		/* Ensure ADC is not already in use by another process */
		ASSERT(adc_process == NULL);
		adc_process = proc_current();
	#endif

	/* Start convertion */
	HWREG(ADC0_BASE + ADC_O_PSSI) |= ADC_PSSI_SS3;

	#if CONFIG_KERN
		/* Ensure IRQs enabled. */
		IRQ_ASSERT_ENABLED();
		sig_wait(SIG_ADC_COMPLETE);

		/* Prevent race condition in case of preemptive kernel */
		uint16_t ret = (uint16_t)HWREG(ADC_BASE + ADC_O_SSFIFO3);
		MEMORY_BARRIER;
		adc_process = NULL;
		return ret;
	#else
		/* Wait in polling until conversion is done */
		while (!(HWREG(ADC_BASE + ADC_O_SSFSTAT3) & ADC_SSFSTAT3_FULL));

		/* Return the last converted data */
		return (uint16_t)HWREG(ADC_BASE + ADC_O_SSFIFO3);
	#endif
}

/**
 * Init ADC hardware.
 */
void adc_hw_init(void)
{
	/* Enable ADC0 clock */
	SYSCTL_RCGC_R |= SYSCTL_RCGC_ADC;

	/*
	 * We wait some time because the clock is istable
	 * and that could cause system hardfault
	 */
	lm3s_busyWait(10);

	/* Disable all sequence */
	HWREG(ADC_BASE + ADC_O_ACTSS) = 0;
	/* Set trigger event to programmed (for all sequence) */
	HWREG(ADC_BASE + ADC_O_EMUX) = 0;

	#if CONFIG_KERN
		adc_enable_irq();
	#endif
}
