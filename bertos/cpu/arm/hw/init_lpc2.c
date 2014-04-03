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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief LPC2378 CRT.
 * notest:arm
 */
#include <cpu/irq.h>
#include <drv/vic_lpc2.h>
#include <io/lpc23xx.h>

#if CPU_FREQ != 72000000UL
	/* Avoid errors on nightly test */
	#if !defined(ARCH_NIGHTTEST) || !(ARCH & ARCH_NIGHTTEST)
		#warning Clock registers set for 72MHz operation, revise following code if you want a different clock.
	#endif
#endif

/*
 * With a 12MHz cristal, master clock is:
 * (((2 * 12 * (PLL_MUL_VAL + 1)) / (PLL_DIV_VAL + 1)) / (LPC2_CPUCLOCK_DIV + 1))= 72MHz
 */
#define PLL_MUL_VAL  11
#define PLL_DIV_VAL  0
#define LPC2_CPUCLOCK_DIV 3


/* PLL feed sequence */
#define PLL_FEED_SEQ() ATOMIC(PLLFEED = 0xAA; PLLFEED = 0x55;)

static void configurePll(void)
{
	/* Disconnect and disable the PLL, if already active */
	if (PLLSTAT & (1 << 25))
	{
		/* Disconnect PLL, but leave it enabled */
		PLLCON = 0x01;
		PLL_FEED_SEQ();
		/* Disable PLL */
		PLLCON = 0;
		PLL_FEED_SEQ();
	}

	/* Enable the main oscillator and wait for it to be stable */
	SCS |= (1 << 5);
    while (!(SCS & (1 << 6))) ;

	/* Select the main oscillator as the PLL clock source */
	CLKSRCSEL = 0x01;

	/* Set up PLL mul and div */
	PLLCFG = PLL_MUL_VAL | (PLL_DIV_VAL << 16);
	PLL_FEED_SEQ();
	
	/* Enable PLL, disconnected */
	PLLCON = 0x01;
	PLL_FEED_SEQ();

	/* Set clock divider */
	CCLKCFG = LPC2_CPUCLOCK_DIV;

	/* Wait for the PLL to lock */
	while (!(PLLSTAT & (1 << 26))) ;
    
	/* Enable and connect the PLL */
    PLLCON = 0x03;
	PLL_FEED_SEQ();
}

void __init1(void);

void __init1(void)
{
	/* Map irq vectors to internal flash */
	MEMMAP = 0x01;
	/* Configure PLL, switch from IRC to Main OSC */
	configurePll();

	/* Set memory accelerator module flash timings */
#if CPU_FREQ < 20000000UL
	MAMTIM = 1;
#elif CPU_FREQ < 40000000UL
	MAMTIM = 2;
#elif CPU_FREQ < 60000000UL
	MAMTIM = 3;
#else
	MAMTIM = 4;
#endif
	
	/* Memory accelerator module fully enabled */
	MAMCR = 0x02;
}

void __init2(void);

void __init2(void)
{
	vic_init();
}
