/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option  any later version.
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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * SAM3 Digital to Analog to Converter.
 *
 * $WIZ$
 */


#ifndef SAM3_DACC_H
#define SAM3_DACC_H

#include <io/cm3.h>

/** DACC registers base. */
#define DACC_BASE               0x400C8000

/**
 * DACC control register
 * \{
 */
#define DACC_CR_OFF              0x00000000     ///< Control register offeset.
#define DACC_CR          (*((reg32_t*)(DACC_BASE + DACC_CR_OFF)))    ///< Control register address.
#define DACC_SWRST                        0      ///< Software reset.
/* \} */

/**
 * DACC mode register
 * \{
 */
#define DACC_MR_OFF              0x00000004     ///< Mode register offeset.
#define DACC_MR          (*((reg32_t*) (DACC_BASE + DACC_MR_OFF)))    ///< Mode register address.
#define DACC_TRGEN                        0      ///< Trigger enable.
#define DACC_TRGSEL_MASK               0x14      ///< Trigger selection mask.
#define DACC_TRGSEL_SHIFT                 1      ///< Trigger selection shift.
#define DACC_WORD                         4      ///< Word transfer.
#define DACC_SLEEP                        5      ///< Sleep mode.Fast Wake up Mode
#define DACC_FASTWKUP                     6      ///< Fast Wake up Mode
#define DACC_REFRESH_MASK            0xFF00      ///< Refresh Period mask
#define DACC_REFRESH_SHIFT                8      ///< Refresh Period shift
#define DACC_USER_SEL_MASK          0x30000      ///< User Channel Selection mask
#define DACC_USER_SEL_SHIFT              16      ///< User Channel Selection shift
#define DACC_TAG                         20      ///< Tag selection mode
#define DACC_MAXS                        21      ///< Max speed mode
#define DACC_STARTUP_MASK        0x3F000000      ///< Startup time selection
#define DACC_STARTUP_SHIFT               24      ///< Startup time selsection shift


/**
 * Trigger selection.
 * $WIZ$ sam3x_dac_tc = "DACC_TRGSEL_TIO_CH0", "DACC_TRGSEL_TIO_CH1", "DACC_TRGSEL_TIO_CH2", "DACC_TRGSEL_PWM0", "DACC_TRGSEL_PWM1"
 * \{
 */
 #define DACC_TRGSEL_TIO_CH0    1
 #define DACC_TRGSEL_TIO_CH1    2
 #define DACC_TRGSEL_TIO_CH2    3
 #define DACC_TRGSEL_PWM0       4
 #define DACC_TRGSEL_PWM1       5
/* \} */

#define DACC_MR_STARTUP_0                 0      ///< 0 periods of DACClock
#define DACC_MR_STARTUP_8                 1      ///< 8 periods of DACClock
#define DACC_MR_STARTUP_16                2      ///< 16 periods of of DACClock
#define DACC_MR_STARTUP_24                3      ///< 24 periods of of DACClock
#define DACC_MR_STARTUP_64                4      ///< 64 periods of of DACClock
#define DACC_MR_STARTUP_80                5      ///< 70 periods of of DACClock
#define DACC_MR_STARTUP_96                6      ///< 96 periods of of DACClock
#define DACC_MR_STARTUP_112               7      ///< 112 periods of of DACClock
#define DACC_MR_STARTUP_512               8      ///< 512 periods of DACClock
#define DACC_MR_STARTUP_576               9      ///< 576 periods of DACClock
#define DACC_MR_STARTUP_640               10     ///< 640 periods of DACClock
#define DACC_MR_STARTUP_704               11     ///< 704 periods of DACClock
#define DACC_MR_STARTUP_768               12     ///< 768 periods of DACClock
#define DACC_MR_STARTUP_832               13     ///< 832 periods of DACClock
#define DACC_MR_STARTUP_896               14     ///< 896 periods of DACClock
#define DACC_MR_STARTUP_960               15     ///< 960 periods of DACClock
#define DACC_MR_STARTUP_1024              16     ///< 1024 periods of DACClock
#define DACC_MR_STARTUP_1088              17     ///< 1088 periods of DACClock
#define DACC_MR_STARTUP_1152              18     ///< 1152 periods of DACClock
#define DACC_MR_STARTUP_1216              19     ///< 1216 periods of DACClock
#define DACC_MR_STARTUP_1280              20     ///< 1280 periods of DACClock
#define DACC_MR_STARTUP_1344              21     ///< 1344 periods of DACClock
#define DACC_MR_STARTUP_1408              22     ///< 1408 periods of DACClock
#define DACC_MR_STARTUP_1472              23     ///< 1472 periods of DACClock
#define DACC_MR_STARTUP_1536              24     ///< 1536 periods of DACClock
#define DACC_MR_STARTUP_1600              25     ///< 1600 periods of DACClock
#define DACC_MR_STARTUP_1664              26     ///< 1664 periods of DACClock
#define DACC_MR_STARTUP_1728              27     ///< 1728 periods of DACClock
#define DACC_MR_STARTUP_1792              28     ///< 1792 periods of DACClock
#define DACC_MR_STARTUP_1856              29     ///< 1856 periods of DACClock
#define DACC_MR_STARTUP_1920              30     ///< 1920 periods of DACClock
#define DACC_MR_STARTUP_1984              31     ///< 1984 periods of DACClock
/* \} */

/**
 * DACC channel enable register
 */
#define DACC_CHER_OFF             0x00000010     ///< Channel enable register offeset.
#define DACC_CHER          (*((reg32_t*) (DACC_BASE + DACC_CHER_OFF)))    ///< Channel enable register address.

/**
 * DACC channel disable register
 */
#define DACC_CHDR_OFF             0x00000014     ///< Channel disable register offeset.
#define DACC_CHDR          (*((reg32_t*) (DACC_BASE + DACC_CHDR_OFF)))    ///< Channel  disable register address.

/**
 * DACC channel status register
 */
#define DACC_CHSR_OFF             0x00000018     ///< Channel status register offeset.
#define DACC_CHSR          (*((reg32_t*) (DACC_BASE + DACC_CHSR_OFF)))    ///< Channel  status register address.

#define DACC_CH0                           0      ///< Channel 0.
#define DACC_CH1                           1      ///< Channel 1.
/* \} */

/**
 * DACC Conversion data register
 */
#define DACC_CDR_OFF             0x00000020     ///< Conversion data register offeset.
#define DACC_CDR          (*((reg32_t*) (DACC_BASE + DACC_CDR_OFF)))    ///< Conversion data register address.


/**
 * DACC Interrupt enable register
 */
#define DACC_IER_OFF             0x00000024     ///< Interrupt enable register offeset.
#define DACC_IER          (*((reg32_t*) (DACC_BASE + DACC_IER_OFF)))    ///< Interrupt enable register address.

/**
 * DACC Interrupt disable register
 */
#define DACC_IDR_OFF             0x00000028     ///< Interrupt disable register offeset.
#define DACC_IDR          (*((reg32_t*) (DACC_BASE + DACC_IDR_OFF)))    ///< Interrupt disable register address.

/**
 * DACC Interrupt disable register
 */
#define DACC_IMR_OFF             0x0000002C     ///< Interrupt mask register offeset.
#define DACC_IMR          (*((reg32_t*) (DACC_BASE + DACC_IMR_OFF)))    ///< Interrupt mask register address.

/**
 * DACC Interrupt status register
 */
#define DACC_ISR_OFF             0x00000030    ///< Interrupt disable status offeset.
#define DACC_ISR          (*((reg32_t*) (DACC_BASE + DACC_ISR_OFF)))    ///< Interrupt status register address.

#define DACC_TXRDY                        0     ///< Transmit ready interrupt
#define DACC_EOC                          1     ///< End of conversion interrupt
#define DACC_ENDTX                        2     ///< End of DMA Interrupt Flag
#define DACC_TXBUFE                       3     ///< Transmit buffer empty interrupt


/**
 * DMA controller for DACC
 * DACC PDC register.
 */
#define DACC_RPR       (*((reg32_t*) (DACC_BASE + PERIPH_RPR_OFF)))  ///< Receive Pointer Register.
#define DACC_RCR       (*((reg32_t*) (DACC_BASE + PERIPH_RCR_OFF)))  ///<  Receive Counter Register.
#define DACC_TPR       (*((reg32_t*) (DACC_BASE + PERIPH_TPR_OFF)))  ///<  Transmit Pointer Register.
#define DACC_TCR       (*((reg32_t*) (DACC_BASE + PERIPH_TCR_OFF)))  ///< Transmit Counter Register.
#define DACC_RNPR      (*((reg32_t*) (DACC_BASE + PERIPH_RNPR_OFF))) ///< Receive Next Pointer Register.
#define DACC_RNCR      (*((reg32_t*) (DACC_BASE + PERIPH_RNCR_OFF))) ///< Receive Next Counter Register.
#define DACC_TNPR      (*((reg32_t*) (DACC_BASE + PERIPH_TNPR_OFF))) ///< Transmit Next Pointer Register.
#define DACC_TNCR      (*((reg32_t*) (DACC_BASE + PERIPH_TNCR_OFF))) ///< Transmit Next Counter Register.
#define DACC_PTCR      (*((reg32_t*) (DACC_BASE + PERIPH_PTCR_OFF))) ///< Transfer Control Register.
#define DACC_PTSR      (*((reg32_t*) (DACC_BASE + PERIPH_PTSR_OFF))) ///< Transfer Status Register.


#define DACC_PTCR_RXTEN               0  ///< DACC_PTCR  Receiver Transfer Enable.
#define DACC_PTCR_RXTDIS              1  ///< DACC_PTCR  Receiver Transfer Disable.
#define DACC_PTCR_TXTEN               8  ///< DACC_PTCR  Transmitter Transfer Enable.
#define DACC_PTCR_TXTDIS              9  ///< DACC_PTCR  Transmitter Transfer Disable.
#define DACC_PTSR_RXTEN               0  ///< DACC_PTSR  Receiver Transfer Enable.
#define DACC_PTSR_TXTEN               8  ///< DACC_PTSR  Transmitter Transfer Enable.

#endif /* SAM3_DACC_H */
