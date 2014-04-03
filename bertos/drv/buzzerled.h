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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief Generic library to handle buzzers and leds
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

#ifndef DRV_BUZZERLED_H
#define DRV_BUZZERLED_H

#include <cpu/attr.h>

#if 0
/** Include hw_buzzerled.h. We expect hw_buzzerled.h to define enum BLD_DEVICE, which must contain
 *  an enumarator for each device, plus a special symbol NUM_BLDS containing the
 *  number of devices.
 */
#include "hw/hw_buzzerled.h"


/* Include hw-level implementation. This allows inlining of bld_set, which in turns
 * should allow fast constant propagation for the common case (where the parameter
 * device is a constant).
 */
#include CPU_HEADER(buzzerled)


/** Initialize the buzzerled library.
 *
 * \note This function must be called before any other function in the library.
 */
void bld_init(void);


/** Set or reset a device.
 *
 * \param device Device to be set
 * \param enable Enable/disable status
 */
#define bld_set(device, enable)  bld_hw_set(device, enable)


/** Enable a device for a certain interval of time
 *
 * \param device Device to be enabled
 * \param duration Number of milliseconds the device must be enabled
 *
 * \note This function is non-blocking, so it will return immediately.
 */
void bld_beep(enum BLD_DEVICE device, uint16_t duration);


/** Enable a device for a certain interval of time and wait.
 *
 * \param device Device to be enabled
 * \param duration Number of milliseconds the device must be enabled
 *
 * \note This function is blocking, so it will return after the specified period of time.
 */
void bld_beep_and_wait(enum BLD_DEVICE device, uint16_t duration);

#endif

#endif /* DRV_BUZZERLED_H */
