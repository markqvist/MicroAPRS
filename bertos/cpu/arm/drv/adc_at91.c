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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
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


#include "adc_at91.h"

#include <cpu/irq.h>

#include "cfg/cfg_adc.h"
#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"
#include <cfg/macros.h>
#include <cfg/compiler.h>

// Define log settings for cfg/log.h.
#define LOG_LEVEL         ADC_LOG_LEVEL
#define LOG_FORMAT        ADC_LOG_FORMAT
#include <cfg/log.h>

#include <drv/adc.h>

#include <io/arm.h>

#if CONFIG_KERN
	#include <cfg/module.h>
	#include <kern/proc.h>
	#include <kern/signal.h>


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

		/* Inform hw that we have served the IRQ */
		AIC_EOICR = 0;
	}

	static void adc_enable_irq(void)
	{

		// Disable all interrupt
		ADC_IDR = 0xFFFFFFFF;

		//Register interrupt vector
		AIC_SVR(ADC_ID) = adc_conversion_end_irq;
		AIC_SMR(ADC_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED;
		AIC_IECR = BV(ADC_ID);

		//Enable data ready irq
		ADC_IER = BV(ADC_DRDY);
	}

#endif /* CONFIG_KERN */


/**
 * Select mux channel \a ch.
 * \todo only first 8 channels are selectable!
 */
void adc_hw_select_ch(uint8_t ch)
{
	//Disable all channels
	ADC_CHDR = ADC_CH_MASK;
	//Enable select channel
	ADC_CHER = BV(ch);
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

	// Start convertion
	ADC_CR = BV(ADC_START);

	#if CONFIG_KERN
		// Ensure IRQs enabled.
		IRQ_ASSERT_ENABLED();
		sig_wait(SIG_ADC_COMPLETE);

		/* Prevent race condition in case of preemptive kernel */
		uint16_t ret = ADC_LCDR;
		MEMORY_BARRIER;
		adc_process = NULL;
		return ret;
	#else
		//Wait in polling until is done
		while (!(ADC_SR & BV(ADC_DRDY)));

		//Return the last converted data
		return(ADC_LCDR);
	#endif
}

/**
 * Init ADC hardware.
 */
void adc_hw_init(void)
{
	//Init ADC pins.
	ADC_INIT_PINS();

	/*
	 * Set adc mode register:
	 * - Disable hardware trigger and enable software trigger.
	 * - Select normal mode.
	 * - Set ADC_BITS bit convertion resolution.
	 *
	 * \{
	 */
	ADC_MR = 0;
	#if ADC_BITS == 10
		ADC_MR &= ~BV(ADC_LOWRES);
	#elif ADC_BITS == 8
		ADC_MR |= BV(ADC_LOWRES);
	#else
		#error No select bit resolution is supported to this CPU
	#endif
	/* \} */

	LOG_INFO("Computed ADC_CLOCK %ld\n", ADC_COMPUTED_CLOCK);
	LOG_INFO("prescaler[%ld], stup[%ld], shtim[%ld]\n",ADC_COMPUTED_PRESCALER, ADC_COMPUTED_STARTUPTIME,  ADC_COMPUTED_SHTIME);


	//Apply computed prescaler value
	ADC_MR &= ~ADC_PRESCALER_MASK;
	ADC_MR |= ((ADC_COMPUTED_PRESCALER << ADC_PRESCALER_SHIFT) & ADC_PRESCALER_MASK);
	LOG_INFO("prescaler[%ld]\n", (ADC_COMPUTED_PRESCALER << ADC_PRESCALER_SHIFT) & ADC_PRESCALER_MASK);

	//Apply computed start up time
	ADC_MR &= ~ADC_STARTUP_MASK;
	ADC_MR |= ((ADC_COMPUTED_STARTUPTIME << ADC_STARTUP_SHIFT) & ADC_STARTUP_MASK);
	LOG_INFO("sttime[%ld]\n", (ADC_COMPUTED_STARTUPTIME << ADC_STARTUP_SHIFT) & ADC_STARTUP_MASK);

	//Apply computed sample and hold time
	ADC_MR &= ~ADC_SHTIME_MASK;
	ADC_MR |= ((ADC_COMPUTED_SHTIME << ADC_SHTIME_SHIFT) & ADC_SHTIME_MASK);
	LOG_INFO("shtime[%ld]\n", (ADC_COMPUTED_SHTIME << ADC_SHTIME_SHIFT) & ADC_SHTIME_MASK);

	#if CONFIG_KERN
		//Register and enable irq for adc.
		adc_enable_irq();
	#endif

}
