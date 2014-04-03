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
 * AT91 Debug unit.
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
#ifndef AT91_DBGU_H
#define AT91_DBGU_H

#define DBGU_CR     (*((reg32_t *)(DBGU_BASE + US_CR_OFF)))     ///<DBGU control register address.
#define DBGU_MR     (*((reg32_t *)(DBGU_BASE + US_MR_OFF)))     ///<DBGU mode register address.
#define DBGU_IER    (*((reg32_t *)(DBGU_BASE + US_IER_OFF)))    ///<DBGU interrupt enable register address.
#define DBGU_IDR    (*((reg32_t *)(DBGU_BASE + US_IDR_OFF)))    ///<DBGU interrupt disable register address.
#define DBGU_IMR    (*((reg32_t *)(DBGU_BASE + US_IMR_OFF)))    ///<DBGU interrupt mask register address.
#define DBGU_SR     (*((reg32_t *)(DBGU_BASE + US_CSR_OFF)))    ///<DBGU status register address.
#define DBGU_RHR    (*((reg32_t *)(DBGU_BASE + US_RHR_OFF)))    ///<DBGU receiver holding register address.
#define DBGU_THR    (*((reg32_t *)(DBGU_BASE + US_THR_OFF)))    ///<DBGU transmitter holding register address.
#define DBGU_BRGR   (*((reg32_t *)(DBGU_BASE + US_BRGR_OFF)))   ///<DBGU baud rate register address.

#define DBGU_CIDR_OFF           0x00000040      ///<DBGU chip ID register offset.
#define DBGU_CIDR   (*((reg32_t *)(DBGU_BASE + DBGU_CIDR_OFF))) ///<DBGU chip ID register.

#define DBGU_EXID_OFF           0x00000044      ///<DBGU chip ID extension register offset.
#define DBGU_EXID   (*((reg32_t *)(DBGU_BASE + DBGU_EXID_OFF))) ///<DBGU chip ID extension register.

#define DBGU_FNR_OFF            0x00000048      ///<DBGU force NTRST register offset.
#define DBGU_FNR    (*((reg32_t *)(DBGU_BASE + DBGU_FNR_OFF)))  ///<DBGU force NTRST register.

#if defined(DBGU_HAS_PDC)
#define DBGU_RPR    (*((reg32_t *)(DBGU_BASE + PERIPH_RPR_OFF)))    ///<PDC receive pointer register.
#define DBGU_RCR    (*((reg32_t *)(DBGU_BASE + PERIPH_RCR_OFF)))    ///<PDC receive counter register.
#define DBGU_TPR    (*((reg32_t *)(DBGU_BASE + PERIPH_TPR_OFF)))    ///<PDC transmit pointer register.
#define DBGU_TCR    (*((reg32_t *)(DBGU_BASE + PERIPH_TCR_OFF)))    ///<PDC transmit counter register.
#define DBGU_RNPR   (*((reg32_t *)(DBGU_BASE + PERIPH_RNPR_OFF)))   ///<PDC receive next pointer register.
#define DBGU_RNCR   (*((reg32_t *)(DBGU_BASE + PERIPH_RNCR_OFF)))   ///<PDC receive next counter register.
#define DBGU_TNPR   (*((reg32_t *)(DBGU_BASE + PERIPH_TNPR_OFF)))   ///<PDC transmit next pointer register.
#define DBGU_TNCR   (*((reg32_t *)(DBGU_BASE + PERIPH_TNCR_OFF)))   ///<PDC transmit next counter register.
#define DBGU_PTCR   (*((reg32_t *)(DBGU_BASE + PERIPH_PTCR_OFF)))   ///<PDC transfer control register.
#define DBGU_PTSR   (*((reg32_t *)(DBGU_BASE + PERIPH_PTSR_OFF)))   ///<PDC transfer status register.
#endif

#endif /* AT91_DBGU_H */
