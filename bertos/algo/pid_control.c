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
 * \brief Proportional, integral, derivative controller (PID controller) (implementation)
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "pid_control.h"

#include "cfg/cfg_pid.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PID_LOG_LEVEL
#define LOG_VERBOSITY     PID_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>

/**
 * Compute next value for reaching \a target point.
 */
piddata_t pid_control_update(PidContext *pid_ctx, piddata_t target, piddata_t curr_pos)
{
	piddata_t P;
	piddata_t I;
	piddata_t D;
	piddata_t err;

	//Compute current error.
	err = target - curr_pos;

	/*
	 * Compute Proportional contribute
	 */
	P = err * pid_ctx->cfg->kp;

	//Update integral state error
	pid_ctx->i_state += err;

	//Clamp integral state between i_min and i_max
	pid_ctx->i_state  = MINMAX(pid_ctx->cfg->i_min, pid_ctx->i_state, pid_ctx->cfg->i_max);

	/*
	 * Compute Integral contribute
	 *
	 * note: for computing the integral contribute we use a sample period in seconds
	 * and so we divide sample_period in microsenconds for 1000.
	 */
	I = pid_ctx->i_state * pid_ctx->cfg->ki * ((piddata_t)pid_ctx->cfg->sample_period / 1000);


	/*
	 * Compute derivative contribute
	 */
	D = (err - pid_ctx->prev_err) * pid_ctx->cfg->kd / ((piddata_t)pid_ctx->cfg->sample_period / 1000);


 	LOG_INFO("curr_pos[%lf],tgt[%lf],err[%f],P[%f],I[%f],D[%f]", curr_pos, target, err, P, I, D);


	//Store the last error value
	pid_ctx->prev_err = err;
	piddata_t pid = MINMAX(pid_ctx->cfg->out_min, (P + I + D), pid_ctx->cfg->out_max);

 	LOG_INFO("pid[%lf]",pid);

	//Clamp out between out_min and out_max
	return pid;
}

/**
 * Init PID control.
 */
void pid_control_init(PidContext *pid_ctx, const PidCfg *pid_cfg)
{
	/*
	 * Init all values of pid control struct
	 */
	pid_ctx->cfg = pid_cfg;

	pid_control_reset(pid_ctx);

}

