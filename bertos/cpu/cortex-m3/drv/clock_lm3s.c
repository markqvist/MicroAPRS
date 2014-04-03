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
 * \brief LM3S1968 Clocking driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "clock_lm3s.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <io/lm3s.h>


/* The PLL VCO frequency is 400 MHz */
#define PLL_VCO	400000000UL

/* Extract the system clock divisor from the RCC register */
#define RCC_TO_DIV(rcc)						\
		(((rcc & SYSCTL_RCC_SYSDIV_MASK) >>		\
				SYSCTL_RCC_SYSDIV_SHIFT) + 1)

/*
 * Very small delay: each loop takes 3 cycles.
 */
void NAKED lm3s_busyWait(unsigned long iterations)
{
	register uint32_t __n asm("r0") = iterations;

	asm volatile (
		"1: subs r0, #1\n\t"
		"bne 1b\n\t"
		"bx lr\n\t"
		: : "r"(__n) : "memory", "cc");
}

INLINE unsigned long clock_get_rate(void)
{
	reg32_t rcc = HWREG(SYSCTL_RCC);

	return rcc & SYSCTL_RCC_USESYSDIV ?
			PLL_VCO / 2 / RCC_TO_DIV(rcc) : PLL_VCO;
}

/*
 * Try to evaluate the correct SYSDIV value depending on the desired CPU
 * frequency.
 */
INLINE int evaluate_sysdiv(unsigned long freq)
{
	int i;

	 /*
	  * NOTE: with BYPASS=0, SYSDIV < 3 are reserved values (see LM3S1968
	  * Microcontroller DATASHEET, p.78).
	  */
	for (i = 3; i < 16; i++)
		if (freq >= (PLL_VCO / 2 / (i + 1)))
			break;
	return i;
}

void clock_init(void)
{
	reg32_t rcc, rcc2;
	unsigned long clk;
	int i;

	/*
	 * PLL may not function properly at default LDO setting.
	 *
	 * Description:
	 *
	 * In designs that enable and use the PLL module, unstable device
	 * behavior may occur with the LDO set at its default of 2.5 volts or
	 * below (minimum of 2.25 volts). Designs that do not use the PLL
	 * module are not affected.
	 *
	 * Workaround: Prior to enabling the PLL module, it is recommended that
	 * the default LDO voltage setting of 2.5 V be adjusted to 2.75 V using
	 * the LDO Power Control (LDOPCTL) register.
	 *
	 * Silicon Revision Affected: A1, A2
	 *
	 * See also: Stellaris LM3S1968 A2 Errata documentation.
	 */
	if (REVISION_IS_A1 | REVISION_IS_A2)
		HWREG(SYSCTL_LDOPCTL) = SYSCTL_LDOPCTL_2_75V;

	rcc = HWREG(SYSCTL_RCC);
	rcc2 = HWREG(SYSCTL_RCC2);

	/*
	 * Step #1: bypass the PLL and system clock divider by setting the
	 * BYPASS bit and clearing the USESYS bit in the RCC register. This
	 * configures the system to run off a “raw” clock source (using the
	 * main oscillator or internal oscillator) and allows for the new PLL
	 * configuration to be validated before switching the system clock to
	 * the PLL.
	 */
	rcc |= SYSCTL_RCC_BYPASS;
	rcc &= ~SYSCTL_RCC_USESYSDIV;
	rcc2 |= SYSCTL_RCC2_BYPASS2;

	/* Write back RCC/RCC2 registers */
	HWREG(SYSCTL_RCC) = rcc;
	HWREG(SYSCTL_RCC) = rcc2;

	lm3s_busyWait(16);

	/*
	 * Step #2: select the crystal value (XTAL) and oscillator source
	 * (OSCSRC), and clear the PWRDN bit in RCC/RCC2. Setting the XTAL
	 * field automatically pulls valid PLL configuration data for the
	 * appropriate crystal, and clearing the PWRDN bit powers and enables
	 * the PLL and its output.
	 */

	/* Enable the main oscillator first. */
	rcc &= ~(SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS);
	rcc |= SYSCTL_RCC_IOSCDIS;

	/* Do not override RCC register fields */
	rcc2 &= ~SYSCTL_RCC2_USERCC2;

	rcc &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M | SYSCTL_RCC_PWRDN);
	rcc |= XTAL_FREQ | SYSCTL_RCC_OSCSRC_MAIN;

	/* Clear the PLL lock interrupt. */
	HWREG(SYSCTL_MISC) = SYSCTL_INT_PLL_LOCK;

        HWREG(SYSCTL_RCC) = rcc;
	HWREG(SYSCTL_RCC) = rcc2;

	lm3s_busyWait(16);

	/*
	 * Step #3: select the desired system divider (SYSDIV) in RCC/RCC2 and
	 * set the USESYS bit in RCC. The SYSDIV field determines the system
	 * frequency for the microcontroller.
	 */
	rcc &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV);

	clk = PLL_VCO / 2;
	for (i = 3; i < 16; i++)
		if (CPU_FREQ >= (clk / (i + 1)))
			break;
	rcc |= SYSCTL_RCC_USESYSDIV |
			(evaluate_sysdiv(CPU_FREQ) << SYSCTL_RCC_SYSDIV_SHIFT);

	/*
	 * Step #4: wait for the PLL to lock by polling the PLLLRIS bit in the
	 * Raw Interrupt Status (RIS) register.
	 */
        for (i = 0; i < 32768; i++)
		if (HWREG(SYSCTL_RIS) & SYSCTL_INT_PLL_LOCK)
			break;

	/*
	 * Step #5: enable use of the PLL by clearing the BYPASS bit in
	 * RCC/RCC2.
	 */
        rcc &= ~SYSCTL_RCC_BYPASS;

	HWREG(SYSCTL_RCC) = rcc;

	lm3s_busyWait(16);
}
