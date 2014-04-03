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
 * All Rights Reserved.
 * -->
 *
 * \brief NTC hardware-specific definition
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * A NTC acts as a variable resistor, whose resistance changes as a
 * function of the temperature it measures. To sample it correctly, it is
 * usually parallelized and serialized with two fixed resistor. The following diagram shows
 * what is the exact disposition of the components, as handled by this
 * library:
 *
 *<pre>
 *                 o Vref
 *                 |
 *                 |                               o Vref
 *                 |                               |
 *               -----                             |
 *              |     |                        ---------
 *              | Rser|                       |         |
 *              |     |                       |         |
 *               -----     -----              |   ADC   |
 *                 | Vp   |     |             |         |
 *      -----------|------| Amp |-------------|         |
 *     |           |      |     |      Vadc   |         |
 *   -----       -----     -----               ---------
 *  |     |     |     |
 *  | NTC |     | Rpar|
 *  |     |     |     |
 *   -----       -----
 *     |           |
 *     |           |
 *   -----       -----
 *    ---         ---
 *</pre>
 * Amp is an amplifier that amplify of AMP times the signal.
 * If we indicate Rp as the parallel of NTC with Rpar, ADCBITS as the bits of the ad converter
 * and ADCVAL as the result from the adc convertion (Not Vadc but just the value read
 * from the adc register), after various calculation, the expression of Rp is:
 *
 *<pre>
 *
 *            ADCVAL * Rser
 * Rp = ------------------------
 *         ADCBITS
 *	2         * AMP - ADCVAL
 *</pre>
 *
 * And after that NTC obvisiously is:
 *<pre>
 *        Rpar * Rp
 * NTC = ----------
 *        Rpar - Rp
 *</pre>
 *
 *
 * The function ntc_hw_read() compute the resistence using these formulas above.
 */

#ifndef HW_NTC_H
#define HW_NTC_H

#include "ntc_map.h"

#include <cfg/debug.h>

#include <drv/ntc.h>
#include <drv/adc.h>

#warning TODO:This is an example implementation, you must implement it!

extern const res_t      NTC_RSER[NTC_CNT];
extern const res_t      NTC_RPAR[NTC_CNT];
extern const amp_t      NTC_AMP[NTC_CNT];
extern const NtcHwInfo* NTC_INFO[NTC_CNT];


/*!
 * Read the resistence of ntc device \a dev.
 * Return the result in res_t type.
 */
INLINE res_t ntc_hw_read(NtcDev dev)
{
	ASSERT(dev < NTC_CNT);
	// See above for formula explanation.
	adcread_t adcval = adc_read((uint16_t)dev);
	float rp = (adcval * NTC_RSER[dev] ) / ((1 << adc_bits()) * NTC_AMP[dev] - adcval);

	//kprintf("Rp[%f], Rntc[%f]\n", rp/100, ((NTC_RPAR[dev] * rp) / (NTC_RPAR[dev] - rp)) / 100.0);

	return ( (NTC_RPAR[dev] * rp) / (NTC_RPAR[dev] - rp) );
}


/*!
 * Return the info (aka the table) associated with ntc device \a dev.
 */
INLINE const NtcHwInfo* ntc_hw_getInfo(NtcDev dev)
{
	return NTC_INFO[dev];
}

#define NTC_HW_INIT	do  { /* Implement me! */ } while(0)

#endif /* HW_NTC_H */
