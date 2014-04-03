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
 * -->
 *
 * \brief Simple serial I/O driver
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef DRV_SER_SIMPLE_AVR_H
#define DRV_SER_SIMPLE_AVR_H

#warning FIXME:This module is obsolete, yuo must refactor it.

#if 0
/* For checking which serial driver is linked */
#define SER_SIMPLE

#include <appconfig.h>
#include <cfg/compiler.h>


#if 0
#if CPU_AVR
	typedef uint8_t serstatus_t;

	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /**< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(5)  /**< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(6)  /**< Transmit timeout */

	/* Hardware errors */
	#define SERRF_RXSROVERRUN    BV(3)  /**< Rx shift register overrun */
	#define SERRF_FRAMEERROR     BV(4)  /**< Stop bit missing */
	#define SERRF_PARITYERROR    BV(7)  /**< Parity error */
#else
	#error unknown architecture
#endif
/*\}*/

/**
 * \name Serial hw numbers
 *
 * \{
 */
enum
{
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128
	SER_UART0,
	SER_UART1,
	SER_SPI,
#elif CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA8
	SER_UART0,
	SER_SPI,
#else
	#error unknown architecture
#endif
	SER_CNT  /**< Number of serial ports */
};
/*\}*/
#endif

/** \name Parity settings for ser_setparity() */
/*\{*/
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/


/** Serial handle structure */
struct Serial;

/* Function prototypes */
extern int _ser_putchar(int c);
extern int _ser_getchar(void);
extern int _ser_getchar_nowait(void);
/*
extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, void *buf, size_t size);

extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);

extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);
*/
extern int _ser_print(const char *s);

extern void _ser_setbaudrate(unsigned long rate);
extern void _ser_setparity(int parity);
extern void _ser_settimeouts(void);
extern void _ser_setstatus(void);
/*
extern void ser_resync(struct Serial *port, time_t delay);
extern void ser_drain(struct Serial *port);
*/
extern void _ser_purge(void);
extern struct Serial *_ser_open(void);
extern void _ser_close(void);

/**
 * \name Functions implemented as macros
 *
 * \{
 */
#define ser_putchar(c, port)        _ser_putchar(c)
#define ser_getchar(port)           _ser_getchar()
#define ser_getchar_nowait(port)    _ser_getchar_nowait()
#define ser_print(port, s)          _ser_print(s)
#define ser_setbaudrate(port, y)    _ser_setbaudrate(y)
#define ser_setparity(port, par)    _ser_setparity(par)
#define ser_settimeouts(port, y, z) _ser_settimeouts()
#define ser_purge(port)             _ser_purge()
#define ser_open(port)              _ser_open()
#define ser_getstatus(h)            0
#define ser_setstatus(h, x)         do {(void)(x);} while(0)
/* \} */

#endif /* DRV_SER_SIMPLE_AVR_H */

#endif
