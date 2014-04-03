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
 * \brief LM3S1968 PWM hardware definitions.
 */

#ifndef L3MS_PWM_H
#define L3MS_PWM_H

/**
 * The following are defines for the PWM register offsets.
 */
#define PWM_O_CTL               (*((reg32_t *)(PWMC_BASE + 0x00000000)))  //< PWM Master Control
#define PWM_O_SYNC              (*((reg32_t *)(PWMC_BASE + 0x00000004)))  //< PWM Time Base Sync
#define PWM_O_ENABLE            (*((reg32_t *)(PWMC_BASE + 0x00000008)))  //< PWM Output Enable
#define PWM_O_INVERT            (*((reg32_t *)(PWMC_BASE + 0x0000000C)))  //< PWM Output Inversion
#define PWM_O_FAULT             (*((reg32_t *)(PWMC_BASE + 0x00000010)))  //< PWM Output Fault
#define PWM_O_INTEN             (*((reg32_t *)(PWMC_BASE + 0x00000014)))  //< PWM Interrupt Enable
#define PWM_O_RIS               (*((reg32_t *)(PWMC_BASE + 0x00000018)))  //< PWM Raw Interrupt Status
#define PWM_O_ISC               (*((reg32_t *)(PWMC_BASE + 0x0000001C)))  //< PWM Interrupt Status and Clear
#define PWM_O_STATUS            (*((reg32_t *)(PWMC_BASE + 0x00000020)))  //< PWM Status
#define PWM_O_FAULTVAL          (*((reg32_t *)(PWMC_BASE + 0x00000024)))  //< PWM Fault Condition Value
#define PWM_O_ENUPD             (*((reg32_t *)(PWMC_BASE + 0x00000028)))  //< PWM Enable Update
#define PWM_O_0_CTL             (*((reg32_t *)(PWMC_BASE + 0x00000040)))  //< PWM0 Control
#define PWM_O_0_INTEN           (*((reg32_t *)(PWMC_BASE + 0x00000044)))  //< PWM0 Interrupt and Trigger Enable
#define PWM_O_0_RIS             (*((reg32_t *)(PWMC_BASE + 0x00000048)))  //< PWM0 Raw Interrupt Status
#define PWM_O_0_ISC             (*((reg32_t *)(PWMC_BASE + 0x0000004C)))  //< PWM0 Interrupt Status and Clear
#define PWM_O_0_LOAD            (*((reg32_t *)(PWMC_BASE + 0x00000050)))  //< PWM0 Load
#define PWM_O_0_COUNT           (*((reg32_t *)(PWMC_BASE + 0x00000054)))  //< PWM0 Counter
#define PWM_O_0_CMPA            (*((reg32_t *)(PWMC_BASE + 0x00000058)))  //< PWM0 Compare A
#define PWM_O_0_CMPB            (*((reg32_t *)(PWMC_BASE + 0x0000005C)))  //< PWM0 Compare B
#define PWM_O_0_GENA            (*((reg32_t *)(PWMC_BASE + 0x00000060)))  //< PWM0 Generator A Control
#define PWM_O_0_GENB            (*((reg32_t *)(PWMC_BASE + 0x00000064)))  //< PWM0 Generator B Control
#define PWM_O_0_DBCTL           (*((reg32_t *)(PWMC_BASE + 0x00000068)))  //< PWM0 Dead-Band Control
#define PWM_O_0_DBRISE          (*((reg32_t *)(PWMC_BASE + 0x0000006C)))  //< PWM0 Dead-Band Rising-Edge Delay
#define PWM_O_0_DBFALL          (*((reg32_t *)(PWMC_BASE + 0x00000070)))  //< PWM0 Dead-Band Falling-Edge-Delay
#define PWM_O_0_FLTSRC0         (*((reg32_t *)(PWMC_BASE + 0x00000074)))  //< PWM0 Fault Source 0
#define PWM_O_0_FLTSRC1         (*((reg32_t *)(PWMC_BASE + 0x00000078)))  //< PWM0 Fault Source 1
#define PWM_O_0_MINFLTPER       (*((reg32_t *)(PWMC_BASE + 0x0000007C)))  //< PWM0 Minimum Fault Period
#define PWM_O_1_CTL             (*((reg32_t *)(PWMC_BASE + 0x00000080)))  //< PWM1 Control
#define PWM_O_1_INTEN           (*((reg32_t *)(PWMC_BASE + 0x00000084)))  //< PWM1 Interrupt and Trigger Enable
#define PWM_O_1_RIS             (*((reg32_t *)(PWMC_BASE + 0x00000088)))  //< PWM1 Raw Interrupt Status
#define PWM_O_1_ISC             (*((reg32_t *)(PWMC_BASE + 0x0000008C)))  //< PWM1 Interrupt Status and Clear
#define PWM_O_1_LOAD            (*((reg32_t *)(PWMC_BASE + 0x00000090)))  //< PWM1 Load
#define PWM_O_1_COUNT           (*((reg32_t *)(PWMC_BASE + 0x00000094)))  //< PWM1 Counter
#define PWM_O_1_CMPA            (*((reg32_t *)(PWMC_BASE + 0x00000098)))  //< PWM1 Compare A
#define PWM_O_1_CMPB            (*((reg32_t *)(PWMC_BASE + 0x0000009C)))  //< PWM1 Compare B
#define PWM_O_1_GENA            (*((reg32_t *)(PWMC_BASE + 0x000000A0)))  //< PWM1 Generator A Control
#define PWM_O_1_GENB            (*((reg32_t *)(PWMC_BASE + 0x000000A4)))  //< PWM1 Generator B Control
#define PWM_O_1_DBCTL           (*((reg32_t *)(PWMC_BASE + 0x000000A8)))  //< PWM1 Dead-Band Control
#define PWM_O_1_DBRISE          (*((reg32_t *)(PWMC_BASE + 0x000000AC)))  //< PWM1 Dead-Band Rising-Edge Delay
#define PWM_O_1_DBFALL          (*((reg32_t *)(PWMC_BASE + 0x000000B0)))  //< PWM1 Dead-Band Falling-Edge-Delay
#define PWM_O_1_FLTSRC0         (*((reg32_t *)(PWMC_BASE + 0x000000B4)))  //< PWM1 Fault Source 0
#define PWM_O_1_FLTSRC1         (*((reg32_t *)(PWMC_BASE + 0x000000B8)))  //< PWM1 Fault Source 1
#define PWM_O_1_MINFLTPER       (*((reg32_t *)(PWMC_BASE + 0x000000BC)))  //< PWM1 Minimum Fault Period
#define PWM_O_2_CTL             (*((reg32_t *)(PWMC_BASE + 0x000000C0)))  //< PWM2 Control
#define PWM_O_2_INTEN           (*((reg32_t *)(PWMC_BASE + 0x000000C4)))  //< PWM2 Interrupt and Trigger Enable
#define PWM_O_2_RIS             (*((reg32_t *)(PWMC_BASE + 0x000000C8)))  //< PWM2 Raw Interrupt Status
#define PWM_O_2_ISC             (*((reg32_t *)(PWMC_BASE + 0x000000CC)))  //< PWM2 Interrupt Status and Clear
#define PWM_O_2_LOAD            (*((reg32_t *)(PWMC_BASE + 0x000000D0)))  //< PWM2 Load
#define PWM_O_2_COUNT           (*((reg32_t *)(PWMC_BASE + 0x000000D4)))  //< PWM2 Counter
#define PWM_O_2_CMPA            (*((reg32_t *)(PWMC_BASE + 0x000000D8)))  //< PWM2 Compare A
#define PWM_O_2_CMPB            (*((reg32_t *)(PWMC_BASE + 0x000000DC)))  //< PWM2 Compare B
#define PWM_O_2_GENA            (*((reg32_t *)(PWMC_BASE + 0x000000E0)))  //< PWM2 Generator A Control
#define PWM_O_2_GENB            (*((reg32_t *)(PWMC_BASE + 0x000000E4)))  //< PWM2 Generator B Control
#define PWM_O_2_DBCTL           (*((reg32_t *)(PWMC_BASE + 0x000000E8)))  //< PWM2 Dead-Band Control
#define PWM_O_2_DBRISE          (*((reg32_t *)(PWMC_BASE + 0x000000EC)))  //< PWM2 Dead-Band Rising-Edge Delay
#define PWM_O_2_DBFALL          (*((reg32_t *)(PWMC_BASE + 0x000000F0)))  //< PWM2 Dead-Band Falling-Edge-Delay
#define PWM_O_2_FLTSRC0         (*((reg32_t *)(PWMC_BASE + 0x000000F4)))  //< PWM2 Fault Source 0
#define PWM_O_2_FLTSRC1         (*((reg32_t *)(PWMC_BASE + 0x000000F8)))  //< PWM2 Fault Source 1
#define PWM_O_2_MINFLTPER       (*((reg32_t *)(PWMC_BASE + 0x000000FC)))  //< PWM2 Minimum Fault Period
#define PWM_O_3_CTL             (*((reg32_t *)(PWMC_BASE + 0x00000100)))  //< PWM3 Control
#define PWM_O_3_INTEN           (*((reg32_t *)(PWMC_BASE + 0x00000104)))  //< PWM3 Interrupt and Trigger Enable
#define PWM_O_3_RIS             (*((reg32_t *)(PWMC_BASE + 0x00000108)))  //< PWM3 Raw Interrupt Status
#define PWM_O_3_ISC             (*((reg32_t *)(PWMC_BASE + 0x0000010C)))  //< PWM3 Interrupt Status and Clear
#define PWM_O_3_LOAD            (*((reg32_t *)(PWMC_BASE + 0x00000110)))  //< PWM3 Load
#define PWM_O_3_COUNT           (*((reg32_t *)(PWMC_BASE + 0x00000114)))  //< PWM3 Counter
#define PWM_O_3_CMPA            (*((reg32_t *)(PWMC_BASE + 0x00000118)))  //< PWM3 Compare A
#define PWM_O_3_CMPB            (*((reg32_t *)(PWMC_BASE + 0x0000011C)))  //< PWM3 Compare B
#define PWM_O_3_GENA            (*((reg32_t *)(PWMC_BASE + 0x00000120)))  //< PWM3 Generator A Control
#define PWM_O_3_GENB            (*((reg32_t *)(PWMC_BASE + 0x00000124)))  //< PWM3 Generator B Control
#define PWM_O_3_DBCTL           (*((reg32_t *)(PWMC_BASE + 0x00000128)))  //< PWM3 Dead-Band Control
#define PWM_O_3_DBRISE          (*((reg32_t *)(PWMC_BASE + 0x0000012C)))  //< PWM3 Dead-Band Rising-Edge Delay
#define PWM_O_3_DBFALL          (*((reg32_t *)(PWMC_BASE + 0x00000130)))  //< PWM3 Dead-Band Falling-Edge-Delay
#define PWM_O_3_FLTSRC0         (*((reg32_t *)(PWMC_BASE + 0x00000134)))  //< PWM3 Fault Source 0
#define PWM_O_3_FLTSRC1         (*((reg32_t *)(PWMC_BASE + 0x00000138)))  //< PWM3 Fault Source 1
#define PWM_O_3_MINFLTPER       (*((reg32_t *)(PWMC_BASE + 0x0000013C)))  //< PWM3 Minimum Fault Period
#define PWM_O_0_FLTSEN          (*((reg32_t *)(PWMC_BASE + 0x00000800)))  //< PWM0 Fault Pin Logic Sense
#define PWM_O_0_FLTSTAT0        (*((reg32_t *)(PWMC_BASE + 0x00000804)))  //< PWM0 Fault Status 0
#define PWM_O_0_FLTSTAT1        (*((reg32_t *)(PWMC_BASE + 0x00000808)))  //< PWM0 Fault Status 1
#define PWM_O_1_FLTSEN          (*((reg32_t *)(PWMC_BASE + 0x00000880)))  //< PWM1 Fault Pin Logic Sense
#define PWM_O_1_FLTSTAT0        (*((reg32_t *)(PWMC_BASE + 0x00000884)))  //< PWM1 Fault Status 0
#define PWM_O_1_FLTSTAT1        (*((reg32_t *)(PWMC_BASE + 0x00000888)))  //< PWM1 Fault Status 1
#define PWM_O_2_FLTSEN          (*((reg32_t *)(PWMC_BASE + 0x00000900)))  //< PWM2 Fault Pin Logic Sense
#define PWM_O_2_FLTSTAT0        (*((reg32_t *)(PWMC_BASE + 0x00000904)))  //< PWM2 Fault Status 0
#define PWM_O_2_FLTSTAT1        (*((reg32_t *)(PWMC_BASE + 0x00000908)))  //< PWM2 Fault Status 1
#define PWM_O_3_FLTSEN          (*((reg32_t *)(PWMC_BASE + 0x00000980)))  //< PWM3 Fault Pin Logic Sense
#define PWM_O_3_FLTSTAT0        (*((reg32_t *)(PWMC_BASE + 0x00000984)))  //< PWM3 Fault Status 0
#define PWM_O_3_FLTSTAT1        (*((reg32_t *)(PWMC_BASE + 0x00000988)))  //< PWM3 Fault Status 1

/**
 * Defines for the bit fields in the PWM_O_CTL register.
 */
#define PWM_CTL_GLOBALSYNC3              3  //< Update PWM Generator 3
#define PWM_CTL_GLOBALSYNC2              2  //< Update PWM Generator 2
#define PWM_CTL_GLOBALSYNC1              1  //< Update PWM Generator 1
#define PWM_CTL_GLOBALSYNC0              0  //< Update PWM Generator 0

/**
 * Defines for the bit fields in the PWM_O_SYNC register.
 */
#define PWM_SYNC_SYNC3                   3  //< Reset Generator 3 Counter
#define PWM_SYNC_SYNC2                   2  //< Reset Generator 2 Counter
#define PWM_SYNC_SYNC1                   1  //< Reset Generator 1 Counter
#define PWM_SYNC_SYNC0                   0  //< Reset Generator 0 Counter

/**
 * Defines for the bit fields in the PWM_O_ENABLE register.
 */
#define PWM_ENABLE_PWM7EN                7  //< PWM7 Output Enable
#define PWM_ENABLE_PWM6EN                6  //< PWM6 Output Enable
#define PWM_ENABLE_PWM5EN                5  //< PWM5 Output Enable
#define PWM_ENABLE_PWM4EN                4  //< PWM4 Output Enable
#define PWM_ENABLE_PWM3EN                3  //< PWM3 Output Enable
#define PWM_ENABLE_PWM2EN                2  //< PWM2 Output Enable
#define PWM_ENABLE_PWM1EN                1  //< PWM1 Output Enable
#define PWM_ENABLE_PWM0EN                0  //< PWM0 Output Enable

/**
 * Defines for the bit fields in the PWM_O_INVERT register.
 */
#define PWM_INVERT_PWM7INV               7  //< Invert PWM7 Signal
#define PWM_INVERT_PWM6INV               6  //< Invert PWM6 Signal
#define PWM_INVERT_PWM5INV               5  //< Invert PWM5 Signal
#define PWM_INVERT_PWM4INV               4  //< Invert PWM4 Signal
#define PWM_INVERT_PWM3INV               3  //< Invert PWM3 Signal
#define PWM_INVERT_PWM2INV               2  //< Invert PWM2 Signal
#define PWM_INVERT_PWM1INV               1  //< Invert PWM1 Signal
#define PWM_INVERT_PWM0INV               0  //< Invert PWM0 Signal

/**
 * Defines for the bit fields in the PWM_O_FAULT register.
 */
#define PWM_FAULT_FAULT7                 7  //< PWM7 Fault
#define PWM_FAULT_FAULT6                 6  //< PWM6 Fault
#define PWM_FAULT_FAULT5                 5  //< PWM5 Fault
#define PWM_FAULT_FAULT4                 4  //< PWM4 Fault
#define PWM_FAULT_FAULT3                 3  //< PWM3 Fault
#define PWM_FAULT_FAULT2                 2  //< PWM2 Fault
#define PWM_FAULT_FAULT1                 1  //< PWM1 Fault
#define PWM_FAULT_FAULT0                 0  //< PWM0 Fault

/**
 * Defines for the bit fields in the PWM_O_INTEN register.
 */
#define PWM_INTEN_INTFAULT3             19  //< Interrupt Fault 3
#define PWM_INTEN_INTFAULT2             18  //< Interrupt Fault 2
#define PWM_INTEN_INTFAULT1             17  //< Interrupt Fault 1
#define PWM_INTEN_INTFAULT              16  //< Fault Interrupt Enable
#define PWM_INTEN_INTFAULT0             16  //< Interrupt Fault 0
#define PWM_INTEN_INTPWM3                3  //< PWM3 Interrupt Enable
#define PWM_INTEN_INTPWM2                2  //< PWM2 Interrupt Enable
#define PWM_INTEN_INTPWM1                1  //< PWM1 Interrupt Enable
#define PWM_INTEN_INTPWM0                0  //< PWM0 Interrupt Enable

/**
 * Defines for the bit fields in the PWM_O_RIS register.
 */
#define PWM_RIS_INTFAULT3               19  //< Interrupt Fault PWM 3
#define PWM_RIS_INTFAULT2               18  //< Interrupt Fault PWM 2
#define PWM_RIS_INTFAULT1               17  //< Interrupt Fault PWM 1
#define PWM_RIS_INTFAULT0               16  //< Interrupt Fault PWM 0
#define PWM_RIS_INTFAULT                16  //< Fault Interrupt Asserted
#define PWM_RIS_INTPWM3                  3  //< PWM3 Interrupt Asserted
#define PWM_RIS_INTPWM2                  2  //< PWM2 Interrupt Asserted
#define PWM_RIS_INTPWM1                  1  //< PWM1 Interrupt Asserted
#define PWM_RIS_INTPWM0                  0  //< PWM0 Interrupt Asserted

/**
 * Defines for the bit fields in the PWM_O_ISC register.
 */
#define PWM_ISC_INTFAULT3               19   //< FAULT3 Interrupt Asserted
#define PWM_ISC_INTFAULT2               18  //< FAULT2 Interrupt Asserted
#define PWM_ISC_INTFAULT1               17  //< FAULT1 Interrupt Asserted
#define PWM_ISC_INTFAULT                16  //< Fault Interrupt Asserted
#define PWM_ISC_INTFAULT0               16  //< FAULT0 Interrupt Asserted
#define PWM_ISC_INTPWM3                  3  //< PWM3 Interrupt Status
#define PWM_ISC_INTPWM2                  2  //< PWM2 Interrupt Status
#define PWM_ISC_INTPWM1                  1  //< PWM1 Interrupt Status
#define PWM_ISC_INTPWM0                  0  //< PWM0 Interrupt Status

/**
 * Defines for the bit fields in the PWM_O_STATUS register.
 */
#define PWM_STATUS_FAULT3                3  //< Generator 3 Fault Status
#define PWM_STATUS_FAULT2                2  //< Generator 2 Fault Status
#define PWM_STATUS_FAULT1                1  //< Generator 1 Fault Status
#define PWM_STATUS_FAULT0                0  //< Generator 0 Fault Status

/**
 * Defines for the bit fields in the PWM_O_FAULTVAL register.
 */
#define PWM_FAULTVAL_PWM7                7  //< PWM7 Fault Value
#define PWM_FAULTVAL_PWM6                6  //< PWM6 Fault Value
#define PWM_FAULTVAL_PWM5                5  //< PWM5 Fault Value
#define PWM_FAULTVAL_PWM4                4  //< PWM4 Fault Value
#define PWM_FAULTVAL_PWM3                3  //< PWM3 Fault Value
#define PWM_FAULTVAL_PWM2                2  //< PWM2 Fault Value
#define PWM_FAULTVAL_PWM1                1  //< PWM1 Fault Value
#define PWM_FAULTVAL_PWM0                0  //< PWM0 Fault Value

/**
 * Defines for the bit fields in the PWM_O_ENUPD register.
 */
#define PWM_ENUPD_ENUPD7_M      0x0000C000  //< PWM7 Enable Update Mode
#define PWM_ENUPD_ENUPD7_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD7_LSYNC  0x00008000  //< Locally Synchronized
#define PWM_ENUPD_ENUPD7_GSYNC  0x0000C000  //< Globally Synchronized
#define PWM_ENUPD_ENUPD6_M      0x00003000  //< PWM6 Enable Update Mode
#define PWM_ENUPD_ENUPD6_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD6_LSYNC  0x00002000  //< Locally Synchronized
#define PWM_ENUPD_ENUPD6_GSYNC  0x00003000  //< Globally Synchronized
#define PWM_ENUPD_ENUPD5_M      0x00000C00  //< PWM5 Enable Update Mode
#define PWM_ENUPD_ENUPD5_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD5_LSYNC  0x00000800  //< Locally Synchronized
#define PWM_ENUPD_ENUPD5_GSYNC  0x00000C00  //< Globally Synchronized
#define PWM_ENUPD_ENUPD4_M      0x00000300  //< PWM4 Enable Update Mode
#define PWM_ENUPD_ENUPD4_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD4_LSYNC  0x00000200  //< Locally Synchronized
#define PWM_ENUPD_ENUPD4_GSYNC  0x00000300  //< Globally Synchronized
#define PWM_ENUPD_ENUPD3_M      0x000000C0  //< PWM3 Enable Update Mode
#define PWM_ENUPD_ENUPD3_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD3_LSYNC  0x00000080  //< Locally Synchronized
#define PWM_ENUPD_ENUPD3_GSYNC  0x000000C0  //< Globally Synchronized
#define PWM_ENUPD_ENUPD2_M      0x00000030  //< PWM2 Enable Update Mode
#define PWM_ENUPD_ENUPD2_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD2_LSYNC  0x00000020  //< Locally Synchronized
#define PWM_ENUPD_ENUPD2_GSYNC  0x00000030  //< Globally Synchronized
#define PWM_ENUPD_ENUPD1_M      0x0000000C  //< PWM1 Enable Update Mode
#define PWM_ENUPD_ENUPD1_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD1_LSYNC  0x00000008  //< Locally Synchronized
#define PWM_ENUPD_ENUPD1_GSYNC  0x0000000C  //< Globally Synchronized
#define PWM_ENUPD_ENUPD0_M      0x00000003  //< PWM0 Enable Update Mode
#define PWM_ENUPD_ENUPD0_IMM    0x00000000  //< Immediate
#define PWM_ENUPD_ENUPD0_LSYNC  0x00000002  //< Locally Synchronized
#define PWM_ENUPD_ENUPD0_GSYNC  0x00000003  //< Globally Synchronized

/**
 * Defines for the bit fields in the PWM_O_X_CTL register.
 */
#define PWM_X_CTL_LATCH         0x00040000  //< Latch Fault Input
#define PWM_X_CTL_MINFLTPER     0x00020000  //< Minimum Fault Period
#define PWM_X_CTL_FLTSRC        0x00010000  //< Fault Condition Source
#define PWM_X_CTL_DBFALLUPD_M   0x0000C000  //< PWMnDBFALL Update Mode
#define PWM_X_CTL_DBFALLUPD_I   0x00000000  //< Immediate
#define PWM_X_CTL_DBFALLUPD_LS  0x00008000  //< Locally Synchronized
#define PWM_X_CTL_DBFALLUPD_GS  0x0000C000  //< Globally Synchronized
#define PWM_X_CTL_DBRISEUPD_M   0x00003000  //< PWMnDBRISE Update Mode
#define PWM_X_CTL_DBRISEUPD_I   0x00000000  //< Immediate
#define PWM_X_CTL_DBRISEUPD_LS  0x00002000  //< Locally Synchronized
#define PWM_X_CTL_DBRISEUPD_GS  0x00003000  //< Globally Synchronized
#define PWM_X_CTL_DBCTLUPD_M    0x00000C00  //< PWMnDBCTL Update Mode
#define PWM_X_CTL_DBCTLUPD_I    0x00000000  //< Immediate
#define PWM_X_CTL_DBCTLUPD_LS   0x00000800  //< Locally Synchronized
#define PWM_X_CTL_DBCTLUPD_GS   0x00000C00  //< Globally Synchronized
#define PWM_X_CTL_GENBUPD_M     0x00000300  //< PWMnGENB Update Mode
#define PWM_X_CTL_GENBUPD_I     0x00000000  //< Immediate
#define PWM_X_CTL_GENBUPD_LS    0x00000200  //< Locally Synchronized
#define PWM_X_CTL_GENBUPD_GS    0x00000300  //< Globally Synchronized
#define PWM_X_CTL_GENAUPD_M     0x000000C0  //< PWMnGENA Update Mode
#define PWM_X_CTL_GENAUPD_I     0x00000000  //< Immediate
#define PWM_X_CTL_GENAUPD_LS    0x00000080  //< Locally Synchronized
#define PWM_X_CTL_GENAUPD_GS    0x000000C0  //< Globally Synchronized
#define PWM_X_CTL_CMPBUPD       0x00000020  //< Comparator B Update Mode
#define PWM_X_CTL_CMPAUPD       0x00000010  //< Comparator A Update Mode
#define PWM_X_CTL_LOADUPD       0x00000008  //< Load Register Update Mode
#define PWM_X_CTL_DEBUG         0x00000004  //< Debug Mode
#define PWM_X_CTL_MODE          0x00000002  //< Counter Mode
#define PWM_X_CTL_ENABLE        0x00000001  //< PWM Block Enable

/**
 * Defines for the bit fields in the PWM_O_X_INTEN register.
 */
#define PWM_X_INTEN_TRCMPBD     0x00002000  //< Trigger for Counter=PWMnCMPB  Down
#define PWM_X_INTEN_TRCMPBU     0x00001000  //< Trigger for Counter=PWMnCMPB Up
#define PWM_X_INTEN_TRCMPAD     0x00000800  //< Trigger for Counter=PWMnCMPA Down
#define PWM_X_INTEN_TRCMPAU     0x00000400  //< Trigger for Counter=PWMnCMPA Up
#define PWM_X_INTEN_TRCNTLOAD   0x00000200  //< Trigger for Counter=PWMnLOAD
#define PWM_X_INTEN_TRCNTZERO   0x00000100  //< Trigger for Counter=0
#define PWM_X_INTEN_INTCMPBD    0x00000020  //< Interrupt for Counter=PWMnCMPB Down
#define PWM_X_INTEN_INTCMPBU    0x00000010  //< Interrupt for Counter=PWMnCMPB Up
#define PWM_X_INTEN_INTCMPAD    0x00000008  //< Interrupt for Counter=PWMnCMPA Down
#define PWM_X_INTEN_INTCMPAU    0x00000004  //< Interrupt for Counter=PWMnCMPA Up
#define PWM_X_INTEN_INTCNTLOAD  0x00000002  //< Interrupt for Counter=PWMnLOAD
#define PWM_X_INTEN_INTCNTZERO  0x00000001  //< Interrupt for Counter=0

/**
 * Defines for the bit fields in the PWM_O_X_RIS register.
 */
#define PWM_X_RIS_INTCMPBD      0x00000020  //< Comparator B Down Interrupt Status
#define PWM_X_RIS_INTCMPBU      0x00000010  //< Comparator B Up Interrupt Status
#define PWM_X_RIS_INTCMPAD      0x00000008  //< Comparator A Down Interrupt Status
#define PWM_X_RIS_INTCMPAU      0x00000004  //< Comparator A Up Interrupt Status
#define PWM_X_RIS_INTCNTLOAD    0x00000002  //< Counter=Load Interrupt Status
#define PWM_X_RIS_INTCNTZERO    0x00000001  //< Counter=0 Interrupt Status

/**
 * Defines for the bit fields in the PWM_O_X_ISC register.
 */
#define PWM_X_ISC_INTCMPBD      0x00000020  //< Comparator B Down Interrupt
#define PWM_X_ISC_INTCMPBU      0x00000010  //< Comparator B Up Interrupt
#define PWM_X_ISC_INTCMPAD      0x00000008  //< Comparator A Down Interrupt
#define PWM_X_ISC_INTCMPAU      0x00000004  //< Comparator A Up Interrupt
#define PWM_X_ISC_INTCNTLOAD    0x00000002  //< Counter=Load Interrupt
#define PWM_X_ISC_INTCNTZERO    0x00000001  //< Counter=0 Interrupt

/**
 * Defines for the bit fields in the PWM_O_X_LOAD register.
 */
#define PWM_X_LOAD_M            0x0000FFFF  //< Counter Load Value
#define PWM_X_LOAD_S            0

/**
 * Defines for the bit fields in the PWM_O_X_COUNT register.
 */
#define PWM_X_COUNT_M           0x0000FFFF  //< Counter Value
#define PWM_X_COUNT_S           0

/**
 * Defines for the bit fields in the PWM_O_X_CMPA register.
 */
#define PWM_X_CMPA_M            0x0000FFFF  //< Comparator A Value
#define PWM_X_CMPA_S            0

/**
 * Defines for the bit fields in the PWM_O_X_CMPB register.
 */
#define PWM_X_CMPB_M            0x0000FFFF  //< Comparator B Value
#define PWM_X_CMPB_S            0

/**
 * Defines for the bit fields in the PWM_O_X_GENA register.
 */
#define PWM_X_GENA_ACTCMPBD_M    0x00000C00  //< Action for Comparator B Down
#define PWM_X_GENA_ACTCMPBD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTCMPBD_INV  0x00000400  //< Invert pwmA
#define PWM_X_GENA_ACTCMPBD_ZERO 0x00000800  //< Drive pwmA Low
#define PWM_X_GENA_ACTCMPBD_ONE  0x00000C00  //< Drive pwmA High
#define PWM_X_GENA_ACTCMPBU_M    0x00000300  //< Action for Comparator B Up
#define PWM_X_GENA_ACTCMPBU_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTCMPBU_INV  0x00000100  //< Invert pwmA
#define PWM_X_GENA_ACTCMPBU_ZERO 0x00000200  //< Drive pwmA Low
#define PWM_X_GENA_ACTCMPBU_ONE  0x00000300  //< Drive pwmA High
#define PWM_X_GENA_ACTCMPAD_M    0x000000C0  //< Action for Comparator A Down
#define PWM_X_GENA_ACTCMPAD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTCMPAD_INV  0x00000040  //< Invert pwmA
#define PWM_X_GENA_ACTCMPAD_ZERO 0x00000080  //< Drive pwmA Low
#define PWM_X_GENA_ACTCMPAD_ONE  0x000000C0  //< Drive pwmA High
#define PWM_X_GENA_ACTCMPAU_M    0x00000030  //< Action for Comparator A Up
#define PWM_X_GENA_ACTCMPAU_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTCMPAU_INV  0x00000010  //< Invert pwmA
#define PWM_X_GENA_ACTCMPAU_ZERO 0x00000020  //< Drive pwmA Low
#define PWM_X_GENA_ACTCMPAU_ONE 0x00000030  //< Drive pwmA High
#define PWM_X_GENA_ACTLOAD_M    0x0000000C  //< Action for Counter=LOAD
#define PWM_X_GENA_ACTLOAD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTLOAD_INV  0x00000004  //< Invert pwmA
#define PWM_X_GENA_ACTLOAD_ZERO 0x00000008  //< Drive pwmA Low
#define PWM_X_GENA_ACTLOAD_ONE  0x0000000C  //< Drive pwmA High
#define PWM_X_GENA_ACTZERO_M    0x00000003  //< Action for Counter=0
#define PWM_X_GENA_ACTZERO_NONE 0x00000000  //< Do nothing
#define PWM_X_GENA_ACTZERO_INV  0x00000001  //< Invert pwmA
#define PWM_X_GENA_ACTZERO_ZERO 0x00000002  //< Drive pwmA Low
#define PWM_X_GENA_ACTZERO_ONE  0x00000003  //< Drive pwmA High

/**
 * Defines for the bit fields in the PWM_O_X_GENB register.
 */
#define PWM_X_GENB_ACTCMPBD_M   0x00000C00  //< Action for Comparator B Down
#define PWM_X_GENB_ACTCMPBD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTCMPBD_INV 0x00000400  //< Invert pwmB
#define PWM_X_GENB_ACTCMPBD_ZERO 0x00000800  //< Drive pwmB Low
#define PWM_X_GENB_ACTCMPBD_ONE 0x00000C00  //< Drive pwmB High
#define PWM_X_GENB_ACTCMPBU_M   0x00000300  //< Action for Comparator B Up
#define PWM_X_GENB_ACTCMPBU_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTCMPBU_INV 0x00000100  //< Invert pwmB
#define PWM_X_GENB_ACTCMPBU_ZERO 0x00000200  //< Drive pwmB Low
#define PWM_X_GENB_ACTCMPBU_ONE 0x00000300  //< Drive pwmB High
#define PWM_X_GENB_ACTCMPAD_M   0x000000C0  //< Action for Comparator A Down
#define PWM_X_GENB_ACTCMPAD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTCMPAD_INV 0x00000040  //< Invert pwmB
#define PWM_X_GENB_ACTCMPAD_ZERO 0x00000080  //< Drive pwmB Low
#define PWM_X_GENB_ACTCMPAD_ONE 0x000000C0  //< Drive pwmB High
#define PWM_X_GENB_ACTCMPAU_M   0x00000030  //< Action for Comparator A Up
#define PWM_X_GENB_ACTCMPAU_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTCMPAU_INV 0x00000010  //< Invert pwmB
#define PWM_X_GENB_ACTCMPAU_ZERO 0x00000020  //< Drive pwmB Low
#define PWM_X_GENB_ACTCMPAU_ONE 0x00000030  //< Drive pwmB High
#define PWM_X_GENB_ACTLOAD_M    0x0000000C  //< Action for Counter=LOAD
#define PWM_X_GENB_ACTLOAD_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTLOAD_INV  0x00000004  //< Invert pwmB
#define PWM_X_GENB_ACTLOAD_ZERO 0x00000008  //< Drive pwmB Low
#define PWM_X_GENB_ACTLOAD_ONE  0x0000000C  //< Drive pwmB High
#define PWM_X_GENB_ACTZERO_M    0x00000003  //< Action for Counter=0
#define PWM_X_GENB_ACTZERO_NONE 0x00000000  //< Do nothing
#define PWM_X_GENB_ACTZERO_INV  0x00000001  //< Invert pwmB
#define PWM_X_GENB_ACTZERO_ZERO 0x00000002  //< Drive pwmB Low
#define PWM_X_GENB_ACTZERO_ONE  0x00000003  //< Drive pwmB High

/**
 * Defines for the bit fields in the PWM_O_X_DBCTL register.
 */
#define PWM_X_DBCTL_ENABLE      0x00000001  //< Dead-Band Generator Enable
/**
 * Defines for the bit fields in the PWM_O_X_DBRISE register.
 */
#define PWM_X_DBRISE_DELAY_M    0x00000FFF  //< Dead-Band Rise Delay
#define PWM_X_DBRISE_DELAY_S    0

/**
 * Defines for the bit fields in the PWM_O_X_DBFALL register.
 */
#define PWM_X_DBFALL_DELAY_M    0x00000FFF  //< Dead-Band Fall Delay
#define PWM_X_DBFALL_DELAY_S    0

/**
 * Defines for the bit fields in the PWM_O_X_FLTSRC0 register.
 */
#define PWM_X_FLTSRC0_FAULT3    0x00000008  //< Fault3 Input
#define PWM_X_FLTSRC0_FAULT2    0x00000004  //< Fault2 Input
#define PWM_X_FLTSRC0_FAULT1    0x00000002  //< Fault1 Input
#define PWM_X_FLTSRC0_FAULT0    0x00000001  //< Fault0 Input

/**
 * The following are defines for the bit fields in the PWM_O_X_FLTSRC1 register.
 */
#define PWM_X_FLTSRC1_DCMP7     0x00000080  //< Digital Comparator 7
#define PWM_X_FLTSRC1_DCMP6     0x00000040  //< Digital Comparator 6
#define PWM_X_FLTSRC1_DCMP5     0x00000020  //< Digital Comparator 5
#define PWM_X_FLTSRC1_DCMP4     0x00000010  //< Digital Comparator 4
#define PWM_X_FLTSRC1_DCMP3     0x00000008  //< Digital Comparator 3
#define PWM_X_FLTSRC1_DCMP2     0x00000004  //< Digital Comparator 2
#define PWM_X_FLTSRC1_DCMP1     0x00000002  //< Digital Comparator 1
#define PWM_X_FLTSRC1_DCMP0     0x00000001  //< Digital Comparator 0

/**
 * Defines for the bit fields in the PWM_O_X_MINFLTPER register.
 */
#define PWM_X_MINFLTPER_M       0x0000FFFF  //< Minimum Fault Period
#define PWM_X_MINFLTPER_S       0

/**
 * Defines for the bit fields in the PWM_O_X_FLTSEN register.
 */
#define PWM_X_FLTSEN_FAULT3     0x00000008  //< Fault3 Sense
#define PWM_X_FLTSEN_FAULT2     0x00000004  //< Fault2 Sense
#define PWM_X_FLTSEN_FAULT1     0x00000002  //< Fault1 Sense
#define PWM_X_FLTSEN_FAULT0     0x00000001  //< Fault0 Sense

/**
 * Defines for the bit fields in the PWM_O_X_FLTSTAT0 register.
 */
#define PWM_X_FLTSTAT0_FAULT3   0x00000008  //< Fault Input 3
#define PWM_X_FLTSTAT0_FAULT2   0x00000004  //< Fault Input 2
#define PWM_X_FLTSTAT0_FAULT1   0x00000002  //< Fault Input 1
#define PWM_X_FLTSTAT0_FAULT0   0x00000001  //< Fault Input 0

/**
 * Defines for the bit fields in the PWM_O_X_FLTSTAT1 register.
 */
#define PWM_X_FLTSTAT1_DCMP7    0x00000080  //< Digital Comparator 7 Trigger
#define PWM_X_FLTSTAT1_DCMP6    0x00000040  //< Digital Comparator 6 Trigger
#define PWM_X_FLTSTAT1_DCMP5    0x00000020  //< Digital Comparator 5 Trigger
#define PWM_X_FLTSTAT1_DCMP4    0x00000010  //< Digital Comparator 4 Trigger
#define PWM_X_FLTSTAT1_DCMP3    0x00000008  //< Digital Comparator 3 Trigger
#define PWM_X_FLTSTAT1_DCMP2    0x00000004  //< Digital Comparator 2 Trigger
#define PWM_X_FLTSTAT1_DCMP1    0x00000002  //< Digital Comparator 1 Trigger
#define PWM_X_FLTSTAT1_DCMP0    0x00000001  //< Digital Comparator 0 Trigger

/**
 * Defines for the PWM Generator standard offsets.
 */
#define PWM_O_X_CTL             (*((reg32_t *)(PWMC_BASE + 0x00000000)))  //< Gen Control Reg
#define PWM_O_X_INTEN           (*((reg32_t *)(PWMC_BASE + 0x00000004)))  //< Gen Int/Trig Enable Reg
#define PWM_O_X_RIS             (*((reg32_t *)(PWMC_BASE + 0x00000008)))  //< Gen Raw Int Status Reg
#define PWM_O_X_ISC             (*((reg32_t *)(PWMC_BASE + 0x0000000C)))  //< Gen Int Status Reg
#define PWM_O_X_LOAD            (*((reg32_t *)(PWMC_BASE + 0x00000010)))  //< Gen Load Reg
#define PWM_O_X_COUNT           (*((reg32_t *)(PWMC_BASE + 0x00000014)))  //< Gen Counter Reg
#define PWM_O_X_CMPA            (*((reg32_t *)(PWMC_BASE + 0x00000018)))  //< Gen Compare A Reg
#define PWM_O_X_CMPB            (*((reg32_t *)(PWMC_BASE + 0x0000001C)))  //< Gen Compare B Reg
#define PWM_O_X_GENA            (*((reg32_t *)(PWMC_BASE + 0x00000020)))  //< Gen Generator A Ctrl Reg
#define PWM_O_X_GENB            (*((reg32_t *)(PWMC_BASE + 0x00000024)))  //< Gen Generator B Ctrl Reg
#define PWM_O_X_DBCTL           (*((reg32_t *)(PWMC_BASE + 0x00000028)))  //< Gen Dead Band Ctrl Reg
#define PWM_O_X_DBRISE          (*((reg32_t *)(PWMC_BASE + 0x0000002C)))  //< Gen DB Rising Edge Delay Reg
#define PWM_O_X_DBFALL          (*((reg32_t *)(PWMC_BASE + 0x00000030)))  //< Gen DB Falling Edge Delay Reg
#define PWM_O_X_FLTSRC0         (*((reg32_t *)(PWMC_BASE + 0x00000034)))  //< Fault pin, comparator condition
#define PWM_O_X_FLTSRC1         (*((reg32_t *)(PWMC_BASE + 0x00000038)))  //< Digital comparator condition
#define PWM_O_X_MINFLTPER       (*((reg32_t *)(PWMC_BASE + 0x0000003C)))  //< Fault minimum period extension
#define PWM_GEN_0_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000040)))  //< PWM0 base
#define PWM_GEN_1_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000080)))  //< PWM1 base
#define PWM_GEN_2_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x000000C0)))  //< PWM2 base
#define PWM_GEN_3_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000100)))  //< PWM3 base

/**
 * Defines for the PWM Generator extended offsets.
 */
#define PWM_O_X_FLTSEN          (*((reg32_t *)(PWMC_BASE + 0x00000000)))  //< Fault logic sense
#define PWM_O_X_FLTSTAT0        (*((reg32_t *)(PWMC_BASE + 0x00000004)))  //< Pin and comparator status
#define PWM_O_X_FLTSTAT1        (*((reg32_t *)(PWMC_BASE + 0x00000008)))  //< Digital comparator status
#define PWM_EXT_0_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000800)))  //< PWM0 extended base
#define PWM_EXT_1_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000880)))  //< PWM1 extended base
#define PWM_EXT_2_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000900)))  //< PWM2 extended base
#define PWM_EXT_3_OFFSET        (*((reg32_t *)(PWMC_BASE + 0x00000980)))  //< PWM3 extended base

#endif /* LM3S_PWM_H */
