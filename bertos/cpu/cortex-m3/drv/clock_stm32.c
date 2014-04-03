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
 * \brief STM32 Clocking driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "clock_stm32.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <io/stm32.h>

struct RCC *RCC;

INLINE int rcc_get_flag_status(uint32_t flag)
{
	uint32_t id;
	reg32_t reg;

	/* Get the RCC register index */
	id = flag >> 5;
	/* The flag to check is in CR register */
	if (id == 1)
		reg = RCC->CR;
	/* The flag to check is in BDCR register */
	else if (id == 2)
		reg = RCC->BDCR;
	/* The flag to check is in CSR register */
	else
		reg = RCC->CSR;
	/* Get the flag position */
	id = flag & FLAG_MASK;

	return reg & (1 << id);
}

INLINE uint16_t pll_clock(void)
{
	unsigned int div, mul;

	/* Hopefully this is evaluate at compile time... */
	for (div = 2; div; div--)
		for (mul = 2; mul <= 16; mul++)
			if (CPU_FREQ <= (PLL_VCO / div * mul))
				break;
	return mul << 8 | div;
}

INLINE void rcc_pll_config(void)
{
	reg32_t reg = RCC->CFGR & CFGR_PLL_MASK;

	/* Evaluate clock parameters */
	uint16_t clock = pll_clock();
	uint32_t pll_mul = ((clock >> 8) - 2) << 18;
	uint32_t pll_div = ((clock & 0xff) << 1 | 1) << 16;

	/* Set the PLL configuration bits */
	reg |= pll_div | pll_mul;

	/* Store the new value */
	RCC->CFGR = reg;

	/* Enable PLL */
	*CR_PLLON_BB = 1;
}

INLINE void rcc_set_clock_source(uint32_t source)
{
	reg32_t reg;

	reg = RCC->CFGR & CFGR_SW_MASK;
	reg |= source;
	RCC->CFGR = reg;
}

void clock_init(void)
{
	/* Initialize global RCC structure */
	RCC = (struct RCC *)RCC_BASE;

	/* Enable the internal oscillator */
	*CR_HSION_BB = 1;
	while (!rcc_get_flag_status(RCC_FLAG_HSIRDY));

	/* Clock the system from internal HSI RC (8 MHz) */
	rcc_set_clock_source(RCC_SYSCLK_HSI);

	/* Enable external oscillator */
	RCC->CR &= CR_HSEON_RESET;
	RCC->CR &= CR_HSEBYP_RESET;
	RCC->CR |= CR_HSEON_SET;
	while (!rcc_get_flag_status(RCC_FLAG_HSERDY));

	/* Initialize PLL according to CPU_FREQ */
	rcc_pll_config();
	while(!rcc_get_flag_status(RCC_FLAG_PLLRDY));

	/* Configure USB clock (48MHz) */
	*CFGR_USBPRE_BB = RCC_USBCLK_PLLCLK_1DIV5;
	/* Configure ADC clock: PCLK2 (9MHz) */
	RCC->CFGR &= CFGR_ADCPRE_RESET_MASK;
	RCC->CFGR |= RCC_PCLK2_DIV8;
	/* Configure system clock dividers: PCLK2 (72MHz) */
	RCC->CFGR &= CFGR_PPRE2_RESET_MASK;
	RCC->CFGR |= RCC_HCLK_DIV1 << 3;
	/* Configure system clock dividers: PCLK1 (36MHz) */
	RCC->CFGR &= CFGR_PPRE1_RESET_MASK;
	RCC->CFGR |= RCC_HCLK_DIV2;
	/* Configure system clock dividers: HCLK */
	RCC->CFGR &= CFGR_HPRE_RESET_MASK;
	RCC->CFGR |= RCC_SYSCLK_DIV1;

	/* Set 1 wait state for the flash memory */
	*(reg32_t *)FLASH_BASE = 0x12;

	/* Clock the system from the PLL */
	rcc_set_clock_source(RCC_SYSCLK_PLLCLK);
}
