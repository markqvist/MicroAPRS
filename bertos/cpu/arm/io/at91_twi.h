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
 * AT91SAM7 Two wire interface.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * OR TORT (*((reg32_t *)(INCLUDING NEGLIGENCE OR OTHERWISE))) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef AT91_TWI_H
#define AT91_TWI_H


/**
 * TWI Control Register.
 * \{
 */
#define TWI_CR_OFF              0x00000000      ///< Control register offset.
#define TWI_CR      (*((reg32_t *)(TWI_BASE + TWI_CR_OFF)))     ///< Control register address.
#define TWI_START                        0      ///< Send start condition.
#define TWI_STOP                         1      ///< Send stop condition.
#define TWI_MSEN                         2      ///< Enable master mode.
#define TWI_MSDIS                        3      ///< Disable master mode.
/*
#define TWI_SVEN                         4      ///< Enable slave mode.
#define TWI_SVDIS                        5      ///< Disable slave mode.
*/
#define TWI_SWRST                        7      ///< Software reset.
/*\}*/

/**
 * TWI Master Mode Register.
 * \{
 */
#define TWI_MMR_OFF             0x00000004      ///< Master mode register offset.
#define TWI_MMR     (*((reg32_t *)(TWI_BASE + TWI_MMR_OFF)))    ///< Master mode register address.
#define TWI_IADRSZ_SHIFT                 8      ///< Internal device address size shift.
#define TWI_IADRSZ              0x00000300      ///< Internal device address size mask.
#define TWI_IADRSZ_NONE         0x00000000      ///< No internal device address.
#define TWI_IADRSZ_1BYTE        0x00000100      ///< One byte internal device address.
#define TWI_IADRSZ_2BYTE        0x00000200      ///< Two byte internal device address.
#define TWI_IADRSZ_3BYTE        0x00000300      ///< Three byte internal device address.
#define TWI_MREAD                       12      ///< Master read direction.
#define TWI_DADR                0x007F0000      ///< Device address mask.
#define TWI_DADR_SHIFT                  16      ///< Device address LSB.
/*\}*/

/**
 * TWI Internal Address Register.
 * \{
 */
#define TWI_IADR_OFF           0x0000000C       ///< Internal address register offset.
#define TWI_IADR   (*((reg32_t *)(TWI_BASE + TWI_IADR_OFF)))  ///< Internal address register address.
#define TWI_IADR_MASK           0x00FFFFFF      ///< Internal address mask.
#define TWI_IADR_SHIFT                   0      ///< Internal address LSB.
/*\}*/

/**
 * TWI Clock Waveform Generator Register.
 * \{
 */
#define TWI_CWGR_OFF             0x00000010     ///< Clock waveform generator register offset.
#define TWI_CWGR    (*((reg32_t *)(TWI_BASE + TWI_CWGR_OFF)))   ///< Clock waveform generator register address.
#define TWI_CLDIV                0x000000FF     ///< Clock low divider mask.
#define TWI_CLDIV_SHIFT                   0     ///< Clock low divider LSB.
#define TWI_CHDIV                0x0000FF00     ///< Clock high divider mask.
#define TWI_CHDIV_SHIFT                   8     ///< Clock high divider LSB.
#define TWI_CKDIV                0x00070000     ///< Clock divider mask.
#define TWI_CKDIV_SHIFT                  16     ///< Clock divider LSB.
/*\}*/

/**
 * TWI Status and Interrupt Registers.
 * \{
 */
#define TWI_SR_OFF              0x00000020      ///< Status register offset.
#define TWI_SR      (*((reg32_t *)(TWI_BASE + TWI_SR_OFF)))     ///< Status register address.

#define TWI_IER_OFF             0x00000024      ///< Interrupt enable register offset.
#define TWI_IER     (*((reg32_t *)(TWI_BASE + TWI_IER_OFF)))    ///< Interrupt enable register address.

#define TWI_IDR_OFF             0x00000028      ///< Interrupt disable register offset.
#define TWI_IDR     (*((reg32_t *)(TWI_BASE + TWI_IDR_OFF)))    ///< Interrupt disable register address.

#define TWI_IMR_OFF             0x0000002C      ///< Interrupt mask register offset.
#define TWI_IMR     (*((reg32_t *)(TWI_BASE + TWI_IMR_OFF)))    ///< Interrupt mask register address.

#define TWI_TXCOMP                       0      ///< Transmission completed.
#define TWI_RXRDY                        1      ///< Receive holding register ready.
#define TWI_TXRDY                        2      ///< Transmit holding register ready.

/*
#define TWI_SVREAD              0x00000008      ///< Slave read.
#define TWI_SVACC               0x00000010      ///< Slave access.
#define TWI_GACC                0x00000020      ///< General call access.
*/

#if CPU_ARM_SAM7X
#define TWI_OVRE                         6      ///< Overrun error.
#define TWI_UNRE                         7      ///< Underrun error.
#endif

#define TWI_NACK                         8      ///< Not acknowledged.
/*
#define TWI_ARBLST              0x00000200      ///< Arbitration lost.
#define TWI_SCLWS               0x00000400      ///< Clock wait state.
#define TWI_EOSACC              0x00000800      ///< End of slave access.
*/
/*\}*/

/**
 * TWI Receive Holding Register.
 * \{
 */
#define TWI_RHR_OFF             0x00000030      ///< Receive holding register offset.
#define TWI_RHR     (*((reg32_t *)(TWI_BASE + TWI_RHR_OFF)))    ///< Receive holding register address.
/*\}*/

/**
 * TWI Transmit Holding Register.
 * \{
 */
#define TWI_THR_OFF             0x00000034      ///< Transmit holding register offset.
#define TWI_THR     (*((reg32_t *)(TWI_BASE + TWI_THR_OFF)))    ///< Transmit holding register address.
/*\}*/


#endif /* AT91_TWI_H */
