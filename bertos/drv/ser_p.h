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
 *
 * -->
 *
 * \brief Hardware dependent serial driver (interface)
 *
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 */


#ifndef DRV_SER_P_H
#define DRV_SER_P_H

#include <cfg/compiler.h> /* size_t */



struct SerialHardware;
struct Serial;

struct SerialHardwareVT
{
	void (*init)(struct SerialHardware *ctx, struct Serial *ser);
	void (*cleanup)(struct SerialHardware *ctx);
	void (*setBaudrate)(struct SerialHardware *ctx, unsigned long rate);
	void (*setParity)(struct SerialHardware *ctx, int parity);
	void (*txStart)(struct SerialHardware *ctx);
	bool (*txSending)(struct SerialHardware *ctx);
};

struct SerialHardware
{
	const struct SerialHardwareVT *table;
	unsigned char *txbuffer;
	unsigned char *rxbuffer;
	size_t         txbuffer_size;
	size_t         rxbuffer_size;
};

struct SerialHardware *ser_hw_getdesc(int unit);



#endif /* DRV_SER_P_H */
