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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief SAM3 TWI definitions.
 */

#ifndef SAM3_TWI_H
#define SAM3_TWI_H

/** I2C registers base. */
#if CPU_CM3_SAM3X
	#define TWI0_BASE  0x4008C000
	#define TWI1_BASE  0x40090000
#elif CPU_CM3_SAM3N || CPU_CM3_SAM3S
	#define TWI0_BASE  0x40018000
	#define TWI1_BASE  0x4001C000
#elif CPU_CM3_SAM3U
	#define TWI0_BASE  0x40084000
	#define TWI1_BASE  0x40088000
#else
	#error TWI registers not defined for selected CPU
#endif


/**
 * TWI register offsets.
 */
/*\{*/
#define TWI_CR_OFF      0x000
#define TWI_MMR_OFF     0x004
#define TWI_SMR_OFF     0x008
#define TWI_IADR_OFF    0x00C
#define TWI_CWGR_OFF    0x010
#define TWI_SR_OFF      0x020
#define TWI_IER_OFF     0x024
#define TWI_IDR_OFF     0x028
#define TWI_IMR_OFF     0x02C
#define TWI_RHR_OFF     0x030
#define TWI_THR_OFF     0x034
#define TWI_RPR_OFF     0x100
#define TWI_RCR_OFF     0x104
#define TWI_TPR_OFF     0x108
#define TWI_TCR_OFF     0x10C
#define TWI_RNPR_OFF    0x110
#define TWI_RNCR_OFF    0x114
#define TWI_TNPR_OFF    0x118
#define TWI_TNCR_OFF    0x11C
#define TWI_PTCR_OFF    0x120
#define TWI_PTSR_OFF    0x124
/*\}*/

/**
 * TWI registers.
 */
/*\{*/
#ifdef TWI_BASE
	#define TWI_CR    (HWREG(TWI_BASE + TWI_CR_OFF))
	#define TWI_MMR   (HWREG(TWI_BASE + TWI_MMR_OFF))
	#define TWI_SMR   (HWREG(TWI_BASE + TWI_SMR_OFF))
	#define TWI_IADR  (HWREG(TWI_BASE + TWI_IADR_OFF))
	#define TWI_CWGR  (HWREG(TWI_BASE + TWI_CWGR_OFF))
	#define TWI_SR    (HWREG(TWI_BASE + TWI_SR_OFF))
	#define TWI_IER   (HWREG(TWI_BASE + TWI_IER_OFF))
	#define TWI_IDR   (HWREG(TWI_BASE + TWI_IDR_OFF))
	#define TWI_IMR   (HWREG(TWI_BASE + TWI_IMR_OFF))
	#define TWI_RHR   (HWREG(TWI_BASE + TWI_RHR_OFF))
	#define TWI_THR   (HWREG(TWI_BASE + TWI_THR_OFF))
	#define TWI_RPR   (HWREG(TWI_BASE + TWI_RPR_OFF))
	#define TWI_RCR   (HWREG(TWI_BASE + TWI_RCR_OFF))
	#define TWI_TPR   (HWREG(TWI_BASE + TWI_TPR_OFF))
	#define TWI_TCR   (HWREG(TWI_BASE + TWI_TCR_OFF))
	#define TWI_RNPR  (HWREG(TWI_BASE + TWI_RNPR_OFF))
	#define TWI_RNCR  (HWREG(TWI_BASE + TWI_RNCR_OFF))
	#define TWI_TNPR  (HWREG(TWI_BASE + TWI_TNPR_OFF))
	#define TWI_TNCR  (HWREG(TWI_BASE + TWI_TNCR_OFF))
	#define TWI_PTCR  (HWREG(TWI_BASE + TWI_PTCR_OFF))
	#define TWI_PTSR  (HWREG(TWI_BASE + TWI_PTSR_OFF))
#endif // TWI_BASE

#ifdef TWI0_BASE
	#define TWI0_CR    (HWREG(TWI0_BASE + TWI_CR_OFF))
	#define TWI0_MMR   (HWREG(TWI0_BASE + TWI_MMR_OFF))
	#define TWI0_SMR   (HWREG(TWI0_BASE + TWI_SMR_OFF))
	#define TWI0_IADR  (HWREG(TWI0_BASE + TWI_IADR_OFF))
	#define TWI0_CWGR  (HWREG(TWI0_BASE + TWI_CWGR_OFF))
	#define TWI0_SR    (HWREG(TWI0_BASE + TWI_SR_OFF))
	#define TWI0_IER   (HWREG(TWI0_BASE + TWI_IER_OFF))
	#define TWI0_IDR   (HWREG(TWI0_BASE + TWI_IDR_OFF))
	#define TWI0_IMR   (HWREG(TWI0_BASE + TWI_IMR_OFF))
	#define TWI0_RHR   (HWREG(TWI0_BASE + TWI_RHR_OFF))
	#define TWI0_THR   (HWREG(TWI0_BASE + TWI_THR_OFF))
	#define TWI0_RPR   (HWREG(TWI0_BASE + TWI_RPR_OFF))
	#define TWI0_RCR   (HWREG(TWI0_BASE + TWI_RCR_OFF))
	#define TWI0_TPR   (HWREG(TWI0_BASE + TWI_TPR_OFF))
	#define TWI0_TCR   (HWREG(TWI0_BASE + TWI_TCR_OFF))
	#define TWI0_RNPR  (HWREG(TWI0_BASE + TWI_RNPR_OFF))
	#define TWI0_RNCR  (HWREG(TWI0_BASE + TWI_RNCR_OFF))
	#define TWI0_TNPR  (HWREG(TWI0_BASE + TWI_TNPR_OFF))
	#define TWI0_TNCR  (HWREG(TWI0_BASE + TWI_TNCR_OFF))
	#define TWI0_PTCR  (HWREG(TWI0_BASE + TWI_PTCR_OFF))
	#define TWI0_PTSR  (HWREG(TWI0_BASE + TWI_PTSR_OFF))
#endif // TWI0_BASE

#ifdef TWI1_BASE
	#define TWI1_CR    (HWREG(TWI1_BASE + TWI_CR_OFF))
	#define TWI1_MMR   (HWREG(TWI1_BASE + TWI_MMR_OFF))
	#define TWI1_SMR   (HWREG(TWI1_BASE + TWI_SMR_OFF))
	#define TWI1_IADR  (HWREG(TWI1_BASE + TWI_IADR_OFF))
	#define TWI1_CWGR  (HWREG(TWI1_BASE + TWI_CWGR_OFF))
	#define TWI1_SR    (HWREG(TWI1_BASE + TWI_SR_OFF))
	#define TWI1_IER   (HWREG(TWI1_BASE + TWI_IER_OFF))
	#define TWI1_IDR   (HWREG(TWI1_BASE + TWI_IDR_OFF))
	#define TWI1_IMR   (HWREG(TWI1_BASE + TWI_IMR_OFF))
	#define TWI1_RHR   (HWREG(TWI1_BASE + TWI_RHR_OFF))
	#define TWI1_THR   (HWREG(TWI1_BASE + TWI_THR_OFF))
	#define TWI1_RPR   (HWREG(TWI1_BASE + TWI_RPR_OFF))
	#define TWI1_RCR   (HWREG(TWI1_BASE + TWI_RCR_OFF))
	#define TWI1_TPR   (HWREG(TWI1_BASE + TWI_TPR_OFF))
	#define TWI1_TCR   (HWREG(TWI1_BASE + TWI_TCR_OFF))
	#define TWI1_RNPR  (HWREG(TWI1_BASE + TWI_RNPR_OFF))
	#define TWI1_RNCR  (HWREG(TWI1_BASE + TWI_RNCR_OFF))
	#define TWI1_TNPR  (HWREG(TWI1_BASE + TWI_TNPR_OFF))
	#define TWI1_TNCR  (HWREG(TWI1_BASE + TWI_TNCR_OFF))
	#define TWI1_PTCR  (HWREG(TWI1_BASE + TWI_PTCR_OFF))
	#define TWI1_PTSR  (HWREG(TWI1_BASE + TWI_PTSR_OFF))
#endif // TWI1_BASE
/*\}*/


/**
 * TWI_CR: (TWI Offset: 0x00) Control Register
 */
/*\{*/
#define TWI_CR_START BV(0)
#define TWI_CR_STOP BV(1)
#define TWI_CR_MSEN BV(2)
#define TWI_CR_MSDIS BV(3)
#define TWI_CR_SVEN BV(4)
#define TWI_CR_SVDIS BV(5)
#define TWI_CR_QUICK BV(6)
#define TWI_CR_SWRST BV(7)
/*\}*/

/**
 * TWI_MMR: (TWI Offset: 0x04) Master Mode Register
 */
/*\{*/
#define TWI_MMR_IADRSZ_SHIFT 8
#define TWI_MMR_IADRSZ_MASK (0x3 << TWI_MMR_IADRSZ_SHIFT)
#define   TWI_MMR_IADRSZ_NONE (0x0 << 8)
#define   TWI_MMR_IADRSZ_1_BYTE BV(8)
#define   TWI_MMR_IADRSZ_2_BYTE (0x2 << 8)
#define   TWI_MMR_IADRSZ_3_BYTE (0x3 << 8)
#define TWI_MMR_MREAD BV(12)
#define TWI_MMR_DADR_SHIFT 16
#define TWI_MMR_DADR_MASK (0x7f << TWI_MMR_DADR_SHIFT)
#define TWI_MMR_DADR(value) ((TWI_MMR_DADR_MASK & ((value) << TWI_MMR_DADR_SHIFT)))
/*\}*/

/**
 * TWI_SMR: (TWI Offset: 0x08) Slave Mode Register
 */
/*\{*/
#define TWI_SMR_SADR_SHIFT 16
#define TWI_SMR_SADR_MASK (0x7f << TWI_SMR_SADR_SHIFT)
#define TWI_SMR_SADR(value) ((TWI_SMR_SADR_MASK & ((value) << TWI_SMR_SADR_SHIFT)))
/*\}*/

/**
 * TWI_IADR: (TWI Offset: 0x0C) Internal Address Register
 */
/*\{*/
#define TWI_IADR_IADR_SHIFT 0
#define TWI_IADR_IADR_MASK (0xffffff << TWI_IADR_IADR_SHIFT)
#define TWI_IADR_IADR(value) ((TWI_IADR_IADR_MASK & ((value) << TWI_IADR_IADR_SHIFT)))
/*\}*/

/**
 * TWI_CWGR: (TWI Offset: 0x10) Clock Waveform Generator Register
 */
/*\{*/
#define TWI_CWGR_CLDIV_SHIFT 0
#define TWI_CWGR_CLDIV_MASK (0xff << TWI_CWGR_CLDIV_SHIFT)
#define TWI_CWGR_CLDIV(value) ((TWI_CWGR_CLDIV_MASK & ((value) << TWI_CWGR_CLDIV_SHIFT)))
#define TWI_CWGR_CHDIV_SHIFT 8
#define TWI_CWGR_CHDIV_MASK (0xff << TWI_CWGR_CHDIV_SHIFT)
#define TWI_CWGR_CHDIV(value) ((TWI_CWGR_CHDIV_MASK & ((value) << TWI_CWGR_CHDIV_SHIFT)))
#define TWI_CWGR_CKDIV_SHIFT 16
#define TWI_CWGR_CKDIV_MASK (0x7 << TWI_CWGR_CKDIV_SHIFT)
#define TWI_CWGR_CKDIV(value) ((TWI_CWGR_CKDIV_MASK & ((value) << TWI_CWGR_CKDIV_SHIFT)))
/*\}*/

/**
 * TWI_SR: (TWI Offset: 0x20) Status Register
 */
/*\{*/
#define TWI_SR_TXCOMP BV(0)
#define TWI_SR_RXRDY BV(1)
#define TWI_SR_TXRDY BV(2)
#define TWI_SR_SVREAD BV(3)
#define TWI_SR_SVACC BV(4)
#define TWI_SR_GACC BV(5)
#define TWI_SR_OVRE BV(6)
#define TWI_SR_NACK BV(8)
#define TWI_SR_ARBLST BV(9)
#define TWI_SR_SCLWS BV(10)
#define TWI_SR_EOSACC BV(11)
#define TWI_SR_ENDRX BV(12)
#define TWI_SR_ENDTX BV(13)
#define TWI_SR_RXBUFF BV(14)
#define TWI_SR_TXBUFE BV(15)
/*\}*/

/**
 * TWI_IER: (TWI Offset: 0x24) Interrupt Enable Register
 */
/*\{*/
#define TWI_IER_TXCOMP BV(0)
#define TWI_IER_RXRDY BV(1)
#define TWI_IER_TXRDY BV(2)
#define TWI_IER_SVACC BV(4)
#define TWI_IER_GACC BV(5)
#define TWI_IER_OVRE BV(6)
#define TWI_IER_NACK BV(8)
#define TWI_IER_ARBLST BV(9)
#define TWI_IER_SCL_WS BV(10)
#define TWI_IER_EOSACC BV(11)
#define TWI_IER_ENDRX BV(12)
#define TWI_IER_ENDTX BV(13)
#define TWI_IER_RXBUFF BV(14)
#define TWI_IER_TXBUFE BV(15)
/*\}*/

/**
 * TWI_IDR: (TWI Offset: 0x28) Interrupt Disable Register
 */
/*\{*/
#define TWI_IDR_TXCOMP BV(0)
#define TWI_IDR_RXRDY BV(1)
#define TWI_IDR_TXRDY BV(2)
#define TWI_IDR_SVACC BV(4)
#define TWI_IDR_GACC BV(5)
#define TWI_IDR_OVRE BV(6)
#define TWI_IDR_NACK BV(8)
#define TWI_IDR_ARBLST BV(9)
#define TWI_IDR_SCL_WS BV(10)
#define TWI_IDR_EOSACC BV(11)
#define TWI_IDR_ENDRX BV(12)
#define TWI_IDR_ENDTX BV(13)
#define TWI_IDR_RXBUFF BV(14)
#define TWI_IDR_TXBUFE BV(15)
/*\}*/

/**
 * TWI_IMR: (TWI Offset: 0x2C) Interrupt Mask Register
 */
/*\{*/
#define TWI_IMR_TXCOMP BV(0)
#define TWI_IMR_RXRDY BV(1)
#define TWI_IMR_TXRDY BV(2)
#define TWI_IMR_SVACC BV(4)
#define TWI_IMR_GACC BV(5)
#define TWI_IMR_OVRE BV(6)
#define TWI_IMR_NACK BV(8)
#define TWI_IMR_ARBLST BV(9)
#define TWI_IMR_SCL_WS BV(10)
#define TWI_IMR_EOSACC BV(11)
#define TWI_IMR_ENDRX BV(12)
#define TWI_IMR_ENDTX BV(13)
#define TWI_IMR_RXBUFF BV(14)
#define TWI_IMR_TXBUFE BV(15)
/*\}*/

/**
 * TWI_RHR: (TWI Offset: 0x30) Receive Holding Register
 */
/*\{*/
#define TWI_RHR_RXDATA_SHIFT 0
#define TWI_RHR_RXDATA_MASK (0xff << TWI_RHR_RXDATA_SHIFT)
/*\}*/

/**
 * TWI_THR: (TWI Offset: 0x34) Transmit Holding Register
 */
/*\{*/
#define TWI_THR_TXDATA_SHIFT 0
#define TWI_THR_TXDATA_MASK (0xff << TWI_THR_TXDATA_SHIFT)
#define TWI_THR_TXDATA(value) ((TWI_THR_TXDATA_MASK & ((value) << TWI_THR_TXDATA_SHIFT)))
/*\}*/

/**
 * TWI_RPR: (TWI Offset: 0x100) Receive Pointer Register
 */
/*\{*/
#define TWI_RPR_RXPTR_SHIFT 0
#define TWI_RPR_RXPTR_MASK (0xffffffff << TWI_RPR_RXPTR_SHIFT)
#define TWI_RPR_RXPTR(value) ((TWI_RPR_RXPTR_MASK & ((value) << TWI_RPR_RXPTR_SHIFT)))
/*\}*/

/**
 * TWI_RCR: (TWI Offset: 0x104) Receive Counter Register
 */
/*\{*/
#define TWI_RCR_RXCTR_SHIFT 0
#define TWI_RCR_RXCTR_MASK (0xffff << TWI_RCR_RXCTR_SHIFT)
#define TWI_RCR_RXCTR(value) ((TWI_RCR_RXCTR_MASK & ((value) << TWI_RCR_RXCTR_SHIFT)))
/*\}*/

/**
 * TWI_TPR: (TWI Offset: 0x108) Transmit Pointer Register
 */
/*\{*/
#define TWI_TPR_TXPTR_SHIFT 0
#define TWI_TPR_TXPTR_MASK (0xffffffff << TWI_TPR_TXPTR_SHIFT)
#define TWI_TPR_TXPTR(value) ((TWI_TPR_TXPTR_MASK & ((value) << TWI_TPR_TXPTR_SHIFT)))
/*\}*/

/**
 * TWI_TCR: (TWI Offset: 0x10C) Transmit Counter Register
 */
/*\{*/
#define TWI_TCR_TXCTR_SHIFT 0
#define TWI_TCR_TXCTR_MASK (0xffff << TWI_TCR_TXCTR_SHIFT)
#define TWI_TCR_TXCTR(value) ((TWI_TCR_TXCTR_MASK & ((value) << TWI_TCR_TXCTR_SHIFT)))
/*\}*/

/**
 * TWI_RNPR: (TWI Offset: 0x110) Receive Next Pointer Register
 */
/*\{*/
#define TWI_RNPR_RXNPTR_SHIFT 0
#define TWI_RNPR_RXNPTR_MASK (0xffffffff << TWI_RNPR_RXNPTR_SHIFT)
#define TWI_RNPR_RXNPTR(value) ((TWI_RNPR_RXNPTR_MASK & ((value) << TWI_RNPR_RXNPTR_SHIFT)))
/*\}*/

/**
 * TWI_RNCR: (TWI Offset: 0x114) Receive Next Counter Register
 */
/*\{*/
#define TWI_RNCR_RXNCTR_SHIFT 0
#define TWI_RNCR_RXNCTR_MASK (0xffff << TWI_RNCR_RXNCTR_SHIFT)
#define TWI_RNCR_RXNCTR(value) ((TWI_RNCR_RXNCTR_MASK & ((value) << TWI_RNCR_RXNCTR_SHIFT)))
/*\}*/

/**
 * TWI_TNPR: (TWI Offset: 0x118) Transmit Next Pointer Register
 */
/*\{*/
#define TWI_TNPR_TXNPTR_SHIFT 0
#define TWI_TNPR_TXNPTR_MASK (0xffffffff << TWI_TNPR_TXNPTR_SHIFT)
#define TWI_TNPR_TXNPTR(value) ((TWI_TNPR_TXNPTR_MASK & ((value) << TWI_TNPR_TXNPTR_SHIFT)))
/*\}*/

/**
 * TWI_TNCR: (TWI Offset: 0x11C) Transmit Next Counter Register
 */
/*\{*/
#define TWI_TNCR_TXNCTR_SHIFT 0
#define TWI_TNCR_TXNCTR_MASK (0xffff << TWI_TNCR_TXNCTR_SHIFT)
#define TWI_TNCR_TXNCTR(value) ((TWI_TNCR_TXNCTR_MASK & ((value) << TWI_TNCR_TXNCTR_SHIFT)))
/*\}*/

/**
 * TWI_PTCR: (TWI Offset: 0x120) Transfer Control Register
 */
/*\{*/
#define TWI_PTCR_RXTEN BV(0)
#define TWI_PTCR_RXTDIS BV(1)
#define TWI_PTCR_TXTEN BV(8)
#define TWI_PTCR_TXTDIS BV(9)
/*\}*/

/**
 * TWI_PTSR: (TWI Offset: 0x124) Transfer Status Register
 */
/*\{*/
#define TWI_PTSR_RXTEN BV(0)
#define TWI_PTSR_TXTEN BV(8)
/*\}*/

#endif /* SAM3_TWI_H */
