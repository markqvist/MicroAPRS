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
 * \brief LPC23xx UART driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef SER_LPC2_H
#define SER_LPC2_H

#include <cfg/cfg_debug.h>
#include <io/lpc23xx.h>

/* Serial hardware numbers */
enum
{
	SER_UART0,
	SER_UART1,
	SER_UART2,
	SER_UART3,

	SER_CNT //< Number of serial ports
};

/* Software errors */
#define SERRF_RXFIFOOVERRUN  BV(0) //< Rx FIFO buffer overrun
#define SERRF_RXTIMEOUT      BV(1) //< Receive timeout
#define SERRF_TXTIMEOUT      BV(2) //< Transmit timeout

/*
 * Hardware errors.
 */
#define SERRF_RXSROVERRUN    0 //< Input overrun
#define SERRF_FRAMEERROR     0 //< Stop bit missing
#define SERRF_PARITYERROR    0 //< Parity error
#define SERRF_NOISEERROR     0 //< Noise error

/* Serial error/status flags */
typedef uint32_t serstatus_t;

void lpc2_uartInit(int port);

#endif /* SER_LPC2_H */
