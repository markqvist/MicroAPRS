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
 *
 * -->
 *
 * \brief Emulated SPI driver.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "spi_bitbang"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_spi_bitbang.h"
 * $WIZ$ module_hw = "bertos/hw/hw_spi.h"
 */


#ifndef DRV_SPI_BITBANG_H
#define DRV_SPI_BITBANG_H

#include "cfg/cfg_spi_bitbang.h"

#include <cfg/compiler.h>

/**
 * Define send and receive order bit.
 *
 * $WIZ$ ordet_bit_list = "SPI_LSB_FIRST", "SPI_MSB_FIRST"
 */
#define SPI_LSB_FIRST 1
#define SPI_MSB_FIRST 2

#if CONFIG_SPI_DATAORDER == SPI_LSB_FIRST
	#define  SPI_DATAORDER_START    1
	#define  SPI_DATAORDER_SHIFT(i) ((i) <<= 1)
#elif CONFIG_SPI_DATAORDER == SPI_MSB_FIRST
	#define  SPI_DATAORDER_START    0x80
	#define  SPI_DATAORDER_SHIFT(i) ((i) >>= 1)
#endif

void spi_write(const void *buf, size_t len);
void spi_read(void *buf, size_t len);
uint8_t spi_sendRecv(uint8_t c);
void spi_init(void);
void spi_assertSS(void);
void spi_deassertSS(void);

#endif /* DRV_SPI_BITBANG_H */
