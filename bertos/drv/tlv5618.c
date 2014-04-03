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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * Texas instrument TLV5618 DAC driver.
 */

#include "tlv5618.h"
#include "hw/hw_tlv5618.h"

#include <io/kfile.h>

#include <string.h> //memset


static void tlv5618_write(Tlv5618 *ctx, uint16_t val)
{
	TLV5618_CSLOW(ctx->cs_pin);
	kfile_putc(val >> 8, ctx->ch);
	kfile_putc(val & 0xFF, ctx->ch);
	kfile_flush(ctx->ch);
	TLV5618_CSHIGH(ctx->cs_pin);
}

#define POWERDOWN 0x2000
#define OUTA 0xC000
#define OUTB 0x4000

/**
 * Set DAC output A to \a val.
 * \a val ranges from 0 to 4095 (12 bit).
 * \note : if in power down, the device will be awaken.
 */
void tlv5618_setOutA(Tlv5618 *ctx, uint16_t val)
{
	val &= 0x0FFF;
	tlv5618_write(ctx, val | OUTA);
}

/**
 * Set DAC output B to \a val.
 * \a val ranges from 0 to 4095 (12 bit).
 * \note : if in power down, the device will be awaken.
 */
void tlv5618_setOutB(Tlv5618 *ctx, uint16_t val)
{
	val &= 0x0FFF;
	tlv5618_write(ctx, val | OUTB);
}

/**
 * Set the TLV5618 in the power down state.
 */
void tlv5618_setPowerDown(Tlv5618 *ctx)
{
	tlv5618_write(ctx, POWERDOWN);
}


/**
 * Init the TLV5618 with CS connected to \a cs_pin.
 * \a ch should be the SPI channel needed to communicate with the DAC.
 *       This SPI should also be correctly configured (the MOSI should
 *       change on the rising edge of the SCK clock).
 * \note the device is set in power down mode.
 */
void tlv5618_init(Tlv5618 *ctx, KFile *ch, int cs_pin)
{
	memset(ctx, 0, sizeof(*ctx));
	ctx->cs_pin = cs_pin;
	ctx->ch = ch;
	TLV5618_CSINIT(cs_pin);
	tlv5618_setPowerDown(ctx);
}
