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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief ADC hardware-specific implementation
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "adc_sam3.h"

#include "cfg/cfg_adc.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

// Define log settings for cfg/log.h.
#define LOG_LEVEL         ADC_LOG_LEVEL
#define LOG_FORMAT        ADC_LOG_FORMAT
#include <cfg/log.h>

#include <drv/adc.h>
#include <drv/irq_cm3.h>

#include <cpu/irq.h>

#include <mware/event.h>

#include <io/cm3.h>


/* We use event to signal the end of conversion */
static Event data_ready;
/* The last converted data */
static uint32_t data;

/**
 * ADC ISR.
 *
 * The interrupt is connected to ready data, so when the
 * adc ends the conversion we generate an event and then
 * we return the converted value.
 *
 * \note to clear the Ready data bit and End of conversion
 * bit we should read the Last Converted Data register, otherwise
 * the ready data interrupt loop on this call.
 */
static DECLARE_ISR(adc_conversion_end_irq)
{
	data = 0;
	if (ADC_ISR & BV(ADC_DRDY))
	{
		data = ADC_LDATA;
		event_do(&data_ready);
	}
}

/**
 * Select mux channel \a ch.
 */
void adc_hw_select_ch(uint8_t ch)
{
	/* Disable all channels */
	ADC_CHDR = ADC_CH_MASK;
	/* Enable select channel */
	ADC_CHER = BV(ch);
}

/**
 * Start an ADC convertion.
 */
uint16_t adc_hw_read(void)
{
	ADC_CR = BV(ADC_START);
	event_wait(&data_ready);
	return(data);
}

/**
 * Init ADC hardware.
 */
void adc_hw_init(void)
{
	/* Make sure that interrupt are enabled */
	IRQ_ASSERT_ENABLED();

	/* Initialize the dataready event */
	event_initGeneric(&data_ready);

	/* Clock ADC peripheral */
	pmc_periphEnable(ADC_ID);

	 /* Reset adc controller */
	ADC_CR = ADC_SWRST;

	/*
	 * Set adc mode register:
	 * - Disable hardware trigger and enable software trigger.
	 * - Select normal mode.
	 */
	ADC_MR = 0;

	/* Set ADC_BITS bit convertion resolution. */
	#if ADC_BITS == 12
		ADC_MR &= ~BV(ADC_LOWRES);
	#elif ADC_BITS == 10
		ADC_MR |= BV(ADC_LOWRES);
	#else
		#error No select bit resolution is supported to this CPU
	#endif

	/* Setup ADC */
	LOG_INFO("Computed ADC_CLOCK %ld\n", ADC_CLOCK);
	ADC_MR |= ((ADC_PRESCALER << ADC_PRESCALER_SHIFT) & ADC_PRESCALER_MASK);
	LOG_INFO("prescaler[%ld]\n", ADC_PRESCALER);
	ADC_MR |= ((CONFIG_ADC_SUT << ADC_STARTUP_SHIFT) & ADC_STARTUP_MASK);
	LOG_INFO("starup[%d]\n", CONFIG_ADC_SUT);
	ADC_MR |= ((CONFIG_ADC_STTLING << ADC_SETTLING_SHIFT) & ADC_SETTLING_MASK);
	LOG_INFO("sttime[%d]\n", CONFIG_ADC_STTLING);
	ADC_MR |= ((CONFIG_ADC_TRACKTIM << ADC_TRACKTIM_SHIFT) & ADC_TRACKTIM_MASK);
	LOG_INFO("tracking[%d]\n", CONFIG_ADC_TRACKTIM);
	ADC_MR |= ((CONFIG_ADC_TRANSFER << ADC_TRANSFER_SHIFT) & ADC_TRANSFER_MASK);
	LOG_INFO("tranfer[%d]\n", CONFIG_ADC_TRANSFER);

	/* Register and enable irq for adc. */
	sysirq_setHandler(INT_ADC, adc_conversion_end_irq);
	ADC_IER = BV(ADC_DRDY);
}
