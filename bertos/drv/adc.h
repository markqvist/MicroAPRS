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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \defgroup adc Generic ADC driver
 * \ingroup drivers
 * \{
 * \brief Analog to Digital Converter driver (ADC).
 *
 * <b>Configuration file</b>: cfg_adc.h
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "adc"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_adc.h"
 * $WIZ$ module_supports = "not atmega103"
 */


#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cpu/attr.h>
#include CPU_HEADER(adc)

/** Type for ADC return value. */
typedef uint16_t adcread_t;

/** Type for channel */
typedef uint8_t adc_ch_t;

#define adc_bits() ADC_BITS

adcread_t adc_read(adc_ch_t ch);
void adc_init(void);

/**
 * Macro used to convert data from adc range (0...(2 ^ADC_BITS - 1)) to
 * \a y1 ... \a y2 range.
 * \note \a y1, \a y2 can be negative, and put in ascending or descending order as well.
 * \note \a data and \a y2 are evaluated only once, \a y1 twice.
 */
#define ADC_RANGECONV(data, y1, y2) (((((int32_t)(data)) * ((y2) - (y1))) / ((1 << ADC_BITS) - 1)) + (y1))

/** \} */ //defgroup adc
#endif /* DRV_ADC_H */
