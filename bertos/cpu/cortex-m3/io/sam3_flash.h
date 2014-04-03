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
 * \brief Atmel SAM3 enhanced embedded flash controller definitions.
 */

#ifndef SAM3_FLASH_H
#define SAM3_FLASH_H

/**
 * EEFC base registers addresses.
 */
/*\{*/
#define EEFC0_BASE  0x400E0A00
#ifdef CPU_CM3_SAM3X
	#define EEFC1_BASE  0x400E0C00
#endif
/*\}*/


/**
 * EFC register offsets.
 */
/*\{*/
#define EEFC_FMR_OFF  0x0   ///< Flash Mode Register
#define EEFC_FCR_OFF  0x4   ///< Flash Command Register
#define EEFC_FSR_OFF  0x8   ///< Flash Status Register
#define EEFC_FRR_OFF  0xC   ///< Flash Result Register
/*\}*/

/**
 * EEFC registers.
 */
/*\{*/
#define EEFC0_FMR  (*((reg32_t *)(EEFC0_BASE + EEFC_FMR_OFF)))  ///< Flash Mode Register
#define EEFC0_FCR  (*((reg32_t *)(EEFC0_BASE + EEFC_FCR_OFF)))  ///< Flash Command Register
#define EEFC0_FSR  (*((reg32_t *)(EEFC0_BASE + EEFC_FSR_OFF)))  ///< Flash Status Register
#define EEFC0_FRR  (*((reg32_t *)(EEFC0_BASE + EEFC_FRR_OFF)))  ///< Flash Result Register

#ifdef CPU_CM3_SAM3X
	#define EEFC1_FMR  (*((reg32_t *)(EEFC1_BASE + EEFC_FMR_OFF)))  ///< Flash Mode Register
	#define EEFC1_FCR  (*((reg32_t *)(EEFC1_BASE + EEFC_FCR_OFF)))  ///< Flash Command Register
	#define EEFC1_FSR  (*((reg32_t *)(EEFC1_BASE + EEFC_FSR_OFF)))  ///< Flash Status Register
	#define EEFC1_FRR  (*((reg32_t *)(EEFC1_BASE + EEFC_FRR_OFF)))  ///< Flash Result Register
#endif
/*\}*/



/**
 * Defines for bit fields in EEFC_FMR register.
 */
/*\{*/
#define EEFC_FMR_FRDY        0                       ///< Ready Interrupt Enable
#define EEFC_FMR_FWS_SHIFT   8
#define EEFC_FMR_FWS_MASK    (0xf << EEFC_FMR_FWS_SHIFT) ///< Flash Wait State
#define EEFC_FMR_FWS(value)  (EEFC_FMR_FWS_MASK & ((value) << EEFC_FMR_FWS_SHIFT))
#define EEFC_FMR_FAM         24                      ///< Flash Access Mode
/*\}*/

/**
 * Defines for bit fields in EEFC_FCR register.
 */
/*\{*/
#define EEFC_FCR_FCMD_MASK    0xff                        ///< Flash Command
#define EEFC_FCR_FCMD(value)  (EEFC_FCR_FCMD_MASK & (value))
#define EEFC_FCR_FARG_SHIFT   8
#define EEFC_FCR_FARG_MASK    (0xffff << EEFC_FCR_FARG_SHIFT) ///< Flash Command Argument
#define EEFC_FCR_FARG(value)  (EEFC_FCR_FARG_MASK & ((value) << EEFC_FCR_FARG_SHIFT))
#define EEFC_FCR_FKEY_SHIFT   24
#define EEFC_FCR_FKEY_MASK    (0xff << EEFC_FCR_FKEY_SHIFT)   ///< Flash Writing Protection Key
#define EEFC_FCR_FKEY(value)  (EEFC_FCR_FKEY_MASK & ((value) << EEFC_FCR_FKEY_SHIFT))
/*\}*/

/**
 * Defines for bit fields in EEFC_FSR register.
 */
/*\{*/
#define EEFC_FSR_FRDY       0  ///< Flash Ready Status
#define EEFC_FSR_FCMDE      1  ///< Flash Command Error Status
#define EEFC_FSR_FLOCKE     2  ///< Flash Lock Error Status
/*\}*/

#endif /* SAM3_FLASH_H */
