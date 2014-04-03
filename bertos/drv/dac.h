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
 * \defgroup dac Generic DAC driver
 * \ingroup drivers
 * \{
 * \brief Digital to Analog Converter driver (DAC).
 *
 * <b>Configuration file</b>: cfg_dac.h
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "dac"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_dac.h"
 * $WIZ$ module_supports = "sam3x"
 */


#ifndef DRV_DAC_H
#define DRV_DAC_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

#include <cpu/attr.h>

#include CPU_HEADER(dac)

struct DacContext;
struct Dac;

typedef int (*DacWriteFunc_t) (struct Dac *dac, unsigned channel, uint16_t sample);
typedef void (*SetChannelMaskFunc_t) (struct Dac *dac, uint32_t mask);
typedef void (*SetSamplingRate_t) (struct Dac *dac, uint32_t rate);
typedef void (*DmaConversionBufFunc_t) (struct Dac *dac, void *buf, size_t len);
typedef bool (*DmaConversionIsFinished_t) (struct Dac *dac);
typedef void (*DmaStartStreamingFunc_t) (struct Dac *dac, void *buf, size_t len, size_t slice_len);
typedef void (*DmaStopFunc_t) (struct Dac *dac);
typedef void (*DmaCallbackFunc_t) (struct Dac *dac, void *_buf, size_t len);

typedef struct DacContext
{
	DacWriteFunc_t write;
	SetChannelMaskFunc_t setCh;
	SetSamplingRate_t setSampleRate;
	DmaConversionBufFunc_t conversion;
	DmaConversionIsFinished_t isFinished;
	DmaStartStreamingFunc_t start;
	DmaStopFunc_t stop;
	DmaCallbackFunc_t callback;
	size_t slice_len;

	DB(id_t _type);

} DacContext;

typedef struct Dac
{
	DacContext ctx;
	struct DacHardware *hw;
} Dac;

INLINE int dac_write(Dac *dac, unsigned channel, uint16_t sample)
{
	ASSERT(dac->ctx.write);
	return dac->ctx.write(dac, channel, sample);
}

INLINE void dac_setChannelMask(struct Dac *dac, uint32_t mask)
{
	ASSERT(dac->ctx.setCh);
	dac->ctx.setCh(dac, mask);
}

INLINE void dac_setSamplingRate(Dac *dac, uint32_t rate)
{
	ASSERT(dac->ctx.setSampleRate);
	dac->ctx.setSampleRate(dac, rate);
}

/*
 * Convert \param len samples stored into \param buf.
 */
INLINE void dac_dmaConversionBuffer(Dac *dac, void *buf, size_t len)
{
	ASSERT(dac->ctx.conversion);
	dac->ctx.conversion(dac, buf, len);
}

/*
 * Check if a dma transfer is finished.
 *
 * Useful for kernel-less applications.
 */
INLINE bool dac_dmaIsFinished(Dac *dac)
{
	ASSERT(dac->ctx.isFinished);
	return dac->ctx.isFinished(dac);
}

/*
 * \param slicelen Must be a divisor of len, ie. len % slicelen == 0.
 */
INLINE void dac_dmaStartStreaming(Dac *dac, void *buf, size_t len, size_t slice_len, DmaCallbackFunc_t callback)
{
	ASSERT(dac->ctx.start);
	ASSERT(len % slice_len == 0);
	ASSERT(callback);

	dac->ctx.callback = callback;
	dac->ctx.slice_len = slice_len;
	dac->ctx.start(dac, buf, len, slice_len);
}

INLINE void dac_dmaStop(Dac *dac)
{
	ASSERT(dac->ctx.stop);
	dac->ctx.stop(dac);
}

#define dac_bits() DAC_BITS

void dac_init(Dac *dac);

/** \} */ //defgroup dac
#endif /* DRV_DAC_H */
