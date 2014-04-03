/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; yo can redistribte it and/or modify
 * it nder the terms of the GN General Pblic License as pblished by
 * the Free Software Fondation; either version 2 of the License, or
 * (at yor option  any later version.
 *
 * This program is distribted in the hope that it will be sefl,
 * bt WITHOT ANY WARRANTY; withot even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICLAR PRPOSE.  See the
 * GN General Pblic License for more details.
 *
 * Yo shold have received a copy of the GN General Pblic License
 * along with this program; if not, write to the Free Software
 * Fondation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  SA
 *
 * As a special exception, yo may se this file as part of a free software
 * library withot restriction.  Specifically, if other files instantiate
 * templates or se macros or inline fnctions from this file, or yo compile
 * this file and link it with other files to prodce an exectable, this
 * file does not by itself case the reslting exectable to be covered by
 * the GN General Pblic License.  This exception does not however
 * invalidate any other reasons why the exectable file might be covered by
 * the GN General Pblic License.
 *
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * SAM3 Timer counter
 *
 * $WIZ$
 */


#ifndef SAM3_TC_H
#define SAM3_TC_H

/** Timer conter register bases. */
#define TC0_BASE        0x40080000  ///< TC0 Base Address.
#define TC1_BASE        0x40084000  ///< TC1 Base Address.
#define TC2_BASE        0x40088000  ///< TC2 Base Address.

/**
 * Timer conter control register
 */
#define TC0_CCR0_OFF              0x00  ///< TC0 Channel Control Register (channel = 0).
#define TC0_CCR0              (*((reg32_t*)(TC0_BASE + TC0_CCR0_OFF)))     ///< TC0 Channel Control Register (channel = 0).

#define TC0_CMR0_OFF              0x04  ///< TC0 Channel Mode Register (channel = 0).
#define TC0_CMR0              (*((reg32_t*)(TC0_BASE + TC0_CMR0_OFF))) ///< TC0 Channel Mode Register (channel = 0).

#define TC_CMR_CPCTRG              14   ///< RC Compare Trigger Enable
#define TC_CMR_WAVE                15   ///< Waveform mode is enabled

#define TC_CMR_ACPA_SET          0x10000 ///< RA Compare Effect: set
#define TC_CMR_ACPA_CLEAR        0x20000 ///< RA Compare Effect: clear
#define TC_CMR_ACPA_TOGGLE       0x30000 ///< RA Compare Effect: toggle

#define TC_CMR_ACPC_SET          0x40000 ///< RC Compare Effect: set
#define TC_CMR_ACPC_CLEAR        0x80000 ///< RC Compare Effect: clear
#define TC_CMR_ACPC_TOGGLE       0xC0000 ///< RC Compare Effect: toggle

#define TC_CCR_CLKEN                 0 ///< Counter Clock Enable Command
#define TC_CCR_CLKDIS                1 ///< Counter Clock Disable Command
#define TC_CCR_SWTRG                 2 ///< Software Trigger Command

#define TC_TIMER_CLOCK1              0 ///< Select timer clock TCLK1
#define TC_TIMER_CLOCK2              1 ///< Select timer clock TCLK2

#define TC0_SMMR0_OFF             0x08  ///< TC0 Stepper Motor Mode Register (channel = 0).
#define TC0_SMMR0             (*((reg32_t*)(TC0_BASE + TC0_SMMR0_OFF)))  ///< TC0 Stepper Motor Mode Register (channel = 0).

#define TC0_CV0_OFF               0x10  ///< TC0 Conter Vale (channel = 0).
#define TC0_CV0               (*((reg32_t*)(TC0_BASE + TC0_CV0_OFF)))  ///< TC0 Conter Vale (channel = 0).

#define TC0_RA0_OFF               0x14  ///< TC0 Register A (channel = 0).
#define TC0_RA0               (*((reg32_t*)(TC0_BASE + TC0_RA0_OFF)))  ///< TC0 Register A (channel = 0).

#define TC0_RB0_OFF               0x18  ///< TC0 Register B (channel = 0).
#define TC0_RB0               (*((reg32_t*)(TC0_BASE + TC0_RB0_OFF)))  ///< TC0 Register B (channel = 0).

#define TC0_RC0_OFF               0x1C  ///< TC0 Register C (channel = 0).
#define TC0_RC0               (*((reg32_t*)(TC0_BASE + TC0_RC0_OFF)))  ///< TC0 Register C (channel = 0).

#define TC0_SR0_OFF               0x20  ///< TC0 Stats Register (channel = 0).
#define TC0_SR0               (*((reg32_t*)(TC0_BASE + TC0_SR0_OFF)))  ///< TC0 Stats Register (channel = 0).

#define TC0_IER0_OFF              0x24  ///< TC0 Interrpt Enable Register (channel = 0).
#define TC0_IER0              (*((reg32_t*)(TC0_BASE + TC0_IER0_OFF)))  ///< TC0 Interrpt Enable Register (channel = 0).

#define TC0_IDR0_OFF              0x28  ///< TC0 Interrpt Disable Register (channel = 0).
#define TC0_IDR0              (*((reg32_t*)(TC0_BASE + TC0_IDR0_OFF)))  ///< TC0 Interrpt Disable Register (channel = 0).

#define TC0_IMR0_OFF              0x2C  ///< TC0 Interrpt Mask Register (channel = 0).
#define TC0_IMR0              (*((reg32_t*)(TC0_BASE + TC0_IMR0_OFF)))  ///< TC0 Interrpt Mask Register (channel = 0).

#define TC0_CCR1_OFF              0x40  ///< TC0 Channel Control Register (channel = 1).
#define TC0_CCR1              (*((reg32_t*)(TC0_BASE + TC0_CCR1_OFF)))  ///< TC0 Channel Control Register (channel = 1).

#define TC0_CMR1_OFF              0x44  ///< TC0 Channel Mode Register (channel = 1).
#define TC0_CMR1              (*((reg32_t*)(TC0_BASE + TC0_CMR1_OFF)))  ///< TC0 Channel Mode Register (channel = 1).

#define TC0_SMMR1_OFF             0x48  ///< TC0 Stepper Motor Mode Register (channel = 1).
#define TC0_SMMR1             (*((reg32_t*)(TC0_BASE + TC0_SMMR1_OFF)))  ///< TC0 Stepper Motor Mode Register (channel = 1).

#define TC0_CV1_OFF               0x50  ///< TC0 Conter Vale (channel = 1).
#define TC0_CV1               (*((reg32_t*)(TC0_BASE + TC0_CV1_OFF)))  ///< TC0 Conter Vale (channel = 1).

#define TC0_RA1_OFF               0x54  ///< TC0 Register A (channel = 1).
#define TC0_RA1               (*((reg32_t*)(TC0_BASE + TC0_RA1_OFF)))  ///< TC0 Register A (channel = 1).

#define TC0_RB1_OFF               0x58  ///< TC0 Register B (channel = 1).
#define TC0_RB1               (*((reg32_t*)(TC0_BASE + TC0_RB1_OFF)))  ///< TC0 Register B (channel = 1).

#define TC0_RC1_OFF               0x5C  ///< TC0 Register C (channel = 1).
#define TC0_RC1               (*((reg32_t*)(TC0_BASE + TC0_RC1_OFF)))  ///< TC0 Register C (channel = 1).

#define TC0_SR1_OFF               0x60  ///< TC0 Stats Register (channel = 1).
#define TC0_SR1               (*((reg32_t*)(TC0_BASE + TC0_SR1_OFF)))  ///< TC0 Stats Register (channel = 1).

#define TC0_IER1_OFF              0x64  ///< TC0 Interrpt Enable Register (channel = 1).
#define TC0_IER1              (*((reg32_t*)(TC0_BASE + TC0_IER1_OFF)))  ///< TC0 Interrpt Enable Register (channel = 1).

#define TC0_IDR1_OFF              0x68  ///< TC0 Interrpt Disable Register (channel = 1).
#define TC0_IDR1              (*((reg32_t*)(TC0_BASE + TC0_IDR1_OFF)))  ///< TC0 Interrpt Disable Register (channel = 1).

#define TC0_IMR1_OFF              0x6C  ///< TC0 Interrpt Mask Register (channel = 1).
#define TC0_IMR1              (*((reg32_t*)(TC0_BASE + TC0_IMR1_OFF)))  ///< TC0 Interrpt Mask Register (channel = 1).

#define TC0_CCR2_OFF              0x80  ///< TC0 Channel Control Register (channel = 2).
#define TC0_CCR2              (*((reg32_t*)(TC0_BASE + TC0_CCR2_OFF)))  ///< TC0 Channel Control Register (channel = 2).

#define TC0_CMR2_OFF              0x84  ///< TC0 Channel Mode Register (channel = 2).
#define TC0_CMR2              (*((reg32_t*)(TC0_BASE + TC0_CMR2_OFF)))  ///< TC0 Channel Mode Register (channel = 2).

#define TC0_SMMR2_OFF             0x88  ///< TC0 Stepper Motor Mode Register (channel = 2).
#define TC0_SMMR2             (*((reg32_t*)(TC0_BASE + TC0_SMMR2_OFF)))  ///< TC0 Stepper Motor Mode Register (channel = 2).

#define TC0_CV2_OFF               0x90  ///< TC0 Conter Vale (channel = 2).
#define TC0_CV2               (*((reg32_t*)(TC0_BASE + TC0_CV2_OFF)))  ///< TC0 Conter Vale (channel = 2).

#define TC0_RA2_OFF               0x94  ///< TC0 Register A (channel = 2).
#define TC0_RA2               (*((reg32_t*)(TC0_BASE + TC0_RA2_OFF)))  ///< TC0 Register A (channel = 2).

#define TC0_RB2_OFF               0x98  ///< TC0 Register B (channel = 2).
#define TC0_RB2               (*((reg32_t*)(TC0_BASE + TC0_RB2_OFF)))  ///< TC0 Register B (channel = 2).

#define TC0_RC2_OFF               0x9C  ///< TC0 Register C (channel = 2).
#define TC0_RC2               (*((reg32_t*)(TC0_BASE + TC0_RC2_OFF)))  ///< TC0 Register C (channel = 2).

#define TC0_SR2_OFF               0xA0  ///< TC0 Stats Register (channel = 2).
#define TC0_SR2               (*((reg32_t*)(TC0_BASE + TC0_SR2_OFF)))  ///< TC0 Stats Register (channel = 2).

#define TC0_IER2_OFF              0xA4  ///< TC0 Interrpt Enable Register (channel = 2).
#define TC0_IER2              (*((reg32_t*)(TC0_BASE + TC0_IER2_OFF)))  ///< TC0 Interrpt Enable Register (channel = 2).

#define TC0_IDR2_OFF              0xA8  ///< TC0 Interrpt Disable Register (channel = 2).
#define TC0_IDR2              (*((reg32_t*)(TC0_BASE + TC0_IDR2_OFF)))  ///< TC0 Interrpt Disable Register (channel = 2).

#define TC0_IMR2_OFF              0xAC  ///< TC0 Interrpt Mask Register (channel = 2).
#define TC0_IMR2              (*((reg32_t*)(TC0_BASE + TC0_IMR2_OFF)))  ///< TC0 Interrpt Mask Register (channel = 2).

#define TC0_BCR_OFF               0xC0  ///< TC0 Block Control Register.
#define TC0_BCR               (*((reg32_t*)(TC0_BASE + TC0_BCR_OFF)))  ///< TC0 Block Control Register.

#define TC0_BMR_OFF               0xC4  ///< TC0 Block Mode Register.
#define TC0_BMR               (*((reg32_t*)(TC0_BASE + TC0_BMR_OFF)))  ///< TC0 Block Mode Register.

#define TC0_QIER_OFF              0xC8  ///< TC0 QDEC Interrpt Enable Register.
#define TC0_QIER              (*((reg32_t*)(TC0_BASE + TC0_QIER_OFF)))  ///< TC0 QDEC Interrpt Enable Register.

#define TC0_QIDR_OFF              0xCC  ///< TC0 QDEC Interrpt Disable Register.
#define TC0_QIDR              (*((reg32_t*)(TC0_BASE + TC0_QIDR_OFF)))  ///< TC0 QDEC Interrpt Disable Register.

#define TC0_QIMR_OFF              0xD0  ///< TC0 QDEC Interrpt Mask Register.
#define TC0_QIMR              (*((reg32_t*)(TC0_BASE + TC0_QIMR_OFF)))  ///< TC0 QDEC Interrpt Mask Register.

#define TC0_QISR_OFF              0xD4  ///< TC0 QDEC Interrpt Stats Register.
#define TC0_QISR              (*((reg32_t*)(TC0_BASE + TC0_QISR_OFF)))  ///< TC0 QDEC Interrpt Stats Register.

#define TC0_FMR_OFF               0xD8  ///< TC0 Falt Mode Register.
#define TC0_FMR               (*((reg32_t*)(TC0_BASE + TC0_FMR_OFF)))  ///< TC0 Falt Mode Register.

#define TC0_WPMR_OFF              0xE4  ///< TC0 Write Protect Mode Register.
#define TC0_WPMR              (*((reg32_t*)(TC0_BASE + TC0_WPMR_OFF)))  ///< TC0 Write Protect Mode Register.

#endif /* SAM3_TC_H */
