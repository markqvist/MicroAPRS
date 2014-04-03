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
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief High level serial I/O API
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef SER_AT91_H
#define SER_AT91_H

#include <cfg/macros.h> /* BV() */
#include <cfg/compiler.h>  /* uint32_t */
#include <cpu/detect.h>  /* CPU_* */

/** \name Serial Error/status flags. */
/*\{*/
typedef uint32_t serstatus_t;

/* Software errors */
#define SERRF_RXFIFOOVERRUN  BV(0)  /**< Rx FIFO buffer overrun */
#define SERRF_RXTIMEOUT      BV(1)  /**< Receive timeout */
#define SERRF_TXTIMEOUT      BV(2)  /**< Transmit timeout */

/*
 * Hardware errors.
 * These flags map directly to the ARM USART Channel Status Register (US_CSR).
 */
#define SERRF_RXSROVERRUN    BV(5)  /**< Rx shift register overrun */
#define SERRF_FRAMEERROR     BV(6)  /**< Stop bit missing */
#define SERRF_PARITYERROR    BV(7)  /**< Parity error */
#define SERRF_NOISEERROR     0      /**< Unsupported */
/*\}*/

/**
 * \name Serial hw numbers
 *
 * \{
 */
enum
{
SER_UART0,
SER_UART1,
SER_SPI0,
#if CPU_ARM_SAM7X
SER_SPI1,
#endif
SER_CNT  /**< Number of serial ports */
};
/*\}*/

#endif /* SER_AT91_H */
