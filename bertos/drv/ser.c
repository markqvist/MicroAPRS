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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Buffered serial I/O driver
 *
 * The serial rx interrupt buffers incoming data in a software FIFO
 * to decouple the higher level protocols from the line speed.
 * Outgoing data is buffered as well for better performance.
 * This driver is not optimized for best performance, but it
 * has proved to be fast enough to handle transfer rates up to
 * 38400bps on a 16MHz 80196.
 *
 * MODULE CONFIGURATION
 *
 *  \li \c CONFIG_SER_HWHANDSHAKE - set to 1 to enable RTS/CTS handshake.
 *         Support is incomplete/untested.
 *  \li \c CONFIG_SER_TXTIMEOUT - Enable software serial transmission timeouts
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "ser.h"
#include "wdt.h"
#include "timer.h"
#include "ser_p.h"

#include "cfg/cfg_ser.h"
#include "cfg/cfg_proc.h"
#include <cfg/debug.h>

#include <mware/formatwr.h>

#include <cpu/power.h> /* cpu_relax() */

#include <string.h> /* memset() */

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_KERN) || ((CONFIG_KERN != 0) && CONFIG_KERN != 1)
	#error CONFIG_KERN must be set to either 0 or 1 in cfg_kern.h
#endif
#if !defined(CONFIG_SER_RXTIMEOUT)
	#error CONFIG_SER_TXTIMEOUT missing in cfg_ser.h
#endif
#if !defined(CONFIG_SER_RXTIMEOUT)
	#error CONFIG_SER_RXTIMEOUT missing in cfg_ser.h
#endif
#if !defined(CONFIG_SER_DEFBAUDRATE)
	#error CONFIG_SER_DEFBAUDRATE missing in cfg_ser.h
#endif


struct Serial *ser_handles[SER_CNT];

/**
 * Insert \a c in tx FIFO buffer.
 * \note This function will switch out the calling process
 * if the tx buffer is full. If the buffer is full
 * and \a port->txtimeout is 0 return EOF immediatly.
 *
 * \return EOF on error or timeout, \a c otherwise.
 */
static int ser_putchar(int c, struct Serial *port)
{
	if (fifo_isfull_locked(&port->txfifo))
	{
#if CONFIG_SER_TXTIMEOUT != -1
		/* If timeout == 0 we don't want to wait */
		if (port->txtimeout == 0)
			return EOF;

		ticks_t start_time = timer_clock();
#endif

		/* Wait while buffer is full... */
		do
		{
			cpu_relax();

#if CONFIG_SER_TXTIMEOUT != -1
			if (timer_clock() - start_time >= port->txtimeout)
			{
				ATOMIC(port->status |= SERRF_TXTIMEOUT);
				return EOF;
			}
#endif /* CONFIG_SER_TXTIMEOUT */
		}
		while (fifo_isfull_locked(&port->txfifo));
	}

	fifo_push_locked(&port->txfifo, (unsigned char)c);

	/* (re)trigger tx interrupt */
	port->hw->table->txStart(port->hw);

	/* Avoid returning signed extended char */
	return (int)((unsigned char)c);
}


/**
 * Fetch a character from the rx FIFO buffer.
 * \note This function will switch out the calling process
 * if the rx buffer is empty. If the buffer is empty
 * and \a port->rxtimeout is 0 return EOF immediatly.
 *
 * \return EOF on error or timeout, \a c otherwise.
 */
static int ser_getchar(struct Serial *port)
{
	if (fifo_isempty_locked(&port->rxfifo))
	{
#if CONFIG_SER_RXTIMEOUT != -1
		/* If timeout == 0 we don't want to wait for chars */
		if (port->rxtimeout == 0)
			return EOF;

		ticks_t start_time = timer_clock();
#endif

		/* Wait while buffer is empty */
		do
		{
			cpu_relax();

#if CONFIG_SER_RXTIMEOUT != -1
			if (timer_clock() - start_time >= port->rxtimeout)
			{
				ATOMIC(port->status |= SERRF_RXTIMEOUT);
				return EOF;
			}
#endif /* CONFIG_SER_RXTIMEOUT */
		}
		while (fifo_isempty_locked(&port->rxfifo) && (ser_getstatus(port) & SERRF_RX) == 0);
	}

	/*
	 * Get a byte from the FIFO (avoiding sign-extension),
	 * re-enable RTS, then return result.
	 */
	if (ser_getstatus(port) & SERRF_RX)
		return EOF;
	return (int)(unsigned char)fifo_pop_locked(&port->rxfifo);
}

/**
 * Fetch a character from the rx FIFO buffer.
 * If the buffer is empty, ser_getchar_nowait() returns
 * EOF immediatly.
 * \note Deprecated, use ser_getchar with rx_timeout set to 0.
 */
int ser_getchar_nowait(struct Serial *fd)
{
	if (fifo_isempty_locked(&fd->rxfifo))
		return EOF;

	/* NOTE: the double cast prevents unwanted sign extension */
	return (int)(unsigned char)fifo_pop_locked(&fd->rxfifo);
}



/**
 * Read at most \a size bytes from \a port and put them in \a buf
 *
 * \return number of bytes actually read.
 */
static size_t ser_read(struct KFile *fd, void *_buf, size_t size)
{
	Serial *fds = SERIAL_CAST(fd);

	size_t i = 0;
	char *buf = (char *)_buf;
	int c;

	while (i < size)
	{
		if ((c = ser_getchar(fds)) == EOF)
			break;
		buf[i++] = c;
	}

	return i;
}

/**
 * \brief Write a buffer to serial.
 *
 * \return 0 if OK, EOF in case of error.
 *
 * \todo Optimize with fifo_pushblock()
 */
static size_t ser_write(struct KFile *fd, const void *_buf, size_t size)
{
	Serial *fds = SERIAL_CAST(fd);
	const char *buf = (const char *)_buf;
	size_t i = 0;

	while (size--)
	{
		if (ser_putchar(*buf++, fds) == EOF)
			break;
		i++;
	}
	return i;
}


#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
void ser_settimeouts(struct Serial *fd, mtime_t rxtimeout, mtime_t txtimeout)
{
	#if CONFIG_SER_RXTIMEOUT != -1
		fd->rxtimeout = ms_to_ticks(rxtimeout);
	#else
		(void)rxtimeout;
	#endif

	#if CONFIG_SER_TXTIMEOUT != -1
		fd->txtimeout = ms_to_ticks(txtimeout);
	#else
		(void)txtimeout;
	#endif
}
#endif /* CONFIG_SER_RXTIMEOUT || CONFIG_SER_TXTIMEOUT */


/**
 * Set the baudrate for the serial port
 */
void ser_setbaudrate(struct Serial *fd, unsigned long rate)
{
	fd->hw->table->setBaudrate(fd->hw, rate);
}


/**
 * Set the parity for the \a fd serial port
 */
void ser_setparity(struct Serial *fd, int parity)
{
	fd->hw->table->setParity(fd->hw, parity);
}

static int ser_error(struct KFile *fd)
{
	Serial *fds = SERIAL_CAST(fd);
	return ser_getstatus(fds);
}

static void ser_clearerr(struct KFile *fd)
{
	Serial *fds = SERIAL_CAST(fd);
	ser_setstatus(fds, 0);
}



/**
 * Flush both the RX and TX buffers.
 */
void ser_purge(struct Serial *fd)
{
	ser_purgeRx(fd);
	ser_purgeTx(fd);
}

/**
 * Flush RX buffer.
 */
void ser_purgeRx(struct Serial *fd)
{
	fifo_flush_locked(&fd->rxfifo);
}

/**
 * Flush TX buffer.
 */
void ser_purgeTx(struct Serial *fd)
{
	fifo_flush_locked(&fd->txfifo);
}


/**
 * Wait until all pending output is completely
 * transmitted to the other end.
 *
 * \note The current implementation only checks the
 *       software transmission queue. Any hardware
 *       FIFOs are ignored.
 */
static int ser_flush(struct KFile *fd)
{
	Serial *fds = SERIAL_CAST(fd);

	/*
	 * Wait until the FIFO becomes empty, and then until the byte currently in
	 * the hardware register gets shifted out.
	 */
	while (!fifo_isempty(&fds->txfifo)
	       || fds->hw->table->txSending(fds->hw))
		cpu_relax();
	return 0;
}


/**
 * Initialize a serial port.
 *
 * \param fd KFile Serial struct interface.
 * \param unit  Serial unit to open. Possible values are architecture dependant.
 */
static struct Serial *ser_open(struct Serial *fd, unsigned int unit)
{
	ASSERT(unit < countof(ser_handles));

	ser_handles[unit] = fd;
	ASSERT(!fd->is_open);
	DB(fd->is_open = true);

	fd->unit = unit;

	fd->hw = ser_hw_getdesc(unit);

	/* Initialize circular buffers */
	ASSERT(fd->hw->txbuffer);
	ASSERT(fd->hw->rxbuffer);
	fifo_init(&fd->txfifo, fd->hw->txbuffer, fd->hw->txbuffer_size);
	fifo_init(&fd->rxfifo, fd->hw->rxbuffer, fd->hw->rxbuffer_size);

	fd->hw->table->init(fd->hw, fd);

	/* Set default values */
#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
	ser_settimeouts(fd, CONFIG_SER_RXTIMEOUT, CONFIG_SER_TXTIMEOUT);
#endif
#if CONFIG_SER_DEFBAUDRATE
	ser_setbaudrate(fd, CONFIG_SER_DEFBAUDRATE);
#endif

	/* Clear error flags */
	ser_setstatus(fd, 0);

	return fd;
}


/**
 * Clean up serial port, disabling the associated hardware.
 */
static int ser_close(struct KFile *fd)
{
	Serial *fds = SERIAL_CAST(fd);
	Serial *port = fds;

	ASSERT(port->is_open);
	DB(port->is_open = false);

	// Wait until we finish sending everything
	ser_flush(fd);

	port->hw->table->cleanup(port->hw);
	DB(port->hw = NULL);

	/*
	 * We purge the FIFO buffer only after the low-level cleanup, so that
	 * we are sure that there are no more interrupts.
	 */
	ser_purge(fds);
	return 0;
}

/**
 * Reopen serial port.
 */
static struct KFile *ser_reopen(struct KFile *fd)
{
	Serial *fds = SERIAL_CAST(fd);

	ser_close(fd);
	ser_open(fds, fds->unit);
	return (KFile *)fds;
}

/**
 * Init serial driver for \a unit.
 *
 * Use values SER_UARTn as values for \a unit.
 */
void ser_init(struct Serial *fds, unsigned int unit)
{
	memset(fds, 0, sizeof(*fds));

	DB(fds->fd._type = KFT_SERIAL);
	fds->fd.reopen = ser_reopen;
	fds->fd.close = ser_close;
	fds->fd.read = ser_read;
	fds->fd.write = ser_write;
	fds->fd.flush = ser_flush;
	fds->fd.error = ser_error;
	fds->fd.clearerr = ser_clearerr;
	ser_open(fds, unit);
}


/**
 * Read data from SPI bus.
 * Since we are master, we have to trigger slave by sending
 * fake chars on the bus.
 */
static size_t spimaster_read(struct KFile *fd, void *_buf, size_t size)
{
	Serial *fd_spi = SERIAL_CAST(fd);

	ser_flush(&fd_spi->fd);
	ser_purgeRx(fd_spi);

	size_t total_rd = 0;
	uint8_t *buf = (uint8_t *)_buf;
	int c;

	while (size--)
	{
		/*
		 * Send and receive chars 1 by 1, otherwise the rxfifo
		 * will overrun.
		 */
		ser_putchar(0, fd_spi);

		if ((c = ser_getchar(fd_spi)) == EOF)
			break;

		*buf++ = c;
		total_rd++;
	}
	return total_rd;
}

/**
 * Write data to SPI bus.
 */
static size_t spimaster_write(struct KFile *fd, const void *buf, size_t size)
{
	Serial *fd_spi = SERIAL_CAST(fd);

	ser_purgeRx(fd_spi);

	return ser_write(&fd_spi->fd, buf, size);
}


/**
 * Init SPI serial driver \a unit in master mode.
 *
 * Use SER_SPIn for \a unit parameter.
 *
 * This interface implements the SPI master protocol over a serial SPI
 * driver. This is needed because normal serial driver send/receive data
 * at the same time. SPI slaves like memories and other peripherals
 * first receive and *then* send response back instead.
 * To achieve this, when we are master and we are *sending*,
 * we have to discard all incoming data. Then, when we want to
 * receive, we must write fake data to SPI to trigger slave devices.
 */
void spimaster_init(Serial *fds, unsigned int unit)
{
	ser_init(fds, unit);
	fds->fd.read = spimaster_read;
	fds->fd.write = spimaster_write;
}


