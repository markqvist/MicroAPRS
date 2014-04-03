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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Serial port emulator for hosted environments.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "cfg/cfg_ser.h"

#include <cfg/debug.h>
#include <cfg/compiler.h>

#include <drv/ser.h>
#include <drv/ser_p.h>

#include <struct/fifobuf.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h> /* open() */
#include <unistd.h> /* read(), write() */


/* From the high-level serial driver */
extern struct Serial ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];


/**
 * Internal state structure
 */
struct EmulSerial
{
	struct SerialHardware hw;
	struct Serial *ser;
	int fd;
};


/*
 * Callbacks
 */
static void uart_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	hw->ser = ser;
	hw->fd = open("/dev/ttyS0", O_RDWR);
}

static void uart_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	close(hw->fd);
	hw->fd = -1;
}

static void uart_txStart(struct SerialHardware * _hw)
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	while(!fifo_isempty(&hw->ser->txfifo))
	{
		char c = fifo_pop(&hw->ser->txfifo);
		write(hw->fd, &c, 1);
	}
}

static bool uart_txSending(UNUSED_ARG(struct SerialHardware *, _hw))
{
	return false;
}


static void uart_setBaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	TRACEMSG("rate=%lu", rate);
	// TODO

}

static void uart_setParity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	TRACEMSG("parity=%d", parity);
	// TODO
}

// FIXME: move into compiler.h?  Ditch?
#if COMPILER_C99
	#define	C99INIT(name,val) .name = val
#elif defined(__GNUC__)
	#define C99INIT(name,val) name: val
#else
	#warning No designated initializers, double check your code
	#define C99INIT(name,val) (val)
#endif

/*
 * High-level interface data structures.
 */
static const struct SerialHardwareVT uart_vtable =
{
	C99INIT(init, uart_init),
	C99INIT(cleanup, uart_cleanup),
	C99INIT(setBaudrate, uart_setBaudrate),
	C99INIT(setParity, uart_setParity),
	C99INIT(txStart, uart_txStart),
	C99INIT(txSending, uart_txSending),
};

static struct EmulSerial UARTDescs[SER_CNT] =
{
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &uart_vtable),
			C99INIT(txbuffer, uart0_txbuffer),
			C99INIT(rxbuffer, uart0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart0_rxbuffer)),
		},
		C99INIT(ser, NULL),
		C99INIT(fd, -1),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &uart_vtable),
			C99INIT(txbuffer, uart1_txbuffer),
			C99INIT(rxbuffer, uart1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart1_rxbuffer)),
		},
		C99INIT(ser, NULL),
		C99INIT(fd, -1),
	},
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}
