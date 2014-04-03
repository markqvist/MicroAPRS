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
 * Copyright 2004, 2005, 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Thermo-control driver.
 *
 * This module implements multiple thermo controls, which is the logic needed to try
 * keeping the temperature of a device constant. For this module, a "device" is a black box
 * whose temperature can be measured, and which has a mean to make it hotter or colder.
 * For instance, a device could be the combination of a NTC (analog temperature reader) and
 * a Peltier connected to the same physic block.
 *
 * This module relies on a low-level driver to communicate with the device (implementation
 * of the black box). This low-level driver also controls the units in which the temperature
 * is expressed: thermo control treats it just as a number.
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "thermo"
 * $WIZ$ module_depends = "timer", "ntc"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_thermo.h"
 * $WIZ$ module_hw = "bertos/hw/hw_thermo.h", "bertos/hw/thermo_map.h"
 */

#ifndef DRV_THERMO_H
#define DRV_THERMO_H

#include "hw/thermo_map.h"

#include <drv/ntc.h>
#include <drv/timer.h>

typedef uint8_t thermostatus_t;


/**
 * Set the target temperature at which a given device should be kept.
 *
 * \param dev Device
 * \param temperature Target temperature
 */
void thermo_setTarget(ThermoDev dev, deg_t temperature);

/**
 * Start thermo control for a certain device \a dev and stop it after
 *  \a on_time msec.
 */
void thermo_timer(ThermoDev dev, mtime_t on_time);

/** Start thermo control for a certain device \a dev */
void thermo_start(ThermoDev dev);

/** Stop thermo control for a certain device \a dev */
void thermo_stop(ThermoDev dev);

/** Clear errors for channel \a dev */
void thermo_clearErrors(ThermoDev dev);

/** Return the status of the specific \a dev thermo-device. */
thermostatus_t thermo_status(ThermoDev dev);

/**
 * Return the current temperature of a device currently under thermo
 * control.
 *
 * \param dev Device
 * \return Current temperature (Celsius degrees * 10)
 */
deg_t thermo_readTemperature(ThermoDev dev);

void thermo_init(void);


#endif /* DRV_THERMO_H */
