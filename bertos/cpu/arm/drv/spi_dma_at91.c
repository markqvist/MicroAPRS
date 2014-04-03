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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief SPI driver with DMA.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include <drv/spi_dma.h>

#include "cfg/cfg_spi_dma.h"
#include "hw/hw_spi_dma.h"

#include <io/at91sam7.h>
#include <io/kfile.h>

#include <struct/fifobuf.h>
#include <struct/kfile_fifo.h>

#include <drv/timer.h>

#include <cpu/attr.h>
#include <cpu/power.h>

#include <string.h> /* memset */


void spi_dma_setclock(uint32_t rate)
{
	SPI0_CSR0 &= ~SPI_SCBR;

	ASSERT((uint8_t)DIV_ROUND(CPU_FREQ, rate));
	SPI0_CSR0 |= DIV_ROUND(CPU_FREQ, rate) << SPI_SCBR_SHIFT;
}


static int spi_dma_flush(UNUSED_ARG(struct KFile *, fd))
{
	/* Wait for DMA to finish */
	while (!(SPI0_SR & BV(SPI_TXBUFE)))
		cpu_relax();

	/* Wait until last bit has been shifted out */
	while (!(SPI0_SR & BV(SPI_TXEMPTY)))
		cpu_relax();

	return 0;
}

static size_t spi_dma_write(struct KFile *fd, const void *_buf, size_t size)
{
	SPI0_PTCR = BV(PDC_TXTDIS);
	SPI0_TPR = (reg32_t)_buf;
	SPI0_TCR = size;
	SPI0_PTCR = BV(PDC_TXTEN);
	spi_dma_flush(fd);
	return size;
}


/*
 * Dummy buffer used to transmit 0xff chars while receiving data.
 * This buffer is completetly constant and the compiler should allocate it
 * in flash memory.
 */
static const uint8_t tx_dummy_buf[CONFIG_SPI_DMA_MAX_RX] = { [0 ... (CONFIG_SPI_DMA_MAX_RX - 1)] = 0xFF };

static size_t spi_dma_read(UNUSED_ARG(struct KFile *, fd), void *_buf, size_t size)
{
	size_t count, total_rx = 0;
	uint8_t *buf = (uint8_t *)_buf;

	while (size)
	{
		count = MIN(size, (size_t)CONFIG_SPI_DMA_MAX_RX);

		SPI0_PTCR = BV(PDC_TXTDIS) | BV(PDC_RXTDIS);

		SPI0_RPR = (reg32_t)buf;
		SPI0_RCR = count;
		SPI0_TPR = (reg32_t)tx_dummy_buf;
		SPI0_TCR = count;

		/* Avoid reading the previous sent char */
		*buf = SPI0_RDR;

		/* Start transfer */
		SPI0_PTCR = BV(PDC_RXTEN) | BV(PDC_TXTEN);

		/* wait for transfer to finish */
		while (!(SPI0_SR & BV(SPI_ENDRX)))
			cpu_relax();

		size -= count;
		total_rx += count;
		buf += count;
	}
	SPI0_PTCR = BV(PDC_RXTDIS) | BV(PDC_TXTDIS);

	return total_rx;
}

#define SPI_DMA_IRQ_PRIORITY 4

void spi_dma_init(SpiDma *spi)
{
	/* Disable PIO on SPI pins */
	PIOA_PDR = BV(SPI0_SPCK) | BV(SPI0_MOSI) | BV(SPI0_MISO);

	/* Reset device */
	SPI0_CR = BV(SPI_SWRST);

	/*
	 * Set SPI to master mode, fixed peripheral select, chip select directly connected to a peripheral device,
	 * SPI clock set to MCK, mode fault detection disabled, loopback disable, NPCS0 active, Delay between CS = 0
	 */
	SPI0_MR = BV(SPI_MSTR) | BV(SPI_MODFDIS);

	/*
	 * Set SPI mode.
	 * At reset clock division factor is set to 0, that is
	 * *forbidden*. Set SPI clock to minimum to keep it valid.
	 */
	SPI0_CSR0 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);

	/* Disable all irqs */
	SPI0_IDR = 0xFFFFFFFF;
	/* Enable SPI clock. */
	PMC_PCER = BV(SPI0_ID);

	/* Enable SPI */
	SPI0_CR = BV(SPI_SPIEN);

	DB(spi->fd._type = KFT_SPIDMA);
	spi->fd.write = spi_dma_write;
	spi->fd.read = spi_dma_read;
	spi->fd.flush = spi_dma_flush;

	SPI_DMA_STROBE_INIT();
}
