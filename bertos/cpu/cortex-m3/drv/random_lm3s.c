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
 * \brief LM3 backend implementation entropy pulling.
 * \author Giovanni Bajo <rasky@develer.com>
 */

#include "random_p.h"

#include <cpu/power.h>

#include <io/cm3.h>

#include <drv/clock_cm3.h>

/*
 * Return the cpu core temperature in raw format
 */
INLINE uint16_t hw_readRawTemp(void)
{
    /* Trig the temperature sampling */
    HWREG(ADC0_BASE + ADC_O_PSSI) |= ADC_PSSI_SS3;

	while (!(HWREG(ADC0_BASE + ADC_O_SSFSTAT3) & ADC_SSFSTAT3_FULL))
		cpu_relax();

    return (uint16_t)HWREG(ADC0_BASE + ADC_O_SSFIFO3);
}

INLINE void hw_initIntTemp(void)
{
    SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0;

	lm3s_busyWait(10);

    /* Disable all sequence */
    HWREG(ADC0_BASE + ADC_O_ACTSS) = 0;
    /* Set trigger event to programmed (for all sequence) */
    HWREG(ADC0_BASE + ADC_O_EMUX) = 0;
    /* Enalbe read of temperature sensor */
    HWREG(ADC0_BASE + ADC_O_SSCTL3) |= ADC_SSCTL3_TS0;
    /* Enable sequence S03 (single sample on select channel) */
    HWREG(ADC0_BASE + ADC_O_ACTSS) |= ADC_ACTSS_ASEN3;
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
