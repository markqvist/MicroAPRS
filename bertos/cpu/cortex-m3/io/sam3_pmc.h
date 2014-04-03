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
 * \brief SAM3 PMC hardware.
 */

#ifndef SAM3_PMC_H
#define SAM3_PMC_H

#include <cfg/macros.h>
#include <cfg/debug.h>


/** PMC registers base. */
#if CPU_CM3_SAM3X
	#define PMC_BASE  0x400E0600
#else
	#define PMC_BASE  0x400E0400
#endif

/**
 * PMC register offsets.
 */
/*\{*/
#define PMC_SCER_OFF  0x00         ///< System Clock Enable Register
#define PMC_SCDR_OFF  0x04         ///< System Clock Disable Register
#define PMC_SCSR_OFF  0x08         ///< System Clock Status Register
#define PMC_MOR_OFF   0x20         ///< Main Oscillator Register
#define PMC_MCFR_OFF  0x24         ///< Main Clock Frequency Register
#define PMC_MCKR_OFF  0x30         ///< Master Clock Register
#define PMC_IER_OFF   0x60         ///< Interrupt Enable Register
#define PMC_IDR_OFF   0x64         ///< Interrupt Disable Register
#define PMC_SR_OFF    0x68         ///< Status Register
#define PMC_IMR_OFF   0x6C         ///< Interrupt Mask Register
#define PMC_FSMR_OFF  0x70         ///< Fast Startup Mode Register
#define PMC_FSPR_OFF  0x74         ///< Fast Startup Polarity Register
#define PMC_FOCR_OFF  0x78         ///< Fault Output Clear Register
#define PMC_WPMR_OFF  0xE4         ///< Write Protect Mode Register
#define PMC_WPSR_OFF  0xE8         ///< Write Protect Status Register

#if CPU_CM3_SAM3N
	#define PMC_PCER_OFF   0x10    ///< Peripheral Clock Enable Register
	#define PMC_PCDR_OFF   0x14    ///< Peripheral Clock Disable Register
	#define PMC_PCSR_OFF   0x18    ///< Peripheral Clock Status Register
	#define PMC_PLLR_OFF   0x28    ///< PLL Register
	#define PMC_PCK_OFF    0x40    ///< Programmable Clock 0 Register
	#define PMC_OCR_OFF    0x110   ///< Oscillator Calibration Register
#elif CPU_CM3_SAM3X
	#define PMC_PCER0_OFF  0x10    ///< Peripheral Clock Enable Register
	#define PMC_PCDR0_OFF  0x14    ///< Peripheral Clock Disable Register
	#define PMC_PCSR0_OFF  0x18    ///< Peripheral Clock Status Register
	#define PMC_UCKR_OFF   0x1C    ///< UTMI clock register
	#define PMC_PLLAR_OFF  0x28    ///< PLL Register
	#define PMC_USB_OFF    0x38    ///< USB clock register
	#define PMC_PCK0_OFF   0x40    ///< Programmable Clock 0 Register
	#define PMC_PCK1_OFF   0x44    ///< Programmable Clock 1 Register
	#define PMC_PCK2_OFF   0x48    ///< Programmable Clock 2 Register
	#define PMC_PCER1_OFF  0x100   ///< Peripheral Clock Enable Register
	#define PMC_PCDR1_OFF  0x104   ///< Peripheral Clock Disable Register
	#define PMC_PCSR1_OFF  0x108   ///< Peripheral Clock Status Register
	#define PMC_PCR_OFF    0x10C   ///< Oscillator Calibration Register

	#define PMC_PLLROFF    PMC_PLLAR_OFF
#else
	#warning Some PMC registers undefined for the selected CPU
#endif
/*\}*/


/**
 * Programmable clock ids.
 * \{
 */
#define  PMC_PCK0_ID   8
#define  PMC_PCK1_ID   9
#define  PMC_PCK2_ID  10
/*\}*/

/**
 * Programmable clock status.
 * \{
 */
#define PMC_PCKRDY0                          8  ///< Programmable clock 0 ready.
#define PMC_PCKRDY1                          9  ///< Programmable clock 1 ready.
#define PMC_PCKRDY2                         10  ///< Programmable clock 2 ready.
/*\}*/

/**
 * PMC registers.
 */
/*\{*/
#define PMC_SCER  (*((reg32_t *)(PMC_BASE + PMC_SCER_OFF)))   ///< System Clock Enable Register
#define PMC_SCDR  (*((reg32_t *)(PMC_BASE + PMC_SCDR_OFF)))   ///< System Clock Disable Register
#define PMC_SCSR  (*((reg32_t *)(PMC_BASE + PMC_SCSR_OFF)))   ///< System Clock Status Register
#define CKGR_MOR  (*((reg32_t *)(PMC_BASE + PMC_MOR_OFF )))   ///< Main Oscillator Register
#define CKGR_MCFR (*((reg32_t *)(PMC_BASE + PMC_MCFR_OFF)))   ///< Main Clock Frequency Register
#define PMC_MCKR  (*((reg32_t *)(PMC_BASE + PMC_MCKR_OFF)))   ///< Master Clock Register
#define PMC_IER   (*((reg32_t *)(PMC_BASE + PMC_IER_OFF )))   ///< Interrupt Enable Register
#define PMC_IDR   (*((reg32_t *)(PMC_BASE + PMC_IDR_OFF )))   ///< Interrupt Disable Register
#define PMC_SR    (*((reg32_t *)(PMC_BASE + PMC_SR_OFF  )))   ///< Status Register
#define PMC_IMR   (*((reg32_t *)(PMC_BASE + PMC_IMR_OFF )))   ///< Interrupt Mask Register
#define PMC_FSMR  (*((reg32_t *)(PMC_BASE + PMC_FSMR_OFF)))   ///< Fast Startup Mode Register
#define PMC_FSPR  (*((reg32_t *)(PMC_BASE + PMC_FSPR_OFF)))   ///< Fast Startup Polarity Register
#define PMC_FOCR  (*((reg32_t *)(PMC_BASE + PMC_FOCR_OFF)))   ///< Fault Output Clear Register
#define PMC_WPMR  (*((reg32_t *)(PMC_BASE + PMC_WPMR_OFF)))   ///< Write Protect Mode Register
#define PMC_WPSR  (*((reg32_t *)(PMC_BASE + PMC_WPSR_OFF)))   ///< Write Protect Status Register

#if CPU_CM3_SAM3N
	#define PMC_PCER   (*((reg32_t *)(PMC_BASE + PMC_PCER_OFF)))  ///< Peripheral Clock Enable Register
	#define PMC_PCDR   (*((reg32_t *)(PMC_BASE + PMC_PCDR_OFF)))  ///< Peripheral Clock Disable Register
	#define PMC_PCSR   (*((reg32_t *)(PMC_BASE + PMC_PCSR_OFF)))  ///< Peripheral Clock Status Register
	#define CKGR_PLLR  (*((reg32_t *)(PMC_BASE + PMC_PLLR_OFF)))  ///< PLL Register
	#define PMC_PCK    (*((reg32_t *)(PMC_BASE + PMC_PCK_OFF )))  ///< Programmable Clock 0 Register
	#define PMC_OCR    (*((reg32_t *)(PMC_BASE + PMC_OCR_OFF )))  ///< Oscillator Calibration Register
#elif CPU_CM3_SAM3X
	#define PMC_PCER0  (*((reg32_t *)(PMC_BASE + PMC_PCER0_OFF)))     ///< Peripheral Clock Enable Register
	#define PMC_PCDR0  (*((reg32_t *)(PMC_BASE + PMC_PCDR0_OFF)))     ///< Peripheral Clock Disable Register
	#define PMC_PCSR0  (*((reg32_t *)(PMC_BASE + PMC_PCSR0_OFF)))     ///< Peripheral Clock Status Register
	#define PMC_UCKR   (*((reg32_t *)(PMC_BASE + PMC_UCKR_OFF)))     ///< UTMI clock register
	#define CKGR_PLLAR (*((reg32_t *)(PMC_BASE + PMC_PLLAR_OFF)))     ///< PLL Register
	#define PMC_USB_O  (*((reg32_t *)(PMC_BASE + PMC_USB_O_OFF)))     ///< USB clock register
	#define PMC_PCK0   (*((reg32_t *)(PMC_BASE + PMC_PCK0_OFF)))     ///< Programmable Clock 0 Register
	#define PMC_PCK1   (*((reg32_t *)(PMC_BASE + PMC_PCK1_OFF)))     ///< Programmable Clock 1 Register
	#define PMC_PCK2   (*((reg32_t *)(PMC_BASE + PMC_PCK2_OFF)))     ///< Programmable Clock 2 Register
	#define PMC_PCER1  (*((reg32_t *)(PMC_BASE + PMC_PCER1_OFF)))     ///< Peripheral Clock Enable Register
	#define PMC_PCDR1  (*((reg32_t *)(PMC_BASE + PMC_PCDR1_OFF)))     ///< Peripheral Clock Disable Register
	#define PMC_PCSR1  (*((reg32_t *)(PMC_BASE + PMC_PCSR1_OFF)))     ///< Peripheral Clock Status Register
	#define PMC_PCR    (*((reg32_t *)(PMC_BASE + PMC_PCR_OFF)))     ///< Oscillator Calibration Register

	#define CKGR_PLLR  CKGR_PLLAR
#endif
/*\}*/

/**
 * Enable a peripheral clock.
 *
 * \param id  peripheral id of the peripheral whose clock is enabled
 */
#ifdef PMC_PCER1

INLINE void pmc_periphEnable(unsigned id)
{
	ASSERT(id < 64);
	if (id < 32)
		PMC_PCER0 = BV(id);
	else
		PMC_PCER1 = BV(id - 32);
}

#else

INLINE void pmc_periphEnable(unsigned id)
{
	ASSERT(id < 32);
	PMC_PCER = BV(id);
}

#endif

/**
 * Disable a peripheral clock.
 *
 * \param id  peripheral id of the peripheral whose clock is enabled
 */
#ifdef PMC_PCER1

INLINE void pmc_periphDisable(unsigned id)
{
	ASSERT(id < 64);
	if (id < 32)
		PMC_PCDR0 = BV(id);
	else
		PMC_PCDR1 = BV(id - 32);
}

#else

INLINE void pmc_periphDisable(unsigned id)
{
	ASSERT(id < 32);
	PMC_PCDR = BV(id);
}

#endif

/**
 * Defines for bit fields in PMC_SCER register.
 */
/*\{*/
#define PMC_SCER_PCK0  8   ///< Programmable Clock 0 Output Enable
#define PMC_SCER_PCK1  9   ///< Programmable Clock 1 Output Enable
#define PMC_SCER_PCK2  10  ///< Programmable Clock 2 Output Enable
/*\}*/

/**
 * Defines for bit fields in PMC_SCDR register.
 */
/*\{*/
#define PMC_SCDR_PCK0  8   ///< Programmable Clock 0 Output Disable
#define PMC_SCDR_PCK1  9   ///< Programmable Clock 1 Output Disable
#define PMC_SCDR_PCK2  10  ///< Programmable Clock 2 Output Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SCSR register.
 */
/*\{*/
#define PMC_SCSR_PCK0  8   ///< Programmable Clock 0 Output Status
#define PMC_SCSR_PCK1  9   ///< Programmable Clock 1 Output Status
#define PMC_SCSR_PCK2  10  ///< Programmable Clock 2 Output Status
/*\}*/

/**
 * Defines for bit fields in CKGR_MOR register.
 */
/*\{*/
#define CKGR_MOR_MOSCXTEN         0   ///< Main Crystal Oscillator Enable
#define CKGR_MOR_MOSCXTBY         1   ///< Main Crystal Oscillator Bypass
#define CKGR_MOR_WAITMODE         2   ///< Wait Mode Command
#define CKGR_MOR_MOSCRCEN         3   ///< Main On-Chip RC Oscillator Enable
#define CKGR_MOR_MOSCRCF_SHIFT    4
#define CKGR_MOR_MOSCRCF_MASK     (0x7 << CKGR_MOR_MOSCRCF_SHIFT)   ///< Main On-Chip RC Oscillator Frequency Selection
#define CKGR_MOR_MOSCRCF(value)   ((CKGR_MOR_MOSCRCF_MASK & ((value) << CKGR_MOR_MOSCRCF_SHIFT)))
#define CKGR_MOR_MOSCRCF_4MHZ   (0x0 << CKGR_MOR_MOSCRCF_SHIFT)
#define CKGR_MOR_MOSCRCF_8MHZ   (0x1 << CKGR_MOR_MOSCRCF_SHIFT)
#define CKGR_MOR_MOSCRCF_12MHZ  (0x2 << CKGR_MOR_MOSCRCF_SHIFT)
#define CKGR_MOR_MOSCXTST_SHIFT   8
#define CKGR_MOR_MOSCXTST_MASK    (0xff << CKGR_MOR_MOSCXTST_SHIFT)   ///< Main Crystal Oscillator Start-up Time
#define CKGR_MOR_MOSCXTST(value)  ((CKGR_MOR_MOSCXTST_MASK & ((value) << CKGR_MOR_MOSCXTST_SHIFT)))
#define CKGR_MOR_KEY_SHIFT        16
#define CKGR_MOR_KEY_MASK         (0xffu << CKGR_MOR_KEY_SHIFT)   ///< Password
#define CKGR_MOR_KEY(value)       ((CKGR_MOR_KEY_MASK & ((value) << CKGR_MOR_KEY_SHIFT)))
#define CKGR_MOR_MOSCSEL          24   ///< Main Oscillator Selection
#define CKGR_MOR_CFDEN            25   ///< Clock Failure Detector Enable
/*\}*/

/**
 * Defines for bit fields in CKGR_MCFR register.
 */
/*\{*/
#define CKGR_MCFR_MAINF_MASK  0xffff    ///< Main Clock Frequency mask
#define CKGR_MCFR_MAINFRDY    16        ///< Main Clock Ready
/*\}*/

/**
 * Defines for bit fields in CKGR_PLLR register.
 */
/*\{*/
#define CKGR_PLLR_DIV_MASK        0xff   ///< Divider mask
#define CKGR_PLLR_DIV(value)      (CKGR_PLLR_DIV_MASK & (value))
#define CKGR_PLLR_PLLCOUNT_SHIFT  8
#define CKGR_PLLR_PLLCOUNT_MASK   (0x3f << CKGR_PLLR_PLLCOUNT_SHIFT)   ///< PLL Counter mask
#define CKGR_PLLR_PLLCOUNT(value) (CKGR_PLLR_PLLCOUNT_MASK & ((value) << CKGR_PLLR_PLLCOUNT_SHIFT))
#define CKGR_PLLR_MUL_SHIFT       16
#define CKGR_PLLR_MUL_MASK        (0x7ff << CKGR_PLLR_MUL_SHIFT)   ///< PLL Multiplier mask
#define CKGR_PLLR_MUL(value)      (CKGR_PLLR_MUL_MASK & ((value) << CKGR_PLLR_MUL_SHIFT))
#define CKGR_PLLR_STUCKTO1        29
/*\}*/

/**
 * Defines for bit fields in PMC_MCKR register.
 */
/*\{*/
#define   PMC_MCKR_CSS_MASK        0x3   ///< Master Clock Source Selection mask
#define   PMC_MCKR_CSS_SLOW_CLK  0x0   ///< Slow Clock is selected
#define   PMC_MCKR_CSS_MAIN_CLK  0x1   ///< Main Clock is selected
#define   PMC_MCKR_CSS_PLL_CLK   0x2   ///< PLL Clock is selected
#define   PMC_MCKR_PRES_SHIFT      4
#define   PMC_MCKR_PRES_MASK       (0x7 << PMC_MCKR_PRES_SHIFT)    ///< Processor Clock Prescaler mask
#define   PMC_MCKR_PRES_CLK      (0x0 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock
#define   PMC_MCKR_PRES_CLK_2    (0x1 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 2
#define   PMC_MCKR_PRES_CLK_4    (0x2 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 4
#define   PMC_MCKR_PRES_CLK_8    (0x3 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 8
#define   PMC_MCKR_PRES_CLK_16   (0x4 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 16
#define   PMC_MCKR_PRES_CLK_32   (0x5 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 32
#define   PMC_MCKR_PRES_CLK_64   (0x6 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 64
#define   PMC_MCKR_PRES_CLK_3    (0x7 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 3
#define   PMC_MCKR_PLLDIV2         12   ///< PLL Divisor by 2
/*\}*/

/**
 * Defines for bit fields in PMC_PCK[3] register.
 */
/*\{*/
#define   PMC_PCK_CSS_MASK     0x7   ///< Master Clock Source Selection mask
#define   PMC_PCK_CSS_SLOW     0x0   ///< Slow Clock is selected
#define   PMC_PCK_CSS_MAIN     0x1   ///< Main Clock is selected
#define   PMC_PCK_CSS_PLL      0x2   ///< PLL Clock is selected
#define   PMC_PCK_CSS_MCK      0x4   ///< Master Clock is selected
#define   PMC_PCK_PRES_SHIFT     4
#define   PMC_PCK_PRES_MASK    (0x7 << PMC_PCK_PRES_SHIFT)   ///< Programmable Clock Prescaler
#define   PMC_PCK_PRES_CLK     (0x0 << PMC_PCK_PRES_SHIFT)   ///< Selected clock
#define   PMC_PCK_PRES_CLK_2   (0x1 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 2
#define   PMC_PCK_PRES_CLK_4   (0x2 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 4
#define   PMC_PCK_PRES_CLK_8   (0x3 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 8
#define   PMC_PCK_PRES_CLK_16  (0x4 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 16
#define   PMC_PCK_PRES_CLK_32  (0x5 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 32
#define   PMC_PCK_PRES_CLK_64  (0x6 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 64
/*\}*/

/**
 * Defines for bit fields in PMC_IER register.
 */
/*\{*/
#define PMC_IER_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Enable
#define PMC_IER_LOCK      1   ///< PLL Lock Interrupt Enable
#define PMC_IER_MCKRDY    3   ///< Master Clock Ready Interrupt Enable
#define PMC_IER_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Enable
#define PMC_IER_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Enable
#define PMC_IER_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Enable
#define PMC_IER_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Enable
#define PMC_IER_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Enable
#define PMC_IER_CFDEV     18  ///< Clock Failure Detector Event Interrupt Enable
/*\}*/

/**
 * Defines for bit fields in PMC_IDR register.
 */
/*\{*/
#define PMC_IDR_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Disable
#define PMC_IDR_LOCK      1   ///< PLL Lock Interrupt Disable
#define PMC_IDR_MCKRDY    3   ///< Master Clock Ready Interrupt Disable
#define PMC_IDR_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Disable
#define PMC_IDR_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Disable
#define PMC_IDR_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Disable
#define PMC_IDR_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Disable
#define PMC_IDR_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Disable
#define PMC_IDR_CFDEV     18  ///< Clock Failure Detector Event Interrupt Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SR register.
 */
/*\{*/
#define PMC_SR_MOSCXTS   0   ///< Main XTAL Oscillator Status
#define PMC_SR_LOCK      1   ///< PLL Lock Status
#define PMC_SR_MCKRDY    3   ///< Master Clock Status
#define PMC_SR_OSCSELS   7   ///< Slow Clock Oscillator Selection
#define PMC_SR_PCKRDY0   8   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY1   9   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY2   10  ///< Programmable Clock Ready Status
#define PMC_SR_MOSCSELS  16  ///< Main Oscillator Selection Status
#define PMC_SR_MOSCRCS   17  ///< Main On-Chip RC Oscillator Status
#define PMC_SR_CFDEV     18  ///< Clock Failure Detector Event
#define PMC_SR_CFDS      19  ///< Clock Failure Detector Status
#define PMC_SR_FOS       20  ///< Clock Failure Detector Fault Output Status
/*\}*/

/**
 * Defines for bit fields in PMC_IMR register.
 */
/*\{*/
#define PMC_IMR_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Mask
#define PMC_IMR_LOCK      1   ///< PLL Lock Interrupt Mask
#define PMC_IMR_MCKRDY    3   ///< Master Clock Ready Interrupt Mask
#define PMC_IMR_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Mask
#define PMC_IMR_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Mask
#define PMC_IMR_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Mask
#define PMC_IMR_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Mask
#define PMC_IMR_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Mask
#define PMC_IMR_CFDEV     18  ///< Clock Failure Detector Event Interrupt Mask
/*\}*/

/**
 * Defines for bit fields in PMC_FSMR register.
 */
/*\{*/
#define PMC_FSMR_FSTT0   0   ///< Fast Startup Input Enable 0
#define PMC_FSMR_FSTT1   1   ///< Fast Startup Input Enable 1
#define PMC_FSMR_FSTT2   2   ///< Fast Startup Input Enable 2
#define PMC_FSMR_FSTT3   3   ///< Fast Startup Input Enable 3
#define PMC_FSMR_FSTT4   4   ///< Fast Startup Input Enable 4
#define PMC_FSMR_FSTT5   5   ///< Fast Startup Input Enable 5
#define PMC_FSMR_FSTT6   6   ///< Fast Startup Input Enable 6
#define PMC_FSMR_FSTT7   7   ///< Fast Startup Input Enable 7
#define PMC_FSMR_FSTT8   8   ///< Fast Startup Input Enable 8
#define PMC_FSMR_FSTT9   9   ///< Fast Startup Input Enable 9
#define PMC_FSMR_FSTT10  10  ///< Fast Startup Input Enable 10
#define PMC_FSMR_FSTT11  11  ///< Fast Startup Input Enable 11
#define PMC_FSMR_FSTT12  12  ///< Fast Startup Input Enable 12
#define PMC_FSMR_FSTT13  13  ///< Fast Startup Input Enable 13
#define PMC_FSMR_FSTT14  14  ///< Fast Startup Input Enable 14
#define PMC_FSMR_FSTT15  15  ///< Fast Startup Input Enable 15
#define PMC_FSMR_RTTAL   16  ///< RTT Alarm Enable
#define PMC_FSMR_RTCAL   17  ///< RTC Alarm Enable
#define PMC_FSMR_LPM     20  ///< Low Power Mode
/*\}*/

/**
 * Defines for bit fields in PMC_FSPR register.
 */
/*\{*/
#define PMC_FSPR_FSTP0   0   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP1   1   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP2   2   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP3   3   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP4   4   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP5   5   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP6   6   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP7   7   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP8   8   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP9   9   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP10  10  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP11  11  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP12  12  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP13  13  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP14  14  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP15  15  ///< Fast Startup Input Polarityx
/*\}*/

/**
 * Defines for bit fields in PMC_FOCR register.
 */
/*\{*/
#define PMC_FOCR_FOCLR  0   ///< Fault Output Clear
/*\}*/

/**
 * Defines for bit fields in PMC_WPMR register.
 */
/*\{*/
#define PMC_WPMR_WPEN          0   ///< Write Protect Enable
#define PMC_WPMR_WPKEY_SHIFT   8
#define PMC_WPMR_WPKEY_MASK    (0xffffff << PMC_WPMR_WPKEY_SHIFT)   ///< Write Protect key mask
#define PMC_WPMR_WPKEY(value)  ((PMC_WPMR_WPKEY_MASK & ((value) << PMC_WPMR_WPKEY_SHIFT)))
/*\}*/

/**
 * Defines for bit fields in PMC_WPSR register.
 */
/*\{*/
#define PMC_WPSR_WPVS          0   ///< Write Protect Violation Status
#define PMC_WPSR_WPVSRC_SHIFT  8
#define PMC_WPSR_WPVSRC_MASK   (0xffff << PMC_WPSR_WPVSRC_SHIFT)   ///< Write Protect Violation Source mask
/*\}*/

/**
 * Defines for bit fields in PMC_OCR register.
 */
/*\{*/
#define PMC_OCR_CAL4_MASK     0x7f  ///< RC Oscillator Calibration bits for 4 MHz mask
#define PMC_OCR_CAL4(value)   (PMC_OCR_CAL4_MASK & (value))
#define PMC_OCR_SEL4          7   ///< Selection of RC Oscillator Calibration bits for 4 MHz
#define PMC_OCR_CAL8_SHIFT    8
#define PMC_OCR_CAL8_MASK     (0x7f << PMC_OCR_CAL8_SHIFT)   ///< RC Oscillator Calibration bits for 8 MHz mask
#define PMC_OCR_CAL8(value)   ((PMC_OCR_CAL8_MASK & ((value) << PMC_OCR_CAL8_SHIFT)))
#define PMC_OCR_SEL8          15  ///< Selection of RC Oscillator Calibration bits for 8 MHz
#define PMC_OCR_CAL12_SHIFT   16
#define PMC_OCR_CAL12_MASK    (0x7f << PMC_OCR_CAL12_SHIFT)   ///< RC Oscillator Calibration bits for 12 MHz mask
#define PMC_OCR_CAL12(value)  ((PMC_OCR_CAL12_MASK & ((value) << PMC_OCR_CAL12_SHIFT)))
#define PMC_OCR_SEL12         23   ///< Selection of RC Oscillator Calibration bits for 12 MHz
/*\}*/


#endif /* SAM3_PMC_H */
