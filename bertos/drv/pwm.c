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
 * \brief PWM driver (implementation)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cfg/cfg_pwm.h"

#include <cfg/macros.h>
#include <cfg/module.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PWM_LOG_LEVEL
#define LOG_VERBOSITY     PWM_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>

#include <drv/pwm.h>

#include CPU_HEADER(pwm)

#include <cpu/types.h>
#include <cpu/irq.h>

#include <string.h>

#if CFG_PWM_ENABLE_OLD_API

	/**
	 * Set duty of PWM channel \p dev.
	 * The current output frequency will be maintained.
	 * \param dev PWM channel.
	 * \param duty the new duty cycle.
	 */
	void pwm_setDuty(PwmDev dev, pwm_duty_t duty)
	{
		pwm_period_t period = 0;
		pwm_duty_t real_duty = 0;

		duty = MIN(duty, PWM_MAX_DUTY);

		period = pwm_hw_getPeriod(dev);

		real_duty = (uint64_t)(duty * period) >> (uint64_t)PWM_MAX_PERIOD_LOG2;

		LOG_INFO("real_duty[%d] duty[%d], period[%d]\n", real_duty, duty, period);
		pwm_hw_setDutyUnlock(dev, real_duty);
	}

	/**
	 * Set frequency of PWM channel \p dev at \p freq.
	 * \param dev PWM channel.
	 * \param freq new frequency, in Hz.
 	 * \warning This function has to be called with PWM disabled, otherwise
	 *          the output value will be undefined.
	 *          The current duty cycle value will be lost, after calling this
	 *          function the duty cycle have to be set again.
	 */
	void pwm_setFrequency(PwmDev dev, pwm_freq_t freq)
	{
		pwm_hw_setFrequency(dev, freq);
	}

	/**
	 * Enable/Disable PWM channel \p dev.
	 * \param dev PWM channel.
	 * \param state if true the PWM on \p dev is activated, if false is disabled.
	 */
	void pwm_enable(PwmDev dev, bool state)
	{
		if (state)
			pwm_hw_enable(dev);
		else
			pwm_hw_disable(dev);
	}

	MOD_DEFINE(pwm);

	/**
	 * Initialize the PWM driver.
	 * \warning all available PWM channels are initialized.
	 */
	void pwm_init(void)
	{
		cpu_flags_t flags;
		PwmDev dev;

		IRQ_SAVE_DISABLE(flags);

		pwm_hw_init();

		/* set all pwm to 0 */
		for (dev = 0; dev < PWM_CNT; dev++)
			pwm_setDuty(dev, 0);

		IRQ_RESTORE(flags);
		MOD_INIT(pwm);
	}
#endif

#if !CFG_PWM_ENABLE_OLD_API || defined(__doxygen__)

	INLINE void setRealDuty(Pwm *ctx, pwm_duty_t duty)
	{
		if (ctx->pol == PWM_POL_LOW_PULSE)
			duty = PWM_MAX_DUTY - duty;

		pwm_hwreg_t period = pwm_hw_getPeriod(ctx);
		pwm_hwreg_t hw_duty;

		switch (duty)
		{
		case 0:
			hw_duty = 0;
			break;
		case PWM_MAX_DUTY:
			hw_duty = period;
			break;
		default:
			hw_duty = (uint64_t)(duty * period) >> (uint64_t)PWM_MAX_PERIOD_LOG2;
		}

		pwm_hw_setDuty(ctx, hw_duty);
	}

	/**
	 * Set the duty cycle of the PWM channel linked to \p ctx.
	 * The modification will be applied to the channel immediatly.
	 * The current frequency of the channel will be maintained.
	 * \param ctx PWM channel context.
	 * \param duty the new duty cycle value.
	 * \see pwm_duty_t
	 */
	void pwm_setDuty(Pwm *ctx, pwm_duty_t duty)
	{
		ctx->duty = duty;

		if (ctx->enabled)
			setRealDuty(ctx, duty);
	}

	/**
	 * Set PWM frequency of channel linked to \p ctx.
	 * The modification will be applied to the channel immediatly.
	 * The duty cycle of the channel will be maintained.
	 * \param ctx PWM channel context.
	 * \param freq the new frequency of the signal, in Hz.
	 * \note Depending on the hardware implementation, this function may
	 *       generate a glitch in the output signal upon frequency changing.
	 */
	void pwm_setFrequency(Pwm *ctx, pwm_freq_t freq)
	{
		pwm_hw_setFrequency(ctx, freq);
		pwm_enable(ctx, ctx->enabled);
	}

	/**
	 * Set PWM polarity of pwm channel linked to \p ctx.
	 * The modification will be applied to the channel immediatly.
	 * \param ctx PWM channel context.
	 * \param pol the new polarity of the signal.
	 *
	 * \note if a channel is disabled, changing its polarity will change the
	 *       current steady output level.
	 * \see pwm_enable
	 * \see PwmPolarity
	 */
	void pwm_setPolarity(Pwm *ctx, PwmPolarity pol)
	{
		ctx->pol = pol;
		pwm_enable(ctx, ctx->enabled);
	}

	/**
	 * Enable/Disable the pwm channel linked to \p ctx.
	 * The modification will be applied to the channel immediatly.
	 * \param ctx PWM channel context.
	 * \param enable if true the channel will be enabled, if false will be disabled.
	 *
	 * \note When a PWM channel is disabled, the output level will be the same
	 *       as if the duty would be set to 0%.
	 *       So, if current polarity is positive, a disabled channel will be
	 *       low, if polarity is negative will be high.
	 * \see pwm_setPolarity
	 */
	void pwm_enable(Pwm *ctx, bool enable)
	{
		ctx->enabled = enable;

		if (enable)
			setRealDuty(ctx, ctx->duty);
		else
			setRealDuty(ctx, 0);
	}

	/**
	 * Initialize PWM driver.
	 * \param ctx pointer to a PWM context structure, used for holding PWM
	 *            driver related information.
	 * \param channel the channel you want to initialize.
	 * \note The channel will be initialized disabled and with High polarity.
	 */
	void pwm_init(Pwm *ctx, unsigned channel)
	{
		memset(ctx, 0, sizeof(*ctx));
		ctx->ch = channel;

		pwm_hw_init(ctx, channel);
	}

#endif


