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
 * \brief SAM3 SDRAM controller definitions.
 */

#ifndef SAM3_SDRAMC_H
#define SAM3_SDRAMC_H

#include <cfg/macros.h>


#if CPU_CM3_SAM3X

/** SDRAMC registers base. */
#define SDRAMC_BASE  0x400E0200


/**
 * SDRAMC register offsets.
 */
/*\{*/
#define SDRAMC_MR_OFF    0x00   ///< Mode Register
#define SDRAMC_TR_OFF    0x04   ///< Refresh Timer Register
#define SDRAMC_CR_OFF    0x08   ///< Configuration Register
#define SDRAMC_LPR_OFF   0x10   ///< Low Power Register
#define SDRAMC_IER_OFF   0x14   ///< Interrupt Enable Register
#define SDRAMC_IDR_OFF   0x18   ///< Interrupt Disable Register
#define SDRAMC_IMR_OFF   0x1C   ///< Interrupt Mask Register
#define SDRAMC_ISR_OFF   0x20   ///< Interrupt Status Register
#define SDRAMC_MDR_OFF   0x24   ///< Memory Device Register
#define SDRAMC_CR1_OFF   0x28   ///< Configuration Register 1
#define SDRAMC_OCMS_OFF  0x2C   ///< OCMS Register 1
/*\}*/

/**
 * SDRAMC registers.
 */
/*\{*/
#define SDRAMC_MR    (*(reg32_t *)(SDRAMC_BASE + SDRAMC_MR_OFF  ))
#define SDRAMC_TR    (*(reg32_t *)(SDRAMC_BASE + SDRAMC_TR_OFF  ))
#define SDRAMC_CR    (*(reg32_t *)(SDRAMC_BASE + SDRAMC_CR_OFF  ))
#define SDRAMC_LPR   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_LPR_OFF ))
#define SDRAMC_IER   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_IER_OFF ))
#define SDRAMC_IDR   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_IDR_OFF ))
#define SDRAMC_IMR   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_IMR_OFF ))
#define SDRAMC_ISR   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_ISR_OFF ))
#define SDRAMC_MDR   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_MDR_OFF ))
#define SDRAMC_CR1   (*(reg32_t *)(SDRAMC_BASE + SDRAMC_CR1_OFF ))
#define SDRAMC_OCMS  (*(reg32_t *)(SDRAMC_BASE + SDRAMC_OCMS_OFF))
/*\}*/


/**
 * Defines for bit fields in SDRAMC_MR register.
 */
/*\{*/
#define SDRAMC_MR_MODE_MASK                0x7
#define SDRAMC_MR_MODE_NORMAL              0x0  ///< Normal mode. Any access to the SDRAM is decoded normally. To activate this mode, command must be followed by a write to the SDRAM.
#define SDRAMC_MR_MODE_NOP                 0x1  ///< The SDRAM Controller issues a NOP command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM.
#define SDRAMC_MR_MODE_ALLBANKS_PRECHARGE  0x2  ///< The SDRAM Controller issues an "All Banks Precharge" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM.
#define SDRAMC_MR_MODE_LOAD_MODEREG        0x3  ///< The SDRAM Controller issues a "Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, command must be followed by a write to the SDRAM.
#define SDRAMC_MR_MODE_AUTO_REFRESH        0x4  ///< The SDRAM Controller issues an "Auto-Refresh" Command when the SDRAM device is accessed regardless of the cycle. Previously, an "All Banks Precharge" command must be issued. To activate this mode, command must be followed by a write to the SDRAM.
#define SDRAMC_MR_MODE_EXT_LOAD_MODEREG    0x5  ///< The SDRAM Controller issues an "Extended Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the "Extended Load Mode Register" command must be followed by a write to the SDRAM. The write in the SDRAM must be done in the appropriate bank; most low-power SDRAM devices use the bank 1.
#define SDRAMC_MR_MODE_DEEP_POWERDOWN      0x6  ///< Deep power-down mode. Enters deep power-down mode.
/*\}*/

/**
 * Defines for bit fields in SDRAMC_TR register.
 */
/*\{*/
#define SDRAMC_TR_COUNT_MASK    0xfff
#define SDRAMC_TR_COUNT(value)  (SDRAMC_TR_COUNT_MASK & (value))
/*\}*/

/**
 * Defines for bit fields in SDRAMC_CR register.
 */
/*\{*/
#define SDRAMC_CR_NC_SHIFT         0
#define SDRAMC_CR_NC_MASK          0x3 ///< Number of Column Bits
#define SDRAMC_CR_NC_COL8          0x0 ///< 8 column bits
#define SDRAMC_CR_NC_COL9          0x1 ///< 9 column bits
#define SDRAMC_CR_NC_COL10         0x2 ///< 10 column bits
#define SDRAMC_CR_NC_COL11         0x3 ///< 11 column bits
#define SDRAMC_CR_NR_SHIFT         2
#define SDRAMC_CR_NR_MASK          (0x3 << 2) ///< Number of Row Bits
#define SDRAMC_CR_NR_ROW11         (0x0 << 2) ///< 11 row bits
#define SDRAMC_CR_NR_ROW12         (0x1 << 2) ///< 12 row bits
#define SDRAMC_CR_NR_ROW13         (0x2 << 2) ///< 13 row bits
#define SDRAMC_CR_NB               (0x1 << 4) ///< Number of Banks
#define SDRAMC_CR_NB_BANK2         (0x0 << 4) ///< 2 banks
#define SDRAMC_CR_NB_BANK4         (0x1 << 4) ///< 4 banks
#define SDRAMC_CR_CAS_SHIFT        5
#define SDRAMC_CR_CAS_MASK         (0x3 << SDRAMC_CR_CAS_SHIFT) ///< CAS Latency
#define SDRAMC_CR_CAS_LATENCY1     (0x1 << 5) ///< 1 cycle CAS latency
#define SDRAMC_CR_CAS_LATENCY2     (0x2 << 5) ///< 2 cycle CAS latency
#define SDRAMC_CR_CAS_LATENCY3     (0x3 << 5) ///< 3 cycle CAS latency
#define SDRAMC_CR_DBW              (0x1 << 7) ///< Data Bus Width
#define SDRAMC_CR_TWR_SHIFT        8
#define SDRAMC_CR_TWR_MASK         (0xf << SDRAMC_CR_TWR_SHIFT) ///< Write Recovery Delay
#define SDRAMC_CR_TWR(value)       (SDRAMC_CR_TWR_MASK & ((value) << SDRAMC_CR_TWR_SHIFT))
#define SDRAMC_CR_TRC_TRFC_SHIFT   12
#define SDRAMC_CR_TRC_TRFC_MASK    (0xf << SDRAMC_CR_TRC_TRFC_SHIFT) ///< Row Cycle Delay and Row Refresh Cycle
#define SDRAMC_CR_TRC_TRFC(value)  (SDRAMC_CR_TRC_TRFC_MASK & ((value) << SDRAMC_CR_TRC_TRFC_SHIFT))
#define SDRAMC_CR_TRP_SHIFT        16
#define SDRAMC_CR_TRP_MASK         (0xf << SDRAMC_CR_TRP_SHIFT) ///< Row Precharge Delay
#define SDRAMC_CR_TRP(value)       (SDRAMC_CR_TRP_MASK & ((value) << SDRAMC_CR_TRP_SHIFT))
#define SDRAMC_CR_TRCD_SHIFT       20
#define SDRAMC_CR_TRCD_MASK        (0xf << SDRAMC_CR_TRCD_SHIFT) ///< Row to Column Delay
#define SDRAMC_CR_TRCD(value)      (SDRAMC_CR_TRCD_MASK & ((value) << SDRAMC_CR_TRCD_SHIFT))
#define SDRAMC_CR_TRAS_SHIFT       24
#define SDRAMC_CR_TRAS_MASK        (0xf << SDRAMC_CR_TRAS_SHIFT) ///< Active to Precharge Delay
#define SDRAMC_CR_TRAS(value)      (SDRAMC_CR_TRAS_MASK & ((value) << SDRAMC_CR_TRAS_SHIFT))
#define SDRAMC_CR_TXSR_SHIFT       28
#define SDRAMC_CR_TXSR_MASK        (0xf << SDRAMC_CR_TXSR_SHIFT) ///< Exit Self Refresh to Active Delay
#define SDRAMC_CR_TXSR(value)      (SDRAMC_CR_TXSR_MASK & ((value) << SDRAMC_CR_TXSR_SHIFT))
/*\}*/

/**
 * Defines for bit fields in SDRAMC_MDR register.
 */
/*\{*/
#define SDRAMC_MDR_MD_MASK     0x3
#define SDRAMC_MDR_MD_SDRAM    0x0
#define SDRAMC_MDR_MD_LPSDRAM  0x1
/*\}*/

#endif /* CPU_CM_SAM3X */

#endif /* SAM3_SDRAMC_H */
