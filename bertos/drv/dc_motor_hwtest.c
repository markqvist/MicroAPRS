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
 * -->
 *
 *
 * \brief Test for PWM driver (implementation)
 *
 * This is a simple test for PWM driver. This module
 * is target independent, so you can test all target that
 * BeRTOS support.
 * To use this test you should include a pwm_map.h header where
 * are defined the PWM channels for your target. Then you should add
 * or remove a test setting in pwm_test_cfg array, and edit a value for
 * your specific test.
 * Afther this, all is ready and you can test PWM driver.
 *
 * The test check first if all PWM channel starts, and then try
 * to change a PWM duty cicle for all channel.
 * The change of duty cycle is operate when a PWM channel is enable,
 * in this way you can see if a pwm signal is clean and work properly.
 * The duty value is change incrementaly, and when it arrive to 100% or 0%,
 * we reset the duty value and restart the test.
 * Further the duty test, we check also a PWM polarity, infact when we
 * reach a reset duty value, we invert a polary of PWM wavform.
 * So you can see if the hardware manage correctly this situation.
 *
 * Note: To be simple and target independently we not use a timer module,
 * and so the delay is do with a for cycle.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief HW test for DC Motor.
 */

#include <cfg/cfg_dc_motor.h>

#include <cfg/debug.h>
// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         DC_MOTOR_LOG_LEVEL
#define LOG_VERBOSITY     DC_MOTOR_LOG_FORMAT
#include <cfg/log.h>


#include <algo/pid_control.h>

#include <drv/timer.h>
#include <drv/dc_motor.h>
#include <drv/adc.h>
#include <drv/pwm.h>
#include <drv/dc_motor.h>

#include <kern/proc.h>

#include <cpu/irq.h>

#include <verstag.h>
#include <buildrev.h>

static DCMotorConfig motor =
{
    /* PID */
    {
        .kp = 1,           /* Proportional coefficient */
        .ki = 4,           /* Integral coefficient */
        .kd = 0.008,         /* Derivate coefficient */
        .i_max = 2E33,       /* Integrale max error value */
        .i_min = -2E33,      /* Integrale min error value */
        .out_max = 65535,    /* Max output value */
        .out_min = 0,        /* Min output value */
        .sample_period = 0  /* Millisecod between 2 output singal sampling */
    },
    .pid_enable = true,      /* Enable or disable pid control */

    /* PWM */
    .pwm_dev = 2,            /* PWM channel */
    .freq = 3000,            /* Frquency of PWM output waveform */

    /* ADC */
    .adc_ch = 2,             /* ADC channel */
    .adc_max = 65535,        /* Max range value for ADC */
    .adc_min = 0,            /* Min range value for ADC */

	/* DC Motor */
    .dir = 1,                 /* Default spin direction of DC motor */
	.braked = true,

    .speed = 10000,          /* Fixed speed value for seldc_motor_enableect DC motor, if enable_dev_speed flag is false */
    .speed_dev_id = 7,        /* Index of the device where read speed */
};

int dc_motor_testSetUp(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();
	proc_init();
#if !CFG_PWM_ENABLE_OLD_API
	pwm_init();
#endif
	adc_init();

	return 0;
}

#define MOTOR      2

void NORETURN dc_motor_testRun(void)
{
	dc_motor_init();

	/*
	 * Assign the configuration to motor.
	 */
	dc_motor_setup(MOTOR, &motor);

	while (1)
	{
		/*
		 * Using enable and disable
		 */
		dc_motor_setDir(MOTOR, 1);
		dc_motor_setSpeed(MOTOR, 10000);
		dc_motor_enable(MOTOR, true);
		timer_delay(500);
		dc_motor_enable(MOTOR, false);


		dc_motor_setDir(MOTOR, 0);
		dc_motor_setSpeed(MOTOR, 60000);
		dc_motor_enable(MOTOR, true);
		timer_delay(150);
		dc_motor_enable(MOTOR, false);

		/*
		 * Using timer
		 */
		dc_motor_setDir(MOTOR, 1);
		dc_motor_setSpeed(MOTOR, 60000);
		dc_motor_startTimer(MOTOR, 150);
		dc_motor_waitStop(MOTOR);

		dc_motor_setDir(MOTOR, 0);
		dc_motor_setSpeed(MOTOR, 10000);
		dc_motor_startTimer(MOTOR, 500);
		dc_motor_waitStop(MOTOR);
	}

}

int dc_motor_testTearDown(void)
{
	return 0;
}
