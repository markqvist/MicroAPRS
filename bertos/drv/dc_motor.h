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
 * -->
 *
 *
 * \brief DC motor driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "dc_motor"
 * $WIZ$ module_depends = "pwm", "pid_control", "adc", "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_dc_motor.h"
 * $WIZ$ module_hw = "bertos/hw/hw_dc_motor.h"
 */

#ifndef DRV_DC_MOTOR_H
#define DRV_DC_MOTOR_H

#include "hw/hw_dc_motor.h"

#include "cfg/cfg_dc_motor.h"
#include "cfg/cfg_pwm.h"
#include <cfg/macros.h>

#include <algo/pid_control.h>

#include <drv/pwm.h>
#include <drv/timer.h>
#include <drv/adc.h>

#define DC_MOTOR_NO_EXPIRE        -1  ///< The DC motor runs do not expire, so it runs forever.
#define DC_MOTOR_NO_DEV_SPEED     -1  ///< Disable the speed acquire from device (like trimmer, etc.).

/**
 * Type for DC motor.
 */
typedef uint16_t dc_speed_t;

/**
 * DC motor configuration stucture
 */
typedef struct DCMotorConfig
{
	PidCfg pid_cfg;         ///< Pid control.
	bool pid_enable;        ///< Flag to disable or enable pid control.

	unsigned pwm_dev;       ///< Pwm channel.
	pwm_freq_t freq;        ///< Pwm waveform frequency.

	adc_ch_t adc_ch;        ///< ADC channel.
	adcread_t adc_max;      ///< ADC max scale value.
	adcread_t adc_min;      ///< ADC min scale value.

	bool dir;               ///< Default direction for select DC motor.
	bool braked;            ///< If true the motor is braked when we turn off it.

	dc_speed_t speed;       ///< Default speed value for select DC motor.

	int speed_dev_id;       ///<  Index of the device where read speed, to disable set to DC_MOTOR_NO_DEV_SPEED.

} DCMotorConfig;


/**
 * Context structure for DC motor.
 */
typedef struct DCMotor
{
	const DCMotorConfig *cfg; // All configuration for select DC motor.
	PidContext pid_ctx;       // Pid control.

#if !CFG_PWM_ENABLE_OLD_API
	Pwm pwm;                  // Pwm context
#endif

	int index;                // DC motor id.
	uint32_t status;          // Status of select DC motor
	dc_speed_t tgt_speed;     // Target speed for select DC motor

	ticks_t expire_time;      // Amount of time that  dc motor run

} DCMotor;

void dc_motor_setDir(int index, bool dir);
void dc_motor_enable(int index, bool state);
void dc_motor_setSpeed(int index, dc_speed_t speed);
void dc_motor_startTimer(int index, mtime_t on_time);
void dc_motor_waitStop(int index);
void dc_motor_setup(int index, DCMotorConfig *dcm_conf);
dc_speed_t dc_motor_readTargetSpeed(int index);
void dc_motor_setPriority(int priority);
void dc_motor_init(void);


/**
 * Test function prototypes.
 *
 * See dc_motor_hwtest.c file.
 */
int dc_motor_testSetUp(void);
void dc_motor_testRun(void);
int dc_motor_testTearDown(void);

#endif /* DRV_DC_MOTOR_H */
