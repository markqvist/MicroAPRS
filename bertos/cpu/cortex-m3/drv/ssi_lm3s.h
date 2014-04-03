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
 */

#ifndef SSI_LM3S_H
#define SSI_LM3S_H

#include <cpu/power.h> /* cpu_relax() */
#include <io/kfile.h> /* KFile */
#include <io/lm3s.h>

/**
 * LM3S1968 SSI frame format
 */
/*\{*/
#define SSI_FRF_MOTO_MODE_0     0x00000000  //< Moto fmt, polarity 0, phase 0
#define SSI_FRF_MOTO_MODE_1     0x00000002  //< Moto fmt, polarity 0, phase 1
#define SSI_FRF_MOTO_MODE_2     0x00000001  //< Moto fmt, polarity 1, phase 0
#define SSI_FRF_MOTO_MODE_3     0x00000003  //< Moto fmt, polarity 1, phase 1
#define SSI_FRF_TI              0x00000010  //< TI frame format
#define SSI_FRF_NMW             0x00000020  //< National MicroWire frame format
/*\}*/

/**
 * LM3S1968 SSI operational mode
 */
/*\{*/
#define SSI_MODE_MASTER         0x00000000  //< SSI master
#define SSI_MODE_SLAVE          0x00000001  //< SSI slave
#define SSI_MODE_SLAVE_OD       0x00000002  //< SSI slave with output disabled
/*\}*/

/* LM3S SSI handle properties */
enum
{
	/* Non-blocking I/O */
	LM3S_SSI_NONBLOCK = 1,
};

/** LM3S1968 SSI handle structure */
typedef struct LM3SSSI
{
	/* SSI Kfile structure */
	KFile fd;

	/* Handle properties */
	uint32_t flags;

	/* SSI port address */
	uint32_t addr;
} LM3SSSI;

/**
 * ID for LM3S SSI.
 */
#define KFT_LM3SSSI MAKE_ID('L', 'S', 'S', 'I')

INLINE LM3SSSI *LM3SSSI_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_LM3SSSI);
	return (LM3SSSI *)fd;
}

/* KFile interface to LM3S SSI */
void lm3s_ssiInit(struct LM3SSSI *fds, uint32_t addr, uint32_t frame, int mode,
			int bitrate, uint32_t data_width);

/* Raw interface to LM3S SSI */
int lm3s_ssiOpen(uint32_t addr, uint32_t frame, int mode,
			int bitrate, uint32_t data_width);

/*
 * Check if the SSI transmitter is busy or not
 *
 * This allows to determine whether the TX FIFO have been cleared by the
 * hardware, so the transmission can be safely considered completed.
 */
INLINE bool lm3s_ssiTxDone(uint32_t base)
{
	return (HWREG(base + SSI_O_SR) & SSI_SR_BSY) ? true : false;
}

/*
 * Check if the SSI TX FIFO is full
 */
INLINE bool lm3s_ssiTxReady(uint32_t base)
{
	return (HWREG(base + SSI_O_SR) & SSI_SR_TNF) ? true : false;
}

/*
 * Check for data available in the RX FIFO
 */
INLINE bool lm3s_ssiRxReady(uint32_t base)
{
	return (HWREG(base + SSI_O_SR) & SSI_SR_RNE) ? true : false;
}

/*
 * Get a frame into the SSI receive FIFO without blocking.
 *
 * Return the number of frames read from the RX FIFO.
 */
INLINE int lm3s_ssiReadFrameNonBlocking(uint32_t base, uint32_t *val)
{
	/* Check for data available in the RX FIFO */
	if (!lm3s_ssiRxReady(base))
		return 0;
	/* Read data from SSI RX FIFO */
	*val = HWREG(base + SSI_O_DR);
	return 1;
}

/*
 * Get a frame from the SSI receive FIFO.
 */
INLINE void lm3s_ssiReadFrame(uint32_t base, uint32_t *val)
{
	/* Wait for data available in the RX FIFO */
	while (!lm3s_ssiRxReady(base))
		cpu_relax();
	/* Read data from SSI RX FIFO */
	*val = HWREG(base + SSI_O_DR);
}

/*
 * Put a frame into the SSI transmit FIFO without blocking.
 *
 * NOTE: the upper bits of the frame will be automatically discarded by the
 * hardware according to the frame data width.
 *
 * Return the number of frames written to the TX FIFO.
 */
INLINE int lm3s_ssiWriteFrameNonBlocking(uint32_t base, uint32_t val)
{
	/* Check for available space in the TX FIFO */
	if (!lm3s_ssiTxReady(base))
		return 0;
	/* Enqueue data to the TX FIFO */
	HWREG(base + SSI_O_DR) = val;
	return 1;
}

/*
 * Put a frame into the SSI transmit FIFO.
 *
 * NOTE: the upper bits of the frame will be automatically discarded by the
 * hardware according to the frame data width.
 */
INLINE void lm3s_ssiWriteFrame(uint32_t base, uint32_t val)
{
	/* Wait for available space in the TX FIFO */
	while (!lm3s_ssiTxReady(base))
		cpu_relax();
	/* Enqueue data to the TX FIFO */
	HWREG(base + SSI_O_DR) = val;
}

#endif /* SSI_LM3S_H */
