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
 * AT91 power management controller.
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

#ifndef AT91_PMC_H
#define AT91_PMC_H

/** System Clock Enable, Disable and Status Register */
/*\{*/
#define PMC_SCER_OFF                0x00000000  ///< System clock enable register offset.
#define PMC_SCER    (*((reg32_t *)(PMC_BASE + PMC_SCER_OFF)))   ///< System clock enable register address.
#define PMC_SCDR_OFF                0x00000004  ///< System clock disable register offset.
#define PMC_SCDR    (*((reg32_t *)(PMC_BASE + PMC_SCDR_OFF)))   ///< System clock disable register address.
#define PMC_SCSR_OFF                0x00000008  ///< System clock status register offset.
#define PMC_SCSR    (*((reg32_t *)(PMC_BASE + PMC_SCSR_OFF)))   ///< System clock status register address.

#define PMC_PCK                              0  ///< Processor clock.
#define PMC_UDP                              7  ///< USB device port clock.
#define PMC_PCK0                             8  ///< Programmable clock 0 output.
#define PMC_PCK1                             9  ///< Programmable clock 1 output.
#define PMC_PCK2                            10  ///< Programmable clock 2 output.
/*\}*/

/** Peripheral Clock Enable, Disable and Status Register */
/*\{*/
#define PMC_PCER_OFF                0x00000010  ///< Peripheral clock enable register offset.
#define PMC_PCER    (*((reg32_t *)(PMC_BASE + PMC_PCER_OFF)))   ///< Peripheral clock enable register address.
#define PMC_PCDR_OFF                0x00000014  ///< Peripheral clock disable register offset.
#define PMC_PCDR    (*((reg32_t *)(PMC_BASE + PMC_PCDR_OFF)))   ///< Peripheral clock disable register address.
#define PMC_PCSR_OFF                0x00000018  ///< Peripheral clock status register offset.
#define PMC_PCSR    (*((reg32_t *)(PMC_BASE + PMC_PCSR_OFF)))   ///< Peripheral clock status register address.
/*\}*/

/** Clock Generator Main Oscillator Register */
/*\{*/
#define CKGR_MOR_OFF                0x00000020  ///< Main oscillator register offset.
#define CKGR_MOR    (*((reg32_t *)(PMC_BASE + CKGR_MOR_OFF)))   ///< Main oscillator register address.

#define CKGR_MOSCEN                          0  ///< Main oscillator enable.
#define CKGR_OSCBYPASS                       1  ///< Main oscillator bypass.
#define CKGR_OSCOUNT_MASK           0x0000FF00  ///< Main oscillator start-up time mask.
#define CKGR_OSCOUNT_SHIFT                   8  ///< Main oscillator start-up time LSB.
/*\}*/

/** Clock Generator Main Clock Frequency Register */
/*\{*/
#define CKGR_MCFR_OFF               0x00000024  ///< Main clock frequency register offset.
#define CKGR_MCFR   (*((reg32_t *)(PMC_BASE + CKGR_MCFR_OFF)))  ///< Main clock frequency register address.

#define CKGR_MAINF_MASK             0x0000FFFF  ///< Main clock frequency mask mask.
#define CKGR_MAINRDY                        16  ///< Main clock ready.
/*\}*/

/** PLL Registers */
/*\{*/
#define CKGR_PLLR_OFF               0x0000002C  ///< Clock generator PLL register offset.
#define CKGR_PLLR   (*((reg32_t *)(PMC_BASE + CKGR_PLLR_OFF)))  ///< Clock generator PLL register address.

#define CKGR_DIV_MASK               0x000000FF  ///< Divider.
#define CKGR_DIV_SHIFT                       0  ///< Least significant bit of the divider.
#define CKGR_DIV_0                  0x00000000  ///< Divider output is 0.
#define CKGR_DIV_BYPASS             0x00000001  ///< Divider is bypassed.
#define CKGR_PLLCOUNT_MASK          0x00003F00  ///< PLL counter mask.
#define CKGR_PLLCOUNT_SHIFT                  8  ///< PLL counter LSB.

#define CKGR_OUT_MASK               0x0000C000  ///< PLL output frequency range.
#define CKGR_OUT_0                  0x00000000  ///< Please refer to the PLL datasheet.
#define CKGR_OUT_1                  0x00004000  ///< Please refer to the PLL datasheet.
#define CKGR_OUT_2                  0x00008000  ///< Please refer to the PLL datasheet.
#define CKGR_OUT_3                  0x0000C000  ///< Please refer to the PLL datasheet.
#define CKGR_MUL_MASK               0x07FF0000  ///< PLL multiplier.
#define CKGR_MUL_SHIFT                      16  ///< Least significant bit of the PLL multiplier.

#define CKGR_USBDIV_MASK            0x30000000  ///< Divider for USB clocks.
#define CKGR_USBDIV_1               0x00000000  ///< Divider output is PLL clock output.
#define CKGR_USBDIV_2               0x10000000  ///< Divider output is PLL clock output divided by 2.
#define CKGR_USBDIV_4               0x20000000  ///< Divider output is PLL clock output divided by 4.
/*\}*/

/** Master Clock Register */
/*\{*/
#define PMC_MCKR_OFF                0x00000030  ///< Master clock register offset.
#define PMC_MCKR    (*((reg32_t *)(PMC_BASE + PMC_MCKR_OFF)))   ///< Master clock register address.

#define PMC_PCKR0_OFF               0x00000040  ///< Programmable clock 0 register offset.
#define PMC_PCKR0   (*((reg32_t *)(PMC_BASE + PMC_PCKR0_OFF)))  ///< Programmable clock 0 register address.
#define PMC_PCKR1_OFF               0x00000044  ///< Programmable clock 1 register offset.
#define PMC_PCKR1   (*((reg32_t *)(PMC_BASE + PMC_PCKR1_OFF)))  ///< Programmable clock 1 register address.
#define PMC_PCKR2_OFF               0x00000048  ///< Programmable clock 2 register offset.
#define PMC_PCKR2   (*((reg32_t *)(PMC_BASE + PMC_PCKR2_OFF)))  ///< Programmable clock 2 register address.

#define PMC_CSS_MASK                0x00000003  ///< Clock selection mask.
#define PMC_CSS_SLOW_CLK            0x00000000  ///< Slow clock selected.
#define PMC_CSS_MAIN_CLK            0x00000001  ///< Main clock selected.
#define PMC_CSS_PLL_CLK             0x00000003  ///< PLL clock selected.

#define PMC_PRES_MASK               0x0000001C  ///< Clock prescaler mask.
#define PMC_PRES_SHIFT                       2   ///< Clock prescaler LSB.
#define PMC_PRES_CLK                0x00000000  ///< Selected clock, not divided.
#define PMC_PRES_CLK_2              0x00000004  ///< Selected clock divided by 2.
#define PMC_PRES_CLK_4              0x00000008  ///< Selected clock divided by 4.
#define PMC_PRES_CLK_8              0x0000000C  ///< Selected clock divided by 8.
#define PMC_PRES_CLK_16             0x00000010  ///< Selected clock divided by 16.
#define PMC_PRES_CLK_32             0x00000014  ///< Selected clock divided by 32.
#define PMC_PRES_CLK_64             0x00000018  ///< Selected clock divided by 64.
/*\}*/

/** Power Management Status and Interrupt Registers */
/*\{*/
#define PMC_IER_OFF                 0x00000060  ///< Interrupt enable register offset.
#define PMC_IER     (*((reg32_t *)(PMC_BASE + PMC_IER_OFF)))    ///< Interrupt enable register address.
#define PMC_IDR_OFF                 0x00000064  ///< Interrupt disable register offset.
#define PMC_IDR     (*((reg32_t *)(PMC_BASE + PMC_IDR_OFF)))    ///< Interrupt disable register address.
#define PMC_SR_OFF                  0x00000068  ///< Status register offset.
#define PMC_SR      (*((reg32_t *)(PMC_BASE + PMC_SR_OFF)))     ///< Status register address.
#define PMC_IMR_OFF                 0x0000006C  ///< Interrupt mask register offset.
#define PMC_IMR     (*((reg32_t *)(PMC_BASE + PMC_IMR_OFF)))    ///< Interrupt mask register address.

#define PMC_MOSCS                            0  ///< Main oscillator.
#define PMC_LOCK                             2  ///< PLL lock.
#define PMC_MCKRDY                           3  ///< Master clock ready.
#define PMC_PCKRDY0                          8  ///< Programmable clock 0 ready.
#define PMC_PCKRDY1                          9  ///< Programmable clock 1 ready.
#define PMC_PCKRDY2                         10  ///< Programmable clock 2 ready.
/*\}*/

#endif /* AT91_PMC_H */
