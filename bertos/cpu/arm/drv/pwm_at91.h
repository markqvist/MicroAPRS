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
 *
 * -->
 *
 *
 * \brief PWM hardware-specific definition
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef DRV_PWM_AT91_H
#define DRV_PWM_AT91_H

#include <cfg/compiler.h>
#include <cfg/macros.h>

#include "cfg/cfg_pwm.h"

#include <io/arm.h>

#if CFG_PWM_ENABLE_OLD_API

	#include "hw/pwm_map.h"

	/**
	 * Type definition for pwm period.
	 */
	typedef uint16_t pwm_period_t;

	/**
	 * Structur definition for pwm driver.
	 */
	typedef struct PwmChannel
	{
		bool duty_zero;         ///< True if duty cyle is zero, false otherwise.
		bool pol;               ///< PWM polarty flag.
		int pwm_pin;            ///< PWM pin.
		reg32_t *mode_reg;      ///< PWM mode register.
		reg32_t *duty_reg;      ///< PWM duty cycle register.
		reg32_t *period_reg;    ///< PWM periodic register.
		reg32_t *update_reg;    ///< Update setting register for PWM.

	} PwmChannel;


	void pwm_hw_init(void);
	void pwm_hw_setFrequency(PwmDev dev, uint32_t freq);
	void pwm_hw_setDutyUnlock(PwmDev dev, uint16_t duty);
	void pwm_hw_disable(PwmDev dev);
	void pwm_hw_enable(PwmDev dev);
	void pwm_hw_setPolarity(PwmDev dev, bool pol);
	pwm_period_t pwm_hw_getPeriod(PwmDev dev);

#else
	#include <drv/pwm.h>

	typedef uint16_t pwm_hwreg_t;

	struct PwmChannelRegs; //fwd decl

	typedef struct PwmHardware
	{
		uint32_t pwm_pin;       ///< PWM pin.
		volatile struct PwmChannelRegs *base;
	} PwmHardware;

	pwm_hwreg_t pwm_hw_getPeriod(Pwm *ctx);
	void pwm_hw_setFrequency(struct Pwm *ctx, pwm_freq_t freq);
	void pwm_hw_setDuty(Pwm *ctx, pwm_hwreg_t duty);
	void pwm_hw_init(struct Pwm *ctx, unsigned ch);

#endif

#endif /* DRV_ADC_AT91_H */
