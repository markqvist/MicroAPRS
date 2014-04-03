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
 * Copyright 2010,2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief SAM3 interrupt definitions.
 */

#ifndef SAM3_INTS_H
#define SAM3_INTS_H

/**
 * Defines for the fault assignments.
 */
/*\{*/
#define FAULT_NMI               2           ///< NMI fault
#define FAULT_HARD              3           ///< Hard fault
#define FAULT_MPU               4           ///< MPU fault
#define FAULT_BUS               5           ///< Bus fault
#define FAULT_USAGE             6           ///< Usage fault
#define FAULT_SVCALL            11          ///< SVCall
#define FAULT_DEBUG             12          ///< Debug monitor
#define FAULT_PENDSV            14          ///< PendSV
#define FAULT_SYSTICK           15          ///< System Tick
/*\}*/

/**
 * Defines for the interrupt assignments.
 */
/*\{*/
#define INT_PERIPH_BASE  16

#if CPU_CM3_SAM3N
	#define INT_SUPC        (INT_PERIPH_BASE + SUPC_ID)
	#define INT_RSTC        (INT_PERIPH_BASE + RSTC_ID)
	#define INT_RTC         (INT_PERIPH_BASE + RTC_ID)
	#define INT_RTT         (INT_PERIPH_BASE + RTT_ID)
	#define INT_WDT         (INT_PERIPH_BASE + WDT_ID)
	#define INT_PMC         (INT_PERIPH_BASE + PMC_ID)
	#define INT_EFC         (INT_PERIPH_BASE + EFC_ID)
	#define INT_UART0       (INT_PERIPH_BASE + UART0_ID)
	#define INT_UART1       (INT_PERIPH_BASE + UART1_ID)
	#define INT_PIOA        (INT_PERIPH_BASE + PIOA_ID)
	#define INT_PIOB        (INT_PERIPH_BASE + PIOB_ID)
	#define INT_PIOC        (INT_PERIPH_BASE + PIOC_ID)
	#define INT_US0         (INT_PERIPH_BASE + US0_ID)
	#define INT_US1         (INT_PERIPH_BASE + US1_ID)
	#define INT_TWI0        (INT_PERIPH_BASE + TWI0_ID)
	#define INT_TWI1        (INT_PERIPH_BASE + TWI1_ID)
	#define INT_SPI0        (INT_PERIPH_BASE + SPI0_ID)
	#define INT_TC0         (INT_PERIPH_BASE + TC0_ID)
	#define INT_TC1         (INT_PERIPH_BASE + TC1_ID)
	#define INT_TC2         (INT_PERIPH_BASE + TC2_ID)
	#define INT_TC3         (INT_PERIPH_BASE + TC3_ID)
	#define INT_TC4         (INT_PERIPH_BASE + TC4_ID)
	#define INT_TC5         (INT_PERIPH_BASE + TC5_ID)
	#define INT_ADC         (INT_PERIPH_BASE + ADC_ID)
	#define INT_DACC        (INT_PERIPH_BASE + DACC_ID)
	#define INT_PWM         (INT_PERIPH_BASE + PWM_ID)

	// Total number of interrupts.
	#define NUM_INTERRUPTS  48

#elif CPU_CM3_SAM3X
	#define INT_SUPC        (INT_PERIPH_BASE + SUPC_ID)
	#define INT_RSTC        (INT_PERIPH_BASE + RSTC_ID)
	#define INT_RTC         (INT_PERIPH_BASE + RTC_ID)
	#define INT_RTT         (INT_PERIPH_BASE + RTT_ID)
	#define INT_WDT         (INT_PERIPH_BASE + WDT_ID)
	#define INT_PMC         (INT_PERIPH_BASE + PMC_ID)
	#define INT_EEFC0       (INT_PERIPH_BASE + EEFC0_ID)
	#define INT_EEFC1       (INT_PERIPH_BASE + EEFC1_ID)
	#define INT_UART0       (INT_PERIPH_BASE + UART0_ID)
	#define INT_SMC_SDRAMC  (INT_PERIPH_BASE + SMC_SDRAMC_ID)
	#define INT_SDRAMC      (INT_PERIPH_BASE + SDRAMC_ID)
	#define INT_PIOA        (INT_PERIPH_BASE + PIOA_ID)
	#define INT_PIOB        (INT_PERIPH_BASE + PIOB_ID)
	#define INT_PIOC        (INT_PERIPH_BASE + PIOC_ID)
	#define INT_PIOD        (INT_PERIPH_BASE + PIOD_ID)
	#define INT_PIOE        (INT_PERIPH_BASE + PIOE_ID)
	#define INT_PIOF        (INT_PERIPH_BASE + PIOF_ID)
	#define INT_US0         (INT_PERIPH_BASE + US0_ID)
	#define INT_US1         (INT_PERIPH_BASE + US1_ID)
	#define INT_US2         (INT_PERIPH_BASE + US2_ID)
	#define INT_US3         (INT_PERIPH_BASE + US3_ID)
	#define INT_HSMCI       (INT_PERIPH_BASE + HSMCI_ID)
	#define INT_TWI0        (INT_PERIPH_BASE + TWI0_ID)
	#define INT_TWI1        (INT_PERIPH_BASE + TWI1_ID)
	#define INT_SPI0        (INT_PERIPH_BASE + SPI0_ID)
	#define INT_SPI1        (INT_PERIPH_BASE + SPI1_ID)
	#define INT_SSC         (INT_PERIPH_BASE + SSC_ID)
	#define INT_TC0         (INT_PERIPH_BASE + TC0_ID)
	#define INT_TC1         (INT_PERIPH_BASE + TC1_ID)
	#define INT_TC2         (INT_PERIPH_BASE + TC2_ID)
	#define INT_TC3         (INT_PERIPH_BASE + TC3_ID)
	#define INT_TC4         (INT_PERIPH_BASE + TC4_ID)
	#define INT_TC5         (INT_PERIPH_BASE + TC5_ID)
	#define INT_TC6         (INT_PERIPH_BASE + TC6_ID)
	#define INT_TC7         (INT_PERIPH_BASE + TC7_ID)
	#define INT_TC8         (INT_PERIPH_BASE + TC8_ID)
	#define INT_PWM         (INT_PERIPH_BASE + PWM_ID)
	#define INT_ADC         (INT_PERIPH_BASE + ADC_ID)
	#define INT_DACC        (INT_PERIPH_BASE + DACC_ID)
	#define INT_DMAC        (INT_PERIPH_BASE + DMAC_ID)
	#define INT_UOTGHS      (INT_PERIPH_BASE + UOTGHS_ID)
	#define INT_TRNG        (INT_PERIPH_BASE + TRNG_ID)
	#define INT_EMAC        (INT_PERIPH_BASE + EMAC_ID)
	#define INT_CAN0        (INT_PERIPH_BASE + CAN0_ID)
	#define INT_CAN1        (INT_PERIPH_BASE + CAN1_ID)

	// Total number of interrupts.
	#define NUM_INTERRUPTS  61

#else
	#error Peripheral IDs undefined
#endif
/*\}*/

#endif /* SAM3_INTS_H */
