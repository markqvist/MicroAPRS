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
 * \author Daniele Basile <asterix@develer.com>
 *
 * AT91SAM7 Conunter timer definition.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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

#ifndef AT91_TC_H
#define AT91_TC_H


/**
 * Timer Counter Control Register
 */
#define TC_TC0_OFF              0x00000000     ///< Channel 0 control register offset.
#define TC_TC1_OFF              0x00000040     ///< Channel 1 control register offset.
#define TC_TC2_OFF              0x00000080     ///< Channel 2 control register offset.
#define TC0_CCR         (*((reg32_t *)(TC_BASE + TC_TC0_OFF))) ///< Channel 0 control register address.
#define TC1_CCR         (*((reg32_t *)(TC_BASE + TC_TC1_OFF))) ///< Channel 1 control register address.
#define TC2_CCR         (*((reg32_t *)(TC_BASE + TC_TC2_OFF))) ///< Channel 2 control register address.
#define TC_CLKEN                         0      ///< Clock enable command.
#define TC_CLKDIS                        1      ///< Clock disable command.
#define TC_SWTRG                         2      ///< Software trigger command.

/**
 * Timer Counter Channel Mode Register
 */
#define TC_CMR_OFF              0x00000004      ///< Mode register offset.
#define TC0_CMR         (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_CMR_OFF))) ///< Channel 0 mode register address.
#define TC1_CMR         (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_CMR_OFF))) ///< Channel 1 mode register address.
#define TC2_CMR         (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_CMR_OFF))) ///< Channel 2 mode register address.

#define TC_CLKS_MASK            0x00000007      ///< Clock selection mask.
#define TC_CLKS_MCK2            0x00000000      ///< Selects MCK / 2.
#define TC_CLKS_MCK8            0x00000001      ///< Selects MCK / 8.
#define TC_CLKS_MCK32           0x00000002      ///< Selects MCK / 32.
#define TC_CLKS_MCK128          0x00000003      ///< Selects MCK / 128.
#define TC_CLKS_MCK1024         0x00000004      ///< Selects MCK / 1024.
#define TC_CLKS_XC0             0x00000005      ///< Selects external clock 0.
#define TC_CLKS_XC1             0x00000006      ///< Selects external clock 1.
#define TC_CLKS_XC2             0x00000007      ///< Selects external clock 2.

#define TC_CLKI                          3      ///< Increments on falling edge.

#define TC_BURST_MASK           0x00000030      ///< Burst signal selection mask.
#define TC_BURST_NONE           0x00000000      ///< Clock is not gated by an external signal.
#define TC_BUSRT_XC0            0x00000010      ///< ANDed with external clock 0.
#define TC_BURST_XC1            0x00000020      ///< ANDed with external clock 1.
#define TC_BURST_XC2            0x00000030      ///< ANDed with external clock 2.



#define TC_WAVE                         15      ///< Selects waveform mode.
//To select capture mode you must set TC_WAVE bit to 0.
//#define TC_CAPT                         15      ///< Selects capture mode.

/**
 * Capture Mode
 */
#define TC_CPCTRG                       14      ///< RC Compare Enable Trigger Enable.
#define TC_LDBSTOP                       6      ///< Counter clock stopped on RB loading.
#define TC_LDBDIS                        7      ///< Counter clock disabled on RB loading.

#define TC_ETRGEDG_MASK         0x00000300      ///< External trigger edge selection mask.
#define TC_ETRGEDG_RISING_EDGE  0x00000100      ///< Trigger on external rising edge.
#define TC_ETRGEDG_FALLING_EDGE 0x00000200      ///< Trigger on external falling edge.
#define TC_ETRGEDG_BOTH_EDGE    0x00000300      ///< Trigger on both external edges.

#define TC_ABETRG_MASK          0x00000400      ///< TIOA or TIOB external trigger selection mask.
#define TC_ABETRG_TIOA                  10      ///< TIOA used as an external trigger.
//To use external trigger TIOB you must set TC_ABETRG_TIOA bit to 0.
//#define TC_ABETRG_TIOB                  10      ///< TIOB used as an external trigger.


#define TC_LDRA_MASK            0x00030000      ///< RA loading selection mask.
#define TC_LDRA_RISING_EDGE     0x00010000      ///< Load RA on rising edge of TIOA.
#define TC_LDRA_FALLING_EDGE    0x00020000      ///< Load RA on falling edge of TIOA.
#define TC_LDRA_BOTH_EDGE       0x00030000      ///< Load RA on any edge of TIOA.

#define TC_LDRB_MASK            0x000C0000      ///< RB loading selection mask.
#define TC_LDRB_RISING_EDGE     0x00040000      ///< Load RB on rising edge of TIOA.
#define TC_LDRB_FALLING_EDGE    0x00080000      ///< Load RB on falling edge of TIOA.
#define TC_LDRB_BOTH_EDGE       0x000C0000      ///< Load RB on any edge of TIOA.


/**
 * Waveform Mode
 */
#define TC_CPCSTOP                       6      ///< Counter clock stopped on RC compare.
#define TC_CPCDIS                        7      ///< Counter clock disabled on RC compare.

#define TC_EEVTEDG_MASK         0x00000300      ///< External event edge selection mask.
#define TC_EEVTEDG_RISING_EDGE  0x00000100      ///< External event on rising edge..
#define TC_EEVTEDG_FALLING_EDGE 0x00000200      ///< External event on falling edge..
#define TC_EEVTEDG_BOTH_EDGE    0x00000300      ///< External event on any edge..

#define TC_EEVT_MASK            0x00000C00      ///< External event selection mask.
#define TC_EEVT_TIOB            0x00000000      ///< TIOB selected as external event.
#define TC_EEVT_XC0             0x00000400      ///< XC0 selected as external event.
#define TC_EEVT_XC1             0x00000800      ///< XC1 selected as external event.
#define TC_EEVT_XC2             0x00000C00      ///< XC2 selected as external event.

#define TC_ENETRG                       12      ///< External event trigger enable.

#define TC_WAVSEL_MASK          0x00006000      ///< Waveform selection mask.
#define TC_WAVSEL_UP            0x00000000      ///< UP mode whitout automatic trigger on RC compare.
#define TC_WAVSEL_UP_RC_TRG     0x00004000      ///< UP mode whit automatic trigger on RC compare.
#define TC_WAVSEL_UPDOWN        0x00002000      ///< UPDOWN mode whitout automatic trigger on RC compare.
#define TC_WAVSEL_UPDOWN_RC_TRG 0x00003000      ///< UPDOWN mode whit automatic trigger on RC compare.


#define TC_ACPA_MASK            0x00030000      ///< Masks RA compare effect on TIOA.
#define TC_ACPA_SET_OUTPUT      0x00010000      ///< RA compare sets TIOA.
#define TC_ACPA_CLEAR_OUTPUT    0x00020000      ///< RA compare clears TIOA.
#define TC_ACPA_TOGGLE_OUTPUT   0x00030000      ///< RA compare toggles TIOA.

#define TC_ACPC_MASK            0x000C0000      ///< Masks RC compare effect on TIOA.
#define TC_ACPC_SET_OUTPUT      0x00040000      ///< RC compare sets TIOA.
#define TC_ACPC_CLEAR_OUTPUT    0x00080000      ///< RC compare clears TIOA.
#define TC_ACPC_TOGGLE_OUTPUT   0x000C0000      ///< RC compare toggles TIOA.

#define TC_AEEVT_MASK           0x00300000      ///< Masks external event effect on TIOA.
#define TC_AEEVT_SET_OUTPUT     0x00100000      ///< External event sets TIOA.
#define TC_AEEVT_CLEAR_OUTPUT   0x00200000      ///< External event clears TIOA.
#define TC_AEEVT_TOGGLE_OUTPUT  0x00300000      ///< External event toggles TIOA.

#define TC_ASWTRG_MASK          0x00C00000      ///< Masks software trigger effect on TIOA.
#define TC_ASWTRG_SET_OUTPUT    0x00400000      ///< Software trigger sets TIOA.
#define TC_ASWTRG_CLEAR_OUTPUT  0x00800000      ///< Software trigger clears TIOA.
#define TC_ASWTRG_TOGGLE_OUTPUT 0x00C00000      ///< Software trigger toggles TIOA.

#define TC_BCPB_MASK            0x03000000      ///< Masks RB compare effect on TIOB.
#define TC_BCPB_SET_OUTPUT      0x01000000      ///< RB compare sets TIOB.
#define TC_BCPB_CLEAR_OUTPUT    0x02000000      ///< RB compare clears TIOB.
#define TC_BCPB_TOGGLE_OUTPUT   0x03000000      ///< RB compare toggles TIOB.

#define TC_BCPC_MASK            0x0C000000      ///< Masks RC compare effect on TIOB.
#define TC_BCPC_SET_OUTPUT      0x04000000      ///< RC compare sets TIOB.
#define TC_BCPC_CLEAR_OUTPUT    0x08000000      ///< RC compare clears TIOB.
#define TC_BCPC_TOGGLE_OUTPUT   0x0C000000      ///< RC compare toggles TIOB.

#define TC_BEEVT_MASK           0x30000000      ///< Masks external event effect on TIOB.
#define TC_BEEVT_SET_OUTPUT     0x10000000      ///< External event sets TIOB.
#define TC_BEEVT_CLEAR_OUTPUT   0x20000000      ///< External event clears TIOB.
#define TC_BEEVT_TOGGLE_OUTPUT  0x30000000      ///< External event toggles TIOB.

#define TC_BSWTRG_MASK          0xC0000000      ///< Masks software trigger effect on TIOB.
#define TC_BSWTRG_SET_OUTPUT    0x40000000      ///< Software trigger sets TIOB.
#define TC_BSWTRG_CLEAR_OUTPUT  0x80000000      ///< Software trigger clears TIOB.
#define TC_BSWTRG_TOGGLE_OUTPUT 0xC0000000      ///< Software trigger toggles TIOB.

/**
 * Counter Value Register
 */
#define TC_CV_OFF               0x00000010      ///< Counter register value offset.
#define TC0_CV          (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_CV_OFF))) ///< Counter 0 value.
#define TC1_CV          (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_CV_OFF))) ///< Counter 1 value.
#define TC2_CV          (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_CV_OFF))) ///< Counter 2 value.

/**
 * Timer Counter Register A
 */
#define TC_RA_OFF               0x00000014      ///< Register A offset.
#define TC0_RA          (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_RA_OFF))) ///< Channel 0 register A.
#define TC1_RA          (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_RA_OFF))) ///< Channel 1 register A.
#define TC2_RA          (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_RA_OFF))) ///< Channel 2 register A.


/**
 * Timer Counter Register B
 */
#define TC_RB_OFF               0x00000018      ///< Register B offset.
#define TC0_RB           (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_RB_OFF))) ///< Channel 0 register B.
#define TC1_RB           (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_RB_OFF))) ///< Channel 1 register B.
#define TC2_RB           (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_RB_OFF))) ///< Channel 2 register B.


/**
 * Timer Counter Register C
 */
#define TC_RC_OFF               0x0000001C      ///< Register C offset.
#define TC0_RC          (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_RC_OFF))) ///< Channel 0 register C.
#define TC1_RC          (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_RC_OFF))) ///< Channel 1 register C.
#define TC2_RC          (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_RC_OFF))) ///< Channel 2 register C.



/**
 * Timer Counter Status and Interrupt Registers
 */
#define TC_SR_OFF               0x00000020      ///< Status Register offset.
#define TC0_SR          (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_SR_OFF))) ///< Status register address.
#define TC1_SR          (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_SR_OFF))) ///< Status register address.
#define TC2_SR          (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_SR_OFF))) ///< Status register address.

#define TC_IER_OFF              0x00000024      ///< Interrupt Enable Register offset.
#define TC0_IER         (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_IER_OFF))) ///< Channel 0 interrupt enable register address.
#define TC1_IER         (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_IER_OFF))) ///< Channel 1 interrupt enable register address.
#define TC2_IER         (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_IER_OFF))) ///< Channel 2 interrupt enable register address.

#define TC_IDR_OFF              0x00000028      ///< Interrupt Disable Register offset.
#define TC0_IDR        (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_IDR_OFF))) ///< Channel 0 interrupt disable register address.
#define TC1_IDR        (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_IDR_OFF))) ///< Channel 1 interrupt disable register address.
#define TC2_IDR        (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_IDR_OFF))) ///< Channel 2 interrupt disable register address.

#define TC_IMR_OFF              0x0000002C      ///< Interrupt Mask Register offset.
#define TC0_IMR        (*((reg32_t *)(TC_BASE + TC_TC0_OFF + TC_IMR_OFF))) ///< Channel 0 interrupt mask register address.
#define TC1_IMR        (*((reg32_t *)(TC_BASE + TC_TC1_OFF + TC_IMR_OFF))) ///< Channel 1 interrupt mask register address.
#define TC2_IMR        (*((reg32_t *)(TC_BASE + TC_TC2_OFF + TC_IMR_OFF))) ///< Channel 2 interrupt mask register address.

#define TC_COVFS                         0      ///< Counter overflow flag.
#define TC_LOVRS                         1      ///< Load overrun flag.
#define TC_CPAS                          2      ///< RA compare flag.
#define TC_CPBS                          3      ///< RB compare flag.
#define TC_CPCS                          4      ///< RC compare flag.
#define TC_LDRAS                         5      ///< RA loading flag.
#define TC_LDRBS                         6      ///< RB loading flag.
#define TC_ETRGS                         7      ///< External trigger flag.
#define TC_CLKSTA                       16      ///< Clock enable flag.
#define TC_MTIOA                        17      ///< TIOA flag.
#define TC_MTIOB                        18      ///< TIOB flag.


/**
 * Timer Counter Block Control Register
 */
#define TC_BCR_OFF              0x000000C0      ///< Block control register offset.
#define TC_BCR         (*((reg32_t *)(TC_BASE + TC_BCR_OFF))) ///< Block control register address.
#define TC_SYNC                          0      ///< Synchronisation trigger


/**
 * Timer Counter Block Mode Register
 */
#define TC_BMR_OFF              0x000000C4      ///< Block mode register offset.
#define TC_BMR         (*((reg32_t *)(TC_BASE + TC_BMR_OFF))) ///< Block mode register address.
#define TC_TC0XC0S              0x00000003      ///< External clock signal 0 selection mask.
#define TC_TCLK0XC0             0x00000000      ///< Selects TCLK0.
#define TC_NONEXC0              0x00000001      ///< None selected.
#define TC_TIOA1XC0             0x00000002      ///< Selects TIOA1.
#define TC_TIOA2XC0             0x00000003      ///< Selects TIOA2.

#define TC_TC1XC1S              0x0000000C      ///< External clock signal 1 selection mask.
#define TC_TCLK1XC1             0x00000000      ///< Selects TCLK1.
#define TC_NONEXC1              0x00000004      ///< None selected.
#define TC_TIOA0XC1             0x00000008      ///< Selects TIOA0.
#define TC_TIOA2XC1             0x0000000C      ///< Selects TIOA2.

#define TC_TC2XC2S              0x00000030      ///< External clock signal 2 selection mask.
#define TC_TCLK2XC2             0x00000000      ///< Selects TCLK2.
#define TC_NONEXC2              0x00000010      ///< None selected.
#define TC_TIOA0XC2             0x00000020      ///< Selects TIOA0.
#define TC_TIOA1XC2             0x00000030      ///< Selects TIOA1.


#endif /* AT91_TC_H */
