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
 * \brief DC motor driver (implementation)
 *
 * Thi module provide a simple api to controll a DC motor in direction and
 * speed, to allow this we use a  Back-EMF technique.
 * This technique is based, on the capability of the DC motor to become a generator
 * of voltage when we turn off its supply. This happen every time we turn off the
 * DC motor supply, and it continues to rotate for a short time thanks to its mechanical
 * energy. Using this idea we can turn off the motor for a very short time, and
 * we read the volage value from DC motor supply pins. This voltage say to us
 * the actual speed of the motor.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "dc_motor.h"
#include "hw/hw_dc_motor.h"
#include "cfg/cfg_pwm.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         DC_MOTOR_LOG_LEVEL
#define LOG_FORMAT        DC_MOTOR_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>

#include <algo/pid_control.h>

#include <drv/timer.h>

#include <kern/proc.h>

#include <cpu/power.h>

#include <string.h>

#if CFG_PWM_ENABLE_OLD_API
	#define PWM_ENABLE(dcm, en)    pwm_enable((dcm)->cfg->pwm_dev, (en))
	#define PWM_SETDUTY(dcm, duty) pwm_setDuty((dcm)->cfg->pwm_dev, (duty))
	#define PWM_SETFREQ(dcm, freq) pwm_setFrequency((dcm)->cfg->pwm_dev, (freq))
	#define PWM_SETPOL(dcm, pol)   pwm_setPolarity((dcm)->cfg->pwm_dev, (pol))
#else
	#define PWM_ENABLE(dcm, en)    pwm_enable(&(dcm)->pwm, (en))
	#define PWM_SETDUTY(dcm, duty) pwm_setDuty(&(dcm)->pwm, (duty))
	#define PWM_SETFREQ(dcm, freq) pwm_setFrequency(&(dcm)->pwm, (freq))
	#define PWM_SETPOL(dcm, pol)   pwm_setPolarity(&(dcm)->pwm, (pol))
#endif

/**
 * Define status bit for DC motor device.
 */
#define DC_MOTOR_ACTIVE           BV(0)     ///< DC motor enable or disable flag.
#define DC_MOTOR_DIR              BV(1)     ///< Spin direction of DC motor.

/*
 * Some utility macro for motor directions
 */
#define POS_DIR                   1
#define NEG_DIR                   0
#define DC_MOTOR_POS_DIR(x)       ((x) |= DC_MOTOR_DIR)   // Set directions status positive
#define DC_MOTOR_NEG_DIR(x)       ((x) &= ~DC_MOTOR_DIR)  // Set directions status negative

// Update the status with current direction
#define DC_MOTOR_SET_STATUS_DIR(status, dir) \
		(dir == POS_DIR ? DC_MOTOR_POS_DIR(status) : DC_MOTOR_NEG_DIR(status))

#if (CONFIG_KERN && CONFIG_KERN_PREEMPT)
	#if CONFIG_DC_MOTOR_USE_SEM
		#include <kern/sem.h>

		Semaphore dc_motor_sem;
		#define DC_MOTOR_LOCK        sem_obtain(&dc_motor_sem)
		#define DC_MOTOR_UNLOCK      sem_release(&dc_motor_sem)
	#else
		#define DC_MOTOR_LOCK        proc_forbid()
		#define DC_MOTOR_UNLOCK      proc_permit()
	#endif
#else
	#define DC_MOTOR_LOCK        /* None */
	#define DC_MOTOR_UNLOCK      /* None */
#endif

/**
 * DC motor definition.
 */
static DCMotor dcm_all[CONFIG_NUM_DC_MOTOR];

/*
 * Process to poll dc motor status
 */
struct Process *dc_motor;


// Stack process for DC motor poll.
static PROC_DEFINE_STACK(dc_motor_poll_stack, 500);

// Only for Debug
LOG_INFOB(static int debug_msg_delay = 0;);


INLINE dc_speed_t dc_motor_readSpeed(int index)
{
	DCMotor *dcm = &dcm_all[index];
	return HW_DC_MOTOR_READ_VALUE(dcm->cfg->adc_ch, dcm->cfg->adc_min, dcm->cfg->adc_max);
}

/**
 * Read the target speed from select device.
 */
dc_speed_t dc_motor_readTargetSpeed(int index)
{
	DCMotor *dcm = &dcm_all[index];
	return HW_DC_MOTOR_READ_VALUE(dcm->cfg->speed_dev_id, CONFIG_DC_MOTOR_MIN_SPEED, CONFIG_DC_MOTOR_MAX_SPEED);
}

static void dc_motor_start(int index)
{
	DCMotor *dcm = &dcm_all[index];

	DC_MOTOR_LOCK;
	/*
	 * Clean all PID stutus variable, becouse
	 * we start with new one.
	 */
	pid_control_reset(&dcm->pid_ctx);
	dcm->status |= DC_MOTOR_ACTIVE;
	DC_MOTOR_UNLOCK;
}

/*
 * There are two \a mode to stop the dc motor:
 *  - DC_MOTOR_DISABLE_MODE
 *  - DC_MOTOR_IDLE
 *
 * The DC_MOTOR_DISABLE_MODE shut down the DC motor and
 * leave it floating to rotate.
 * The DC_MOTOR_IDLE does not shut down DC motor, but put
 * its supply pin in short circuite, in this way the motor result
 * braked from intentional rotation.
 */
static void dc_motor_stop(int index)
{
	DCMotor *dcm = &dcm_all[index];

	DC_MOTOR_LOCK;

	dcm->status &= ~DC_MOTOR_ACTIVE;
	dcm->expire_time = DC_MOTOR_NO_EXPIRE;
	PWM_ENABLE(dcm, false);

	if (dcm->cfg->braked)
	{
		DC_MOTOR_STOP_BRAKED(dcm->index);
	}
	else
	{
		DC_MOTOR_STOP_FLOAT(dcm->index);
	}

	DC_MOTOR_UNLOCK;
}

/*
 * Sampling a signal on DC motor and compute
 * a new value of speed according with PID control.
 */
static void dc_motor_do(int index)
{
	DCMotor *dcm = &dcm_all[index];

	dc_speed_t curr_pos = 0;
	pwm_duty_t new_pid = 0;

	DC_MOTOR_LOCK;

	//If select DC motor is not active we return
	if (!(dcm->status & DC_MOTOR_ACTIVE))
	{
		DC_MOTOR_UNLOCK;
		return;
	}

	/*
	 * To set dc motor direction we must also set the
	 * PWM polarity according with dc motor driver chip
	 */
	PWM_SETPOL(dcm, dcm->status & DC_MOTOR_DIR);
	DC_MOTOR_SET_DIR(dcm->index, dcm->status & DC_MOTOR_DIR);

	//Compute next value for reaching target speed from current position
	if (dcm->cfg->pid_enable)
	{
		/*
		 * Here we cannot disable the switch context because the
		 * driver, that read the speed could be need to use signal or
		 * other thing that needs the kernel switch context, for this
		 * reason we unlock before to read the speed.
		 */
		DC_MOTOR_UNLOCK;
		curr_pos = dc_motor_readSpeed(index);
		DC_MOTOR_LOCK;
		new_pid = pid_control_update(&dcm->pid_ctx, dcm->tgt_speed, curr_pos);
	}
	else
	{
		new_pid = dcm->tgt_speed;
	}

	LOG_INFOB(
		if (debug_msg_delay == 20)
		{
			LOG_INFO("DC Motor[%d]: curr_speed[%d],curr_pos[%d],tgt[%d]\n", dcm->index,
								curr_pos, new_pid, dcm->tgt_speed);
			debug_msg_delay = 0;
		}
		debug_msg_delay++;
	);

	//Apply the compute duty value
	PWM_SETDUTY(dcm, new_pid);

	//Restart dc motor
	PWM_ENABLE(dcm, true);

	DC_MOTOR_ENABLE(dcm->index);
	DC_MOTOR_UNLOCK;
}


/*
 * Check if the DC motor run time is expired, if this happend
 * we turn off motor and reset status.
 */
INLINE bool check_timerIsExpired(int index)
{

	DC_MOTOR_LOCK;
	bool check = ((dcm_all[index].expire_time - timer_clock()) < 0) &&
			(dcm_all[index].expire_time != DC_MOTOR_NO_EXPIRE);
	DC_MOTOR_UNLOCK;

	return check;
}

/**
 * Process to poll DC motor status.
 * To use a Back-EMF technique (see brief for more details),
 * we turn off a motor for CONFIG_DC_MOTOR_SAMPLE_DELAY, that value are stored
 * in each DC motor config. For this implementation we assume
 * that have a common CONFIG_DC_MOTOR_SAMPLE_DELAY, choose among a max delay
 * to all DC motor configuration.
 * The DC motor off time is choose to allow the out signal to
 * be stable, so we can read and process this value for feedback controll loop.
 * The period (CONFIG_DC_MOTOR_SAMPLE_PERIOD - CONFIG_DC_MOTOR_SAMPLE_DELAY)
 * that every time we turn off a DC motor is choose to have a feedback controll
 * more responsive or less responsive.
 */
static void NORETURN dc_motor_poll(void)
{
	for (;;)
	{
		/*
		 * For all DC motor we read and process output singal,
		 * and choose the max value to off time
		 */
		for (int i = 0; i < CONFIG_NUM_DC_MOTOR; i++)
		{
			if (!dcm_all[i].cfg)
				continue;

			if (check_timerIsExpired(i))
				dc_motor_stop(i);
			else
				dc_motor_do(i);

			/*
			 * If we read speed from trimmer we update the target
			 * speed value when motor is running so we can make
			 * dc motor speed regulation.
			 */
			if (dcm_all[i].cfg->speed_dev_id != DC_MOTOR_NO_DEV_SPEED)
				dc_motor_setSpeed(i, dc_motor_readTargetSpeed(i));
		}

		//Wait for next sampling
		timer_delay(CONFIG_DC_MOTOR_SAMPLE_PERIOD - CONFIG_DC_MOTOR_SAMPLE_DELAY);

		for (int i = 0; i < CONFIG_NUM_DC_MOTOR; i++)
		{
			if (!dcm_all[i].cfg)
				continue;

			if (check_timerIsExpired(i))
				dc_motor_stop(i);

			DC_MOTOR_LOCK;
			if (dcm_all[i].status & DC_MOTOR_ACTIVE)
			{
				DC_MOTOR_DISABLE(dcm_all[i].index);
				PWM_ENABLE(&dcm_all[i], false);
			}
			DC_MOTOR_UNLOCK;
		}

		//Wait some time to allow signal to stabilize before sampling
		timer_delay(CONFIG_DC_MOTOR_SAMPLE_DELAY);
	}
}

/**
 * Set spin direction of DC motor.
 *
 * \a index number of DC motor
 * \a dir direction of DC motor
 */
void dc_motor_setDir(int index, bool dir)
{
	DCMotor *dcm = &dcm_all[index];
	DC_MOTOR_LOCK;
	DC_MOTOR_SET_STATUS_DIR(dcm->status, dir);
	DC_MOTOR_UNLOCK;
}

/**
 * Set DC motor speed.
 */
void dc_motor_setSpeed(int index, dc_speed_t speed)
{
	DCMotor *dcm = &dcm_all[index];

	DC_MOTOR_LOCK;
	dcm->tgt_speed = speed;
	DC_MOTOR_UNLOCK;

	LOG_INFO("DC Motor[%d]: tgt_speed[%d]\n", index, dcm->tgt_speed);
}

/**
 * Set among of time that dc motor should run.
 */
void dc_motor_startTimer(int index, mtime_t on_time)
{
	DC_MOTOR_LOCK;
	dcm_all[index].expire_time = DC_MOTOR_NO_EXPIRE;
	if (on_time != DC_MOTOR_NO_EXPIRE)
	{
		dcm_all[index].expire_time = timer_clock() + ms_to_ticks(on_time);
		dc_motor_start(index);
	}
	DC_MOTOR_UNLOCK;
}

void dc_motor_waitStop(int index)
{
	DCMotor *dcm = &dcm_all[index];
	bool loop = true;

	while (loop)
	{
		DC_MOTOR_LOCK;
		loop = dcm->status & DC_MOTOR_ACTIVE;
		DC_MOTOR_UNLOCK;

		cpu_relax();
	}
}

/**
 * Enable or disable dc motor.
 */
void dc_motor_enable(int index, bool state)
{
	if (state)
		dc_motor_start(index);
	else
		dc_motor_stop(index);
}

/**
 * Apply a confinguration to select DC motor.
 */
void dc_motor_setup(int index, DCMotorConfig *dcm_conf)
{
	DCMotor *dcm = &dcm_all[index];

	DC_MOTOR_LOCK;
	/*
	 * We are using the same sample period for each
	 * motor, and so we check if this value is the same
	 * for all. The sample period time is defined in pid
	 * configuration.
	 *
	 * TODO: Use a different sample period for each motor
	 * and refactor a module to allow to use a timer interrupt,
	 * in this way we can controll a DC motor also without a
	 * kernel, increasing a portability on other target.
	 */
	pid_control_setPeriod(&dcm_conf->pid_cfg, CONFIG_DC_MOTOR_SAMPLE_PERIOD);

	//Init pid control
	pid_control_init(&dcm->pid_ctx, &dcm_conf->pid_cfg);


	dcm->cfg = dcm_conf;

	/*
	 * Apply config value.
	 */
	dcm->index = index;

	/*
	 * By default the motor run forever..
	 */
	dcm->expire_time = DC_MOTOR_NO_EXPIRE;

	/*
	 * By default set target speed.
	 */
	dcm->tgt_speed = dcm_conf->speed;

	/*
	 * Clear the status.
	 */
	dcm->status = 0;
#if !CFG_PWM_ENABLE_OLD_API
	pwm_init(&dcm->pwm, dcm_conf->pwm_dev);
#endif
	PWM_SETFREQ(dcm, dcm->cfg->freq);
	PWM_ENABLE(dcm, false);

	//Set default direction for DC motor
	DC_MOTOR_SET_DIR(dcm->index, dcm->cfg->dir);
	DC_MOTOR_SET_STATUS_DIR(dcm->status, dcm->cfg->dir);

	DC_MOTOR_UNLOCK;

	LOG_INFO("DC motor[%d]:\n", dcm->index);
	LOG_INFO("> PID: kp[%f],ki[%f],kd[%f]\n", dcm->cfg->pid_cfg.kp, dcm->cfg->pid_cfg.ki, dcm->cfg->pid_cfg.kd);
	LOG_INFO("> PWM: pwm_dev[%d], freq[%ld], sample[%d]\n", dcm->cfg->pwm_dev, dcm->cfg->freq,CONFIG_DC_MOTOR_SAMPLE_DELAY);
	LOG_INFO("> ADC: adc_ch[%d], adc_max[%d], adc_min[%d]\n", dcm->cfg->adc_ch, dcm->cfg->adc_max, dcm->cfg->adc_min);
	LOG_INFO("> DC: dir[%d], speed[%d]\n", dcm->cfg->dir, dcm->cfg->speed);
}

/**
 * If we had enabled the priority scheduling, we can adjust the
 * DC motor poll process priority.
 */
void dc_motor_setPriority(int priority)
{
	ASSERT(CONFIG_KERN);
	ASSERT(dc_motor);
	proc_setPri(dc_motor, priority);
}

/**
 * Init DC motor.
 * \a priority: sets the dc motor process priority.
 */
void dc_motor_init(void)
{
	ASSERT(CONFIG_KERN);

	MOTOR_DC_INIT();

	#if (CONFIG_KERN_PREEMPT && CONFIG_DC_MOTOR_USE_SEM)
		sem_init(&dc_motor_sem);
	#endif

	//Create a dc motor poll process
	dc_motor = proc_new_with_name("DC_Motor", dc_motor_poll, NULL, sizeof(dc_motor_poll_stack), dc_motor_poll_stack);
}

