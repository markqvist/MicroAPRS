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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * AT91SAM7 Pulse Width Modulation Controller.
 */

#ifndef AT91_PWM_H
#define AT91_PWM_H

/**
 * PWM Mode Register.
 */
/*\{*/
#define PWM_MR_OFF              0x00000000    ///< PWM Mode Register offset.
#define PWM_MR     (*((reg32_t *)(PWMC_BASE + PWM_MR_OFF))) ///< PWM Mode Register.
#define PWM_MR_DIVA_MASK        0x000000FF    ///< PWM Mode Divide factor A Mask.
#define PWM_MR_DIVA_SHIFT       0             ///< PWM Mode Divide factor A LSB.
#define PWM_MR_DIVB_MASK        0x00FF0000    ///< PWM Mode Divide factor B Mask.
#define PWM_MR_DIVB_SHIFT       16            ///< PWM Mode Divide factor B LSB.

#define PWM_MR_PREA_MASK        0x00000F00    ///< PWM Mode prescaler A Mask.
#define PWM_MR_PREA_SHIFT       8             ///< PWM Mode prescaler A LSB.
#define PWM_MR_PREB_MASK        0x0F000000    ///< PWM Mode prescaler B Mask.
#define PWM_MR_PREB_SHIFT       24            ///< PWM Mode prescaler B LSB.

#define PWM_MR_PRE_MCK          0             ///< PWM Mode prescaler set to MCK.
#define PWM_MR_PRE_MCK_DIV2     1             ///< PWM Mode prescaler set to MCK/2.
#define PWM_MR_PRE_MCK_DIV4     2             ///< PWM Mode prescaler set to MCK/4.
#define PWM_MR_PRE_MCK_DIV8     3             ///< PWM Mode prescaler set to MCK/8.
#define PWM_MR_PRE_MCK_DIV16    4             ///< PWM Mode prescaler set to MCK/16.
#define PWM_MR_PRE_MCK_DIV32    5             ///< PWM Mode prescaler set to MCK/32.
#define PWM_MR_PRE_MCK_DIV64    6             ///< PWM Mode prescaler set to MCK/64.
#define PWM_MR_PRE_MCK_DIV128   7             ///< PWM Mode prescaler set to MCK/128.
#define PWM_MR_PRE_MCK_DIV256   8             ///< PWM Mode prescaler set to MCK/256.
#define PWM_MR_PRE_MCK_DIV512   9             ///< PWM Mode prescaler set to MCK/512.
#define PWM_MR_PRE_MCK_DIV1024  10            ///< PWM Mode prescaler set to MCK/1024.
/*\}*/

/**
 * PWM Channel IDs.
 */
/*\{*/
#define PWM_CHID_MASK           0x0000000F
#define PWM_CHID0               0
#define PWM_CHID1               1
#define PWM_CHID2               2
#define PWM_CHID3               3
/*\}*/

/**
 * PWM Enable Register.
 */
/*\{*/
#define PWM_ENA_OFF             0x00000004    ///< PWM Enable Register offset.
#define PWM_ENA     (*((reg32_t *)(PWMC_BASE + PWM_ENA_OFF))) ///< PWM Enable Register.
/*\}*/

/**
 * PWM Disable Register.
 */
/*\{*/
#define PWM_DIS_OFF             0x00000008    ///< PWM Disable Register offset.
#define PWM_DIS     (*((reg32_t *)(PWMC_BASE + PWM_DIS_OFF))) ///< PWM Disable Register.
/*\}*/

/**
 * PWM Status Register.
 */
/*\{*/
#define PWM_SR_OFF              0x0000000C    ///< PWM Status Register offset.
#define PWM_SR      (*((reg32_t *)(PWMC_BASE + PWM_SR_OFF)))  ///< PWM Status Register.
/*\}*/

/**
 * PWM Interrupt Enable Register.
 */
/*\{*/
#define PWM_IER_OFF             0x00000010    ///< PWM Interrupt Enable Register offset.
#define PWM_IER     (*((reg32_t *)(PWMC_BASE + PWM_IER_OFF))) ///< PWM Interrupt Enable Register.
/*\}*/

/**
 * PWM Interrupt Disable Register.
 */
/*\{*/
#define PWM_IDR_OFF             0x00000014    ///< PWM Interrupt Disable Register offset.
#define PWM_IDR     (*((reg32_t *)(PWMC_BASE + PWM_IDR_OFF))) ///< PWM Interrupt Disable Register.
/*\}*/

/**
 * PWM Interrupt Mask Register.
 */
/*\{*/
#define PWM_IMR_OFF             0x00000018    ///< PWM Interrupt Mask Register offset.
#define PWM_IMR     (*((reg32_t *)(PWMC_BASE + PWM_IMR_OFF))) ///< PWM Interrupt Mask Register.
/*\}*/

/**
 * PWM Interrupt Status Register.
 */
/*\{*/
#define PWM_ISR_OFF             0x0000001C    ///< PWM Interrupt Status Register offset.
#define PWM_ISR     (*((reg32_t *)(PWMC_BASE + PWM_ISR_OFF))) ///< PWM Interrupt Status Register.
/*\}*/

#define PWM_CH0_OFF             0x00000200    ///< PWM Channel 0 registers offset.
#define PWM_CH1_OFF             0x00000220    ///< PWM Channel 1 registers offset.
#define PWM_CH2_OFF             0x00000240    ///< PWM Channel 2 registers offset.
#define PWM_CH3_OFF             0x00000260    ///< PWM Channel 3 registers offset.

/**
 * PWM Channel Mode Register.
 */
/*\{*/
#define PWM_CMR_OFF             0x00000000    ///< PWM Channel Mode Register offset.
#define PWM_CMR0    (*((reg32_t *)(PWMC_BASE + PWM_CMR_OFF + PWM_CH0_OFF))) ///< PWM Channel 0 Mode Register.
#define PWM_CMR1    (*((reg32_t *)(PWMC_BASE + PWM_CMR_OFF + PWM_CH1_OFF))) ///< PWM Channel 1 Mode Register.
#define PWM_CMR2    (*((reg32_t *)(PWMC_BASE + PWM_CMR_OFF + PWM_CH2_OFF))) ///< PWM Channel 2 Mode Register.
#define PWM_CMR3    (*((reg32_t *)(PWMC_BASE + PWM_CMR_OFF + PWM_CH3_OFF))) ///< PWM Channel 3 Mode Register.

#define PWM_CPRE_MCK_MASK       0x0000000F    ///< PWM Mode prescaler mask.
#define PWM_CPRE_MCK            0             ///< PWM Mode prescaler set to MCK.
#define PWM_CPRE_MCK_DIV2       1             ///< PWM Mode prescaler set to MCK/2.
#define PWM_CPRE_MCK_DIV4       2             ///< PWM Mode prescaler set to MCK/4.
#define PWM_CPRE_MCK_DIV8       3             ///< PWM Mode prescaler set to MCK/8.
#define PWM_CPRE_MCK_DIV16      4             ///< PWM Mode prescaler set to MCK/16.
#define PWM_CPRE_MCK_DIV32      5             ///< PWM Mode prescaler set to MCK/32.
#define PWM_CPRE_MCK_DIV64      6             ///< PWM Mode prescaler set to MCK/64.
#define PWM_CPRE_MCK_DIV128     7             ///< PWM Mode prescaler set to MCK/128.
#define PWM_CPRE_MCK_DIV256     8             ///< PWM Mode prescaler set to MCK/256.
#define PWM_CPRE_MCK_DIV512     9             ///< PWM Mode prescaler set to MCK/512.
#define PWM_CPRE_MCK_DIV1024    10            ///< PWM Mode prescaler set to MCK/1024.
#define PWM_CPRE_CLKA           11            ///< PWM Mode prescaler set to CLKA.
#define PWM_CPRE_CLKB           12            ///< PWM Mode prescaler set to CLKB.

#define PWM_CALG                8             ///< PWM Mode channel alignment.
#define PWM_CPOL                9             ///< PWM Mode channel polarity.
#define PWM_CPD                 10            ///< PWM Mode channel update period.
/*\}*/


/**
 * PWM Channel Duty Cycle Register.
 */
/*\{*/
#define PWM_CDTY_OFF            0x00000004    ///< PWM Channel Duty Cycle Register offset.
#define PWM_CDTY0   (*((reg32_t *)(PWMC_BASE + PWM_CDTY_OFF + PWM_CH0_OFF))) ///< PWM Channel 0 Duty Cycle Register.
#define PWM_CDTY1   (*((reg32_t *)(PWMC_BASE + PWM_CDTY_OFF + PWM_CH1_OFF))) ///< PWM Channel 1 Duty Cycle Register.
#define PWM_CDTY2   (*((reg32_t *)(PWMC_BASE + PWM_CDTY_OFF + PWM_CH2_OFF))) ///< PWM Channel 2 Duty Cycle Register.
#define PWM_CDTY3   (*((reg32_t *)(PWMC_BASE + PWM_CDTY_OFF + PWM_CH3_OFF))) ///< PWM Channel 3 Duty Cycle Register.
/*\}*/


/**
 * PWM Channel Period Register.
 */
/*\{*/
#define PWM_CPRD_OFF            0x00000008    ///< PWM Channel Period Register offset.
#define PWM_CPRD0   (*((reg32_t *)(PWMC_BASE + PWM_CPRD_OFF + PWM_CH0_OFF))) ///< PWM Channel 0 Period Register.
#define PWM_CPRD1   (*((reg32_t *)(PWMC_BASE + PWM_CPRD_OFF + PWM_CH1_OFF))) ///< PWM Channel 1 Period Register.
#define PWM_CPRD2   (*((reg32_t *)(PWMC_BASE + PWM_CPRD_OFF + PWM_CH2_OFF))) ///< PWM Channel 2 Period Register.
#define PWM_CPRD3   (*((reg32_t *)(PWMC_BASE + PWM_CPRD_OFF + PWM_CH3_OFF))) ///< PWM Channel 3 Period Register.
/*\}*/


/**
 * PWM Channel Counter Register.
 */
/*\{*/
#define PWM_CCNT_OFF            0x0000000C    ///< PWM Channel Counter Register offset.
#define PWM_CCNT0   (*((reg32_t *)(PWMC_BASE + PWM_CCNT_OFF + PWM_CH0_OFF))) ///< PWM Channel 0 Counter Register.
#define PWM_CCNT1   (*((reg32_t *)(PWMC_BASE + PWM_CCNT_OFF + PWM_CH1_OFF))) ///< PWM Channel 1 Counter Register.
#define PWM_CCNT2   (*((reg32_t *)(PWMC_BASE + PWM_CCNT_OFF + PWM_CH2_OFF))) ///< PWM Channel 2 Counter Register.
#define PWM_CCNT3   (*((reg32_t *)(PWMC_BASE + PWM_CCNT_OFF + PWM_CH3_OFF))) ///< PWM Channel 3 Counter Register.
/*\}*/


/**
 * PWM Channel Update Register.
 */
/*\{*/
#define PWM_CUPD_OFF            0x00000010    ///< PWM Channel Update Register offset.
#define PWM_CUPD0   (*((reg32_t *)(PWMC_BASE + PWM_CUPD_OFF + PWM_CH0_OFF))) ///< PWM Channel 0 Update Register.
#define PWM_CUPD1   (*((reg32_t *)(PWMC_BASE + PWM_CUPD_OFF + PWM_CH1_OFF))) ///< PWM Channel 1 Update Register.
#define PWM_CUPD2   (*((reg32_t *)(PWMC_BASE + PWM_CUPD_OFF + PWM_CH2_OFF))) ///< PWM Channel 2 Update Register.
#define PWM_CUPD3   (*((reg32_t *)(PWMC_BASE + PWM_CUPD_OFF + PWM_CH3_OFF))) ///< PWM Channel 3 Update Register.
/*\}*/

#endif /* AT91_PWM_H */
