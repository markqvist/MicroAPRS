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
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief AT91SAM7S256 CRT.
 */

#include <io/arm.h>
#include <cfg/macros.h>

#define USE_FIXED_PLL 1

#define XTAL_FREQ 18432000UL

#if USE_FIXED_PLL
	#if CPU_FREQ != 48054857L
		/* Avoid errors on nightly test */
		#if !defined(ARCH_NIGHTTEST) || !(ARCH & ARCH_NIGHTTEST)
			#warning Clock registers set for 48.055MHz operation, revise following code if you want a different clock.
		#endif
	#endif

	/*
	 * With a 18.432MHz cristal, master clock is:
	 * (((18.432 * (PLL_MUL_VAL + 1)) / PLL_DIV_VAL) / AT91MCK_PRES) = 48.055MHz
	 */
	#define PLL_MUL_VAL  72  /**< Real multiplier value is PLL_MUL_VAL + 1! */
	#define PLL_DIV_VAL  14
	#define AT91MCK_PRES PMC_PRES_CLK_2

#else /* !USE_FIXED_PLL*/

	#define PLL_IN_MIN  1000000UL
	#define PLL_IN_MAX  32000000UL
	#define PLL_OUT_MIN 80000000UL
	#define PLL_OUT_MAX 160000000UL

	#define DIV_HARD_MIN 1
	#define DIV_HARD_MAX 255

	#define DIV_MIN  (DIV_ROUND(XTAL_FREQ, PLL_IN_MAX) \
		< DIV_HARD_MIN ? DIV_HARD_MIN : DIV_ROUND(XTAL_FREQ, PLL_IN_MAX))

	#define DIV_MAX  (DIV_ROUND(XTAL_FREQ, PLL_IN_MIN) \
		> DIV_HARD_MAX ? DIV_HARD_MAX : DIV_ROUND(XTAL_FREQ, PLL_IN_MIN))

	#define MUL_MIN  0
	#define MUL_MAX  2047

	typedef struct PllRegs
	{
		uint32_t mul;
		uint32_t div;
		uint32_t pres;
	} PllRegs;

	/**
	 * Code used to automatically compute the PLL registers.
	 * Since the processor uses the internal 32kHz oscillator
	 * this function takes a lot of time to be executed (~3s!).
	 */
	static const PllRegs pllCostants(void)
	{
		uint32_t best_err = CPU_FREQ;
		PllRegs res;

		for (uint32_t div = DIV_MIN; div <= DIV_MAX; div++)
		{
			for (uint32_t pres = 0; pres < 8; pres++)
			{
				uint32_t mul = DIV_ROUND((CPU_FREQ * div) << pres, XTAL_FREQ) - 1;
				if (mul <= MUL_MAX)
				{
					uint32_t pll = (XTAL_FREQ * (mul + 1)) / div;
					if (pll >= PLL_OUT_MIN && pll <= PLL_OUT_MAX)
					{
						uint32_t err = ABS((int32_t)((pll >> pres) - CPU_FREQ));
						if (err == 0)
						{
							res.div = div;
							res.mul = mul;
							res.pres = pres;
							return res;
						}
						if (err < best_err)
						{
							best_err = err;
							res.div = div;
							res.mul = mul;
							res.pres = pres;
						}
					}
				}
			}
		}
		return res;
	}
#endif  /* !USE_FIXED_PLL*/

/*
 * Override dummy hardware init functions supplied by the ASM startup
 * routine.
 */

void __init1(void);
void __init2(void);

/**
 * Early hardware initialization routine1.
 * This will be called by the ASM CRT routine just
 * *before* clearing .bss and loading .data sections.
 * Usually only basic tasks are performed here (i.e. setting the PLL).
 * For more generic tasks, __init2() should be used.
 *
 * \note Please keep in mind that since .bss and .data are not yet set, care
 *       must be taken. No static data can be used inside this funcition.
 *       Also some libc functions or floating point operations could potentially
 *       use initialized static data, be aware!
 */
void __init1(void)
{
	/*
	 * Compute number of master clock cycles in 1.5us.
	 * Needed by flash writing functions.
	 * The maximum FMCN value is 0xFF and 0 can be used only if
	 * master clock is less than 33kHz.
	 */
	#define MCN  DIV_ROUNDUP(CPU_FREQ, 666667UL)
	#define FMCN (CPU_FREQ <= 33333UL ? 0 : (MCN < 0xFF ? MCN : 0xFF))

	#if CPU_FREQ < 30000000UL
		/* Use 1 cycles for flash access. */
		MC_FMR = FMCN << MC_FMCN_SHIFT | MC_FWS_1R2W;
	#else
		/* Use 2 cycles for flash access. */
		MC_FMR = FMCN << MC_FMCN_SHIFT | MC_FWS_2R3W;
	#endif

        /* Disable all interrupts. Useful for debugging w/o target reset. */
	AIC_EOICR = 0xFFFFFFFF;
	AIC_IDCR =  0xFFFFFFFF;

        /* The watchdog is enabled after processor reset. Disable it. */
	WDT_MR = BV(WDT_WDDIS);

        /*
         * Enable the main oscillator. Set startup time of 6 * 8 slow
         * clock cycles and wait until oscillator is stabilized.
         */
	CKGR_MOR = (6 << 8) | BV(CKGR_MOSCEN);
	while (!(PMC_SR & BV(PMC_MOSCS))) ;

        /* Switch to Slow oscillator clock. */
	PMC_MCKR &= ~PMC_CSS_MASK;
	while (!(PMC_SR & BV(PMC_MCKRDY))) ;

        /* Switch to prescaler div 1 factor. */
	PMC_MCKR &= ~PMC_PRES_MASK;
	while (!(PMC_SR & BV(PMC_MCKRDY))) ;

	uint32_t div, pres, mul;
	#if USE_FIXED_PLL
		div = PLL_DIV_VAL;
		mul = PLL_MUL_VAL;
		pres = AT91MCK_PRES;
	#else
		PllRegs pll = pllCostants();
		div = pll.div;
		mul = pll.mul;
		pres = pll.pres << PMC_PRES_SHIFT;
	#endif

        /*
         * Set PLL:
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Wait 28 clock cycles until PLL is locked.
         */
	CKGR_PLLR = ((mul << CKGR_MUL_SHIFT)
		| (28 << CKGR_PLLCOUNT_SHIFT) | div);
	while (!(PMC_SR & BV(PMC_LOCK))) ;

	/* Set master clock prescaler.  */
	PMC_MCKR = pres;
	while (!(PMC_SR & BV(PMC_MCKRDY))) ;

        /*
         * Switch to PLL clock. Trying to set this together with the
         * prescaler fails (see datasheets).
         */
	PMC_MCKR |= PMC_CSS_PLL_CLK;
	while (!(PMC_SR & BV(PMC_MCKRDY))) ;
}

/**
 * Early hardware initialization routine2.
 * This will be called by the ASM CRT routine just
 * *after* clearing .bss and loading .data sections and before calling main().
 */
void __init2(void)
{
	/* Enable external reset key. */
	RSTC_MR = (RSTC_KEY | BV(RSTC_URSTEN));

	/* Enable clock for PIO(s) */
	PMC_PCER = BV(PIOA_ID);
	#if CPU_ARM_SAM7X
		PMC_PCER |= BV(PIOB_ID);
	#endif
}
