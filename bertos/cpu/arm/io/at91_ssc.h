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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * AT91SAM7 SSC register definitions.
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

#ifndef AT91_SSC_H
#define AT91_SSC_H

#include <io/at91sam7.h>

/**
 * SSC Control Register
 */
/*\{*/
#define SSC_CR_OFF                  0x00000000  ///< Control register offset.

#define SSC_RXEN                     0  ///< Receive enable.
#define SSC_RXDIS                    1  ///< Receive disable.
#define SSC_TXEN                     8  ///< Transmit enable.
#define SSC_TXDIS                    9  ///< Transmit disable.
#define SSC_SWRST                   15  ///< Software reset.
/*\}*/

/**
 * SSC Clock Mode Register
 */
/*\{*/
#define SSC_CMR_OFF                 0x00000004  ///< Clock mode register offset.

#define SSC_DIV_MASK                0x00000FFF  ///< Clock divider.
/*\}*/

/**
 * SSC Receive/Transmit Clock Mode Register
 */
/*\{*/
#define SSC_RCMR_OFF                0x00000010  ///< Receive clock mode register offset.
#define SSC_TCMR_OFF                0x00000018  ///< Transmit clock mode register offset.

#define SSC_CKS_MASK                0x00000003  ///< Receive clock selection.
#define SSC_CKS_DIV                 0x00000000  ///< Divided clock.
#define SSC_CKS_CLK                 0x00000001  ///< RK/TK clock signal.
#define SSC_CKS_PIN                 0x00000002  ///< TK/RK pin.
#define SSC_CKO_MASK                0x0000001C  ///< Receive clock output mode selection.
#define SSC_CKO_NONE                0x00000000  ///< None.
#define SSC_CKO_CONT                0x00000004  ///< Continous receive clock.
#define SSC_CKO_TRAN                0x00000008  ///< Receive clock only during data transfers.
#define SSC_CKI                              5  ///< Receive clock inversion.
#define SSC_CKG_MASK                0x000000C0  ///< Receive clock gating selection.
#define SSC_CKG_NONE                0x00000000  ///< None, continous clock.
#define SSC_CKG_FL                  0x00000040  ///< Continous receive clock.
#define SSC_CKG_FH                  0x00000080  ///< Receive clock only during data transfers.
#define SSC_START_MASK              0x00000F00  ///< Receive start selection.
#define SSC_START_CONT              0x00000000  ///< Receive start as soon as enabled.
#define SSC_START_TX                0x00000100  ///< Receive start on transmit start.
#define SSC_START_RX                0x00000100  ///< Receive start on receive start.
#define SSC_START_LOW_F             0x00000200  ///< Receive start on low level RF.
#define SSC_START_HIGH_F            0x00000300  ///< Receive start on high level RF.
#define SSC_START_FALL_F            0x00000400  ///< Receive start on falling edge RF.
#define SSC_START_RISE_F            0x00000500  ///< Receive start on rising edge RF.
#define SSC_START_LEVEL_F           0x00000600  ///< Receive start on any RF level change.
#define SSC_START_EDGE_F            0x00000700  ///< Receive start on any RF edge.
#define SSC_START_COMP0             0x00000800  ///< Receive on compare 0.
#define SSC_STOP                            12  ///< Receive stop selection.
#define SSC_STTDLY_MASK             0x00FF0000  ///< Receive start delay.
#define SSC_STTDLY_SHIFT                    16  ///< Least significant bit of receive start delay.
#define SSC_PERIOD_MASK             0xFF000000  ///< Receive period divider selection.
#define SSC_PERIOD_SHIFT                    24  ///< Least significant bit of receive period divider selection.
/*\}*/

/**
 * SSC Receive/Transmit Frame Mode Registers
 */
/*\{*/
#define SSC_RFMR_OFF                0x00000014  ///< Receive frame mode register offset.
#define SSC_TFMR_OFF                0x0000001C  ///< Transmit frame mode register offset.

#define SSC_DATLEN_MASK             0x0000001F  ///< Data length.
#define SSC_LOOP                             5  ///< Receiver loop mode.
#define SSC_DATDEF                           5  ///< Transmit default value.

#define SSC_MSBF                             7  ///< Most significant bit first.
#define SSC_DATNB_MASK              0x00000F00  ///< Data number per frame.
#define SSC_DATNB_SHIFT                      8  ///< Least significant bit of data number per frame.
#define SSC_FSLEN_MASK              0x000F0000  ///< Receive frame sync. length.
#define SSC_FSLEN_SHIFT                     16  ///< Least significant bit of receive frame sync. length.
#define SSC_FSOS                    0x00700000  ///< Receive frame sync. output selection.
#define SSC_FSOS_NONE               0x00000000  ///< No frame sync. Line set to input.
#define SSC_FSOS_NEGATIVE           0x00100000  ///< Negative pulse.
#define SSC_FSOS_POSITIVE           0x00200000  ///< Positive pulse.
#define SSC_FSOS_LOW                0x00300000  ///< Low during transfer.
#define SSC_FSOS_HIGH               0x00400000  ///< High during transfer.
#define SSC_FSOS_TOGGLE             0x00500000  ///< Toggling at each start.
#define SSC_FSDEN                           23  ///< Frame sync. data enable.
#define SSC_FSEDGE                          24  ///< Frame sync. edge detection.
/*\}*/

/**
 * SSC Receive Holding Register
 */
/*\{*/
#define SSC_RHR_OFF                 0x00000020  ///< Receive holding register offset.
/*\}*/

/**
 * SSC Transmit Holding Register
 */
/*\{*/
#define SSC_THR_OFF                 0x00000024  ///< Transmit holding register offset.
/*\}*/

/**
 * SSC Receive Sync. Holding Register
 */
/*\{*/
#define SSC_RSHR_OFF                0x00000030  ///< Receive sync. holding register offset.
/*\}*/

/**
 * SSC Transmit Sync. Holding Register
 */
/*\{*/
#define SSC_TSHR_OFF                0x00000034  ///< Transmit sync. holding register offset.
/*\}*/

/**
 * SSC Receive Compare 0 Register
 */
/*\{*/
#define SSC_RC0R_OFF                0x00000038  ///< Receive compare 0 register offset.
/*\}*/

/**
 * SSC Receive Compare 1 Register
 */
/*\{*/
#define SSC_RC1R_OFF                0x0000003C  ///< Receive compare 1 register offset.
/*\}*/

/**
 * SSC Status and Interrupt Register
 */
/*\{*/
#define SSC_SR_OFF                  0x00000040  ///< Status register offset.
#define SSC_IER_OFF                 0x00000044  ///< Interrupt enable register offset.
#define SSC_IDR_OFF                 0x00000048  ///< Interrupt disable register offset.
#define SSC_IMR_OFF                 0x0000004C  ///< Interrupt mask register offset.

#define SSC_TXRDY                            0  ///< Transmit ready.
#define SSC_TXEMPTY                          1  ///< Transmit empty.
#define SSC_ENDTX                            2  ///< End of transmission.
#define SSC_TXBUFE                           3  ///< Transmit buffer empty.
#define SSC_RXRDY                            4  ///< Receive ready.
#define SSC_OVRUN                            5  ///< Receive overrun.
#define SSC_ENDRX                            6  ///< End of receiption.
#define SSC_RXBUFF                           7  ///< Receive buffer full.
#define SSC_CP0                              8  ///< Compare 0.
#define SSC_CP1                              9  ///< Compare 1.
#define SSC_TXSYN                           10  ///< Transmit sync.
#define SSC_RXSYN                           11  ///< Receive sync.
#define SSC_TXENA                           16  ///< Transmit enable.
#define SSC_RXENA                           17  ///< Receive enable.


#if defined(SSC_BASE)
	#define SSC_CR      (*((reg32_t *)(SSC_BASE + SSC_CR_OFF)))     ///< Control register address.
	#define SSC_CMR     (*((reg32_t *)(SSC_BASE + SSC_CMR_OFF)))    ///< Clock mode register address.
	#define SSC_RCMR    (*((reg32_t *)(SSC_BASE + SSC_RCMR_OFF)))   ///< Receive clock mode register address.
	#define SSC_TCMR    (*((reg32_t *)(SSC_BASE + SSC_TCMR_OFF)))   ///< Transmit clock mode register address.
	#define SSC_RFMR    (*((reg32_t *)(SSC_BASE + SSC_RFMR_OFF)))   ///< Receive frame mode register address.
	#define SSC_TFMR    (*((reg32_t *)(SSC_BASE + SSC_TFMR_OFF)))   ///< Transmit frame mode register address.
	#define SSC_RHR     (*((reg32_t *)(SSC_BASE + SSC_RHR_OFF)))    ///< Receive holding register address.
	#define SSC_THR     (*((reg32_t *)(SSC_BASE + SSC_THR_OFF)))    ///< Transmit holding register address.
	#define SSC_RSHR    (*((reg32_t *)(SSC_BASE + SSC_RSHR_OFF)))   ///< Receive sync. holding register address.
	#define SSC_TSHR    (*((reg32_t *)(SSC_BASE + SSC_TSHR_OFF)))   ///< Transmit sync. holding register address.
	#define SSC_RC0R    (*((reg32_t *)(SSC_BASE + SSC_RC0R_OFF)))   ///< Receive compare 0 register address.
	#define SSC_RC1R    (*((reg32_t *)(SSC_BASE + SSC_RC1R_OFF)))   ///< Receive compare 1 register address.
	#define SSC_SR      (*((reg32_t *)(SSC_BASE + SSC_SR_OFF)))     ///< Status register address.
	#define SSC_IER     (*((reg32_t *)(SSC_BASE + SSC_IER_OFF)))    ///< Interrupt enable register address.
	#define SSC_IDR     (*((reg32_t *)(SSC_BASE + SSC_IDR_OFF)))    ///< Interrupt disable register address.
	#define SSC_IMR     (*((reg32_t *)(SSC_BASE + SSC_IMR_OFF)))    ///< Interrupt mask register address.
	#if defined(SSC_HAS_PDC)
		#define SSC_RPR    (*((reg32_t *)(SSC_BASE + PERIPH_RPR_OFF)))  ///< PDC receive pointer register address.
		#define SSC_RCR    (*((reg32_t *)(SSC_BASE + PERIPH_RCR_OFF)))  ///< PDC receive counter register address.
		#define SSC_TPR    (*((reg32_t *)(SSC_BASE + PERIPH_TPR_OFF)))  ///< PDC transmit pointer register address.
		#define SSC_TCR    (*((reg32_t *)(SSC_BASE + PERIPH_TCR_OFF)))  ///< PDC transmit counter register address.
		#define SSC_RNPR   (*((reg32_t *)(SSC_BASE + PERIPH_RNPR_OFF))) ///< PDC receive next pointer register address.
		#define SSC_RNCR   (*((reg32_t *)(SSC_BASE + PERIPH_RNCR_OFF))) ///< PDC receive next counter register address.
		#define SSC_TNPR   (*((reg32_t *)(SSC_BASE + PERIPH_TNPR_OFF))) ///< PDC transmit next pointer register address.
		#define SSC_TNCR   (*((reg32_t *)(SSC_BASE + PERIPH_TNCR_OFF))) ///< PDC transmit next counter register address.
		#define SSC_PTCR   (*((reg32_t *)(SSC_BASE + PERIPH_PTCR_OFF))) ///< PDC transfer control register address.
		#define SSC_PTSR   (*((reg32_t *)(SSC_BASE + PERIPH_PTSR_OFF))) ///< PDC transfer status register address.
	#endif /* SSC_HAS_PDC */

#endif /* SSC_BASE */


#endif /* AT91_SSC_H */
