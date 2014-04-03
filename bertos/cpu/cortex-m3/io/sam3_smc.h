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
 * \brief SAM3X/A/U Static Memory Controller definitions.
 */

#ifndef SAM3_SMC_H
#define SAM3_SMC_H

/*
 * SMC registers defined only for SAM3X/A and U for now
 */
#if CPU_CM3_SAM3X || CPU_CM3_SAM3U

/** SMC registers base. */
#define SMC_BASE  0x400E0000


/**
 * SMC register offsets.
 */
/*\{*/
#define SMC_CFG_OFF         0x000  ///< NFC Configuration
#define SMC_CTRL_OFF        0x004  ///< NFC Control
#define SMC_SR_OFF          0x008  ///< NFC Status
#define SMC_IER_OFF         0x00C  ///< NFC Interrupt Enable
#define SMC_IDR_OFF         0x010  ///< NFC Interrupt Disable
#define SMC_IMR_OFF         0x014  ///< NFC Interrupt Mask
#define SMC_ADDR_OFF        0x018  ///< NFC Address Cycle Zero
#define SMC_BANK_OFF        0x01C  ///< Bank Address
#define SMC_ECC_CTRL_OFF    0x020  ///< ECC Control
#define SMC_ECC_MD_OFF      0x024  ///< ECC Mode
#define SMC_ECC_SR1_OFF     0x028  ///< ECC Status 1
#define SMC_ECC_PR0_OFF     0x02C  ///< ECC Parity 0
#define SMC_ECC_PR1_OFF     0x030  ///< ECC parity 1
#define SMC_ECC_SR2_OFF     0x034  ///< ECC status 2
#define SMC_ECC_PR2_OFF     0x038  ///< ECC parity 2
#define SMC_ECC_PR3_OFF     0x03C  ///< ECC parity 3
#define SMC_ECC_PR4_OFF     0x040  ///< ECC parity 4
#define SMC_ECC_PR5_OFF     0x044  ///< ECC parity 5
#define SMC_ECC_PR6_OFF     0x048  ///< ECC parity 6
#define SMC_ECC_PR7_OFF     0x04C  ///< ECC parity 7
#define SMC_ECC_PR8_OFF     0x050  ///< ECC parity 8
#define SMC_ECC_PR9_OFF     0x054  ///< ECC parity 9
#define SMC_ECC_PR10_OFF    0x058  ///< ECC parity 10
#define SMC_ECC_PR11_OFF    0x05C  ///< ECC parity 11
#define SMC_ECC_PR12_OFF    0x060  ///< ECC parity 12
#define SMC_ECC_PR13_OFF    0x064  ///< ECC parity 13
#define SMC_ECC_PR14_OFF    0x068  ///< ECC parity 14
#define SMC_ECC_PR15_OFF    0x06C  ///< ECC parity 15
#define SMC_SETUP0_OFF      0x070  ///< SETUP (CS_number = 0)
#define SMC_PULSE0_OFF      0x074  ///< PULSE (CS_number = 0)
#define SMC_CYCLE0_OFF      0x078  ///< CYCLE (CS_number = 0)
#define SMC_TIMINGS0_OFF    0x07C  ///< TIMINGS (CS_number = 0)
#define SMC_MODE0_OFF       0x080  ///< MODE (CS_number = 0)
#define SMC_SETUP1_OFF      0x084  ///< SETUP (CS_number = 1)
#define SMC_PULSE1_OFF      0x088  ///< PULSE (CS_number = 1)
#define SMC_CYCLE1_OFF      0x08C  ///< CYCLE (CS_number = 1)
#define SMC_TIMINGS1_OFF    0x090  ///< TIMINGS (CS_number = 1)
#define SMC_MODE1_OFF       0x094  ///< MODE (CS_number = 1)
#define SMC_SETUP2_OFF      0x098  ///< SETUP (CS_number = 2)
#define SMC_PULSE2_OFF      0x09C  ///< PULSE (CS_number = 2)
#define SMC_CYCLE2_OFF      0x0A0  ///< CYCLE (CS_number = 2)
#define SMC_TIMINGS2_OFF    0x0A4  ///< TIMINGS (CS_number = 2)
#define SMC_MODE2_OFF       0x0A8  ///< MODE (CS_number = 2)
#define SMC_SETUP3_OFF      0x0AC  ///< SETUP (CS_number = 3)
#define SMC_PULSE3_OFF      0x0B0  ///< PULSE (CS_number = 3)
#define SMC_CYCLE3_OFF      0x0B4  ///< CYCLE (CS_number = 3)
#define SMC_TIMINGS3_OFF    0x0B8  ///< TIMINGS (CS_number = 3)
#define SMC_MODE3_OFF       0x0BC  ///< MODE (CS_number = 3)
#define SMC_SETUP4_OFF      0x0C0  ///< SETUP (CS_number = 4)
#define SMC_PULSE4_OFF      0x0C4  ///< PULSE (CS_number = 4)
#define SMC_CYCLE4_OFF      0x0C8  ///< CYCLE (CS_number = 4)
#define SMC_TIMINGS4_OFF    0x0CC  ///< TIMINGS (CS_number = 4)
#define SMC_MODE4_OFF       0x0D0  ///< MODE (CS_number = 4)
#define SMC_SETUP5_OFF      0x0D4  ///< SETUP (CS_number = 5)
#define SMC_PULSE5_OFF      0x0D8  ///< PULSE (CS_number = 5)
#define SMC_CYCLE5_OFF      0x0DC  ///< CYCLE (CS_number = 5)
#define SMC_TIMINGS5_OFF    0x0E0  ///< TIMINGS (CS_number = 5)
#define SMC_MODE5_OFF       0x0E4  ///< MODE (CS_number = 5)
#define SMC_SETUP6_OFF      0x0E8  ///< SETUP (CS_number = 6)
#define SMC_PULSE6_OFF      0x0EC  ///< PULSE (CS_number = 6)
#define SMC_CYCLE6_OFF      0x0F0  ///< CYCLE (CS_number = 6)
#define SMC_TIMINGS6_OFF    0x0F4  ///< TIMINGS (CS_number = 6)
#define SMC_MODE6_OFF       0x0F8  ///< MODE (CS_number = 6)
#define SMC_SETUP7_OFF      0x0FC  ///< SETUP (CS_number = 7)
#define SMC_PULSE7_OFF      0x100  ///< PULSE (CS_number = 7)
#define SMC_CYCLE7_OFF      0x104  ///< CYCLE (CS_number = 7)
#define SMC_TIMINGS7_OFF    0x108  ///< TIMINGS (CS_number = 7)
#define SMC_MODE7_OFF       0x10C  ///< MODE (CS_number = 7)
#define SMC_OCMS_OFF        0x110  ///< OCMS MODE
#define SMC_KEY1_OFF        0x114  ///< KEY1
#define SMC_KEY2_OFF        0x118  ///< KEY2
#define SMC_WPCR_OFF        0x1E4  ///< Write Protection Control
#define SMC_WPSR_OFF        0x1E8  ///< Write Protection Status
/*\}*/

/**
 * SMC registers.
 */
/*\{*/
#define SMC_CFG        (*((reg32_t *)(SMC_BASE + SMC_CFG_OFF)))
#define SMC_CTRL       (*((reg32_t *)(SMC_BASE + SMC_CTRL_OFF)))
#define SMC_SR         (*((reg32_t *)(SMC_BASE + SMC_SR_OFF)))
#define SMC_IER        (*((reg32_t *)(SMC_BASE + SMC_IER_OFF)))
#define SMC_IDR        (*((reg32_t *)(SMC_BASE + SMC_IDR_OFF)))
#define SMC_IMR        (*((reg32_t *)(SMC_BASE + SMC_IMR_OFF)))
#define SMC_ADDR       (*((reg32_t *)(SMC_BASE + SMC_ADDR_OFF)))
#define SMC_BANK       (*((reg32_t *)(SMC_BASE + SMC_BANK_OFF)))
#define SMC_ECC_CTRL   (*((reg32_t *)(SMC_BASE + SMC_ECC_CTRL_OFF)))
#define SMC_ECC_MD     (*((reg32_t *)(SMC_BASE + SMC_ECC_MD_OFF)))
#define SMC_ECC_SR1    (*((reg32_t *)(SMC_BASE + SMC_ECC_SR1_OFF)))
#define SMC_ECC_PR0    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR0_OFF)))
#define SMC_ECC_PR1    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR1_OFF)))
#define SMC_ECC_SR2    (*((reg32_t *)(SMC_BASE + SMC_ECC_SR2_OFF)))
#define SMC_ECC_PR2    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR2_OFF)))
#define SMC_ECC_PR3    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR3_OFF)))
#define SMC_ECC_PR4    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR4_OFF)))
#define SMC_ECC_PR5    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR5_OFF)))
#define SMC_ECC_PR6    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR6_OFF)))
#define SMC_ECC_PR7    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR7_OFF)))
#define SMC_ECC_PR8    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR8_OFF)))
#define SMC_ECC_PR9    (*((reg32_t *)(SMC_BASE + SMC_ECC_PR9_OFF)))
#define SMC_ECC_PR10   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR10_OFF)))
#define SMC_ECC_PR11   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR11_OFF)))
#define SMC_ECC_PR12   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR12_OFF)))
#define SMC_ECC_PR13   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR13_OFF)))
#define SMC_ECC_PR14   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR14_OFF)))
#define SMC_ECC_PR15   (*((reg32_t *)(SMC_BASE + SMC_ECC_PR15_OFF)))
#define SMC_SETUP0     (*((reg32_t *)(SMC_BASE + SMC_SETUP0_OFF)))
#define SMC_PULSE0     (*((reg32_t *)(SMC_BASE + SMC_PULSE0_OFF)))
#define SMC_CYCLE0     (*((reg32_t *)(SMC_BASE + SMC_CYCLE0_OFF)))
#define SMC_TIMINGS0   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS0_OFF)))
#define SMC_MODE0      (*((reg32_t *)(SMC_BASE + SMC_MODE0_OFF)))
#define SMC_SETUP1     (*((reg32_t *)(SMC_BASE + SMC_SETUP1_OFF)))
#define SMC_PULSE1     (*((reg32_t *)(SMC_BASE + SMC_PULSE1_OFF)))
#define SMC_CYCLE1     (*((reg32_t *)(SMC_BASE + SMC_CYCLE1_OFF)))
#define SMC_TIMINGS1   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS1_OFF)))
#define SMC_MODE1      (*((reg32_t *)(SMC_BASE + SMC_MODE1_OFF)))
#define SMC_SETUP2     (*((reg32_t *)(SMC_BASE + SMC_SETUP2_OFF)))
#define SMC_PULSE2     (*((reg32_t *)(SMC_BASE + SMC_PULSE2_OFF)))
#define SMC_CYCLE2     (*((reg32_t *)(SMC_BASE + SMC_CYCLE2_OFF)))
#define SMC_TIMINGS2   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS2_OFF)))
#define SMC_MODE2      (*((reg32_t *)(SMC_BASE + SMC_MODE2_OFF)))
#define SMC_SETUP3     (*((reg32_t *)(SMC_BASE + SMC_SETUP3_OFF)))
#define SMC_PULSE3     (*((reg32_t *)(SMC_BASE + SMC_PULSE3_OFF)))
#define SMC_CYCLE3     (*((reg32_t *)(SMC_BASE + SMC_CYCLE3_OFF)))
#define SMC_TIMINGS3   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS3_OFF)))
#define SMC_MODE3      (*((reg32_t *)(SMC_BASE + SMC_MODE3_OFF)))
#define SMC_SETUP4     (*((reg32_t *)(SMC_BASE + SMC_SETUP4_OFF)))
#define SMC_PULSE4     (*((reg32_t *)(SMC_BASE + SMC_PULSE4_OFF)))
#define SMC_CYCLE4     (*((reg32_t *)(SMC_BASE + SMC_CYCLE4_OFF)))
#define SMC_TIMINGS4   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS4_OFF)))
#define SMC_MODE4      (*((reg32_t *)(SMC_BASE + SMC_MODE4_OFF)))
#define SMC_SETUP5     (*((reg32_t *)(SMC_BASE + SMC_SETUP5_OFF)))
#define SMC_PULSE5     (*((reg32_t *)(SMC_BASE + SMC_PULSE5_OFF)))
#define SMC_CYCLE5     (*((reg32_t *)(SMC_BASE + SMC_CYCLE5_OFF)))
#define SMC_TIMINGS5   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS5_OFF)))
#define SMC_MODE5      (*((reg32_t *)(SMC_BASE + SMC_MODE5_OFF)))
#define SMC_SETUP6     (*((reg32_t *)(SMC_BASE + SMC_SETUP6_OFF)))
#define SMC_PULSE6     (*((reg32_t *)(SMC_BASE + SMC_PULSE6_OFF)))
#define SMC_CYCLE6     (*((reg32_t *)(SMC_BASE + SMC_CYCLE6_OFF)))
#define SMC_TIMINGS6   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS6_OFF)))
#define SMC_MODE6      (*((reg32_t *)(SMC_BASE + SMC_MODE6_OFF)))
#define SMC_SETUP7     (*((reg32_t *)(SMC_BASE + SMC_SETUP7_OFF)))
#define SMC_PULSE7     (*((reg32_t *)(SMC_BASE + SMC_PULSE7_OFF)))
#define SMC_CYCLE7     (*((reg32_t *)(SMC_BASE + SMC_CYCLE7_OFF)))
#define SMC_TIMINGS7   (*((reg32_t *)(SMC_BASE + SMC_TIMINGS7_OFF)))
#define SMC_MODE7      (*((reg32_t *)(SMC_BASE + SMC_MODE7_OFF)))
#define SMC_OCMS       (*((reg32_t *)(SMC_BASE + SMC_OCMS_OFF)))
#define SMC_KEY1       (*((reg32_t *)(SMC_BASE + SMC_KEY1_OFF)))
#define SMC_KEY2       (*((reg32_t *)(SMC_BASE + SMC_KEY2_OFF)))
#define SMC_WPCR       (*((reg32_t *)(SMC_BASE + SMC_WPCR_OFF)))
#define SMC_WPSR       (*((reg32_t *)(SMC_BASE + SMC_WPSR_OFF)))
/*\}*/

/**
 * NFC control addresses.
 */
/*\{*/
#define NFC_SRAM_BASE_ADDR  0x20100000  ///< Base address of NFC SRAM
#define NFC_CMD_BASE_ADDR   0x60000000  ///< Base address for NFC Address Command
/*\}*/

/**
 * NFC address command values.
 */
/*\{*/
#define NFC_CMD_CMD1           (0xFF <<  2)  ///< Command Register Value for Cycle 1
#define NFC_CMD_CMD2           (0xFF << 10)  ///< Command Register Value for Cycle 2
#define NFC_CMD_VCMD2          BV(18)        ///< Valid Cycle 2 Command
#define NFC_CMD_ACYCLE_SHIFT   19
#define NFC_CMD_ACYCLE_MASK    (0x7 << 19)   ///< Number of Address required for the current command
#define NFC_CMD_ACYCLE_NONE    (0x0 << 19)   ///< No address cycle
#define NFC_CMD_ACYCLE_ONE     (0x1 << 19)   ///< One address cycle
#define NFC_CMD_ACYCLE_TWO     (0x2 << 19)   ///< Two address cycles
#define NFC_CMD_ACYCLE_THREE   (0x3 << 19)   ///< Three address cycles
#define NFC_CMD_ACYCLE_FOUR    (0x4 << 19)   ///< Four address cycles
#define NFC_CMD_ACYCLE_FIVE    (0x5 << 19)   ///< Five address cycles
#define NFC_CMD_CSID_SHIFT     22            ///< Chip Select shift
#define NFC_CMD_CSID_MASK      (0x7 << NFC_CMD_CSID_SHIFT)   ///< Chip Select mask
#define NFC_CMD_NFCEN          BV(25)        ///< NFC Enable
#define NFC_CMD_NFCWR          BV(26)        ///< NFC Write Enable
#define NFC_CMD_NFCCMD         BV(27)        ///< NFC Command Enable
/*\}*/


/**
 * Defines for bit fields in SMC_CFG register.
 */
/*\{*/
#define SMC_CFG_PAGESIZE_SHIFT       0
#define SMC_CFG_PAGESIZE_MASK        (0x3 << SMC_CFG_PAGESIZE_SHIFT)
#define SMC_CFG_PAGESIZE_PS512_16    (0x0 << 0)
#define SMC_CFG_PAGESIZE_PS1024_32   (0x1 << 0)
#define SMC_CFG_PAGESIZE_PS2048_64   (0x2 << 0)
#define SMC_CFG_PAGESIZE_PS4096_128  (0x3 << 0)
#define SMC_CFG_WSPARE               (0x1 << 8)
#define SMC_CFG_RSPARE               (0x1 << 9)
#define SMC_CFG_EDGECTRL             (0x1 << 12)
#define SMC_CFG_RBEDGE               (0x1 << 13)
#define SMC_CFG_DTOCYC_SHIFT         16
#define SMC_CFG_DTOCYC_MASK          (0xf << SMC_CFG_DTOCYC_SHIFT)
#define SMC_CFG_DTOCYC(value)        (SMC_CFG_DTOCYC_MASK & ((value) << SMC_CFG_DTOCYC_SHIFT))
#define SMC_CFG_DTOMUL_SHIFT         20
#define SMC_CFG_DTOMUL_MASK          (0x7 << SMC_CFG_DTOMUL_SHIFT)
#define SMC_CFG_DTOMUL_X1            (0x0 << 20)
#define SMC_CFG_DTOMUL_X16           (0x1 << 20)
#define SMC_CFG_DTOMUL_X128          (0x2 << 20)
#define SMC_CFG_DTOMUL_X256          (0x3 << 20)
#define SMC_CFG_DTOMUL_X1024         (0x4 << 20)
#define SMC_CFG_DTOMUL_X4096         (0x5 << 20)
#define SMC_CFG_DTOMUL_X65536        (0x6 << 20)
#define SMC_CFG_DTOMUL_X1048576      (0x7 << 20)
/*\}*/

/**
 * Defines for bit fields in SMC_CTRL register.
 */
/*\{*/
#define SMC_CTRL_NFCEN   BV(0)
#define SMC_CTRL_NFCDIS  BV(1)
/*\}*/

/**
 * Defines for bit fields in SMC_SR register.
 */
/*\{*/
#define SMC_SR_SMCSTS        BV(0)
#define SMC_SR_RB_RISE       BV(4)
#define SMC_SR_RB_FALL       BV(5)
#define SMC_SR_NFCBUSY       BV(8)
#define SMC_SR_NFCWR         BV(11)
#define SMC_SR_NFCSID_SHIFT  12
#define SMC_SR_NFCSID_MASK   (0x7 << SMC_SR_NFCSID_SHIFT)
#define SMC_SR_XFRDONE       BV(16)
#define SMC_SR_CMDDONE       BV(17)
#define SMC_SR_DTOE          BV(20)
#define SMC_SR_UNDEF         BV(21)
#define SMC_SR_AWB           BV(22)
#define SMC_SR_NFCASE        BV(23)
#define SMC_SR_RB_EDGE0      BV(24)
/*\}*/

/**
 * Defines for bit fields in SMC_ECC_CTRL register
 */
/*\{*/
#define SMC_ECC_CTRL_RST    BV(0)
#define SMC_ECC_CTRL_SWRST  BV(1)
/*\}*/

/**
 * Defines for bit fields in SMC_ECC_MD register
 */
/*\{*/
#define SMC_ECC_MD_ECC_PAGESIZE_SHIFT        0
#define SMC_ECC_MD_ECC_PAGESIZE_MASK         0x3
#define SMC_ECC_MD_ECC_PAGESIZE_PS512_16     0x0
#define SMC_ECC_MD_ECC_PAGESIZE_PS1024_32    0x1
#define SMC_ECC_MD_ECC_PAGESIZE_PS2048_64    0x2
#define SMC_ECC_MD_ECC_PAGESIZE_PS4096_128   0x3
#define SMC_ECC_MD_TYPCORREC_SHIFT           4
#define SMC_ECC_MD_TYPCORREC_MASK            (0x3 << SMC_ECC_MD_TYPCORREC_SHIFT)
#define SMC_ECC_MD_TYPCORREC_CPAGE           (0x0 << SMC_ECC_MD_TYPCORREC_SHIFT)
#define SMC_ECC_MD_TYPCORREC_C256B           (0x1 << SMC_ECC_MD_TYPCORREC_SHIFT)
#define SMC_ECC_MD_TYPCORREC_C512B           (0x2 << SMC_ECC_MD_TYPCORREC_SHIFT)
/*\}*/

/**
 * Defines for bit fields in SMC_SETUP registers.
 */
/*\{*/
#define SMC_SETUP_NWE_SETUP_MASK      0x3f
#define SMC_SETUP_NWE_SETUP(x)        (SMC_SETUP_NWE_SETUP_MASK & (x))
#define SMC_SETUP_NCS_WR_SETUP_SHIFT  8
#define SMC_SETUP_NCS_WR_SETUP_MASK   (0x3f << SMC_SETUP_NCS_WR_SETUP_SHIFT)
#define SMC_SETUP_NCS_WR_SETUP(x)     (SMC_SETUP_NCS_WR_SETUP_MASK & ((x) << SMC_SETUP_NCS_WR_SETUP_SHIFT))
#define SMC_SETUP_NRD_SETUP_SHIFT     16
#define SMC_SETUP_NRD_SETUP_MASK      (0x3f << SMC_SETUP_NRD_SETUP_SHIFT)
#define SMC_SETUP_NRD_SETUP(x)        (SMC_SETUP_NRD_SETUP_MASK & ((x) << SMC_SETUP_NRD_SETUP_SHIFT))
#define SMC_SETUP_NCS_RD_SETUP_SHIFT  24
#define SMC_SETUP_NCS_RD_SETUP_MASK   (0x3f << SMC_SETUP_NCS_RD_SETUP_SHIFT)
#define SMC_SETUP_NCS_RD_SETUP(x)     (SMC_SETUP_NCS_RD_SETUP_MASK & ((x) << SMC_SETUP_NCS_RD_SETUP_SHIFT))
/*\}*/

/**
 * Defines for bit fields in SMC_PULSE registers.
 */
/*\{*/
#define SMC_PULSE_NWE_PULSE_MASK      0x3f
#define SMC_PULSE_NWE_PULSE(x)        (SMC_PULSE_NWE_PULSE_MASK & (x))
#define SMC_PULSE_NCS_WR_PULSE_SHIFT  8
#define SMC_PULSE_NCS_WR_PULSE_MASK   (0x3f << SMC_PULSE_NCS_WR_PULSE_SHIFT)
#define SMC_PULSE_NCS_WR_PULSE(x)     (SMC_PULSE_NCS_WR_PULSE_MASK & ((x) << SMC_PULSE_NCS_WR_PULSE_SHIFT))
#define SMC_PULSE_NRD_PULSE_SHIFT     16
#define SMC_PULSE_NRD_PULSE_MASK      (0x3f << SMC_PULSE_NRD_PULSE_SHIFT)
#define SMC_PULSE_NRD_PULSE(x)        (SMC_PULSE_NRD_PULSE_MASK & ((x) << SMC_PULSE_NRD_PULSE_SHIFT))
#define SMC_PULSE_NCS_RD_PULSE_SHIFT  24
#define SMC_PULSE_NCS_RD_PULSE_MASK   (0x3f << SMC_PULSE_NCS_RD_PULSE_SHIFT)
#define SMC_PULSE_NCS_RD_PULSE(x)     (SMC_PULSE_NCS_RD_PULSE_MASK & ((x) << SMC_PULSE_NCS_RD_PULSE_SHIFT))
/*\}*/

/**
 * Defines for bit fields in SMC_CYCLE registers.
 */
/*\{*/
#define SMC_CYCLE_NWE_CYCLE_MASK      0x1ff
#define SMC_CYCLE_NWE_CYCLE(x)        (SMC_CYCLE_NWE_CYCLE_MASK & (x))
#define SMC_CYCLE_NRD_CYCLE_SHIFT     16
#define SMC_CYCLE_NRD_CYCLE_MASK      (0x1ff << SMC_CYCLE_NRD_CYCLE_SHIFT)
#define SMC_CYCLE_NRD_CYCLE(x)        (SMC_CYCLE_NRD_CYCLE_MASK & ((x) << SMC_CYCLE_NRD_CYCLE_SHIFT))
/*\}*/

/**
 * Defines for bit fields in SMC_TIMINGS registers.
 */
/*\{*/
#define SMC_TIMINGS_TCLR_SHIFT     0
#define SMC_TIMINGS_TCLR_MASK      (0xf << SMC_TIMINGS_TCLR_SHIFT)
#define SMC_TIMINGS_TCLR(value)    (SMC_TIMINGS_TCLR_MASK & ((value) << SMC_TIMINGS_TCLR_SHIFT))
#define SMC_TIMINGS_TADL_SHIFT     4
#define SMC_TIMINGS_TADL_MASK      (0xf << SMC_TIMINGS_TADL_SHIFT)
#define SMC_TIMINGS_TADL(value)    (SMC_TIMINGS_TADL_MASK & ((value) << SMC_TIMINGS_TADL_SHIFT))
#define SMC_TIMINGS_TAR_SHIFT      8
#define SMC_TIMINGS_TAR_MASK       (0xf << SMC_TIMINGS_TAR_SHIFT)
#define SMC_TIMINGS_TAR(value)     (SMC_TIMINGS_TAR_MASK & ((value) << SMC_TIMINGS_TAR_SHIFT))
#define SMC_TIMINGS_OCMS           BV(12)
#define SMC_TIMINGS_TRR_SHIFT      16
#define SMC_TIMINGS_TRR_MASK       (0xf << SMC_TIMINGS_TRR_SHIFT)
#define SMC_TIMINGS_TRR(value)     (SMC_TIMINGS_TRR_MASK & ((value) << SMC_TIMINGS_TRR_SHIFT))
#define SMC_TIMINGS_TWB_SHIFT      24
#define SMC_TIMINGS_TWB_MASK       (0xf << SMC_TIMINGS_TWB_SHIFT)
#define SMC_TIMINGS_TWB(value)     (SMC_TIMINGS_TWB_MASK & ((value) << SMC_TIMINGS_TWB_SHIFT))
#define SMC_TIMINGS_RBNSEL_SHIFT   28
#define SMC_TIMINGS_RBNSEL_MASK    (0x7 << SMC_TIMINGS_RBNSEL_SHIFT)
#define SMC_TIMINGS_RBNSEL(value)  (SMC_TIMINGS_RBNSEL_MASK & ((value) << SMC_TIMINGS_RBNSEL_SHIFT))
#define SMC_TIMINGS_NFSEL          BV(31)
/*\}*/

/**
 * Defines for bit fields in SMC_MODE registers.
 */
/*\{*/
#define SMC_MODE_READ_MODE            BV(0)
#define SMC_MODE_WRITE_MODE           BV(1)
#define SMC_MODE_EXNW_MODE_SHIFT      4
#define SMC_MODE_EXNW_MODE_MASK       (0x3 << SMC_MODE_EXNW_MODE_SHIFT)
#define   SMC_MODE_EXNW_MODE_DISABLED (0x0 << SNC_MODE_EXNW_MODE_SHIFT)
#define   SMC_MODE_EXNW_MODE_FROZEN   (0x2 << SNC_MODE_EXNW_MODE_SHIFT)
#define   SMC_MODE_EXNW_MODE_READY    (0x3 << SNC_MODE_EXNW_MODE_SHIFT)
#define SMC_MODE_BAT                  BV(8)
#define SMC_MODE_DBW                  BV(12)
#define SMC_MODE_TDF_CYCLES_SHIFT     16
#define SMC_MODE_TDF_CYCLES_MASK      (0xf << SMC_MODE_TDF_CYCLES_SHIFT)
#define SMC_MODE_TDF_CYCLES(x)        (SMC_MODE_TDF_CYCLES_MASK & ((x) << SMC_MODE_TDF_CYCLES_SHIFT))
#define SMC_MODE_TDF_MODE             BV(20)
/*\}*/

#endif /* CPU_CM3_SAM3X || CPU_CM3_SAM3U */

#endif /* SAM3_SMC_H */
