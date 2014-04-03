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
 * \brief Proportional, integral, derivative controller (PID controller).
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "pid_control"
 * $WIZ$ module_depends = "timer"
 */

#ifndef ALGO_PID_CONTROL_H
#define ALGO_PID_CONTROL_H

#include <drv/timer.h>

/**
 * Data type for pid coefficient.
 */
typedef float pidk_t;
typedef float piddata_t;

/**
 * PID context structure.
 */
typedef struct PidCfg
{
	pidk_t kp;              ///< Proportional term of PID control method (Gain).
	pidk_t ki;              ///< Integral term of PID control method (Integral Gain).
	pidk_t kd;              ///< Derivative of PID control method (Derivative Gain).

	piddata_t i_max;        ///< Max value of integral term.
	piddata_t i_min;        ///< Min value of integral term.

	piddata_t out_max;      ///< Man value of output.
	piddata_t out_min;      ///< Min value of output.

	mtime_t sample_period;  ///< Sample period in milliseconds.

} PidCfg;


/**
 * PID context structure.
 */
typedef struct PidContext
{
	const PidCfg *cfg;

	piddata_t prev_err;     ///< Previous error.
	piddata_t i_state;      ///< Integrator state (sum of all the preceding errors).

} PidContext;

/**
 * Set Kp, Ki, Kd constants of PID control.
 */
INLINE void pid_control_setPid(PidCfg *pid_cfg, pidk_t kp, pidk_t ki, pidk_t kd)
{
	pid_cfg->kp = kp;
	pid_cfg->ki = ki;
	pid_cfg->kd = kd;
}

/**
 * Set sample period for PID control.
 */
INLINE void pid_control_setPeriod(PidCfg *pid_cfg, mtime_t sample_period)
{
	pid_cfg->sample_period = sample_period;
}

/**
 * Clear a pid control structure
 */
INLINE void pid_control_reset(PidContext *pid_ctx)
{
	pid_ctx->i_state = 0;
	pid_ctx->prev_err = 0;
}

piddata_t pid_control_update(PidContext *pid_ctx, piddata_t target, piddata_t curr_pos);
void pid_control_init(PidContext *pid_ctx, const PidCfg *pid_cfg);

#endif /* ALGO_PID_CONTROL_H */
