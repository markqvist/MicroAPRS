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
 * Copyright 2007, 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * \brief Low-level serial module for AVR MEGA(interface).
 *
 */

#ifndef DRV_SER_MEGA_H
#define DRV_SER_MEGA_H

#include <cfg/macros.h> /* BV() */
#include <cfg/compiler.h>  /* uint32_t */

typedef uint8_t serstatus_t;

/* Software errors */
#define SERRF_RXFIFOOVERRUN  BV(0)  /**< Rx FIFO buffer overrun */
#define SERRF_RXTIMEOUT      BV(5)  /**< Receive timeout */
#define SERRF_TXTIMEOUT      BV(6)  /**< Transmit timeout */

/*
* Hardware errors.
* These flags map directly to the AVR UART Status Register (USR).
*/
#define SERRF_RXSROVERRUN    BV(3)  /**< Rx shift register overrun */
#define SERRF_FRAMEERROR     BV(4)  /**< Stop bit missing */
#define SERRF_PARITYERROR    BV(7)  /**< Parity error */
#define SERRF_NOISEERROR     0      /**< Unsupported */


/**
 * SPI clock polarity.
 *
 * $WIZ$ ser_spi_pol = "SPI_NORMAL_LOW", "SPI_NORMAL_HIGH"
 * }
 */
#define SPI_NORMAL_LOW      0
#define SPI_NORMAL_HIGH     1

/**
 * SPI clock phase.
 *
 * $WIZ$ ser_spi_phase = "SPI_SAMPLE_ON_FIRST_EDGE", "SPI_SAMPLE_ON_SECOND_EDGE"
 * }
 */
#define SPI_SAMPLE_ON_FIRST_EDGE    0
#define SPI_SAMPLE_ON_SECOND_EDGE   1

/**
 * \name Serial hw numbers
 *
 * \{
 */
enum
{
#if  CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	SER_UART0,
	SER_UART1,
	SER_UART2,
	SER_UART3,
	SER_SPI,
#elif  CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281
	SER_UART0,
	SER_UART1,
	SER_SPI,
#elif CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA32 || CPU_AVR_ATMEGA168 \
		|| CPU_AVR_ATMEGA328P
	SER_UART0,
	SER_SPI,
#else
	#error unknown architecture
#endif
	SER_CNT  /**< Number of serial ports */
};
/*\}*/

#endif /* DRV_SER_MEGA_H */
