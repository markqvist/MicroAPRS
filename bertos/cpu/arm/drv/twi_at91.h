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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the AT91 ARM TWI (implementation)
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#ifndef DRV_AT91_TWI_H
#define DRV_AT91_TWI_H

#include <cfg/compiler.h>

typedef int16_t twi_iaddr_t;

#define TWI_NO_IADDR (-1)

void twi_init(void);
bool twi_read(uint8_t id, twi_iaddr_t byte1, twi_iaddr_t byte2, twi_iaddr_t byte3, void *_buf, size_t len);
bool twi_write(uint8_t id, twi_iaddr_t byte1, twi_iaddr_t byte2, twi_iaddr_t byte3, const void *_buf, size_t len);

#endif /* DRV_AT91_TWI_H */
