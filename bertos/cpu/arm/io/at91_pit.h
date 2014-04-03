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
 * AT91 periodic interval timer.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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

#ifndef AT91_PIT_H
#define AT91_PIT_H

#include <cfg/compiler.h>
/**
 *Periodic Inverval Timer Mode Register
 *\{
 */
#define PIT_MR_OFF 0x00000000 ///< Mode register offset.
#define PIT_MR     (*((reg32_t *)(PIT_BASE + PIT_MR_OFF))) ///< Mode register address.

#define PIV_MASK   0x000FFFFF ///< Periodic interval value mask.
#define PIV_SHIFT  0          ///< Periodic interval value shift.
#define PITEN      24         ///< Periodic interval timer enable.
#define PITIEN     25         ///< Periodic interval timer interrupt enable.
/*\}*/

/**
 * Periodic Inverval Timer Status Register
 *\{
 */
#define PIT_SR_OFF 0x00000004 ///< Status register offset.
#define PIT_SR     (*((reg32_t *)(PIT_BASE + PIT_SR_OFF))) ///< Status register address.

#define PITS       0          ///< Timer has reached PIV.
/*\}*/

/**
 * Periodic Inverval Timer Value and Image Registers
 *\{
 */
#define PIVR_OFF    0x00000008 ///< Value register offset.
#define PIVR        (*((reg32_t *)(PIT_BASE + PIVR_OFF))) ///< Value register address.

#define PIIR_OFF    0x0000000C ///< Image register offset.
#define PIIR        (*((reg32_t *)(PIT_BASE + PIIR_OFF))) ///< Image register address.
#define CPIV_MASK   0x000FFFFF ///< Current periodic interval value mask.
#define CPIV_SHIFT  0          ///< Current periodic interval value SHIFT.
#define PICNT_MASK  0xFFF00000 ///< Periodic interval counter mask.
#define PICNT_SHIFT 20         ///< Periodic interval counter LSB.
/*\}*/

#endif /* AT91_PIT_H */
