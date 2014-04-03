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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * AT91SAM7 register definitions.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef AT91SAM7_H
#define AT91SAM7_H

#include <cfg/compiler.h>

#if CPU_ARM_SAM7X || CPU_ARM_SAM7S_LARGE
	#define FLASH_BASE      0x100000UL
	#define RAM_BASE        0x200000UL

	#define TC_BASE         0xFFFA0000      ///< Timer/counter base address.
	#define UDP_BASE        0xFFFB0000      ///< USB device port base address.
	#define TWI_BASE        0xFFFB8000      ///< Two-wire interface base address.
	#define USART0_BASE     0xFFFC0000      ///< USART 0 base address.
	#define USART1_BASE     0xFFFC4000      ///< USART 1 base address.
	#define PWMC_BASE       0xFFFCC000      ///< PWM controller base address.
	#define SSC_BASE        0xFFFD4000      ///< Serial synchronous controller base address.
	#define ADC_BASE        0xFFFD8000      ///< ADC base address.

	#define AIC_BASE        0xFFFFF000      ///< AIC base address.
	#define DBGU_BASE       0xFFFFF200      ///< DBGU base address.
	#define PIOA_BASE       0xFFFFF400      ///< PIO A base address.
	#define PMC_BASE        0xFFFFFC00      ///< PMC base address.
	#define RSTC_BASE       0xFFFFFD00      ///< Resect controller register base address.
	#define RTT_BASE        0xFFFFFD20      ///< Realtime timer base address.
	#define PIT_BASE        0xFFFFFD30      ///< Periodic interval timer base address.
	#define WDT_BASE        0xFFFFFD40      ///< Watch Dog register base address.
	#define VREG_BASE       0xFFFFFD60      ///< Voltage regulator mode controller base address.
	#define MC_BASE         0xFFFFFF00      ///< Memory controller base.

	#if CPU_ARM_SAM7X
		#define CAN_BASE        0xFFFD0000      ///< PWM controller base address.
		#define EMAC_BASE       0xFFFDC000      ///< Ethernet MAC address.
		#define SPI0_BASE       0xFFFE0000      ///< SPI0 base address.
		#define SPI1_BASE       0xFFFE4000      ///< SPI1 base address.
		#define PIOB_BASE       0xFFFFF600      ///< PIO base address.
	#endif

	#if CPU_ARM_SAM7S_LARGE
		#define SPI_BASE        0xFFFE0000      ///< SPI0 base address.
	#endif

	#define PIO_HAS_MULTIDRIVER        1
	#define PIO_HAS_PULLUP             1
	#define PIO_HAS_PERIPHERALSELECT   1
	#define PIO_HAS_OUTPUTWRITEENABLE  1

	#define DBGU_HAS_PDC               1
	#define SPI_HAS_PDC                1
	#define SSC_HAS_PDC                1
	#define USART_HAS_PDC              1

	/* PDC registers */
	#define PERIPH_RPR_OFF  0x100  ///< Receive Pointer Register.
	#define PERIPH_RCR_OFF  0x104  ///< Receive Counter Register.
	#define PERIPH_TPR_OFF  0x108  ///< Transmit Pointer Register.
	#define PERIPH_TCR_OFF  0x10C  ///< Transmit Counter Register.
	#define PERIPH_RNPR_OFF 0x110  ///< Receive Next Pointer Register.
	#define PERIPH_RNCR_OFF 0x114  ///< Receive Next Counter Register.
	#define PERIPH_TNPR_OFF 0x118  ///< Transmit Next Pointer Register.
	#define PERIPH_TNCR_OFF 0x11C  ///< Transmit Next Counter Register.
	#define PERIPH_PTCR_OFF 0x120  ///< PDC Transfer Control Register.
	#define PERIPH_PTSR_OFF 0x124  ///< PDC Transfer Status Register.

	#define PDC_RXTEN  0
	#define PDC_RXTDIS 1
	#define PDC_TXTEN  8
	#define PDC_TXTDIS 9

#else
	#error No base address register definition for selected ARM CPU

#endif

#if CPU_ARM_AT91SAM7S64
	#define FLASH_MEM_SIZE          0x10000UL ///< Internal flash memory size
	#define FLASH_PAGE_SIZE_BYTES         128 ///< Size of cpu flash memory page in bytes
	#define FLASH_BANKS_NUM                 1 ///< Number of flash banks
	#define FLASH_SECTORS_NUM              16 ///< Number of flash sector
	#define FLASH_PAGE_PER_SECTOR          32 ///< Number of page for sector

#elif CPU_ARM_AT91SAM7S128 || CPU_ARM_AT91SAM7X128
	#define FLASH_MEM_SIZE          0x20000UL ///< Internal flash memory size
	#define FLASH_PAGE_SIZE_BYTES         256 ///< Size of cpu flash memory page in bytes
	#define FLASH_BANKS_NUM                 1 ///< Number of flash banks
	#define FLASH_SECTORS_NUM               8 ///< Number of flash sector
	#define FLASH_PAGE_PER_SECTOR          64 ///< Number of page for sector

#elif CPU_ARM_AT91SAM7S256 || CPU_ARM_AT91SAM7X256
	#define FLASH_MEM_SIZE          0x40000UL ///< Internal flash memory size
	#define FLASH_PAGE_SIZE_BYTES         256 ///< Size of cpu flash memory page in bytes
	#define FLASH_BANKS_NUM                 1 ///< Number of flash banks
	#define FLASH_SECTORS_NUM              16 ///< Number of flash sector
	#define FLASH_PAGE_PER_SECTOR          64 ///< Number of page for sector

#elif CPU_ARM_AT91SAM7S512 || CPU_ARM_AT91SAM7X512
	#define FLASH_MEM_SIZE          0x80000UL ///< Internal flash memory size
	#define FLASH_PAGE_SIZE_BYTES         256 ///< Size of cpu flash memory page in bytes
	#define FLASH_BANKS_NUM                 2 ///< Number of flash banks
	#define FLASH_SECTORS_NUM              32 ///< Number of flash sector
	#define FLASH_PAGE_PER_SECTOR          64 ///< Number of page for sector

#else
	#error Memory size definition for selected ARM CPU
#endif

#include "at91_aic.h"
#include "at91_pit.h"
#include "at91_pmc.h"
#include "at91_mc.h"
#include "at91_wdt.h"
#include "at91_rstc.h"
#include "at91_pio.h"
#include "at91_us.h"
#include "at91_dbgu.h"
#include "at91_tc.h"
#include "at91_adc.h"
#include "at91_pwm.h"
#include "at91_spi.h"
#include "at91_twi.h"
#include "at91_ssc.h"
#include "at91_emac.h"
//TODO: add other peripherals

/**
 * Peripheral Identifiers and Interrupts
 *\{
 */
#if CPU_ARM_SAM7X || CPU_ARM_SAM7S_LARGE
	#define FIQ_ID      0       ///< Fast interrupt ID.
	#define SYSC_ID     1       ///< System controller interrupt.
	#define US0_ID      6       ///< USART 0 ID.
	#define US1_ID      7       ///< USART 1 ID.
	#define SSC_ID      8       ///< Synchronous serial controller ID.
	#define TWI_ID      9       ///< Two-wire interface ID.
	#define PWMC_ID     10      ///< PWM controller ID.
	#define UDP_ID      11      ///< USB device port ID.
	#define TC0_ID      12      ///< Timer 0 ID.
	#define TC1_ID      13      ///< Timer 1 ID.
	#define TC2_ID      14      ///< Timer 2 ID.

	#define IRQ0_ID     30      ///< External interrupt 0 ID.
	#define IRQ1_ID     31      ///< External interrupt 1 ID.

	#if CPU_ARM_SAM7X
		#define PIOA_ID     2       ///< Parallel A I/O controller ID.
		#define PIOB_ID     3       ///< Parallel B I/O controller ID.
		#define SPI0_ID     4       ///< Serial peripheral interface 0 ID.
		#define SPI1_ID     5       ///< Serial peripheral interface 1 ID.
		#define CAN_ID      15      ///< CAN controller ID.
		#define EMAC_ID     16      ///< Ethernet MAC ID.
		#define ADC_ID      17      ///< Analog to digital converter ID.
		/* 18-29 Reserved */

	#endif

	#if CPU_ARM_SAM7S_LARGE
		#define PIOA_ID     2       ///< Parallel I/O controller ID.
		/* ID 3 is reserved */
		#define ADC_ID      4       ///< Analog to digital converter ID.
		#define SPI_ID      5       ///< Serial peripheral interface ID.
		#define SPI0_ID     SPI_ID  ///< Alias
	#endif

#else
	#error No peripheral ID and interrupts definition for selected ARM CPU

#endif
/*\}*/

/**
 * USART & DEBUG pin names
 *\{
 */
#if CPU_ARM_SAM7S_LARGE
	#define RXD0        5
	#define TXD0        6
	#define RXD1       21
	#define TXD1       22
	#define DTXD       10
	#define DRXD        9
#elif CPU_ARM_SAM7X
	#define RXD0        0 // PA0
	#define TXD0        1 // PA1
	#define RXD1        5 // PA5
	#define TXD1        6 // PA6
	#define DTXD       28 // PA28
	#define DRXD       27 // PA27
#else
	#error No USART & debug pin names definition for selected ARM CPU
#endif
/*\}*/

/**
 * SPI pins name
 *\{
 */
#if CPU_ARM_SAM7S_LARGE
	#define SPI0_NPCS0      11  // Same as NSS pin.
	#define SPI0_MISO       12
	#define SPI0_MOSI       13
	#define SPI0_SPCK       14

#elif CPU_ARM_SAM7X
	#define SPI0_NPCS0  12 // Same as NSS pin. PA12
	#define SPI0_NPCS1  13 // PA13
	#define SPI0_NPCS2  14 // PA14
	#define SPI0_NPCS3  15 // PA15
	#define SPI0_MISO   16 // PA16
	#define SPI0_MOSI   17 // PA17
	#define SPI0_SPCK   18 // PA18

	#define SPI1_NPCS0  21 // Same as NSS pin. PA21
	#define SPI1_NPCS1  25 // PA25
	#define SPI1_NPCS2  26 // PA26
	#define SPI1_NPCS3  29 // PA29
	#define SPI1_MISO   24 // PA24
	#define SPI1_MOSI   23 // PA23
	#define SPI1_SPCK   22 // PA22

#else
	#error No SPI pins name definition for selected ARM CPU

#endif
/*\}*/

/**
 * SSC pins name
 *\{
 */
#if CPU_ARM_SAM7S_LARGE

	#define SSC_TF     15 // PA15
	#define SSC_TK     16 // PA16
	#define SSC_TD     17 // PA17
	#define SSC_RD     18 // PA18
	#define SSC_RK     19 // PA19
	#define SSC_RF     20 // PA20

#elif CPU_ARM_SAM7X

	#define SSC_TF     21 // PA21
	#define SSC_TK     22 // PA22
	#define SSC_TD     23 // PA23
	#define SSC_RD     24 // PA24
	#define SSC_RK     25 // PA25
	#define SSC_RF     26 // PA26

#else
	#error No SSC pins name definition for selected ARM CPU

#endif
/*\}*/

/**
 * Timer counter pins definition.
 *\{
 */
#if CPU_ARM_SAM7X
	#define TIOA0  23 // PB23
	#define TIOB0  24 // PB24
	#define TIOA1  25 // PB25
	#define TIOB1  26 // PB26
	#define TIOA2  27 // PB27
	#define TIOB2  28 // PB28

	#define TIO_PIO_PDR     PIOB_PDR
	#define TIO_PIO_ABSR    PIOB_ASR

#elif CPU_ARM_SAM7S_LARGE
	#define TIOA0  0 // PA0
	#define TIOB0  1 // PA1
	#define TIOA1  15 // PA15
	#define TIOB1  16 // PA16
	#define TIOA2  26 // PA26
	#define TIOB2  27 // PA27

	#define TIO_PIO_PDR     PIOA_PDR
	#define TIO_PIO_ABSR    PIOA_BSR

#else
	#error No Timer Counter names of pins definition for selected ARM CPU

#endif
/*\}*/


/**
 * PWM pins definition.
 *\{
 */
#if CPU_ARM_SAM7X
	#define PWM0  19 // PB19
	#define PWM1  20 // PB20
	#define PWM2  21 // PB21
	#define PWM3  22 // PB22

	#define PWM_PIO_PDR     PIOB_PDR
	#define PWM_PIO_PER     PIOB_PER
	#define PWM_PIO_CODR    PIOB_CODR
	#define PWM_PIO_OER     PIOB_OER
	#define PWM_PIO_ABSR    PIOB_ASR

#elif CPU_ARM_SAM7S_LARGE
	#define PWM0  11 // PA11
	#define PWM1  12 // PA12
	#define PWM2  13 // PA13
	#define PWM3  14 // PA14

	#define PWM_PIO_PDR     PIOA_PDR
	#define PWM_PIO_PER     PIOA_PER
	#define PWM_PIO_CODR    PIOA_CODR
	#define PWM_PIO_OER     PIOA_OER
	#define PWM_PIO_ABSR    PIOA_BSR

#else
	#error No PWM names of pins definition for selected ARM CPU

#endif
/*\}*/

/**
 * TWI pins definition.
 *\{
 */
#if CPU_ARM_SAM7X
	#define TWD  10
	#define TWCK 11

#elif CPU_ARM_SAM7S_LARGE
	#define TWD  3    //PA3
	#define TWCK 4    //PA4

#else
	#error No TWI names of pins definition for selected ARM CPU
#endif

/**
 * ADC pins definition.
 *\{
 */
#if CPU_ARM_SAM7X
	#define ADTRG   18 // PB18
	#define AD0     23 // PB27
	#define AD1     24 // PB28
	#define AD2     25 // PB29
	#define AD3     26 // PB30

#elif CPU_ARM_SAM7S_LARGE
	#define ADTRG   18 // PA8
	#define AD0      0 // PA17
	#define AD1      1 // PA18
	#define AD2     15 // PA19
	#define AD3     16 // PA20

#else
	#error No ADC names of pins definition for selected ARM CPU

#endif
/*\}*/

#endif /* AT91SAM7_H */
