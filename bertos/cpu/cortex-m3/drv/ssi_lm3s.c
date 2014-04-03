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
 * \brief LM3S1968 Synchronous Serial Interface (SSI) driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */


#include "ssi_lm3s.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <string.h> /* memset() */

/* SSI clocking informations (CPSDVSR + SCR) */
struct SSIClock
{
	unsigned int cpsdvsr;
	unsigned int scr;
};

/*
 * Evaluate the SSI clock prescale (SSICPSR) and SSI serial clock rate (SCR).
 */
INLINE struct SSIClock
lm3s_ssiPrescale(unsigned int bitrate)
{
	struct SSIClock ret;

	for (ret.cpsdvsr = 2, ret.scr = CPU_FREQ / bitrate / ret.cpsdvsr - 1;
			ret.scr > 255; ret.cpsdvsr += 2);
	ASSERT(ret.cpsdvsr < 255);

	return ret;
}

/*
 * Initialize the SSI interface.
 *
 * Return 0 in case of success, a negative value otherwise.
 */
int lm3s_ssiOpen(uint32_t addr, uint32_t frame, int mode,
			int bitrate, uint32_t data_width)
{
	struct SSIClock ssi_clock;

	ASSERT(addr == SSI0_BASE || addr == SSI1_BASE);
	/* Configure the SSI operating mode */
	switch (mode)
	{
		/* SSI Slave Mode Output Disable */
		case SSI_MODE_SLAVE_OD:
			HWREG(addr + SSI_O_CR1) = SSI_CR1_SOD;
			break;
		/* SSI Slave */
		case SSI_MODE_SLAVE:
			HWREG(addr + SSI_O_CR1) = SSI_CR1_MS;
			break;
		/* SSI Master */
		case SSI_MODE_MASTER:
			HWREG(addr + SSI_O_CR1) = 0;
			break;
		default:
			ASSERT(0);
			return -1;
	}
	/* Configure the peripheral clock and frame format */
	ssi_clock = lm3s_ssiPrescale(bitrate);
	HWREG(addr + SSI_O_CPSR) = ssi_clock.cpsdvsr;
	HWREG(addr + SSI_O_CR0) =
			(ssi_clock.scr << 8)		|
			((frame & 3) << 6)		|
			(frame & SSI_CR0_FRF_M)		|
			(data_width - 1);
	/* Enable the SSI interface */
	HWREG(addr + SSI_O_CR1) |= SSI_CR1_SSE;

	return 0;
}

/*
 * Write data to the SSI bus.
 *
 * Return the number of bytes written to the bus.
 */
static size_t lm3s_ssiWrite(struct KFile *fd, const void *buf, size_t size)
{
	LM3SSSI *fds = LM3SSSI_CAST(fd);
	const char *p = (const char *)buf;
	uint32_t frame;
	size_t count = 0;

	while (count < size)
	{
		frame = p[count];
		if (fds->flags & LM3S_SSI_NONBLOCK)
		{
			if (!lm3s_ssiWriteFrameNonBlocking(fds->addr,
								frame))
				break;
		}
		else
			lm3s_ssiWriteFrame(fds->addr, frame);
		count++;
	}
	return count;
}

/*
 * Read data from the SSI bus.
 *
 * Return the number of bytes read from the bus.
 */
static size_t lm3s_ssiRead(struct KFile *fd, void *buf, size_t size)
{
	LM3SSSI *fds = LM3SSSI_CAST(fd);

	uint8_t *p = (uint8_t *)buf;
	uint32_t frame;
	size_t count = 0;

	while (count < size)
	{
		if (fds->flags & LM3S_SSI_NONBLOCK)
		{
			if (!lm3s_ssiReadFrameNonBlocking(fds->addr, &frame))
				break;
		}
		else
			lm3s_ssiReadFrame(fds->addr, &frame);
		*p++ = (uint8_t)frame;
		count++;
	}
	return count;
}


/* Wait for data in the TX FIFO being actually transmitted */
static int lm3s_ssiFlush(struct KFile *fd)
{
	LM3SSSI *fds = LM3SSSI_CAST(fd);

	while (!lm3s_ssiTxDone(fds->addr))
		cpu_relax();
	return 0;
}

/* Disable the SSI interface */
static int lm3s_ssiClose(struct KFile *fd)
{
	LM3SSSI *fds = LM3SSSI_CAST(fd);

	lm3s_ssiFlush(fd);
	HWREG(fds->addr + SSI_O_CR1) &= ~SSI_CR1_SSE;
	return 0;
}

/**
 * Initialize a LM3S SSI driver.
 */
void lm3s_ssiInit(struct LM3SSSI *fds, uint32_t addr, uint32_t frame, int mode,
			int bitrate, uint32_t data_width)
{
	memset(fds, 0, sizeof(*fds));
	DB(fds->fd._type = KFT_LM3SSSI);

	/* TODO: only 8-bit frame size is supported */
	ASSERT(data_width == 8);

	fds->fd.write = lm3s_ssiWrite;
	fds->fd.read = lm3s_ssiRead;
	fds->fd.close = lm3s_ssiClose;
	fds->fd.flush = lm3s_ssiFlush;

	fds->addr = addr;
	lm3s_ssiOpen(addr, frame, mode, bitrate, data_width);
}
