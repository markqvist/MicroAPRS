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
 * \brief Configuration file for DC motor module.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_DC_MOTOR_H
#define CFG_DC_MOTOR_H

/**
 * Number of the DC motors to manage.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_NUM_DC_MOTOR       4

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define DC_MOTOR_LOG_LEVEL      LOG_LVL_INFO

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define DC_MOTOR_LOG_FORMAT     LOG_FMT_VERBOSE


/**
 * Min value of DC motor speed.
 * \note Generally this value is the min value of the ADC conversion,
 * if you use it.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_DC_MOTOR_MIN_SPEED              0


/**
 * Max value of DC motor speed.
 * \note Generally this value is the max value of the ADC conversion,
 * if you use it.
 *
 * $WIZ$ type = "int"
 * $WIZ$ max = 65535
 */
#define CONFIG_DC_MOTOR_MAX_SPEED         65535


/**
 * Sampling period in millisecond.
 * $WIZ$ type = "int"
 */
#define CONFIG_DC_MOTOR_SAMPLE_PERIOD     40

/**
 * Amount of millisecond before to read sample.
 * $WIZ$ type = "int"
 */
#define CONFIG_DC_MOTOR_SAMPLE_DELAY       2

/**
 * This control set which mode the driver use to lock share
 * resources when we use the preempitive kernel.
 * If we set to 1 we use the semaphore module otherwise the
 * driver disable the switch context every time we need to access
 * to shared sources.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_DC_MOTOR_USE_SEM            1

#endif /* CFG_DC_MOTOR_H */
