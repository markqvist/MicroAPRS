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
 * \defgroup pwm_driver PWM driver
 * \ingroup drivers
 * \{
 * \brief Pulse Width Modulation (PWM) driver.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "pwm"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_pwm.h"
 * $WIZ$ module_hw = "bertos/hw/pwm_map.h"
 * $WIZ$ module_supports = "not avr and not cm3"
 */

#ifndef DRV_PWM_H
#define DRV_PWM_H

#include <cpu/attr.h>

#include <cfg/compiler.h>
#include "cfg/cfg_pwm.h"

/**
 * Maximum PWM duty cycle value (100%)
 */
#define PWM_MAX_DUTY              ((pwm_duty_t)0xFFFF)
#define PWM_MAX_PERIOD                         0xFFFF
#define PWM_MAX_PERIOD_LOG2                        16

/**
 * Type for PWM duty cycle.
 * The value is represented as a 16bit unsigned integer, so it ranges from 0 (0%)
 * to PWM_MAX_DUTY (0xFFFF = 100%).
 */
typedef uint16_t pwm_duty_t;

/**
 * Type for PWM frequency.
 * Unit of measure is Hz.
 */
typedef uint32_t pwm_freq_t;

#if !CFG_PWM_ENABLE_OLD_API || defined(__doxygen__)
	/**
	 * \defgroup pwm_api PWM API
	 * With this driver you can control a device with multiple PWM channels.
	 * You can enable/disable each channel indipendently and also set frequency
	 * and duty cycle.
	 *
	 * API usage example:
	 * \code
	 * Pwm pwm; // declare a context structure
	 * pwm_init(&pwm, 0); // init pwm channel 0
	 * pwm_setFrequency(&pwm, 1000); // Set frequency of channel 0 to 1000Hz
	 * pwm_setDuty(&pwm, 0x7FFF); // Set duty to 50% (0xFFFF/2)
	 * pwm_enable(&pwm, true); // Activate the output
	 * \endcode
	 * \{
	 */

	/**
	 * Enum describing PWM polarities.
	 */
	typedef enum PwmPolarity
	{
		/** High pulse: increasing duty increases the part of the signal at high level. */
		PWM_POL_HIGH_PULSE,
		/** Positive pulse: same as High pulse. */
		PWM_POL_POSITIVE = PWM_POL_HIGH_PULSE,
		/** Low pulse: increasing duty increases the part of the signal at low level. */
		PWM_POL_LOW_PULSE,
		/** Negative pulse: same as Low pulse. */
		PWM_POL_NEGATIVE = PWM_POL_LOW_PULSE,
	} PwmPolarity;

	struct PwmHardware; //Fwd declaration

	/**
	 * PWM context structure.
	 */
	typedef struct Pwm
	{
		unsigned ch;
		pwm_duty_t duty;
		PwmPolarity pol;
		bool enabled;
		struct PwmHardware *hw;
	} Pwm;

	void pwm_setDuty(Pwm *ctx, pwm_duty_t duty);
	void pwm_setFrequency(Pwm *ctx, pwm_freq_t freq);
	void pwm_setPolarity(Pwm *ctx, PwmPolarity pol);
	void pwm_enable(Pwm *ctx, bool state);
	void pwm_init(Pwm *ctx, unsigned channel);
	/** \} */ //defgroup pwm_api
#endif


#if CFG_PWM_ENABLE_OLD_API
	/**
	 * \defgroup pwm_old_api Old PWM API
	 * This API has strong limititations, so it has been deprecated.
	 * It is active by default for backward compatibility reasons, but
	 * for new projects please use the new PWM API.
	 * In order to disable this API, check CFG_PWM_ENABLE_OLD_API.
	 * \see pwm_api
	 * \see CFG_PWM_ENABLE_OLD_API
	 * \{
	 */

	#include CPU_HEADER(pwm)
	#include "hw/pwm_map.h"

	/**
	 * Set PWM polarity of pwm \p dev.
	 * \param dev PWM channel.
	 * \param pol if false positive polarity pulses are generated,
	 *            if true negative polarity pulses are generated.
	 * \warning This function has to be called with PWM disabled, otherwise
	 *          the output value will be undefined.
	 */
	INLINE void pwm_setPolarity(PwmDev dev, bool pol)
	{
		pwm_hw_setPolarity(dev, pol);
	}

	void pwm_setDuty(PwmDev dev, pwm_duty_t duty);
	void pwm_setFrequency(PwmDev dev, pwm_freq_t freq);
	void pwm_enable(PwmDev dev, bool state);
	void pwm_init(void);
	/** \} */ //defgroup pwm_old_api
#endif

/*
 * Test function prototypes.
 *
 * See pwm_test.c for implemntation of these functions.
 */
void pwm_testRun(void);
int pwm_testSetup(void);
/* For backward compatibility */
#define pwm_testSetUp() pwm_testSetup()
int pwm_testTearDown(void);


/** \} */ //defgroup pwm_driver

#endif /* DRV_PWM_H */
