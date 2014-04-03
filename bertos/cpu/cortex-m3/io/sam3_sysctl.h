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
 * \brief Atmel SAM3 system controller registers.
 */

#ifndef SAM3_SYSCTL_H
#define SAM3_SYSCTL_H

/** Supply controller base */
#if CPU_CM3_SAM3X
	#define SUPC_BASE  0x400E1A10
#else
	#define SUPC_BASE  0x400E1410
#endif

/**
 * Supply controller offsets.
 */
/*\{*/
#define SUPC_CR_OFF    0x0   ///< Supply Controller Control
#define SUPC_SMMR_OFF  0x4   ///< Supply Controller Supply Monitor Mode
#define SUPC_MR_OFF    0x8   ///< Supply Controller Mode
#define SUPC_WUMR_OFF  0xC   ///< Supply Controller Wake Up Mode
#define SUPC_WUIR_OFF  0x10  ///< Supply Controller Wake Up Inputs
#define SUPC_SR_OFF    0x14  ///< Supply Controller Status
/*\}*/

/**
 * Supply controller registers.
 */
/*\{*/
#define SUPC_CR   (*((reg32_t *)(SUPC_BASE + SUPC_CR_OFF  )))  ///< Supply Controller Control
#define SUPC_SMMR (*((reg32_t *)(SUPC_BASE + SUPC_SMMR_OFF)))  ///< Supply Controller Supply Monitor Mode
#define SUPC_MR   (*((reg32_t *)(SUPC_BASE + SUPC_MR_OFF  )))  ///< Supply Controller Mode
#define SUPC_WUMR (*((reg32_t *)(SUPC_BASE + SUPC_WUMR_OFF)))  ///< Supply Controller Wake Up Mode
#define SUPC_WUIR (*((reg32_t *)(SUPC_BASE + SUPC_WUIR_OFF)))  ///< Supply Controller Wake Up Inputs
#define SUPC_SR   (*((reg32_t *)(SUPC_BASE + SUPC_SR_OFF  )))  ///< Supply Controller Status
/*\}*/

/**
 * Defines for bit fields in SUPC_CR register.
 */
/*\{*/
#define SUPC_CR_VROFF       2                     ///< Voltage Regulator Off
#define SUPC_CR_XTALSEL     3                     ///< Crystal Oscillator Select
#define SUPC_CR_KEY_P       24
#define SUPC_CR_KEY_MASK    (0xff << SUPC_CR_KEY_P)   ///< SUPC_CR key
#define SUPC_CR_KEY(value)  (SUPC_CR_KEY_MASK & ((value) << SUPC_CR_KEY_P))
/*\}*/

/**
 * Defines for bit fields in SUPC_SR register.
 */
/*\{*/
#define SUPC_SR_WKUPS     1   ///< WKUP Wake Up Status
#define SUPC_SR_SMWS      2   ///< Supply Monitor Detection Wake Up Status
#define SUPC_SR_BODRSTS   3   ///< Brownout Detector Reset Status
#define SUPC_SR_SMRSTS    4   ///< Supply Monitor Reset Status
#define SUPC_SR_SMS       5   ///< Supply Monitor Status
#define SUPC_SR_SMOS      6   ///< Supply Monitor Output Status
#define SUPC_SR_OSCSEL    7   ///< 32-kHz Oscillator Selection Status
#define SUPC_SR_WKUPIS0   16  ///< WKUP Input Status 0
#define SUPC_SR_WKUPIS1   17  ///< WKUP Input Status 1
#define SUPC_SR_WKUPIS2   18  ///< WKUP Input Status 2
#define SUPC_SR_WKUPIS3   19  ///< WKUP Input Status 3
#define SUPC_SR_WKUPIS4   20  ///< WKUP Input Status 4
#define SUPC_SR_WKUPIS5   21  ///< WKUP Input Status 5
#define SUPC_SR_WKUPIS6   22  ///< WKUP Input Status 6
#define SUPC_SR_WKUPIS7   23  ///< WKUP Input Status 7
#define SUPC_SR_WKUPIS8   24  ///< WKUP Input Status 8
#define SUPC_SR_WKUPIS9   25  ///< WKUP Input Status 9
#define SUPC_SR_WKUPIS10  26  ///< WKUP Input Status 10
#define SUPC_SR_WKUPIS11  27  ///< WKUP Input Status 11
#define SUPC_SR_WKUPIS12  28  ///< WKUP Input Status 12
#define SUPC_SR_WKUPIS13  29  ///< WKUP Input Status 13
#define SUPC_SR_WKUPIS14  30  ///< WKUP Input Status 14
#define SUPC_SR_WKUPIS15  31  ///< WKUP Input Status 15
/*\}*/

#endif /* SAM3_SYSCTL_H */
