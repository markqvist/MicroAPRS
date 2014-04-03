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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MPXX6115A Pressure Sensor convert formula.
 *
 * This module convert read voltage value from MPXX6115A
 * pressure sensor into hPascal value unit.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "mpxx6115a"
 * $WIZ$ module_depends = "adc"
 * $WIZ$ module_configuration = ""
 * $WIZ$ module_hw = ""
 */

#ifndef DRV_MPXX6115A_H
#define DRV_MPXX6115A_H

#include <drv/adc.h>

#define MPXX6115A_DIV_CONST    0.009f
#define MPXX6115A_ADD_CONST    0.095f


/**
 * Convert read voltage from MPXX6115A Pressure Sensor in hPascal value.
 *
 * The conversion formula may be consulted on constructor datasheet
 * (see Freescale Semiconductor MP3H6115A, MPXAZ6115A).
 *
 * \param vout output voltage read from pin sensor.
 * \param vref reference voltage that supplies the MPXX6115A sensor.
 *
 * \return integer value that represent measured pressure in hPascal.
 *
 * \note: To compute the pressure we use the Vout/Vref ratio, so
 * these two values can be expressed in any unit, even ADC levels.
 *
 */
INLINE int16_t mpxx6115a_press(adcread_t vout,  adcread_t vref)
{
	float tmp;

	tmp = (float)vout/(float)vref  + MPXX6115A_ADD_CONST;

	// To return hpascal we should multiply by 10 because the ratio is in kpascal
	return (int16_t)(tmp / MPXX6115A_DIV_CONST * 10);
}

#endif /* DRV_MPXX6115A_H */
