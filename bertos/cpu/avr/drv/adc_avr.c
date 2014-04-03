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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief ADC hardware-specific definition
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * This module is automatically included so no need to include
 * in test list.
 * notest: avr
 *
 * $WIZ$
 */

#include "adc_avr.h"

#include "cfg/cfg_adc.h"
#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"
#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <cpu/irq.h> // IRQ_ASSERT_ENABLED()

#include <drv/adc.h>

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * ADC voltage referencese.
 *
 * $WIZ$ avr_adc_refs = "ADC_AVR_AREF", "ADC_AVR_AVCC", "ADC_AVR_INT256"
 * \{
 */
#define ADC_AVR_AREF   0
#define ADC_AVR_AVCC   1
#define ADC_AVR_INT256 2
/* \} */

#if CONFIG_KERN
	#include <cfg/module.h>
	#include <kern/proc.h>
	#include <kern/signal.h>


	#if !CONFIG_KERN_SIGNALS
		#error Signals must be active to use the ADC with kernel
	#endif

	/* Signal adc convertion end */
	#define SIG_ADC_COMPLETE SIG_SINGLE

	/* ADC waiting process */
	static struct Process *adc_process;

	/**
	 * ADC ISR.
	 * Simply signal the adc process that convertion is complete.
	 */
	ISR(ADC_vect)
	{
		sig_post(adc_process, SIG_ADC_COMPLETE);
	}
#endif /* CONFIG_KERN */

/**
 * Select mux channel \a ch.
 * \todo only first 8 channels are selectable!
 */
void adc_hw_select_ch(uint8_t ch)
{
	/* Set to 0 all mux registers */
	#if CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA168
		ADMUX &= ~(BV(MUX3) | BV(MUX2) | BV(MUX1) | BV(MUX0));
	#elif CPU_AVR_ATMEGA32 || CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281 \
	      || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		ADMUX &= ~(BV(MUX4) | BV(MUX3) | BV(MUX2) | BV(MUX1) | BV(MUX0));
		#if CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
			ADCSRB &= ~(BV(MUX5));
		#endif
	#else
		#error Unknown CPU
	#endif

	/* Select channel, only first 8 channel modes are supported */
	ADMUX |= (ch & 0x07);

	#if CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		/* Select channel, all 16 channels are supported */
		if (ch > 0x07)
			ADCSRB |= BV(MUX5);

	#endif

}


/**
 * Start an ADC convertion.
 * If a kernel is present, preempt until convertion is complete, otherwise
 * a busy wait on ADCS bit is done.
 */
uint16_t adc_hw_read(void)
{
	// Ensure another convertion is not running.
	ASSERT(!(ADCSRA & BV(ADSC)));

	// Start convertion
	ADCSRA |= BV(ADSC);

	#if CONFIG_KERN
		// Ensure IRQs enabled.
		IRQ_ASSERT_ENABLED();
		adc_process = proc_current();
		sig_wait(SIG_ADC_COMPLETE);
	#else
		//Wait in polling until is done
		while (ADCSRA & BV(ADSC)) ;
	#endif

	return(ADC);
}

/**
 * Init ADC hardware.
 */
void adc_hw_init(void)
{
	/*
	 * Select channel 0 as default,
	 * result right adjusted.
	 */
	ADMUX = 0;

	#if CONFIG_ADC_AVR_REF == ADC_AVR_AREF
		/* External voltage at AREF as analog ref source */
		/* None */
	#elif CONFIG_ADC_AVR_REF == ADC_AVR_AVCC
		/* AVCC as analog ref source */
		ADMUX |= BV(REFS0);
	#elif CONFIG_ADC_AVR_REF == ADC_AVR_INT256
		/* Internal 2.56V as ref source */
		ADMUX |= BV(REFS1) | BV(REFS0);
	#else
		#error Unsupported ADC ref value.
	#endif

	#if defined(ADCSRB)
	/* Disable Auto trigger source: ADC in Free running mode. */
	ADCSRB = 0;
	#endif

	/* Enable ADC, disable autotrigger mode. */
	ADCSRA = BV(ADEN);

	#if CONFIG_KERN
		MOD_CHECK(proc);
		ADCSRA |= BV(ADIE);
	#endif

	/* Set convertion frequency */
	#if CONFIG_ADC_AVR_DIVISOR == 2
		ADCSRA |= BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 4
		ADCSRA |= BV(ADPS1);
	#elif CONFIG_ADC_AVR_DIVISOR == 8
		ADCSRA |= BV(ADPS1) | BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 16
		ADCSRA |= BV(ADPS2);
	#elif CONFIG_ADC_AVR_DIVISOR == 32
		ADCSRA |= BV(ADPS2) | BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 64
		ADCSRA |= BV(ADPS2) | BV(ADPS1);
	#elif CONFIG_ADC_AVR_DIVISOR == 128
		ADCSRA |= BV(ADPS2) | BV(ADPS1) | BV(ADPS0);
	#else
		#error Unsupported ADC prescaler value.
	#endif

	/* Start a convertion to init ADC hw */
	adc_hw_read();
}
