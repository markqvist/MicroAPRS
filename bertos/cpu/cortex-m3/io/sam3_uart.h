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
 * \brief SAM3 UART hardware.
 */

#ifndef SAM3_UART_H
#define SAM3_UART_H

/**
 * UART registers base addresses.
 */
/*\{*/
#if CPU_CM3_SAM3N
	#define UART0_BASE  0x400E0600
#elif CPU_CM3_SAM3U
	#define UART0_BASE  0x400E0600
	#define UART1_BASE  0x400E0800
#elif CPU_CM3_SAM3X
	#define UART0_BASE  0x400E0800
#endif
/*\}*/

/**
 * UART register offsets.
 */
/*\{*/
#define UART_CR_OFF       0x000  //< Control Register
#define UART_MR_OFF       0x004  //< Mode Register
#define UART_IER_OFF      0x008  //< Interrupt Enable Register
#define UART_IDR_OFF      0x00C  //< Interrupt Disable Register
#define UART_IMR_OFF      0x010  //< Interrupt Mask Register
#define UART_SR_OFF       0x014  //< Status Register
#define UART_RHR_OFF      0x018  //< Receive Holding Register
#define UART_THR_OFF      0x01C  //< Transmit Holding Register
#define UART_BRGR_OFF     0x020  //< Baud Rate Generator Register

#define UART_RPR_OFF      0x100  //< Receive Pointer Register
#define UART_RCR_OFF      0x104  //< Receive Counter Register
#define UART_TPR_OFF      0x108  //< Transmit Pointer Register
#define UART_TCR_OFF      0x10C  //< Transmit Counter Register
#define UART_RNPR_OFF     0x110  //< Receive Next Pointer Register
#define UART_RNCR_OFF     0x114  //< Receive Next Counter Register
#define UART_TNPR_OFF     0x118  //< Transmit Next Pointer Register
#define UART_TNCR_OFF     0x11C  //< Transmit Next Counter Register
#define UART_PTCR_OFF     0x120  //< Transfer Control Register
#define UART_PTSR_OFF     0x124  //< Transfer Status Register
/*\}*/

/**
 * UART register addresses.
 */
/*\{*/
#if defined(UART0_BASE)
	#define UART0_ACCESS(offset) (*((reg32_t *)(UART0_BASE + (offset))))

	#define UART_CR     UART0_ACCESS(UART_CR_OFF)   //< Control Register
	#define UART_MR     UART0_ACCESS(UART_MR_OFF)   //< Mode Register
	#define UART_IER    UART0_ACCESS(UART_IER_OFF)  //< Interrupt Enable Register
	#define UART_IDR    UART0_ACCESS(UART_IDR_OFF)  //< Interrupt Disable Register
	#define UART_IMR    UART0_ACCESS(UART_IMR_OFF)  //< Interrupt Mask Register
	#define UART_SR     UART0_ACCESS(UART_SR_OFF)   //< Status Register
	#define UART_RHR    UART0_ACCESS(UART_RHR_OFF)  //< Receive Holding Register
	#define UART_THR    UART0_ACCESS(UART_THR_OFF)  //< Transmit Holding Register
	#define UART_BRGR   UART0_ACCESS(UART_BRGR_OFF) //< Baud Rate Generator Register

	#define UART_RPR    UART0_ACCESS(UART_RPR_OFF)  //< Receive Pointer Register
	#define UART_RCR    UART0_ACCESS(UART_RCR_OFF)  //< Receive Counter Register
	#define UART_TPR    UART0_ACCESS(UART_TPR_OFF)  //< Transmit Pointer Register
	#define UART_TCR    UART0_ACCESS(UART_TCR_OFF)  //< Transmit Counter Register
	#define UART_RNPR   UART0_ACCESS(UART_RNPR_OFF) //< Receive Next Pointer Register
	#define UART_RNCR   UART0_ACCESS(UART_RNCR_OFF) //< Receive Next Counter Register
	#define UART_TNPR   UART0_ACCESS(UART_TNPR_OFF) //< Transmit Next Pointer Register
	#define UART_TNCR   UART0_ACCESS(UART_TNCR_OFF) //< Transmit Next Counter Register
	#define UART_PTCR   UART0_ACCESS(UART_PTCR_OFF) //< Transfer Control Register
	#define UART_PTSR   UART0_ACCESS(UART_PTSR_OFF) //< Transfer Status Register
#endif /* UART0_BASE */

#if defined(UART1_BASE)
	#define UART1_ACCESS(offset) (*((reg32_t *)(UART1_BASE + (offset))))

	#define UART_CR     UART1_ACCESS(UART_CR_OFF)   //< Control Register
	#define UART_MR     UART1_ACCESS(UART_MR_OFF)   //< Mode Register
	#define UART_IER    UART1_ACCESS(UART_IER_OFF)  //< Interrupt Enable Register
	#define UART_IDR    UART1_ACCESS(UART_IDR_OFF)  //< Interrupt Disable Register
	#define UART_IMR    UART1_ACCESS(UART_IMR_OFF)  //< Interrupt Mask Register
	#define UART_SR     UART1_ACCESS(UART_SR_OFF)   //< Status Register
	#define UART_RHR    UART1_ACCESS(UART_RHR_OFF)  //< Receive Holding Register
	#define UART_THR    UART1_ACCESS(UART_THR_OFF)  //< Transmit Holding Register
	#define UART_BRGR   UART1_ACCESS(UART_BRGR_OFF) //< Baud Rate Generator Register

	#define UART_RPR    UART1_ACCESS(UART_RPR_OFF)  //< Receive Pointer Register
	#define UART_RCR    UART1_ACCESS(UART_RCR_OFF)  //< Receive Counter Register
	#define UART_TPR    UART1_ACCESS(UART_TPR_OFF)  //< Transmit Pointer Register
	#define UART_TCR    UART1_ACCESS(UART_TCR_OFF)  //< Transmit Counter Register
	#define UART_RNPR   UART1_ACCESS(UART_RNPR_OFF) //< Receive Next Pointer Register
	#define UART_RNCR   UART1_ACCESS(UART_RNCR_OFF) //< Receive Next Counter Register
	#define UART_TNPR   UART1_ACCESS(UART_TNPR_OFF) //< Transmit Next Pointer Register
	#define UART_TNCR   UART1_ACCESS(UART_TNCR_OFF) //< Transmit Next Counter Register
	#define UART_PTCR   UART1_ACCESS(UART_PTCR_OFF) //< Transfer Control Register
	#define UART_PTSR   UART1_ACCESS(UART_PTSR_OFF) //< Transfer Status Register
#endif /* UART0_BASE */
/*\}*/

/**
 * Bit fields in the UART_CR register.
 */
/*\{*/
#define UART_CR_RSTRX     2  //< Reset Receiver
#define UART_CR_RSTTX     3  //< Reset Transmitter
#define UART_CR_RXEN      4  //< Receiver Enable
#define UART_CR_RXDIS     5  //< Receiver Disable
#define UART_CR_TXEN      6  //< Transmitter Enable
#define UART_CR_TXDIS     7  //< Transmitter Disable
#define UART_CR_RSTSTA    8  //< Reset Status Bits
/*\}*/

/**
 * Bit fields in the UART_MR register.
 */
/*\{*/
#define UART_MR_PAR_SHIFT                9                             //< Parity Type shift
#define UART_MR_PAR_MASK                 (0x7 << UART_MR_PAR_SHIFT)    //< Parity Type mask
#define   UART_MR_PAR_EVEN               (0x0 << UART_MR_PAR_SHIFT)    //< Even parity
#define   UART_MR_PAR_ODD                (0x1 << UART_MR_PAR_SHIFT)    //< Odd parity
#define   UART_MR_PAR_SPACE              (0x2 << UART_MR_PAR_SHIFT)    //< Space: parity forced to 0
#define   UART_MR_PAR_MARK               (0x3 << UART_MR_PAR_SHIFT)    //< Mark: parity forced to 1
#define   UART_MR_PAR_NO                 (0x4 << UART_MR_PAR_SHIFT)    //< No parity
#define UART_MR_CHMODE_SHIFT             14                            //< Channel Mode shift
#define UART_MR_CHMODE_MASK              (0x3 << UART_MR_CHMODE_SHIFT) //< Channel Mode mask
#define   UART_MR_CHMODE_NORMAL          (0x0 << UART_MR_CHMODE_SHIFT) //< Normal Mode
#define   UART_MR_CHMODE_AUTOMATIC       (0x1 << UART_MR_CHMODE_SHIFT) //< Automatic Echo
#define   UART_MR_CHMODE_LOCAL_LOOPBACK  (0x2 << UART_MR_CHMODE_SHIFT) //< Local Loopback
#define   UART_MR_CHMODE_REMOTE_LOOPBACK (0x3 << UART_MR_CHMODE_SHIFT) //< Remote Loopback
/*\}*/

/**
 * Bit fields in the UART_IER register.
 */
/*\{*/
#define UART_IER_RXRDY    0   //< Enable RXRDY Interrupt
#define UART_IER_TXRDY    1   //< Enable TXRDY Interrupt
#define UART_IER_ENDRX    3   //< Enable End of Receive Transfer Interrupt
#define UART_IER_ENDTX    4   //< Enable End of Transmit Interrupt
#define UART_IER_OVRE     5   //< Enable Overrun Error Interrupt
#define UART_IER_FRAME    6   //< Enable Framing Error Interrupt
#define UART_IER_PARE     7   //< Enable Parity Error Interrupt
#define UART_IER_TXEMPTY  9   //< Enable TXEMPTY Interrupt
#define UART_IER_TXBUFE   11  //< Enable Buffer Empty Interrupt
#define UART_IER_RXBUFF   12  //< Enable Buffer Full Interrupt
/*\}*/

/**
 * Bit fields in the UART_IDR register.
 */
/*\{*/
#define UART_IDR_RXRDY    0   //< Disable RXRDY Interrupt
#define UART_IDR_TXRDY    1   //< Disable TXRDY Interrupt
#define UART_IDR_ENDRX    3   //< Disable End of Receive Transfer Interrupt
#define UART_IDR_ENDTX    4   //< Disable End of Transmit Interrupt
#define UART_IDR_OVRE     5   //< Disable Overrun Error Interrupt
#define UART_IDR_FRAME    6   //< Disable Framing Error Interrupt
#define UART_IDR_PARE     7   //< Disable Parity Error Interrupt
#define UART_IDR_TXEMPTY  9   //< Disable TXEMPTY Interrupt
#define UART_IDR_TXBUFE   11  //< Disable Buffer Empty Interrupt
#define UART_IDR_RXBUFF   12  //< Disable Buffer Full Interrupt
/*\}*/

/**
 * Bit fields in the UART_IMR register.
 */
/*\{*/
#define UART_IMR_RXRDY    0   //< Mask RXRDY Interrupt
#define UART_IMR_TXRDY    1   //< Disable TXRDY Interrupt
#define UART_IMR_ENDRX    3   //< Mask End of Receive Transfer Interrupt
#define UART_IMR_ENDTX    4   //< Mask End of Transmit Interrupt
#define UART_IMR_OVRE     5   //< Mask Overrun Error Interrupt
#define UART_IMR_FRAME    6   //< Mask Framing Error Interrupt
#define UART_IMR_PARE     7   //< Mask Parity Error Interrupt
#define UART_IMR_TXEMPTY  9   //< Mask TXEMPTY Interrupt
#define UART_IMR_TXBUFE   11  //< Mask TXBUFE Interrupt
#define UART_IMR_RXBUFF   12  //< Mask RXBUFF Interrupt
/*\}*/

/**
 * Bit fields in the UART_SR register.
 */
/*\{*/
#define UART_SR_RXRDY     0   //< Receiver Ready
#define UART_SR_TXRDY     1   //< Transmitter Ready
#define UART_SR_ENDRX     3   //< End of Receiver Transfer
#define UART_SR_ENDTX     4   //< End of Transmitter Transfer
#define UART_SR_OVRE      5   //< Overrun Error
#define UART_SR_FRAME     6   //< Framing Error
#define UART_SR_PARE      7   //< Parity Error
#define UART_SR_TXEMPTY   9   //< Transmitter Empty
#define UART_SR_TXBUFE    11  //< Transmission Buffer Empty
#define UART_SR_RXBUFF    12  //< Receive Buffer Full
/*\}*/

/**
 * Bit fields in the UART_RHR register.
 */
/*\{*/
#define UART_RHR_RXCHR_MASK   0xFF  //< Received Character mask
#define UART_RHR_RXCHR_SHIFT  0     //< Received Character shift
/*\}*/

/**
 * Bit fields in the UART_THR register.
 */
/*\{*/
#define UART_THR_TXCHR_MASK   0xFF  //< Character to be Transmitted mask
#define UART_THR_TXCHR_SHIFT  0     //< Character to be Transmitted shift
/*\}*/

/**
 * Bit fields in the UART_BRGR register.
 */
/*\{*/
#define UART_BRGR_CD_MASK   0xFFFF  //< Clock Divisor mask
#define UART_BRGR_CD_SHIFT  0       //< Clock Divisor shift
/*\}*/

/**
 * Bit fields in the UART_RPR register.
 */
/*\{*/
#define UART_RPR_RXPTR_MASK  0xFFFFFFFF  //< Receive Pointer Register mask
#define UART_RPR_RXPTR_SHIFT 0           //< Receive Pointer Register shift
/*\}*/

/**
 * Bit fields in the UART_RCR register.
 */
/*\{*/
#define UART_RCR_RXCTR_MASK  0xFFFF  //< Receive Counter Register mask
#define UART_RCR_RXCTR_SHIFT 0       //< Receive Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_TPR register.
 */
/*\{*/
#define UART_TPR_TXPTR_MASK  0xFFFFFFFF  //< Transmit Counter Register mask
#define UART_TPR_TXPTR_SHIFT 0           //< Transmit Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_TCR register.
 */
/*\{*/
#define UART_TCR_TXCTR_MASK  0xFFFF  //< Transmit Counter Register mask
#define UART_TCR_TXCTR_SHIFT 0       //< Transmit Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_RNPR register.
 */
/*\{*/
#define UART_RNPR_RXNPTR_MASK  0xFFFFFFFF  //< Receive Next Pointer mask
#define UART_RNPR_RXNPTR_SHIFT 0           //< Receive Next Pointer shift
/*\}*/

/**
 * Bit fields in the UART_RNCR register.
 */
/*\{*/
#define UART_RNCR_RXNCTR_MASK  0xFFFF  //< Receive Next Counter mask
#define UART_RNCR_RXNCTR_SHIFT 0       //< Receive Next Counter shift
/*\}*/

/**
 * Bit fields in the UART_TNPR register.
 */
/*\{*/
#define UART_TNPR_TXNPTR_MASK  0xFFFFFFFF  //< Transmit Next Pointer mask
#define UART_TNPR_TXNPTR_SHIFT 0           //< Transmit Next Pointer shift
/*\}*/

/**
 * Bit fields in the UART_TNCR register.
 */
/*\{*/
#define UART_TNCR_TXNCTR_MASK  0xFFFF  //< Transmit Counter Next mask
#define UART_TNCR_TXNCTR_SHIFT 0       //< Transmit Counter Next shift
/*\}*/

/**
 * Bit fields in the UART_PTCR register.
 */
/*\{*/
#define UART_PTCR_RXTEN   0  //< Receiver Transfer Enable
#define UART_PTCR_RXTDIS  1  //< Receiver Transfer Disable
#define UART_PTCR_TXTEN   8  //< Transmitter Transfer Enable
#define UART_PTCR_TXTDIS  9  //< Transmitter Transfer Disable
/*\}*/

/**
 * Bit fields in the UART_PTSR register.
 */
/*\{*/
#define UART_PTSR_RXTEN   0  //< Receiver Transfer Enable
#define UART_PTSR_TXTEN   8  //< Transmitter Transfer Enable
/*\}*/

#endif /* SAM3_UART_H */
