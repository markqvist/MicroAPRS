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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief SPI driver with DMA.
 *
 * \note Only one copy of SpiDma is allowed for each application.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef DRV_SPI_DMA_H
#define DRV_SPI_DMA_H

#include <io/kfile.h>

typedef struct SpiDma
{
	KFile fd;
} SpiDma;

#define KFT_SPIDMA MAKE_ID('S', 'P', 'I', 'D')

INLINE SpiDma * SPIDMA_CAST(KFile *fd)
{
  ASSERT(fd->_type == KFT_SPIDMA);
  return (SpiDma *)fd;
}

/**
 * Init DMA SPI driver.
 * \param spi A pointer to a SpiDma structure.
 */
void spi_dma_init(SpiDma *spi);

/**
 * Set the clock rate for SPI bus.
 *
 * \param rate The rate you want to set for SPI.
 */
void spi_dma_setclock(uint32_t rate);

#endif /* DRV_SPI_DMA_H */
