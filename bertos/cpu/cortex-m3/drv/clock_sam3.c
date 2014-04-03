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
 * \brief Atmel SAM3 clock setup.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "clock_sam3.h"
#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <io/sam3.h>


/* Frequency of board main oscillator */
#define BOARDOSC_FREQ  12000000

/* Timer countdown timeout for clock initialization operations */
#define CLOCK_TIMEOUT    0xFFFFFFFF


#if CPU_FREQ == 84000000 || CPU_FREQ == 48000000

INLINE uint32_t evaluate_pll(void)
{
	return CKGR_PLLR_MUL(CPU_FREQ / BOARDOSC_FREQ * 2 - 1) | CKGR_PLLR_DIV(2);
}

#else

#warning CPU clock frequency non-standard setting: multiplier and divider values \
 will be computed at runtime: effective computed frequency could be different \
 from expected.

/*
 * Try to evaluate the correct divider and multiplier value depending
 * on the desired CPU frequency.
 *
 * We try all combinations in a certain range of divider and multiplier
 * values.  Start with higher multipliers and divisors, generally better.
 */
INLINE uint32_t evaluate_pll(void)
{
	int mul, div, best_mul, best_div;
	int best_delta = CPU_FREQ;
	int freq = 0;

	for (mul = 13; mul > 0; mul--)
	{
		for (div = 24; div > 0; div--)
		{
			freq = BOARDOSC_FREQ / div * (1 + mul);
			if (ABS((int)CPU_FREQ - freq) < best_delta) {
				best_delta = ABS((int)CPU_FREQ - freq);
				best_mul = mul;
				best_div = div;
			}
		}
	}

	return CKGR_PLLR_DIV(best_div) | CKGR_PLLR_MUL(best_mul);
}

#endif /* CPU_FREQ */

void clock_init(void)
{
	uint32_t timeout;

	/* Disable watchdog */
	WDT_MR = BV(WDT_WDDIS);

	/* Set wait states for flash access, needed for higher CPU clock rates */
	EEFC0_FMR = EEFC_FMR_FWS(3);
#ifdef EEFC1_FMR
	EEFC1_FMR = EEFC_FMR_FWS(3);
#endif

	// Initialize main oscillator
	if (!(CKGR_MOR & BV(CKGR_MOR_MOSCSEL)))
	{
		CKGR_MOR = CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCXTST(0x8)
			| BV(CKGR_MOR_MOSCRCEN) | BV(CKGR_MOR_MOSCXTEN);
		timeout = CLOCK_TIMEOUT;
		while (!(PMC_SR & BV(PMC_SR_MOSCXTS)) && --timeout);
	}

	// Switch to external oscillator
	CKGR_MOR = CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCXTST(0x8)
		| BV(CKGR_MOR_MOSCRCEN) | BV(CKGR_MOR_MOSCXTEN) | BV(CKGR_MOR_MOSCSEL);
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MOSCXTS)) && --timeout);

	// Initialize and enable PLL clock
	CKGR_PLLR = evaluate_pll() | BV(CKGR_PLLR_STUCKTO1) | CKGR_PLLR_PLLCOUNT(0x2);
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_LOCK)) && --timeout);

	PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK;
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MCKRDY)) && --timeout);

	PMC_MCKR = PMC_MCKR_CSS_PLL_CLK;
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MCKRDY)) && --timeout);

	/* Enable clock on PIO for inputs */
	// TODO: move this in gpio_init() for better power management?
	pmc_periphEnable(PIOA_ID);
	pmc_periphEnable(PIOB_ID);
	pmc_periphEnable(PIOC_ID);
#ifdef PIOF_ID
	pmc_periphEnable(PIOD_ID);
	pmc_periphEnable(PIOE_ID);
	pmc_periphEnable(PIOF_ID);
#endif
}
