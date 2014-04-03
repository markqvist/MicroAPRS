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
 * Copyright 2007,2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * Atmel SAM3 SPI register definitions.
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

#ifndef SAM3_SPI_H
#define SAM3_SPI_H

/**
 * SPI base addresses.
 */
#define SPI0_BASE  0x40008000
#if CPU_CM3_SAM3X
	#define SPI1_BASE  0x4000C000
#endif

/**
 * SPI Control Register
 */
/*\{*/
#define	SPI_CR_OFF	    0x00000000  ///< Control register offset.

#define SPI_SPIEN                0  ///< SPI enable.
#define SPI_SPIDIS               1  ///< SPI disable.
#define SPI_SWRST                7  ///< Software reset.
#define SPI_LASTXFER            24  ///< Last transfer.
/*\}*/

/**
 * SPI Mode Register
 */
/*\{*/
#define SPI_MR_OFF		0x00000004  ///< Mode register offset.

#define SPI_MSTR                 0  ///< Master mode.
#define SPI_PS                   1  ///< Peripheral select.
#define SPI_PCSDEC               2 ///< Chip select decode.
#define SPI_FDIV                 3  ///< Clock selection.
#define SPI_MODFDIS              4  ///< Mode fault detection.
#define SPI_LLB                  7  ///< Local loopback enable.
#define SPI_PCS         0x000F0000  ///< Peripheral chip select mask.
#define SPI_PCS_0       0x000E0000  ///< Peripheral chip select 0.
#define SPI_PCS_1       0x000D0000  ///< Peripheral chip select 1.
#define SPI_PCS_2       0x000B0000  ///< Peripheral chip select 2.
#define SPI_PCS_3       0x00070000  ///< Peripheral chip select 3.
#define SPI_PCS_SHIFT           16  ///< Least significant bit of peripheral chip select.
#define SPI_DLYBCS      0xFF000000  ///< Mask for delay between chip selects.
#define SPI_DLYBCS_SHIFT        24  ///< Least significant bit of delay between chip selects.
/*\}*/

/**
 * SPI Receive Data Register
 */
/*\{*/
#define SPI_RDR_OFF		0x00000008  ///< Receive data register offset.

#define SPI_RD          0x0000FFFF  ///< Receive data mask.
#define SPI_RD_SHIFT             0  ///< Least significant bit of receive data.
/*\}*/

/**
 * SPI Transmit Data Register
 */
/*\{*/
#define SPI_TDR_OFF		0x0000000C  ///< Transmit data register offset.

#define SPI_TD          0x0000FFFF  ///< Transmit data mask.
#define SPI_TD_SHIFT             0  ///< Least significant bit of transmit data.
/*\}*/

/**
 * SPI Status and Interrupt Register
 */
/*\{*/
#define SPI_SR_OFF		0x00000010  ///< Status register offset.
#define SPI_IER_OFF		0x00000014  ///< Interrupt enable register offset.
#define SPI_IDR_OFF		0x00000018  ///< Interrupt disable register offset.
#define SPI_IMR_OFF		0x0000001C  ///< Interrupt mask register offset.

#define SPI_RDRF                 0  ///< Receive data register full.
#define SPI_TDRE                 1  ///< Transmit data register empty.
#define SPI_MODF                 2  ///< Mode fault error.
#define SPI_OVRES                3  ///< Overrun error status.
#define SPI_ENDRX                4  ///< End of RX buffer.
#define SPI_ENDTX                5  ///< End of TX buffer.
#define SPI_RXBUFF               6  ///< RX buffer full.
#define SPI_TXBUFE               7  ///< TX buffer empty.
#define SPI_NSSR                 8  ///< NSS rising.
#define SPI_TXEMPTY              9  ///< Transmission register empty.
#define SPI_SPIENS              16  ///< SPI enable status.
/*\}*/

/**
 * SPI Chip Select Registers
 */
/*\{*/
#define SPI_CSR0_OFF	0x00000030  ///< Chip select register 0 offset.
#define SPI_CSR1_OFF	0x00000034  ///< Chip select register 1 offset.
#define SPI_CSR2_OFF	0x00000038  ///< Chip select register 2 offset.
#define SPI_CSR3_OFF    0x0000003C  ///< Chip select register 3 offset.

#define SPI_CPOL                 0  ///< Clock polarity.
#define SPI_NCPHA                1  ///< Clock phase.
#define SPI_CSAAT                3  ///< Chip select active after transfer.
#define SPI_BITS        0x000000F0  ///< Bits per transfer mask.
#define SPI_BITS_8      0x00000000  ///< 8 bits per transfer.
#define SPI_BITS_9      0x00000010  ///< 9 bits per transfer.
#define SPI_BITS_10     0x00000020  ///< 10 bits per transfer.
#define SPI_BITS_11     0x00000030  ///< 11 bits per transfer.
#define SPI_BITS_12     0x00000040  ///< 12 bits per transfer.
#define SPI_BITS_13     0x00000050  ///< 13 bits per transfer.
#define SPI_BITS_14     0x00000060  ///< 14 bits per transfer.
#define SPI_BITS_15     0x00000070  ///< 15 bits per transfer.
#define SPI_BITS_16     0x00000080  ///< 16 bits per transfer.
#define SPI_BITS_SHIFT           4  ///< Least significant bit of bits per transfer.
#define SPI_SCBR        0x0000FF00  ///< Serial clock baud rate mask.
#define SPI_SCBR_SHIFT           8  ///< Least significant bit of serial clock baud rate.
#define SPI_DLYBS       0x00FF0000  ///< Delay before SPCK mask.
#define SPI_DLYBS_SHIFT         16  ///< Least significant bit of delay before SPCK.
#define SPI_DLYBCT      0xFF000000  ///< Delay between consecutive transfers mask.
#define SPI_DLYBCT_SHIFT        24  ///< Least significant bit of delay between consecutive transfers.
/*\}*/

/**
 * Single SPI Register Addresses
 */
/*\{*/
#if defined(SPI_BASE)
	#define SPI0_BASE   SPI_BASE
	#define	SPI_CR		(*((reg32_t *)(SPI0_BASE + SPI0_CR_OFF)))     ///< SPI Control Register Write-only.
	#define SPI_MR		(*((reg32_t *)(SPI0_BASE + SPI0_MR_OFF)))     ///< SPI Mode Register Read/Write Reset=0x0.
	#define SPI_RDR		(*((reg32_t *)(SPI0_BASE + SPI0_RDR_OFF)))    ///< SPI Receive Data Register Read-only Reset=0x0.
	#define SPI_TDR		(*((reg32_t *)(SPI0_BASE + SPI0_TDR_OFF)))    ///< SPI Transmit Data Register Write-only .
	#define SPI_SR		(*((reg32_t *)(SPI0_BASE + SPI0_SR_OFF)))     ///< SPI Status Register Read-only Reset=0x000000F0.
	#define SPI_IER		(*((reg32_t *)(SPI0_BASE + SPI0_IER_OFF)))    ///< SPI Interrupt Enable Register Write-only.
	#define SPI_IDR		(*((reg32_t *)(SPI0_BASE + SPI0_IDR_OFF)))    ///< SPI Interrupt Disable Register Write-only.
	#define SPI_IMR		(*((reg32_t *)(SPI0_BASE + SPI0_IMR_OFF)))    ///< SPI Interrupt Mask Register Read-only Reset=0x0.
	#define SPI_CSR0	(*((reg32_t *)(SPI0_BASE + SPI0_CSR0_OFF)))   ///< SPI Chip Select Register 0 Read/Write Reset=0x0.
	#define SPI_CSR1	(*((reg32_t *)(SPI0_BASE + SPI0_CSR1_OFF)))   ///< SPI Chip Select Register 1 Read/Write Reset=0x0.
	#define SPI_CSR2	(*((reg32_t *)(SPI0_BASE + SPI0_CSR2_OFF)))   ///< SPI Chip Select Register 2 Read/Write Reset=0x0.
	#define SPI_CSR3	(*((reg32_t *)(SPI0_BASE + SPI0_CSR3_OFF)))   ///< SPI Chip Select Register 3 Read/Write Reset=0x0.
	#if defined(SPI_HAS_PDC)
		#define SPI_RPR     (*((reg32_t *)(SPI0_BASE + SPI0_RPR_OFF)))    ///< PDC channel 0 receive pointer register.
		#define SPI_RCR     (*((reg32_t *)(SPI0_BASE + SPI0_RCR_OFF)))    ///< PDC channel 0 receive counter register.
		#define SPI_TPR     (*((reg32_t *)(SPI0_BASE + SPI0_TPR_OFF)))    ///< PDC channel 0 transmit pointer register.
		#define SPI_TCR     (*((reg32_t *)(SPI0_BASE + SPI0_TCR_OFF)))    ///< PDC channel 0 transmit counter register.
		#define SPI_RNPR    (*((reg32_t *)(SPI0_BASE + SPI0_RNPR_OFF)))   ///< PDC channel 0 receive next pointer register.
		#define SPI_RNCR    (*((reg32_t *)(SPI0_BASE + SPI0_RNCR_OFF)))   ///< PDC channel 0 receive next counter register.
		#define SPI_TNPR    (*((reg32_t *)(SPI0_BASE + SPI0_TNPR_OFF)))   ///< PDC channel 0 transmit next pointer register.
		#define SPI_TNCR    (*((reg32_t *)(SPI0_BASE + SPI0_TNCR_OFF)))   ///< PDC channel 0 transmit next counter register.
		#define SPI_PTCR    (*((reg32_t *)(SPI0_BASE + SPI0_PTCR_OFF)))   ///< PDC channel 0 transfer control register.
		#define SPI_PTSR    (*((reg32_t *)(SPI0_BASE + SPI0_PTSR_OFF)))   ///< PDC channel 0 transfer status register.
	#endif /* SPI_HAS_PDC */
#endif /* SPI_BASE */
/*\}*/

/**
 * SPI 0 Register Addresses
 */
/*\{*/
#if defined(SPI0_BASE)
	#define	SPI0_CR		(*((reg32_t *)(SPI0_BASE + SPI_CR_OFF)))        ///< SPI Control Register Write-only.
	#define SPI0_MR		(*((reg32_t *)(SPI0_BASE + SPI_MR_OFF)))        ///< SPI Mode Register Read/Write Reset=0x0.
	#define SPI0_RDR	(*((reg32_t *)(SPI0_BASE + SPI_RDR_OFF)))       ///< SPI Receive Data Register Read-only Reset=0x0.
	#define SPI0_TDR	(*((reg32_t *)(SPI0_BASE + SPI_TDR_OFF)))       ///< SPI Transmit Data Register Write-only .
	#define SPI0_SR		(*((reg32_t *)(SPI0_BASE + SPI_SR_OFF)))        ///< SPI Status Register Read-only Reset=0x000000F0.
	#define SPI0_IER	(*((reg32_t *)(SPI0_BASE + SPI_IER_OFF)))       ///< SPI Interrupt Enable Register Write-only.
	#define SPI0_IDR	(*((reg32_t *)(SPI0_BASE + SPI_IDR_OFF)))       ///< SPI Interrupt Disable Register Write-only.
	#define SPI0_IMR	(*((reg32_t *)(SPI0_BASE + SPI_IMR_OFF)))       ///< SPI Interrupt Mask Register Read-only Reset=0x0.
	#define SPI0_CSR0	(*((reg32_t *)(SPI0_BASE + SPI_CSR0_OFF)))      ///< SPI Chip Select Register 0 Read/Write Reset=0x0.
	#define SPI0_CSR1	(*((reg32_t *)(SPI0_BASE + SPI_CSR1_OFF)))      ///< SPI Chip Select Register 1 Read/Write Reset=0x0.
	#define SPI0_CSR2	(*((reg32_t *)(SPI0_BASE + SPI_CSR2_OFF)))      ///< SPI Chip Select Register 2 Read/Write Reset=0x0.
	#define SPI0_CSR3	(*((reg32_t *)(SPI0_BASE + SPI_CSR3_OFF)))      ///< SPI Chip Select Register 3 Read/Write Reset=0x0.
	#if defined(SPI_HAS_PDC)
		#define SPI0_RPR    (*((reg32_t *)(SPI0_BASE + PERIPH_RPR_OFF)))    ///< PDC channel 0 receive pointer register.
		#define SPI0_RCR    (*((reg32_t *)(SPI0_BASE + PERIPH_RCR_OFF)))    ///< PDC channel 0 receive counter register.
		#define SPI0_TPR    (*((reg32_t *)(SPI0_BASE + PERIPH_TPR_OFF)))    ///< PDC channel 0 transmit pointer register.
		#define SPI0_TCR    (*((reg32_t *)(SPI0_BASE + PERIPH_TCR_OFF)))    ///< PDC channel 0 transmit counter register.
		#define SPI0_RNPR   (*((reg32_t *)(SPI0_BASE + PERIPH_RNPR_OFF)))   ///< PDC channel 0 receive next pointer register.
		#define SPI0_RNCR   (*((reg32_t *)(SPI0_BASE + PERIPH_RNCR_OFF)))   ///< PDC channel 0 receive next counter register.
		#define SPI0_TNPR   (*((reg32_t *)(SPI0_BASE + PERIPH_TNPR_OFF)))   ///< PDC channel 0 transmit next pointer register.
		#define SPI0_TNCR   (*((reg32_t *)(SPI0_BASE + PERIPH_TNCR_OFF)))   ///< PDC channel 0 transmit next counter register.
		#define SPI0_PTCR   (*((reg32_t *)(SPI0_BASE + PERIPH_PTCR_OFF)))   ///< PDC channel 0 transfer control register.
		#define SPI0_PTSR   (*((reg32_t *)(SPI0_BASE + PERIPH_PTSR_OFF)))   ///< PDC channel 0 transfer status register.
	#endif /* SPI_HAS_PDC */
#endif /* SPI0_BASE */
/*\}*/

/**
 * SPI 1 Register Addresses
 */
/*\{*/
#if defined(SPI1_BASE)
	#define	SPI1_CR		(*((reg32_t *)(SPI1_BASE + SPI_CR_OFF)))        ///< SPI Control Register Write-only.
	#define SPI1_MR		(*((reg32_t *)(SPI1_BASE + SPI_MR_OFF)))        ///< SPI Mode Register Read/Write Reset=0x0.
	#define SPI1_RDR	(*((reg32_t *)(SPI1_BASE + SPI_RDR_OFF)))       ///< SPI Receive Data Register Read-only Reset=0x0.
	#define SPI1_TDR	(*((reg32_t *)(SPI1_BASE + SPI_TDR_OFF)))       ///< SPI Transmit Data Register Write-only .
	#define SPI1_SR		(*((reg32_t *)(SPI1_BASE + SPI_SR_OFF)))        ///< SPI Status Register Read-only Reset=0x000000F0.
	#define SPI1_IER	(*((reg32_t *)(SPI1_BASE + SPI_IER_OFF)))       ///< SPI Interrupt Enable Register Write-only.
	#define SPI1_IDR	(*((reg32_t *)(SPI1_BASE + SPI_IDR_OFF)))       ///< SPI Interrupt Disable Register Write-only.
	#define SPI1_IMR	(*((reg32_t *)(SPI1_BASE + SPI_IMR_OFF)))       ///< SPI Interrupt Mask Register Read-only Reset=0x0.
	#define SPI1_CSR0	(*((reg32_t *)(SPI1_BASE + SPI_CSR0_OFF)))      ///< SPI Chip Select Register 0 Read/Write Reset=0x0.
	#define SPI1_CSR1	(*((reg32_t *)(SPI1_BASE + SPI_CSR1_OFF)))      ///< SPI Chip Select Register 1 Read/Write Reset=0x0.
	#define SPI1_CSR2	(*((reg32_t *)(SPI1_BASE + SPI_CSR2_OFF)))      ///< SPI Chip Select Register 2 Read/Write Reset=0x0.
	#define SPI1_CSR3	(*((reg32_t *)(SPI1_BASE + SPI_CSR3_OFF)))      ///< SPI Chip Select Register 3 Read/Write Reset=0x0.
	#if defined(SPI_HAS_PDC)
		#define SPI1_RPR    (*((reg32_t *)(SPI1_BASE + PERIPH_RPR_OFF)))    ///< PDC channel 1 receive pointer register.
		#define SPI1_RCR    (*((reg32_t *)(SPI1_BASE + PERIPH_RCR_OFF)))    ///< PDC channel 1 receive counter register.
		#define SPI1_TPR    (*((reg32_t *)(SPI1_BASE + PERIPH_TPR_OFF)))    ///< PDC channel 1 transmit pointer register.
		#define SPI1_TCR    (*((reg32_t *)(SPI1_BASE + PERIPH_TCR_OFF)))    ///< PDC channel 1 transmit counter register.
		#define SPI1_RNPR   (*((reg32_t *)(SPI1_BASE + PERIPH_RNPR_OFF)))   ///< PDC channel 1 receive next pointer register.
		#define SPI1_RNCR   (*((reg32_t *)(SPI1_BASE + PERIPH_RNCR_OFF)))   ///< PDC channel 1 receive next counter register.
		#define SPI1_TNPR   (*((reg32_t *)(SPI1_BASE + PERIPH_TNPR_OFF)))   ///< PDC channel 1 transmit next pointer register.
		#define SPI1_TNCR   (*((reg32_t *)(SPI1_BASE + PERIPH_TNCR_OFF)))   ///< PDC channel 1 transmit next counter register.
		#define SPI1_PTCR   (*((reg32_t *)(SPI1_BASE + PERIPH_PTCR_OFF)))   ///< PDC channel 1 transfer control register.
		#define SPI1_PTSR   (*((reg32_t *)(SPI1_BASE + PERIPH_PTSR_OFF)))   ///< PDC channel 1 transfer status register.
	#endif /* SPI_HAS_PDC */
#endif /* SPI1_BASE */
/*\}*/

#endif  /* SAM3_SPI_H */
