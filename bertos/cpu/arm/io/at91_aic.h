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
 * AT91 advanced interrupt controller.
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

#ifndef AT91_AIC_H
#define AT91_AIC_H

#include <cfg/compiler.h>



/**
 * Source mode register array.
 */
#define AIC_SMR(i)  (*((reg32_t *)(AIC_BASE + (i) * 4)))

/**
 * Priority mask.
 * Priority levels can be between 0 (lowest) and 7 (highest).
 */
#define AIC_PRIOR_MASK 0x00000007

/**
 * Interrupt source type mask.
 * Internal interrupts can level sensitive or edge triggered.
 *
 * External interrupts can triggered on positive or negative levels or
 * on rising or falling edges.
 */
/*\{*/
#define AIC_SRCTYPE_MASK 0x00000060

#define AIC_SRCTYPE_INT_LEVEL_SENSITIVE 0x00000000      ///< Internal level sensitive.
#define AIC_SRCTYPE_INT_EDGE_TRIGGERED  0x00000020      ///< Internal edge triggered.
#define AIC_SRCTYPE_EXT_LOW_LEVEL       0x00000000      ///< External low level.
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE   0x00000020      ///< External falling edge.
#define AIC_SRCTYPE_EXT_HIGH_LEVEL      0x00000040      ///< External high level.
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE   0x00000060      ///< External rising edge.
/*\}*/


/**
 * Type for interrupt handlers.
 */
typedef void (*irq_handler_t)(void);

/** Interrupt Source Vector Registers */
/*\{*/
/** Source vector register array.
 *
 * Stores the addresses of the corresponding interrupt handlers.
 */
#define AIC_SVR(i)  (*((volatile irq_handler_t *)(AIC_BASE + 0x80 + (i) * 4)))
/*\}*/

/** Interrupt Vector Register */
/*\{*/
#define AIC_IVR_OFF 0x00000100  ///< IRQ vector register offset.
#define AIC_IVR     (*((reg32_t *)(AIC_BASE + AIC_IVR_OFF))) ///< IRQ vector register address.
/*\}*/

/** Fast Interrupt Vector Register */
/*\{*/
#define AIC_FVR_OFF 0x00000104  ///< FIQ vector register offset.
#define AIC_FVR     (*((reg32_t *)(AIC_BASE + AIC_FVR_OFF))) ///< FIQ vector register address.
/*\}*/

/** Interrupt Status Register */
/*\{*/
#define AIC_ISR_OFF    0x00000108  ///< Interrupt status register offset.
#define AIC_ISR        (*((reg32_t *)(AIC_BASE + AIC_ISR_OFF))) ///< Interrupt status register address.
#define AIC_IRQID_MASK 0x0000001F  ///< Current interrupt identifier mask.
/*\}*/

/** Interrupt Pending Register */
/*\{*/
#define AIC_IPR_OFF 0x0000010C  ///< Interrupt pending register offset.
#define AIC_IPR     (*((reg32_t *)(AIC_BASE + AIC_IPR_OFF))) ///< Interrupt pending register address.
/*\}*/

/** Interrupt Mask Register */
/*\{*/
#define AIC_IMR_OFF 0x00000110  ///< Interrupt mask register offset.
#define AIC_IMR     (*((reg32_t *)(AIC_BASE + AIC_IMR_OFF))) ///< Interrupt mask register address.
/*\}*/

/** Interrupt Core Status Register */
/*\{*/
#define AIC_CISR_OFF 0x00000114  ///< Core interrupt status register offset.
#define AIC_CISR     (*((reg32_t *)(AIC_BASE + AIC_CISR_OFF))) ///< Core interrupt status register address.
#define AIC_NFIQ     1  ///< Core FIQ Status
#define AIC_NIRQ     2  ///< Core IRQ Status
/*\}*/

/** Interrupt Enable Command Register */
/*\{*/
#define AIC_IECR_OFF 0x00000120  ///< Interrupt enable command register offset.
#define AIC_IECR     (*((reg32_t *)(AIC_BASE + AIC_IECR_OFF)))   ///< Interrupt enable command register address.
/*\}*/

/** Interrupt Disable Command Register */
/*\{*/
#define AIC_IDCR_OFF 0x00000124  ///< Interrupt disable command register offset.
#define AIC_IDCR     (*((reg32_t *)(AIC_BASE + AIC_IDCR_OFF)))   ///< Interrupt disable command register address.
/*\}*/

/** Interrupt Clear Command Register */
/*\{*/
#define AIC_ICCR_OFF 0x00000128  ///< Interrupt clear command register offset.
#define AIC_ICCR     (*((reg32_t *)(AIC_BASE + AIC_ICCR_OFF)))   ///< Interrupt clear command register address.
/*\}*/

/** Interrupt Set Command Register */
/*\{*/
#define AIC_ISCR_OFF 0x0000012C  ///< Interrupt set command register offset.
#define AIC_ISCR     (*((reg32_t *)(AIC_BASE + AIC_ISCR_OFF)))   ///< Interrupt set command register address.
/*\}*/

/** End Of Interrupt Command Register */
/*\{*/
#define AIC_EOICR_OFF 0x00000130  ///< End of interrupt command register offset.
#define AIC_EOICR     (*((reg32_t *)(AIC_BASE + AIC_EOICR_OFF)))  ///< End of interrupt command register address.
/*\}*/

/** Spurious Interrupt Vector Register */
/*\{*/
#define AIC_SPU_OFF 0x00000134  ///< Spurious vector register offset.
#define AIC_SPU     (*((reg32_t *)(AIC_BASE + AIC_SPU_OFF)==    ///< Spurious vector register address.
/*\}*/

/** Debug Control Register */
/*\{*/
#define AIC_DCR_OFF 0x0000138   ///< Debug control register offset.
#define AIC_DCR     (*((reg32_t *)(AIC_BASE + AIC_DCR_OFF)))    ///< Debug control register address.
/*\}*/

/** Fast Forcing Enable Register */
/*\{*/
#define AIC_FFER_OFF 0x00000140  ///< Fast forcing enable register offset.
#define AIC_FFER     (*((reg32_t *)(AIC_BASE + AIC_FFER_OFF)))   ///< Fast forcing enable register address.
/*\}*/

/** Fast Forcing Disable Register */
/*\{*/
#define AIC_FFDR_OFF 0x00000144  ///< Fast forcing disable register address.
#define AIC_FFDR     (*((reg32_t *)(AIC_BASE + AIC_FFDR_OFF)))   ///< Fast forcing disable register address.
/*\}*/

/** Fast Forcing Status Register */
/*\{*/
#define AIC_FFSR_OFF 0x00000148  ///< Fast forcing status register address.
#define AIC_FFSR     (*((reg32_t *)(AIC_BASE + AIC_FFSR_OFF)))   ///< Fast forcing status register address.
/*\}*/

#endif /* AT91_AIC_H */
