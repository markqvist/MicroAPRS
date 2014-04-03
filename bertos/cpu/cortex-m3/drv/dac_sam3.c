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
 * \brief DAC hardware-specific implementation
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "dac_sam3.h"

#include "cfg/cfg_dac.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

// Define log settings for cfg/log.h.
#define LOG_LEVEL         DAC_LOG_LEVEL
#define LOG_FORMAT        DAC_LOG_FORMAT
#include <cfg/log.h>

#include <drv/dac.h>
#include <cpu/irq.h>
#include <drv/irq_cm3.h>

#include <cpu/types.h>

#include <mware/event.h>

#include <io/cm3.h>

#include <string.h>

struct DacHardware
{
	uint16_t channels;
	uint32_t rate;
	bool end;
};

struct DacHardware dac_hw;


/* We use event to signal the end of conversion */
static Event buff_emtpy;

#if CONFIG_DAC_TIMER == DACC_TRGSEL_TIO_CH0 /* Select Timer counter TIO Channel 0 */
	#define DAC_TC_ID         TC0_ID
	#define DAC_TC_CCR        TC0_CCR0
	#define DAC_TC_IDR        TC0_IDR0
	#define DAC_TC_CMR        TC0_CMR0
	#define DAC_TC_SR         TC0_SR0
	#define DAC_TC_RA         TC0_RA0
	#define DAC_TC_RC         TC0_RC0
#elif CONFIG_DAC_TIMER == DACC_TRGSEL_TIO_CH1 /* Select Timer counter TIO Channel 1 */
	#define DAC_TC_ID         TC1_ID
	#define DAC_TC_CCR        TC0_CCR1
	#define DAC_TC_IDR        TC0_IDR1
	#define DAC_TC_CMR        TC0_CMR1
	#define DAC_TC_SR         TC0_SR1
	#define DAC_TC_RA         TC0_RA1
	#define DAC_TC_RC         TC0_RC1
#elif CONFIG_DAC_TIMER == DACC_TRGSEL_TIO_CH2 /* Select Timer counter TIO Channel 2 */
	#define DAC_TC_ID         TC2_ID
	#define DAC_TC_CCR        TC0_CCR2
	#define DAC_TC_IDR        TC0_IDR2
	#define DAC_TC_CMR        TC0_CMR2
	#define DAC_TC_SR         TC0_SR2
	#define DAC_TC_RA         TC0_RA2
	#define DAC_TC_RC         TC0_RC2
#elif CONFIG_DAC_TIMER == DACC_TRGSEL_PWM0 || CONFIG_DAC_TIMER == DACC_TRGSEL_PWM1
	#error unimplemented pwm triger select.
#endif


INLINE void tc_init(void)
{
	pmc_periphEnable(DAC_TC_ID);

	/*  Disable TC clock */
	DAC_TC_CCR = BV(TC_CCR_CLKDIS);
	/*  Disable interrupts */
	DAC_TC_IDR = 0xFFFFFFFF;
	/*  Clear status register */
	volatile uint32_t dummy = DAC_TC_SR;
	(void)dummy;

	/*
	 * Setup the timer counter:
	 * - select clock TCLK1 (MCK/2)
	 * - enable wave form mode
	 * - RA compare effect SET
	 * - RC compare effect CLEAR
	 * - UP mode with automatic trigger on RC Compare
	 */
	DAC_TC_CMR = TC_TIMER_CLOCK1 | BV(TC_CMR_WAVE) | TC_CMR_ACPA_SET | TC_CMR_ACPC_CLEAR | BV(TC_CMR_CPCTRG);


	/* Setup the pio: TODO: fix for more generic */
	PIOB_PDR = BV(25);
	PIO_PERIPH_SEL(PIOB_BASE, BV(25), PIO_PERIPH_B);
}

INLINE void tc_setup(uint32_t freq, size_t n_sample)
{
	/*
	 * Compute the sample frequency
	 * the RC counter will update every MCK/2 (see above)
	 * so to convert one sample at the user freq we generate
	 * the trigger every TC_CLK / (numer_of_sample * user_freq)
	 * where TC_CLK = MCK / 2.
	 */
	uint32_t rc = DIV_ROUND((CPU_FREQ / 2), n_sample * freq);
	DAC_TC_RC = rc;
	/* generate the square wave with duty = 50% */
	DAC_TC_RA = DIV_ROUND(50 * rc, 100);
}

INLINE void tc_start(void)
{
	DAC_TC_CCR = BV(TC_CCR_CLKEN)| BV(TC_CCR_SWTRG);
}

INLINE void tc_stop(void)
{
	DAC_TC_CCR =  BV(TC_CCR_CLKDIS);
}

static int sam3x_dac_write(struct Dac *dac, unsigned channel, uint16_t sample)
{
	(void)dac;

	ASSERT(channel <= DAC_MAXCH);

	DACC_MR |= (channel << DACC_USER_SEL_SHIFT) & DACC_USER_SEL_MASK;
	DACC_CHER |= BV(channel);

	DACC_CDR = sample ;

	return 0;
}

static void sam3x_dac_setCh(struct Dac *dac, uint32_t mask)
{
	/* we have only the ch0 and ch1 */
	ASSERT(mask < BV(3));
	dac->hw->channels = mask;

	if (mask & BV(DACC_CH0))
		DACC_MR |= (DACC_CH0 << DACC_USER_SEL_SHIFT) & DACC_USER_SEL_MASK;

	if (mask & BV(DACC_CH1))
		DACC_MR |= (DACC_CH1 << DACC_USER_SEL_SHIFT) & DACC_USER_SEL_MASK;

	DACC_CHER |= mask;

}

static void sam3x_dac_setSampleRate(struct Dac *dac, uint32_t rate)
{
	/* Eneble hw trigger */
	DACC_MR |= BV(DACC_TRGEN) | (CONFIG_DAC_TIMER << DACC_TRGSEL_SHIFT);
	dac->hw->rate = rate;
}

static void sam3x_dac_conversion(struct Dac *dac, void *buf, size_t len)
{
	/* setup timer and start it */
	tc_setup(dac->hw->rate, len);
	tc_start();

	/* Setup dma and start it */
	DACC_TPR = (uint32_t)buf;
	DACC_TCR = len;
	DACC_PTCR |= BV(DACC_PTCR_TXTEN);
}

static uint16_t *sample_buff;
static size_t next_idx = 0;
static size_t chunk_size = 0;
static size_t remaing_size = 0;

static DECLARE_ISR(irq_dac)
{
	if (DACC_ISR & BV(DACC_ENDTX))
	{
		if (remaing_size > 0)
		{
			DACC_TNPR = (uint32_t)&sample_buff[next_idx];
			DACC_TNCR = chunk_size;

			remaing_size -= chunk_size;
			next_idx += chunk_size;
		}
		else
			/* Clear the pending irq when the dma ends the conversion */
			DACC_TCR = 1;
	}
	event_do(&buff_emtpy);
}


static bool sam3x_dac_isFinished(struct Dac *dac)
{
	return dac->hw->end;
}

static void sam3x_dac_start(struct Dac *dac, void *_buf, size_t len, size_t slice_len)
{
	ASSERT(dac);
	ASSERT(len >= slice_len);

	/* Reset the previous status. */
	dac->hw->end = false;

	sample_buff = (uint16_t *)_buf;
	next_idx = 0;
	chunk_size = slice_len;
	remaing_size = len;


	/* Program the dma with the first and second chunk of samples and update counter */
	dac->ctx.callback(dac, &sample_buff[0], chunk_size);
	DACC_TPR = (uint32_t)&sample_buff[0];
	DACC_TCR = chunk_size;
	remaing_size -= chunk_size;
	next_idx += chunk_size;

	if (chunk_size <= remaing_size)
	{
		dac->ctx.callback(dac, &sample_buff[next_idx], chunk_size);

		DACC_TNPR = (uint32_t)&sample_buff[next_idx];
		DACC_TNCR = chunk_size;

		remaing_size -= chunk_size;
		next_idx += chunk_size;

	}

	DACC_PTCR |= BV(DACC_PTCR_TXTEN);
	DACC_IER = BV(DACC_ENDTX);

	/* Set up timer and trig the conversions */
	tc_setup(dac->hw->rate, len);
	tc_start();

	while (1)
	{
		event_wait(&buff_emtpy);
		if (remaing_size <= 0)
		{
			DAC_TC_CCR = BV(TC_CCR_CLKDIS);
			dac->hw->end = true;
			next_idx = 0;
			chunk_size = 0;
			remaing_size = 0;
			break;
		}

		dac->ctx.callback(dac, &sample_buff[next_idx], chunk_size);
	}
}

static void sam3x_dac_stop(struct Dac *dac)
{
	dac->hw->end = false;
	/* Disable the irq, timer and channel */
	DACC_IDR = BV(DACC_ENDTX);
	DACC_PTCR |= BV(DACC_PTCR_TXTDIS);
	DAC_TC_CCR = BV(TC_CCR_CLKDIS);
}


void dac_init(struct Dac *dac)
{
	/* Initialize the dataready event */
	event_initGeneric(&buff_emtpy);

	/* Fill the virtual table */
	dac->ctx.write = sam3x_dac_write;
	dac->ctx.setCh = sam3x_dac_setCh;
	dac->ctx.setSampleRate = sam3x_dac_setSampleRate;
	dac->ctx.conversion = sam3x_dac_conversion;
	dac->ctx.isFinished = sam3x_dac_isFinished;
	dac->ctx.start = sam3x_dac_start;
	dac->ctx.stop = sam3x_dac_stop;
	DB(dac->ctx._type = DAC_SAM3X;)
	dac->hw = &dac_hw;

	/* Clock DAC peripheral */
	pmc_periphEnable(DACC_ID);

	/* Reset hw */
	DACC_CR |= BV(DACC_SWRST);
	DACC_MR = 0;

	/* Configure the dac */
	DACC_MR |= (CONFIG_DAC_REFRESH << DACC_REFRESH_SHIFT) & DACC_REFRESH_MASK;
	DACC_MR |= (CONFIG_DAC_STARTUP << DACC_STARTUP_SHIFT) & DACC_STARTUP_MASK;

	DACC_IDR = 0xFFFFFFFF;
	sysirq_setHandler(INT_DACC, irq_dac);

	tc_init();
}
