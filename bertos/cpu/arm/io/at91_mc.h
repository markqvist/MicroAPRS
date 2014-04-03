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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * AT91 Memory controller.
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

#ifndef AT91_MC_H
#define AT91_MC_H

#define MC_RCR_OFF              0x00000000      ///< MC remap control register offset.
#define MC_RCR      (*((reg32_t *)(MC_BASE + MC_RCR_OFF)))      ///< MC remap control register address.
#define MC_RCB                           0      ///< Remap command.

#define MC_ASR_OFF              0x00000004      ///< MC abort status register offset.
#define MC_ASR      (*((reg32_t *)(MC_BASE + MC_ASR_OFF)))      ///< MC abort status register address.
#define MC_UNDADD                        0      ///< Undefined Addess Abort status.
#define MC_MISADD                        1      ///< Misaligned Addess Abort status.
#define MC_ABTSZ_MASK           0x00000300      ///< Abort size status mask.
#define MC_ABTSZ_BYTE           0x00000000      ///< Byte size abort.
#define MC_ABTSZ_HWORD          0x00000100      ///< Half-word size abort.
#define MC_ABTSZ_WORD           0x00000200      ///< Word size abort.
#define MC_ABTTYP_MASK          0x00000C00      ///< Abort type status mask.
#define MC_ABTTYP_DATAR         0x00000000      ///< Data read abort.
#define MC_ABTTYP_DATAW         0x00000400      ///< Data write abort.
#define MC_ABTTYP_FETCH         0x00000800      ///< Code fetch abort.
#define MC_MST_PDC              0x00020000      ///< PDC abort source.
#define MC_MST_ARM              0x00040000      ///< ARM abort source.
#define MC_SVMST_PDC            0x02000000      ///< Saved PDC abort source.
#define MC_SVMST_ARM            0x04000000      ///< Saved ARM abort source.

#define MC_AASR_OFF             0x00000008      ///< MC abort address status register offset.
#define MC_AASR     (*((reg32_t *)(MC_BASE + MC_AASR_OFF)))     ///< MC abort address status register address.

#define MC_FMR_OFF              0x00000060      ///< MC flash mode register offset.
#define MC_FMR      (*((reg32_t *)(MC_BASE + MC_FMR_OFF)))      ///< MC flash mode register address.
#define MC_FRDY                          0      ///< Flash ready.
#define MC_LOCKE                         2      ///< Lock error.
#define MC_PROGE                         3      ///< Programming error.
#define MC_NEBP                          7      ///< No erase before programming.
#define MC_FWS_MASK             0x00000300      ///< Flash wait state mask.
#define MC_FWS_1R2W             0x00000000      ///< 1 cycle for read, 2 for write operations.
#define MC_FWS_2R3W             0x00000100      ///< 2 cycles for read, 3 for write operations.
#define MC_FWS_3R4W             0x00000200      ///< 3 cycles for read, 4 for write operations.
#define MC_FWS_4R4W             0x00000300      ///< 4 cycles for read and write operations.
#define MC_FMCN_MASK            0x00FF0000      ///< Flash microsecond cycle number mask.
#define MC_FMCN_SHIFT                   16      ///< Flash microsecond cycle number shift.

#define MC_FCR_OFF              0x00000064      ///< MC flash command register offset.
#define MC_FCR      (*((reg32_t *)(MC_BASE + MC_FCR_OFF)))      ///< MC flash command register address.
#define MC_FCMD_MASK            0x0000000F      ///< Flash command mask.
#define MC_FCMD_NOP             0x00000000      ///< No command.
#define MC_FCMD_WP              0x00000001      ///< Write page.
#define MC_FCMD_SLB             0x00000002      ///< Set lock bit.
#define MC_FCMD_WPL             0x00000003      ///< Write page and lock.
#define MC_FCMD_CLB             0x00000004      ///< Clear lock bit.
#define MC_FCMD_EA              0x00000008      ///< Erase all.
#define MC_FCMD_SGPB            0x0000000B      ///< Set general purpose NVM bit.
#define MC_FCMD_CGPB            0x0000000D      ///< Clear general purpose NVM bit.
#define MC_FCMD_SSB             0x0000000F      ///< Set security bit.
#define MC_PAGEN_MASK           0x0003FF00      ///< Page number mask.
#define MC_KEY                  0x5A000000      ///< Writing protect key.

#define MC_FSR_OFF              0x00000068      ///< MC flash status register offset.
#define MC_FSR      (*((reg32_t *)(MC_BASE + MC_FSR_OFF)))      ///< MC flash status register address.
#define MC_SECURITY                      4      ///< Security bit status.

#define MC_GPNVM0                        8      ///< General purpose NVM bit 0.
#define MC_GPNVM1                        9      ///< General purpose NVM bit 1.
#define MC_GPNVM2                       10      ///< General purpose NVM bit 2.

#define MC_LOCKS0                       16      ///< Lock region 0 lock status.
#define MC_LOCKS1                       17      ///< Lock region 1 lock status.
#define MC_LOCKS2                       18      ///< Lock region 2 lock status.
#define MC_LOCKS3                       19      ///< Lock region 3 lock status.
#define MC_LOCKS4                       20      ///< Lock region 4 lock status.
#define MC_LOCKS5                       21      ///< Lock region 5 lock status.
#define MC_LOCKS6                       22      ///< Lock region 6 lock status.
#define MC_LOCKS7                       23      ///< Lock region 7 lock status.
#define MC_LOCKS8                       24      ///< Lock region 8 lock status.
#define MC_LOCKS9                       25      ///< Lock region 9 lock status.
#define MC_LOCKS10                      26      ///< Lock region 10 lock status.
#define MC_LOCKS11                      27      ///< Lock region 11 lock status.
#define MC_LOCKS12                      28      ///< Lock region 12 lock status.
#define MC_LOCKS13                      29      ///< Lock region 13 lock status.
#define MC_LOCKS14                      30      ///< Lock region 14 lock status.
#define MC_LOCKS15                      31      ///< Lock region 15 lock status.

#endif /* AT91_MC_H */
