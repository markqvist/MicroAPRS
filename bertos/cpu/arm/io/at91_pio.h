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
 * AT91 Parallel input/output controller.
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

#ifndef AT91_PIO_H
#define AT91_PIO_H

/** PIO Register Offsets */
/*\{*/
#define PIO_PER_OFF     0x00000000  ///< PIO enable register offset.
#define PIO_PDR_OFF     0x00000004  ///< PIO disable register offset.
#define PIO_PSR_OFF     0x00000008  ///< PIO status register offset.
#define PIO_OER_OFF     0x00000010  ///< Output enable register offset.
#define PIO_ODR_OFF     0x00000014  ///< Output disable register offset.
#define PIO_OSR_OFF     0x00000018  ///< Output status register offset.
#define PIO_IFER_OFF    0x00000020  ///< Input filter enable register offset.
#define PIO_IFDR_OFF    0x00000024  ///< Input filter disable register offset.
#define PIO_IFSR_OFF    0x00000028  ///< Input filter status register offset.
#define PIO_SODR_OFF    0x00000030  ///< Set output data register offset.
#define PIO_CODR_OFF    0x00000034  ///< Clear output data register offset.
#define PIO_ODSR_OFF    0x00000038  ///< Output data status register offset.
#define PIO_PDSR_OFF    0x0000003C  ///< Pin data status register offset.
#define PIO_IER_OFF     0x00000040  ///< Interrupt enable register offset.
#define PIO_IDR_OFF     0x00000044  ///< Interrupt disable register offset.
#define PIO_IMR_OFF     0x00000048  ///< Interrupt mask register offset.
#define PIO_ISR_OFF     0x0000004C  ///< Interrupt status register offset.
#if PIO_HAS_MULTIDRIVER
#define PIO_MDER_OFF    0x00000050  ///< Multi-driver enable register offset.
#define PIO_MDDR_OFF    0x00000054  ///< Multi-driver disable register offset.
#define PIO_MDSR_OFF    0x00000058  ///< Multi-driver status register offset.
#endif /* PIO_HAS_MULTIDRIVER */
#if PIO_HAS_PULLUP
#define PIO_PUDR_OFF    0x00000060  ///< Pull-up disable register offset.
#define PIO_PUER_OFF    0x00000064  ///< Pull-up enable register offset.
#define PIO_PUSR_OFF    0x00000068  ///< Pull-up status register offset.
#endif /* PIO_HAS_PULLUP */
#if PIO_HAS_PERIPHERALSELECT
#define PIO_ASR_OFF     0x00000070  ///< PIO peripheral A select register offset.
#define PIO_BSR_OFF     0x00000074  ///< PIO peripheral B select register offset.
#define PIO_ABSR_OFF    0x00000078  ///< PIO peripheral AB status register offset.
#endif /* PIO_HAS_PERIPHERALSELECT */
#if PIO_HAS_OUTPUTWRITEENABLE
#define PIO_OWER_OFF    0x000000A0  ///< PIO output write enable register offset.
#define PIO_OWDR_OFF    0x000000A4  ///< PIO output write disable register offset.
#define PIO_OWSR_OFF    0x000000A8  ///< PIO output write status register offset.
#endif /* PIO_HAS_OUTPUTWRITEENABLE */
/*\}*/

/** Single PIO Register Addresses */
/*\{*/
#if defined(PIO_BASE)
	#define PIO_ACCESS(offset) (*((reg32_t *)(PIO_BASE + (offset))))

	#define PIO_PER     PIO_ACCESS(PIO_PER_OFF)    ///< PIO enable register address.
	#define PIO_PDR     PIO_ACCESS(PIO_PDR_OFF)    ///< PIO disable register address.
	#define PIO_PSR     PIO_ACCESS(PIO_PSR_OFF)    ///< PIO status register address.
	#define PIO_OER     PIO_ACCESS(PIO_OER_OFF)    ///< Output enable register address.
	#define PIO_ODR     PIO_ACCESS(PIO_ODR_OFF)    ///< Output disable register address.
	#define PIO_OSR     PIO_ACCESS(PIO_OSR_OFF)    ///< Output status register address.
	#define PIO_IFER    PIO_ACCESS(PIO_IFER_OFF)   ///< Input filter enable register address.
	#define PIO_IFDR    PIO_ACCESS(PIO_IFDR_OFF)   ///< Input filter disable register address.
	#define PIO_IFSR    PIO_ACCESS(PIO_IFSR_OFF)   ///< Input filter status register address.
	#define PIO_SODR    PIO_ACCESS(PIO_SODR_OFF)   ///< Set output data register address.
	#define PIO_CODR    PIO_ACCESS(PIO_CODR_OFF)   ///< Clear output data register address.
	#define PIO_ODSR    PIO_ACCESS(PIO_ODSR_OFF)   ///< Output data status register address.
	#define PIO_PDSR    PIO_ACCESS(PIO_PDSR_OFF)   ///< Pin data status register address.
	#define PIO_IER     PIO_ACCESS(PIO_IER_OFF)    ///< Interrupt enable register address.
	#define PIO_IDR     PIO_ACCESS(PIO_IDR_OFF)    ///< Interrupt disable register address.
	#define PIO_IMR     PIO_ACCESS(PIO_IMR_OFF)    ///< Interrupt mask register address.
	#define PIO_ISR     PIO_ACCESS(PIO_ISR_OFF)    ///< Interrupt status register address.
	#if PIO_HAS_MULTIDRIVER
	#define PIO_MDER    PIO_ACCESS(PIO_MDER_OFF)   ///< Multi-driver enable register address.
	#define PIO_MDDR    PIO_ACCESS(PIO_MDDR_OFF)   ///< Multi-driver disable register address.
	#define PIO_MDSR    PIO_ACCESS(PIO_MDSR_OFF)   ///< Multi-driver status register address.
	#endif /* PIO_HAS_MULTIDRIVER */
	#if PIO_HAS_PULLUP
	#define PIO_PUDR    PIO_ACCESS(PIO_PUDR_OFF)   ///< Pull-up disable register address.
	#define PIO_PUER    PIO_ACCESS(PIO_PUER_OFF)   ///< Pull-up enable register address.
	#define PIO_PUSR    PIO_ACCESS(PIO_PUSR_OFF)   ///< Pull-up status register address.
	#endif /* PIO_HAS_PULLUP */
	#if PIO_HAS_PERIPHERALSELECT
	#define PIO_ASR     PIO_ACCESS(PIO_ASR_OFF)    ///< PIO peripheral A select register address.
	#define PIO_BSR     PIO_ACCESS(PIO_BSR_OFF)    ///< PIO peripheral B select register address.
	#define PIO_ABSR    PIO_ACCESS(PIO_ABSR_OFF)   ///< PIO peripheral AB status register address.
	#endif /* PIO_HAS_PERIPHERALSELECT */
	#if PIO_HAS_OUTPUTWRITEENABLE
	#define PIO_OWER    PIO_ACCESS(PIO_OWER_OFF)   ///< PIO output write enable register address.
	#define PIO_OWDR    PIO_ACCESS(PIO_OWDR_OFF)   ///< PIO output write disable register address.
	#define PIO_OWSR    PIO_ACCESS(PIO_OWSR_OFF)   ///< PIO output write status register address.
	#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIO_BASE */
/*\}*/

/** PIO A Register Addresses */
/*\{*/
#if defined(PIOA_BASE)
	#define PIOA_ACCESS(offset) (*((reg32_t *)(PIOA_BASE + (offset))))

	#define PIOA_PER    PIOA_ACCESS(PIO_PER_OFF)   ///< PIO enable register address.
	#define PIOA_PDR    PIOA_ACCESS(PIO_PDR_OFF)   ///< PIO disable register address.
	#define PIOA_PSR    PIOA_ACCESS(PIO_PSR_OFF)   ///< PIO status register address.
	#define PIOA_OER    PIOA_ACCESS(PIO_OER_OFF)   ///< Output enable register address.
	#define PIOA_ODR    PIOA_ACCESS(PIO_ODR_OFF)   ///< Output disable register address.
	#define PIOA_OSR    PIOA_ACCESS(PIO_OSR_OFF)   ///< Output status register address.
	#define PIOA_IFER   PIOA_ACCESS(PIO_IFER_OFF)  ///< Input filter enable register address.
	#define PIOA_IFDR   PIOA_ACCESS(PIO_IFDR_OFF)  ///< Input filter disable register address.
	#define PIOA_IFSR   PIOA_ACCESS(PIO_IFSR_OFF)  ///< Input filter status register address.
	#define PIOA_SODR   PIOA_ACCESS(PIO_SODR_OFF)  ///< Set output data register address.
	#define PIOA_CODR   PIOA_ACCESS(PIO_CODR_OFF)  ///< Clear output data register address.
	#define PIOA_ODSR   PIOA_ACCESS(PIO_ODSR_OFF)  ///< Output data status register address.
	#define PIOA_PDSR   PIOA_ACCESS(PIO_PDSR_OFF)  ///< Pin data status register address.
	#define PIOA_IER    PIOA_ACCESS(PIO_IER_OFF)   ///< Interrupt enable register address.
	#define PIOA_IDR    PIOA_ACCESS(PIO_IDR_OFF)   ///< Interrupt disable register address.
	#define PIOA_IMR    PIOA_ACCESS(PIO_IMR_OFF)   ///< Interrupt mask register address.
	#define PIOA_ISR    PIOA_ACCESS(PIO_ISR_OFF)   ///< Interrupt status register address.
	#if PIO_HAS_MULTIDRIVER
	#define PIOA_MDER   PIOA_ACCESS(PIO_MDER_OFF)  ///< Multi-driver enable register address.
	#define PIOA_MDDR   PIOA_ACCESS(PIO_MDDR_OFF)  ///< Multi-driver disable register address.
	#define PIOA_MDSR   PIOA_ACCESS(PIO_MDSR_OFF)  ///< Multi-driver status register address.
	#endif /* PIO_HAS_MULTIDRIVER */
	#if PIO_HAS_PULLUP
	#define PIOA_PUDR   PIOA_ACCESS(PIO_PUDR_OFF)  ///< Pull-up disable register address.
	#define PIOA_PUER   PIOA_ACCESS(PIO_PUER_OFF)  ///< Pull-up enable register address.
	#define PIOA_PUSR   PIOA_ACCESS(PIO_PUSR_OFF)  ///< Pull-up status register address.
	#endif /* PIO_HAS_PULLUP */
	#if PIO_HAS_PERIPHERALSELECT
	#define PIOA_ASR    PIOA_ACCESS(PIO_ASR_OFF)   ///< PIO peripheral A select register address.
	#define PIOA_BSR    PIOA_ACCESS(PIO_BSR_OFF)   ///< PIO peripheral B select register address.
	#define PIOA_ABSR   PIOA_ACCESS(PIO_ABSR_OFF)  ///< PIO peripheral AB status register address.
	#endif /* PIO_HAS_PERIPHERALSELECT */
	#if PIO_HAS_OUTPUTWRITEENABLE
	#define PIOA_OWER   PIOA_ACCESS(PIO_OWER_OFF)  ///< PIO output write enable register address.
	#define PIOA_OWDR   PIOA_ACCESS(PIO_OWDR_OFF)  ///< PIO output write disable register address.
	#define PIOA_OWSR   PIOA_ACCESS(PIO_OWSR_OFF)  ///< PIO output write status register address.
	#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOA_BASE */
/*\}*/

/** PIO B Register Addresses */
/*\{*/
#if defined(PIOB_BASE)
	#define PIOB_ACCESS(offset) (*((reg32_t *)(PIOB_BASE + (offset))))

	#define PIOB_PER    PIOB_ACCESS(PIO_PER_OFF)   ///< PIO enable register address.
	#define PIOB_PDR    PIOB_ACCESS(PIO_PDR_OFF)   ///< PIO disable register address.
	#define PIOB_PSR    PIOB_ACCESS(PIO_PSR_OFF)   ///< PIO status register address.
	#define PIOB_OER    PIOB_ACCESS(PIO_OER_OFF)   ///< Output enable register address.
	#define PIOB_ODR    PIOB_ACCESS(PIO_ODR_OFF)   ///< Output disable register address.
	#define PIOB_OSR    PIOB_ACCESS(PIO_OSR_OFF)   ///< Output status register address.
	#define PIOB_IFER   PIOB_ACCESS(PIO_IFER_OFF)  ///< Input filter enable register address.
	#define PIOB_IFDR   PIOB_ACCESS(PIO_IFDR_OFF)  ///< Input filter disable register address.
	#define PIOB_IFSR   PIOB_ACCESS(PIO_IFSR_OFF)  ///< Input filter status register address.
	#define PIOB_SODR   PIOB_ACCESS(PIO_SODR_OFF)  ///< Set output data register address.
	#define PIOB_CODR   PIOB_ACCESS(PIO_CODR_OFF)  ///< Clear output data register address.
	#define PIOB_ODSR   PIOB_ACCESS(PIO_ODSR_OFF)  ///< Output data status register address.
	#define PIOB_PDSR   PIOB_ACCESS(PIO_PDSR_OFF)  ///< Pin data status register address.
	#define PIOB_IER    PIOB_ACCESS(PIO_IER_OFF)   ///< Interrupt enable register address.
	#define PIOB_IDR    PIOB_ACCESS(PIO_IDR_OFF)   ///< Interrupt disable register address.
	#define PIOB_IMR    PIOB_ACCESS(PIO_IMR_OFF)   ///< Interrupt mask register address.
	#define PIOB_ISR    PIOB_ACCESS(PIO_ISR_OFF)   ///< Interrupt status register address.
	#if PIO_HAS_MULTIDRIVER
	#define PIOB_MDER   PIOB_ACCESS(PIO_MDER_OFF)  ///< Multi-driver enable register address.
	#define PIOB_MDDR   PIOB_ACCESS(PIO_MDDR_OFF)  ///< Multi-driver disable register address.
	#define PIOB_MDSR   PIOB_ACCESS(PIO_MDSR_OFF)  ///< Multi-driver status register address.
	#endif /* PIO_HAS_MULTIDRIVER */
	#if PIO_HAS_PULLUP
	#define PIOB_PUDR   PIOB_ACCESS(PIO_PUDR_OFF)  ///< Pull-up disable register address.
	#define PIOB_PUER   PIOB_ACCESS(PIO_PUER_OFF)  ///< Pull-up enable register address.
	#define PIOB_PUSR   PIOB_ACCESS(PIO_PUSR_OFF)  ///< Pull-up status register address.
	#endif /* PIO_HAS_PULLUP */
	#if PIO_HAS_PERIPHERALSELECT
	#define PIOB_ASR    PIOB_ACCESS(PIO_ASR_OFF)   ///< PIO peripheral A select register address.
	#define PIOB_BSR    PIOB_ACCESS(PIO_BSR_OFF)   ///< PIO peripheral B select register address.
	#define PIOB_ABSR   PIOB_ACCESS(PIO_ABSR_OFF)  ///< PIO peripheral AB status register address.
	#endif /* PIO_HAS_PERIPHERALSELECT */
	#if PIO_HAS_OUTPUTWRITEENABLE
	#define PIOB_OWER   PIOB_ACCESS(PIO_OWER_OFF)  ///< PIO output write enable register address.
	#define PIOB_OWDR   PIOB_ACCESS(PIO_OWDR_OFF)  ///< PIO output write disable register address.
	#define PIOB_OWSR   PIOB_ACCESS(PIO_OWSR_OFF)  ///< PIO output write status register address.
	#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOB_BASE */
/*\}*/

/** PIO C Register Addresses */
/*\{*/
#if defined(PIOC_BASE)
	#define PIOC_ACCESS(offset) (*((reg32_t *)(PIOC_BASE + (offset))))

	#define PIOC_PER    PIOC_ACCESS(PIO_PER_OFF)   ///< PIO enable register address.
	#define PIOC_PDR    PIOC_ACCESS(PIO_PDR_OFF)   ///< PIO disable register address.
	#define PIOC_PSR    PIOC_ACCESS(PIO_PSR_OFF)   ///< PIO status register address.
	#define PIOC_OER    PIOC_ACCESS(PIO_OER_OFF)   ///< Output enable register address.
	#define PIOC_ODR    PIOC_ACCESS(PIO_ODR_OFF)   ///< Output disable register address.
	#define PIOC_OSR    PIOC_ACCESS(PIO_OSR_OFF)   ///< Output status register address.
	#define PIOC_IFER   PIOC_ACCESS(PIO_IFER_OFF)  ///< Input filter enable register address.
	#define PIOC_IFDR   PIOC_ACCESS(PIO_IFDR_OFF)  ///< Input filter disable register address.
	#define PIOC_IFSR   PIOC_ACCESS(PIO_IFSR_OFF)  ///< Input filter status register address.
	#define PIOC_SODR   PIOC_ACCESS(PIO_SODR_OFF)  ///< Set output data register address.
	#define PIOC_CODR   PIOC_ACCESS(PIO_CODR_OFF)  ///< Clear output data register address.
	#define PIOC_ODSR   PIOC_ACCESS(PIO_ODSR_OFF)  ///< Output data status register address.
	#define PIOC_PDSR   PIOC_ACCESS(PIO_PDSR_OFF)  ///< Pin data status register address.
	#define PIOC_IER    PIOC_ACCESS(PIO_IER_OFF)   ///< Interrupt enable register address.
	#define PIOC_IDR    PIOC_ACCESS(PIO_IDR_OFF)   ///< Interrupt disable register address.
	#define PIOC_IMR    PIOC_ACCESS(PIO_IMR_OFF)   ///< Interrupt mask register address.
	#define PIOC_ISR    PIOC_ACCESS(PIO_ISR_OFF)   ///< Interrupt status register address.
	#if PIO_HAS_MULTIDRIVER
	#define PIOC_MDER   PIOC_ACCESS(PIO_MDER_OFF)  ///< Multi-driver enable register address.
	#define PIOC_MDDR   PIOC_ACCESS(PIO_MDDR_OFF)  ///< Multi-driver disable register address.
	#define PIOC_MDSR   PIOC_ACCESS(PIO_MDSR_OFF)  ///< Multi-driver status register address.
	#endif /* PIO_HAS_MULTIDRIVER */
	#if PIO_HAS_PULLUP
	#define PIOC_PUDR   PIOC_ACCESS(PIO_PUDR_OFF)  ///< Pull-up disable register address.
	#define PIOC_PUER   PIOC_ACCESS(PIO_PUER_OFF)  ///< Pull-up enable register address.
	#define PIOC_PUSR   PIOC_ACCESS(PIO_PUSR_OFF)  ///< Pull-up status register address.
	#endif /* PIO_HAS_PULLUP */
	#if PIO_HAS_PERIPHERALSELECT
	#define PIOC_ASR    PIOC_ACCESS(PIO_ASR_OFF)   ///< PIO peripheral A select register address.
	#define PIOC_BSR    PIOC_ACCESS(PIO_BSR_OFF)   ///< PIO peripheral B select register address.
	#define PIOC_ABSR   PIOC_ACCESS(PIO_ABSR_OFF)  ///< PIO peripheral AB status register address.
	#endif /* PIO_HAS_PERIPHERALSELECT */
	#if PIO_HAS_OUTPUTWRITEENABLE
	#define PIOC_OWER   PIOC_ACCESS(PIO_OWER_OFF)  ///< PIO output write enable register address.
	#define PIOC_OWDR   PIOC_ACCESS(PIO_OWDR_OFF)  ///< PIO output write disable register address.
	#define PIOC_OWSR   PIOC_ACCESS(PIO_OWSR_OFF)  ///< PIO output write status register address.
	#endif /* PIO_HAS_OUTPUTWRITEENABLE */
#endif /* PIOC_BASE */
/*\}*/

#endif /* AT91_PIO_H */
