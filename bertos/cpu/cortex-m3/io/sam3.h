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
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef SAM3_H
#define SAM3_H

#include <cpu/detect.h>
#include <cfg/compiler.h>

/*
 * Peripherals IDs.
 */
/*\{*/
#if CPU_CM3_SAM3N
	#define SUPC_ID      0   ///< Supply Controller (SUPC)
	#define RSTC_ID      1   ///< Reset Controller (RSTC)
	#define RTC_ID       2   ///< Real Time Clock (RTC)
	#define RTT_ID       3   ///< Real Time Timer (RTT)
	#define WDT_ID       4   ///< Watchdog Timer (WDT)
	#define PMC_ID       5   ///< Power Management Controller (PMC)
	#define EEFC0_ID     6   ///< Enhanced Flash Controller
	#define UART0_ID     8   ///< UART 0 (UART0)
	#define UART1_ID     9   ///< UART 1 (UART1)
	#define PIOA_ID     11   ///< Parallel I/O Controller A (PIOA)
	#define PIOB_ID     12   ///< Parallel I/O Controller B (PIOB)
	#define PIOC_ID     13   ///< Parallel I/O Controller C (PIOC)
	#define US0_ID      14   ///< USART 0 (USART0)
	#define US1_ID      15   ///< USART 1 (USART1)
	#define TWI0_ID     19   ///< Two Wire Interface 0 (TWI0)
	#define TWI1_ID     20   ///< Two Wire Interface 1 (TWI1)
	#define SPI0_ID     21   ///< Serial Peripheral Interface (SPI)
	#define TC0_ID      23   ///< Timer/Counter 0 (TC0)
	#define TC1_ID      24   ///< Timer/Counter 1 (TC1)
	#define TC2_ID      25   ///< Timer/Counter 2 (TC2)
	#define TC3_ID      26   ///< Timer/Counter 3 (TC3)
	#define TC4_ID      27   ///< Timer/Counter 4 (TC4)
	#define TC5_ID      28   ///< Timer/Counter 5 (TC5)
	#define ADC_ID      29   ///< Analog To Digital Converter (ADC)
	#define DACC_ID     30   ///< Digital To Analog Converter (DACC)
	#define PWM_ID      31   ///< Pulse Width Modulation (PWM)
#elif CPU_CM3_SAM3X
	#define SUPC_ID        0   ///< Supply Controller (SUPC)
	#define RSTC_ID        1   ///< Reset Controller (RSTC)
	#define RTC_ID         2   ///< Real Time Clock (RTC)
	#define RTT_ID         3   ///< Real Time Timer (RTT)
	#define WDT_ID         4   ///< Watchdog Timer (WDT)
	#define PMC_ID         5   ///< Power Management Controller (PMC)
	#define EEFC0_ID       6   ///< Enhanced Flash Controller
	#define EEFC1_ID       7   ///< Enhanced Flash Controller
	#define UART0_ID       8   ///< UART 0 (UART0)
	#define SMC_SDRAMC_ID  9   ///< Satic memory controller / SDRAM controller
	#define SDRAMC_ID     10   ///< Satic memory controller / SDRAM controller
	#define PIOA_ID       11   ///< Parallel I/O Controller A
	#define PIOB_ID       12   ///< Parallel I/O Controller B
	#define PIOC_ID       13   ///< Parallel I/O Controller C
	#define PIOD_ID       14   ///< Parallel I/O Controller D
	#define PIOE_ID       15   ///< Parallel I/O Controller E
	#define PIOF_ID       16   ///< Parallel I/O Controller F
	#define US0_ID        17   ///< USART 0
	#define US1_ID        18   ///< USART 1
	#define US2_ID        19   ///< USART 2
	#define US3_ID        20   ///< USART 3
	#define HSMCI_ID      21   ///< High speed multimedia card interface
	#define TWI0_ID       22   ///< Two Wire Interface 0
	#define TWI1_ID       23   ///< Two Wire Interface 1
	#define SPI0_ID       24   ///< Serial Peripheral Interface
	#define SPI1_ID       25   ///< Serial Peripheral Interface
	#define SSC_ID        26   ///< Synchronous serial controller
	#define TC0_ID        27   ///< Timer/Counter 0
	#define TC1_ID        28   ///< Timer/Counter 1
	#define TC2_ID        29   ///< Timer/Counter 2
	#define TC3_ID        30   ///< Timer/Counter 3
	#define TC4_ID        31   ///< Timer/Counter 4
	#define TC5_ID        32   ///< Timer/Counter 5
	#define TC6_ID        33   ///< Timer/Counter 6
	#define TC7_ID        34   ///< Timer/Counter 7
	#define TC8_ID        35   ///< Timer/Counter 8
	#define PWM_ID        36   ///< Pulse width modulation controller
	#define ADC_ID        37   ///< ADC controller
	#define DACC_ID       38   ///< DAC controller
	#define DMAC_ID       39   ///< DMA controller
	#define UOTGHS_ID     40   ///< USB OTG high speed
	#define TRNG_ID       41   ///< True random number generator
	#define EMAC_ID       42   ///< Ethernet MAC
    #define CAN0_ID       43   ///< CAN controller 0
    #define CAN1_ID       44   ///< CAN controller 1
#else
	#error Peripheral IDs undefined
#endif
/*\}*/

/*
 * Hardware features for drivers.
 */
#define USART_HAS_PDC  1
#define SPI_HAS_PDC    1

#if CPU_CM3_SAM3X || CPU_CM3_SAM3U
	#define USART_PORTS    1
	#define UART_PORTS     4
#elif CPU_CM3_SAM3N || CPU_CM3_SAM3S
	#define USART_PORTS    2
	#define UART_PORTS     2
#else
	#error undefined U(S)ART_PORTS for this cpu
#endif

#include "sam3_sysctl.h"
#include "sam3_pdc.h"
#include "sam3_pmc.h"
#include "sam3_smc.h"
#include "sam3_sdramc.h"
#include "sam3_ints.h"
#include "sam3_pio.h"
#include "sam3_nvic.h"
#include "sam3_uart.h"
#include "sam3_usart.h"
#include "sam3_spi.h"
#include "sam3_flash.h"
#include "sam3_wdt.h"
#include "sam3_emac.h"
#include "sam3_rstc.h"
#include "sam3_adc.h"
#include "sam3_dacc.h"
#include "sam3_tc.h"
#include "sam3_twi.h"
#include "sam3_ssc.h"

/**
 * U(S)ART I/O pins
 */
/*\{*/
#if CPU_CM3_SAM3U
	#define UART0_PORT   PIOA_BASE
	#define USART0_PORT  PIOA_BASE
	#define USART1_PORT  PIOA_BASE
	#define USART2_PORT  PIOA_BASE
	#define USART3_PORT  PIOC_BASE

	#define UART0_PERIPH   PIO_PERIPH_A
	#define USART0_PERIPH  PIO_PERIPH_A
	#define USART1_PERIPH  PIO_PERIPH_A
	#define USART2_PERIPH  PIO_PERIPH_A
	#define USART3_PERIPH  PIO_PERIPH_B

	#define URXD0   11
	#define UTXD0   12
	#define RXD0    19
	#define TXD0    18
	#define RXD1    21
	#define TXD1    20
	#define RXD2    23
	#define TXD2    22
	#define RXD3    13
	#define TXD3    12
#elif CPU_CM3_SAM3X
	#define UART0_PORT   PIOA_BASE
	#define USART0_PORT  PIOA_BASE
	#define USART1_PORT  PIOA_BASE
	#define USART2_PORT  PIOB_BASE
	#define USART3_PORT  PIOD_BASE

	#define UART0_PERIPH   PIO_PERIPH_A
	#define USART0_PERIPH  PIO_PERIPH_A
	#define USART1_PERIPH  PIO_PERIPH_A
	#define USART2_PERIPH  PIO_PERIPH_A
	#define USART3_PERIPH  PIO_PERIPH_B

	#define URXD0    8
	#define UTXD0    9
	#define RXD0    10
	#define TXD0    11
	#define RXD1    12
	#define TXD1    13
	#define RXD2    21
	#define TXD2    20
	#define RXD3     5
	#define TXD3     4
#elif CPU_CM3_SAM3N || CPU_CM3_SAM3S
	#define UART0_PORT   PIOA_BASE
	#define UART1_PORT   PIOB_BASE
	#define USART0_PORT  PIOA_BASE
	#define USART1_PORT  PIOA_BASE

	#define UART0_PERIPH   PIO_PERIPH_A
	#define UART1_PERIPH   PIO_PERIPH_A
	#define USART0_PERIPH  PIO_PERIPH_A
	#define USART1_PERIPH  PIO_PERIPH_A

	#define URXD0    9
	#define UTXD0   10
	#define URXD1    2
	#define UTXD1    3
	#define RXD0     5
	#define TXD0     6
	#define RXD1    21
	#define TXD1    22
#endif
/*\}*/

/**
 * SPI I/O pins
 */
/*\{*/
#if CPU_CM3_SAM3U
	#define SPI0_SPCK   15
	#define SPI0_MOSI   14
	#define SPI0_MISO   13
#elif CPU_CM3_SAM3X
	#define SPI0_SPCK   27
	#define SPI0_MOSI   26
	#define SPI0_MISO   25
#else
	#define SPI0_SPCK   14
	#define SPI0_MOSI   13
	#define SPI0_MISO   12
#endif
/*\}*/

/**
 * TWI I/O pins
 */
/*\{*/
#if CPU_CM3_SAM3X
	#define TWI0_PORT   PIOA_BASE
	#define TWI1_PORT   PIOA_BASE

	#define TWI0_PERIPH  PIO_PERIPH_A
	#define TWI1_PERIPH  PIO_PERIPH_A

	#define TWI0_TWD    17
	#define TWI0_TWCK   18
	#define TWI1_TWD    12
	#define TWI1_TWCK   13
#elif CPU_CM3_SAM3N || CPU_CM3_SAM3S
	#define TWI0_PORT   PIOA_BASE
	#define TWI1_PORT   PIOB_BASE

	#define TWI0_PERIPH  PIO_PERIPH_A
	#define TWI1_PERIPH  PIO_PERIPH_A

	#define TWI0_TWD    3
	#define TWI0_TWCK   4
	#define TWI1_TWD    4
	#define TWI1_TWCK   5
#elif CPU_CM3_SAM3U
	#define TWI0_PORT   PIOA_BASE
	#define TWI1_PORT   PIOA_BASE

	#define TWI0_PERIPH  PIO_PERIPH_A
	#define TWI1_PERIPH  PIO_PERIPH_A

	#define TWI0_TWD    9
	#define TWI0_TWCK   10
	#define TWI1_TWD    24
	#define TWI1_TWCK   25
#endif

#if CPU_CM3_SAM3X
	#define SSC_PORT            PIOA_BASE
	#define SSC_PIO_PDR         PIOA_PDR
	#define SSC_RECV_PERIPH     PIO_PERIPH_A
	#define SSC_TRAN_PERIPH     PIO_PERIPH_B
	#define SSC_RD              18
	#define SSC_RF              17
	#define SSC_RK              19
	#define SSC_TD              16
	#define SSC_TF              15
	#define SSC_TK              14
#elif CPU_CM3_SAM3N
	#define SSC_PORT            /* None! */
	#define SSC_PIO_PDR         /* None! */
	#define SSC_RECV_PERIPH     /* None! */
	#define SSC_TRAN_PERIPH     /* None! */
	#define SSC_RD              /* None! */
	#define SSC_RF              /* None! */
	#define SSC_RK              /* None! */
	#define SSC_TD              /* None! */
	#define SSC_TF              /* None! */
	#define SSC_TK              /* None! */
#elif CPU_CM3_SAM3S
	#define SSC_PORT            PIOA_BASE
	#define SSC_PIO_PDR         PIOA_PDR
	#define SSC_RECV_PERIPH     PIO_PERIPH_A
	#define SSC_TRAN_PERIPH     PIO_PERIPH_A
	#define SSC_RD              18
	#define SSC_RF              20
	#define SSC_RK              19
	#define SSC_TD              17
	#define SSC_TF              15
	#define SSC_TK              16
#elif CPU_CM3_SAM3U
	#define SSC_PORT            PIOA_BASE
	#define SSC_PIO_PDR         PIOA_PDR
	#define SSC_RECV_PERIPH     PIO_PERIPH_A
	#define SSC_TRAN_PERIPH     PIO_PERIPH_A
	#define SSC_RD              27
	#define SSC_RF              31
	#define SSC_RK              29
	#define SSC_TD              26
	#define SSC_TF              30
	#define SSC_TK              28
#else
	#error no ssc pins are defined for this cpu
#endif

/*\}*/
#endif /* SAM3_H */
