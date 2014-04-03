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
 * \brief ADC driver (implementation)
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <drv/adc.h>

#ifndef WIZ_AUTOGEN
	#warning Deprecated: now you should include adc_<cpu> directly in the makefile. Remove this line and the following once done.
	#include CPU_CSOURCE(adc)
#else
	#include CPU_HEADER(adc)
#endif

#include <cfg/debug.h>     // ASSERT()
#include <cfg/macros.h>    // MIN()
#include <cfg/compiler.h>
#include <cfg/module.h>

/**
 * Read the ADC channel \a ch.
 */
adcread_t adc_read(adc_ch_t ch)
{
	ASSERT(ch <= (adc_ch_t)ADC_MUX_MAXCH);
	ch = MIN(ch, (adc_ch_t)ADC_MUX_MAXCH);

	adc_hw_select_ch(ch);

	return(adc_hw_read());
}

MOD_DEFINE(adc);

/**
 * Initialize the ADC hardware.
 */
void adc_init(void)
{
	adc_hw_init();

	MOD_INIT(adc);
}
