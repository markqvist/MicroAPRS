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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 *  SAM3X PDC register definitions.
 */

#ifndef SAM3_PDC_H
#define SAM3_PDC_H


/**
 * PDC controller offset
 */
#define PERIPH_RPR_OFF                     0x100 ///< Receive Pointer Register.
#define PERIPH_RCR_OFF                     0x104 ///< Receive Counter Register.
#define PERIPH_TPR_OFF                     0x108 ///< Transmit Pointer Register.
#define PERIPH_TCR_OFF                     0x10C ///< Transmit Counter Register.
#define PERIPH_RNPR_OFF                    0x110 ///< Receive Next Pointer Register.
#define PERIPH_RNCR_OFF                    0x114 ///< Receive Next Counter Register.
#define PERIPH_TNPR_OFF                    0x118 ///< Transmit Next Pointer Register.
#define PERIPH_TNCR_OFF                    0x11C ///< Transmit Next Counter Register.
#define PERIPH_PTCR_OFF                    0x120 ///< Transfer Control Register.
#define PERIPH_PTSR_OFF                    0x124 ///< Transfer Status Register.


#define PDC_PTCR_RXTEN               0  ///< Receiver Transfer Enable.
#define PDC_PTCR_RXTDIS              1  ///< Receiver Transfer Disable.
#define PDC_PTCR_TXTEN               8  ///< Transmitter Transfer Enable.
#define PDC_PTCR_TXTDIS              9  ///< Transmitter Transfer Disable.
#define PDC_PTSR_RXTEN               0  ///< Receiver Transfer Enable.
#define PDC_PTSR_TXTEN               8  ///< Transmitter Transfer Enable.


#endif /* SAM3_PDC_H */
