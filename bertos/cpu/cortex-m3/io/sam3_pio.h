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
 * Copyright 2007,2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * Atmel SAM3 Parallel input/output controller.
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

#ifndef SAM3_PIO_H
#define SAM3_PIO_H

/**
 * PIO registers base addresses.
 */
/*\{*/
#define PIOA_BASE  0x400E0E00
#define PIOB_BASE  0x400E1000
#define PIOC_BASE  0x400E1200

#if CPU_CM3_SAM3X
	#define PIOD_BASE  0x400E1400
	#define PIOE_BASE  0x400E1600
	#define PIOF_BASE  0x400E1800
#endif
/*\}*/

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
#define PIO_MDER_OFF    0x00000050  ///< Multi-driver enable register offset.
#define PIO_MDDR_OFF    0x00000054  ///< Multi-driver disable register offset.
#define PIO_MDSR_OFF    0x00000058  ///< Multi-driver status register offset.
#define PIO_PUDR_OFF    0x00000060  ///< Pull-up disable register offset.
#define PIO_PUER_OFF    0x00000064  ///< Pull-up enable register offset.
#define PIO_PUSR_OFF    0x00000068  ///< Pull-up status register offset.
#if CPU_CM3_SAM3X || CPU_CM3_SAM3U
	#define PIO_ABSR_OFF     0x00000070  ///< PIO peripheral select register offset.
#elif CPU_CM3_SAM3N || CPU_CM3_SAM3S
	#define PIO_ABCDSR1_OFF  0x00000070  ///< PIO peripheral select register 1 offset.
	#define PIO_ABCDSR2_OFF  0x00000074  ///< PIO peripheral select register 2 offset.
#else
	#error Undefined PIO peripheral select register for selected cpu
#endif
#define PIO_OWER_OFF    0x000000A0  ///< PIO output write enable register offset.
#define PIO_OWDR_OFF    0x000000A4  ///< PIO output write disable register offset.
#define PIO_OWSR_OFF    0x000000A8  ///< PIO output write status register offset.
/*\}*/


#if defined(PIOA_BASE)
	/** PIO A Register Addresses */
	/*\{*/
	#define PIOA_ACCESS(offset) (*((reg32_t *)(PIOA_BASE + (offset))))

	#define PIOA_PER      PIOA_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOA_PDR      PIOA_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOA_PSR      PIOA_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOA_OER      PIOA_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOA_ODR      PIOA_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOA_OSR      PIOA_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOA_IFER     PIOA_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOA_IFDR     PIOA_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOA_IFSR     PIOA_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOA_SODR     PIOA_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOA_CODR     PIOA_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOA_ODSR     PIOA_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOA_PDSR     PIOA_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOA_IER      PIOA_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOA_IDR      PIOA_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOA_IMR      PIOA_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOA_ISR      PIOA_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOA_MDER     PIOA_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOA_MDDR     PIOA_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOA_MDSR     PIOA_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOA_PUDR     PIOA_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOA_PUER     PIOA_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOA_PUSR     PIOA_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOA_ABSR     PIOA_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOA_ABCDSR1  PIOA_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOA_ABCDSR2  PIOA_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOA_OWER     PIOA_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOA_OWDR     PIOA_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOA_OWSR     PIOA_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOA_BASE */

#if defined(PIOB_BASE)
	/** PIO B Register Addresses */
	/*\{*/
	#define PIOB_ACCESS(offset) (*((reg32_t *)(PIOB_BASE + (offset))))

	#define PIOB_PER      PIOB_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOB_PDR      PIOB_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOB_PSR      PIOB_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOB_OER      PIOB_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOB_ODR      PIOB_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOB_OSR      PIOB_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOB_IFER     PIOB_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOB_IFDR     PIOB_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOB_IFSR     PIOB_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOB_SODR     PIOB_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOB_CODR     PIOB_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOB_ODSR     PIOB_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOB_PDSR     PIOB_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOB_IER      PIOB_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOB_IDR      PIOB_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOB_IMR      PIOB_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOB_ISR      PIOB_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOB_MDER     PIOB_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOB_MDDR     PIOB_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOB_MDSR     PIOB_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOB_PUDR     PIOB_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOB_PUER     PIOB_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOB_PUSR     PIOB_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOB_ABSR     PIOB_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOB_ABCDSR1  PIOB_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOB_ABCDSR2  PIOB_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOB_OWER     PIOB_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOB_OWDR     PIOB_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOB_OWSR     PIOB_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOB_BASE */

#if defined(PIOC_BASE)
	/** PIO C Register Addresses */
	/*\{*/
	#define PIOC_ACCESS(offset) (*((reg32_t *)(PIOC_BASE + (offset))))

	#define PIOC_PER      PIOC_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOC_PDR      PIOC_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOC_PSR      PIOC_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOC_OER      PIOC_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOC_ODR      PIOC_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOC_OSR      PIOC_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOC_IFER     PIOC_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOC_IFDR     PIOC_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOC_IFSR     PIOC_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOC_SODR     PIOC_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOC_CODR     PIOC_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOC_ODSR     PIOC_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOC_PDSR     PIOC_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOC_IER      PIOC_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOC_IDR      PIOC_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOC_IMR      PIOC_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOC_ISR      PIOC_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOC_MDER     PIOC_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOC_MDDR     PIOC_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOC_MDSR     PIOC_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOC_PUDR     PIOC_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOC_PUER     PIOC_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOC_PUSR     PIOC_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOC_ABSR     PIOC_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOC_ABCDSR1  PIOC_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOC_ABCDSR2  PIOC_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOC_OWER     PIOC_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOC_OWDR     PIOC_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOC_OWSR     PIOC_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOC_BASE */

#if defined(PIOD_BASE)
	/** PIO C Register Addresses */
	/*\{*/
	#define PIOD_ACCESS(offset) (*((reg32_t *)(PIOD_BASE + (offset))))

	#define PIOD_PER      PIOD_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOD_PDR      PIOD_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOD_PSR      PIOD_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOD_OER      PIOD_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOD_ODR      PIOD_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOD_OSR      PIOD_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOD_IFER     PIOD_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOD_IFDR     PIOD_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOD_IFSR     PIOD_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOD_SODR     PIOD_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOD_CODR     PIOD_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOD_ODSR     PIOD_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOD_PDSR     PIOD_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOD_IER      PIOD_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOD_IDR      PIOD_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOD_IMR      PIOD_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOD_ISR      PIOD_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOD_MDER     PIOD_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOD_MDDR     PIOD_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOD_MDSR     PIOD_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOD_PUDR     PIOD_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOD_PUER     PIOD_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOD_PUSR     PIOD_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOD_ABSR     PIOD_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOD_ABCDSR1  PIOD_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOD_ABCDSR2  PIOD_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOD_OWER     PIOD_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOD_OWDR     PIOD_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOD_OWSR     PIOD_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOD_BASE */

#if defined(PIOE_BASE)
	/** PIO C Register Addresses */
	/*\{*/
	#define PIOE_ACCESS(offset) (*((reg32_t *)(PIOE_BASE + (offset))))

	#define PIOE_PER      PIOE_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOE_PDR      PIOE_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOE_PSR      PIOE_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOE_OER      PIOE_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOE_ODR      PIOE_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOE_OSR      PIOE_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOE_IFER     PIOE_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOE_IFDR     PIOE_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOE_IFSR     PIOE_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOE_SODR     PIOE_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOE_CODR     PIOE_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOE_ODSR     PIOE_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOE_PDSR     PIOE_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOE_IER      PIOE_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOE_IDR      PIOE_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOE_IMR      PIOE_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOE_ISR      PIOE_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOE_MDER     PIOE_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOE_MDDR     PIOE_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOE_MDSR     PIOE_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOE_PUDR     PIOE_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOE_PUER     PIOE_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOE_PUSR     PIOE_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOE_ABSR     PIOE_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOE_ABCDSR1  PIOE_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOE_ABCDSR2  PIOE_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOE_OWER     PIOE_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOE_OWDR     PIOE_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOE_OWSR     PIOE_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOE_BASE */

#if defined(PIOF_BASE)
	/** PIO C Register Addresses */
	/*\{*/
	#define PIOF_ACCESS(offset) (*((reg32_t *)(PIOF_BASE + (offset))))

	#define PIOF_PER      PIOF_ACCESS(PIO_PER_OFF)       ///< PIO enable register address.
	#define PIOF_PDR      PIOF_ACCESS(PIO_PDR_OFF)       ///< PIO disable register address.
	#define PIOF_PSR      PIOF_ACCESS(PIO_PSR_OFF)       ///< PIO status register address.
	#define PIOF_OER      PIOF_ACCESS(PIO_OER_OFF)       ///< Output enable register address.
	#define PIOF_ODR      PIOF_ACCESS(PIO_ODR_OFF)       ///< Output disable register address.
	#define PIOF_OSR      PIOF_ACCESS(PIO_OSR_OFF)       ///< Output status register address.
	#define PIOF_IFER     PIOF_ACCESS(PIO_IFER_OFF)      ///< Input filter enable register address.
	#define PIOF_IFDR     PIOF_ACCESS(PIO_IFDR_OFF)      ///< Input filter disable register address.
	#define PIOF_IFSR     PIOF_ACCESS(PIO_IFSR_OFF)      ///< Input filter status register address.
	#define PIOF_SODR     PIOF_ACCESS(PIO_SODR_OFF)      ///< Set output data register address.
	#define PIOF_CODR     PIOF_ACCESS(PIO_CODR_OFF)      ///< Clear output data register address.
	#define PIOF_ODSR     PIOF_ACCESS(PIO_ODSR_OFF)      ///< Output data status register address.
	#define PIOF_PDSR     PIOF_ACCESS(PIO_PDSR_OFF)      ///< Pin data status register address.
	#define PIOF_IER      PIOF_ACCESS(PIO_IER_OFF)       ///< Interrupt enable register address.
	#define PIOF_IDR      PIOF_ACCESS(PIO_IDR_OFF)       ///< Interrupt disable register address.
	#define PIOF_IMR      PIOF_ACCESS(PIO_IMR_OFF)       ///< Interrupt mask register address.
	#define PIOF_ISR      PIOF_ACCESS(PIO_ISR_OFF)       ///< Interrupt status register address.
	#define PIOF_MDER     PIOF_ACCESS(PIO_MDER_OFF)      ///< Multi-driver enable register address.
	#define PIOF_MDDR     PIOF_ACCESS(PIO_MDDR_OFF)      ///< Multi-driver disable register address.
	#define PIOF_MDSR     PIOF_ACCESS(PIO_MDSR_OFF)      ///< Multi-driver status register address.
	#define PIOF_PUDR     PIOF_ACCESS(PIO_PUDR_OFF)      ///< Pull-up disable register address.
	#define PIOF_PUER     PIOF_ACCESS(PIO_PUER_OFF)      ///< Pull-up enable register address.
	#define PIOF_PUSR     PIOF_ACCESS(PIO_PUSR_OFF)      ///< Pull-up status register address.
#ifdef PIO_ABSR_OFF
	#define PIOF_ABSR     PIOF_ACCESS(PIO_ABSR_OFF)      ///< PIO peripheral select register address.
#else
	#define PIOF_ABCDSR1  PIOF_ACCESS(PIO_ABCDSR1_OFF)   ///< PIO peripheral select register 1 address.
	#define PIOF_ABCDSR2  PIOF_ACCESS(PIO_ABCDSR2_OFF)   ///< PIO peripheral select register 2 address.
#endif
	#define PIOF_OWER     PIOF_ACCESS(PIO_OWER_OFF)      ///< PIO output write enable register address.
	#define PIOF_OWDR     PIOF_ACCESS(PIO_OWDR_OFF)      ///< PIO output write disable register address.
	#define PIOF_OWSR     PIOF_ACCESS(PIO_OWSR_OFF)      ///< PIO output write status register address.
	/*\}*/
#endif /* PIOF_BASE */


#define PIO_PERIPH_A  0
#define PIO_PERIPH_B  1
#ifdef PIO_ABCDSR1_OFF
	#define PIO_PERIPH_C  2
	#define PIO_PERIPH_D  3
#endif

/**
 * Set peripheral on I/O ports.
 *
 * \param  base      PIO port base
 * \param  mask      mask of I/O pin to manipulate
 * \param  function  function to assign to selected pins (PIO_PERIPH_A, B, ...)
 */
#ifdef PIO_ABCDSR1_OFF
	#define PIO_PERIPH_SEL(base, mask, function) do { \
		HWREG((base) + PIO_ABCDSR1_OFF) &= ~(mask); \
		HWREG((base) + PIO_ABCDSR2_OFF) &= ~(mask); \
		if ((function) & 1) \
			HWREG((base) + PIO_ABCDSR1_OFF) |= (mask); \
		if ((function) & 2) \
			HWREG((base) + PIO_ABCDSR2_OFF) |= (mask); \
	} while (0)
#else
	#define PIO_PERIPH_SEL(base, mask, function) do { \
		HWREG((base) + PIO_ABSR_OFF) &= ~(mask); \
		if ((function) & 1) \
			HWREG((base) + PIO_ABSR_OFF) |= (mask); \
	} while (0)
#endif


#endif /* SAM3_PIO_H */
