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
 * \brief STM32 backend implementation entropy pulling.
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include <sec/random_p.h>

#include <cpu/power.h>

#include <drv/clock_cm3.h>

#include <io/cm3.h>

struct stm32_adc *adc = (struct stm32_adc *)ADC1_BASE;

/*
 * Return the cpu core temperature in raw format
 */
INLINE uint16_t hw_readRawTemp(void)
{
	/* We sample only from one channel */
	adc->SQR1 |= BV(SQR1_SQ_LEN_SHIFT);
	adc->SQR3 = (ADC_TEMP_CH & SQR3_SQ_MASK);

	/* Start convertion */
	adc->CR2 |= CR2_EXTTRIG_SWSTRT_SET;

	/* Wait in polling until conversion is done */
	while (!(adc->SR & BV(SR_EOC)))
		cpu_relax();

	/* Return the last converted data */
    return (uint16_t)adc->DR;
}

INLINE void hw_initIntTemp(void)
{
	RCC->APB2ENR |= RCC_APB2_ADC1;

	/* Reset registry */
	adc->CR1 = 0;
	adc->CR2 = 0;
	adc->SQR1 = 0;
	adc->SQR2 = 0;
	adc->SQR3 = 0;

	/*
	 * Configure ADC
	 *  - Regular mode
	 *  - Wake up adc
	 *  - Wake up temperature and Vrefint
	 */
	adc->CR2 |= BV(CR2_ADON) | ADC_EXTERNALTRIGCONV_NONE | BV(CR2_TSVREFE);

	/* Set 17.1usec sampling time*/
	adc->SMPR1 |= ((ADC_SAMPLETIME_239CYCLES5 << SMPR1_CH17) | (ADC_SAMPLETIME_239CYCLES5 << SMPR1_CH16));
}


void random_pull_entropy(uint8_t *entropy, size_t len)
{
	// We use the internal temperature sensor of LM3S as a source of entropy.
	// The last bit of the acquisition is very variable and with a decent distribution
	// to consider it "entropic". It does not really matter because it will
	// go through a randomness extractor anyway.
	hw_initIntTemp();

	for (size_t j=0; j<len; j++)
	{
		uint8_t accum = 0;
		for (int b=0; b<8; ++b)
			if (hw_readRawTemp() & 1)
				accum |= 1<<b;

		*entropy++ = accum;
	}
}
