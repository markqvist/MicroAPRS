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
 * \brief High level serial I/O API.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "ser"
 * $WIZ$ module_depends = "kfile", "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_ser.h"
 * $WIZ$ module_hw = "bertos/hw/hw_ser.h"
 * $WIZ$ module_supports =  "not atmega103 and not atmega8"
 */

#ifndef DRV_SER_H
#define DRV_SER_H

#include <io/kfile.h>
#include <struct/fifobuf.h>
#include <cfg/compiler.h>

#if OS_HOSTED
	#include <cfg/macros.h> /* BV() */

	typedef uint16_t serstatus_t;

	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /**< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(1)  /**< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(2)  /**< Transmit timeout */

	/* Hardware errors */
	#define SERRF_RXSROVERRUN    0      /**< Rx shift register overrun, unsupported in emulated serial port. */
	#define SERRF_FRAMEERROR     0      /**< Stop bit missing, unsupported in emulated serial port. */
	#define SERRF_PARITYERROR    0      /**< Parity error, unsupported in emulated serial port. */
	#define SERRF_NOISEERROR     0      /**< Noise error, unsupported in emulated serial port. */

	enum
	{
		SER_UART0,
		SER_UART1,

		SER_CNT  /**< Number of serial ports */
	};

#else
	#include CPU_HEADER(ser)
#endif

#include "cfg/cfg_ser.h"



/**
 * \name Masks to group TX/RX errors.
 * \{
 */
#define SERRF_RX \
	( SERRF_RXFIFOOVERRUN \
	| SERRF_RXTIMEOUT \
	| SERRF_RXSROVERRUN \
	| SERRF_PARITYERROR \
	| SERRF_FRAMEERROR \
	| SERRF_NOISEERROR)       /**< All possible rx errors */
#define SERRF_TX  (SERRF_TXTIMEOUT)  /**< All possible tx errors */
/*\}*/

/**
 * \name LSB or MSB first data order for SPI driver.
 *
 * $WIZ$ ser_order_bit = "SER_MSB_FIRST", "SER_LSB_FIRST"
 */
#define SER_MSB_FIRST 0
#define SER_LSB_FIRST 1

/**
 * \name Parity settings for ser_setparity().
 *
 * \note Values are AVR-specific for performance reasons.
 *       Other processors should either decode them or
 *       redefine these macros.
 * \{
 */
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/


/**
 * \def CONFIG_SER_STROBE
 *
 * This is a debug facility that can be used to
 * monitor SER interrupt activity on an external pin.
 *
 * To use strobes, redefine the macros SER_STROBE_ON,
 * SER_STROBE_OFF and SER_STROBE_INIT and set
 * CONFIG_SER_STROBE to 1.
 */
#if !defined(CONFIG_SER_STROBE) || !CONFIG_SER_STROBE
	#define SER_STROBE_ON    do {/*nop*/} while(0)
	#define SER_STROBE_OFF   do {/*nop*/} while(0)
	#define SER_STROBE_INIT  do {/*nop*/} while(0)
#endif

struct SerialHardware;

/** Human-readable serial error descriptions */
extern const char * const serial_errors[8];

/** Serial handle structure */
typedef struct Serial
{
	/** Serial have a KFile struct implementation **/
	KFile fd;

	/** Physical port number */
	unsigned int unit;

#ifdef _DEBUG
	bool is_open;
#endif

	/**
	 * \name Transmit and receive FIFOs.
	 *
	 * Declared volatile because handled asinchronously by interrupts.
	 *
	 * \{
	 */
	FIFOBuffer txfifo;
	FIFOBuffer rxfifo;
	/* \} */

#if CONFIG_SER_RXTIMEOUT != -1
	ticks_t rxtimeout;
#endif
#if CONFIG_SER_TXTIMEOUT != -1
	ticks_t txtimeout;
#endif

	/** Holds the flags defined above.  Will be 0 when no errors have occurred. */
	volatile serstatus_t status;

	/** Low-level interface to hardware. */
	struct SerialHardware* hw;
} Serial;


/**
 * ID for serial.
 */
#define KFT_SERIAL MAKE_ID('S', 'E', 'R', 'L')


INLINE Serial * SERIAL_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_SERIAL);
	return (Serial *)fd;
}

/* Function prototypes */
//extern int ser_getchar_nowait(struct Serial *port);

void ser_setbaudrate(struct Serial *fd, unsigned long rate);
void ser_setparity(struct Serial *fd, int parity);
void ser_settimeouts(struct Serial *fd, mtime_t rxtimeout, mtime_t txtimeout);
void ser_resync(struct Serial *fd, mtime_t delay);
int ser_getchar_nowait(struct Serial *fd);

void ser_purgeRx(struct Serial *fd);
void ser_purgeTx(struct Serial *fd);
void ser_purge(struct Serial *fd);
void ser_init(struct Serial *fds, unsigned int unit);
void spimaster_init(Serial *fds, unsigned int unit);


/**
 * \name Additional functions implemented as macros
 *
 * \{
 */
#define ser_getstatus(serial)    ((serial)->status)
#define ser_setstatus(serial, new_status) ((serial)->status = (new_status))
/* \} */

#endif /* DRV_SER_H */
