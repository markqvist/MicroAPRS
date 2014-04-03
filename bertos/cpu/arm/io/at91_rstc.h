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
 * AT91 reset controller.
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

#ifndef AT91_RTSC_H
#define AT91_RTSC_H

/** Reset Controller Control Register */
/*\{*/
#define RSTC_CR         (*((reg32_t *)(RSTC_BASE + 0x00)))      ///< Reset controller control register address.
#define RSTC_PROCRST                     0  ///< Processor reset.
#define RSTC_PERRST                      2  ///< Peripheral reset.
#define RSTC_EXTRST                      3  ///< External reset.
#define RSTC_KEY                0xA5000000  ///< Password.
/*\}*/

/** Reset Controller Status Register */
/*\{*/
#define RSTC_SR         (*((reg32_t *)(RSTC_BASE + 0x04)))      ///< Reset controller status register address.
#define RSTC_URSTS                       0  ///< User reset status.
#define RSTC_BODSTS                      1  ///< Brownout detection status.
#define RSTC_RSTTYP_MASK        0x00000700  ///< Reset type.
#define RSTC_RSTTYP_POWERUP     0x00000000  ///< Power-up reset.
//#define RSTC_RSTTYP_WAKEUP      0x00000100      ///< VDDCORE rising.
#define RSTC_RSTTYP_WATCHDOG    0x00000200  ///< Watchdog reset.
#define RSTC_RSTTYP_SOFTWARE    0x00000300  ///< Software reset.
#define RSTC_RSTTYP_USER        0x00000400  ///< User reset.
#define RSTC_RSTTYP_BROWNOUT    0x00000500  ///< Brownout reset.
#define RSTC_NRSTL                      16  ///< NRST pin level.
#define RSTC_SRCMP                      17  ///< Software reset command in progress.
/*\}*/

/** Reset Controller Mode Register */
/*\{*/
#define RSTC_MR         (*((reg32_t *)(RSTC_BASE + 0x08)))      ///< Reset controller mode register address.
#define RSTC_URSTEN                      0  ///< User reset enable.
#define RSTC_URSTIEN                     4  ///< User reset interrupt enable.
#define RSTC_ERSTL_MASK         0x00000F00  ///< External reset length.
#define RSTC_ERSTL_SHIFT                 8  ///< Least significant bit of external reset length.
#define RSTC_BODIEN                     16  ///< Brown-out detection interrupt enable.
/*\}*/


#endif /* AT91_RTSC_H */
