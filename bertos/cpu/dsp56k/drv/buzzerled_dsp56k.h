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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief Hardware support for buzzers and leds in DSP56K-based boards
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.4  2004/11/16 21:54:43  bernie
 *#* Changes for SC Monoboard support.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:36:05  bernie
 *#* Import buzzerled driver.
 *#*
 *#*/

#ifndef DRV_BUZZERLED_DSP56K_H
#define DRV_BUZZERLED_DSP56K_H

#include <cfg/compiler.h>
#include <hw.h>
#include "pwm.h"

#if ARCH & ARCH_HECO

/**
 * \name Connection of the leds to the DSP:
 * <pre>
 *   Led       Line    DSP Pin
 *   ---------------------------
 *   YELLOW    T2      HOME1/TB3
 *   GREEN     T3      INDX1/TB2
 *   RED       T4      PHB1/TB1
 * </pre>
 */

INLINE bool bld_is_inverted_intensity(enum BLD_DEVICE device)
{
	return (device == BLD_GREEN_LED
	        || device == BLD_YELLOW_LED
	        || device == BLD_RED_LED);
}

INLINE bool bld_is_pwm(enum BLD_DEVICE device)
{
	// Only the buzzer is connected to a PWM
	return (device == BLD_BUZZER || device == BLD_READY_LED);
}

INLINE bool bld_is_timer(enum BLD_DEVICE device)
{
	// LEDs are connected to timers
	return (device == BLD_GREEN_LED || device == BLD_YELLOW_LED || device == BLD_RED_LED);
}

INLINE uint16_t bld_get_pwm(enum BLD_DEVICE device)
{
	switch (device)
	{
	default: ASSERT(0);
	case BLD_BUZZER: return 5;  // PWMA5
	case BLD_READY_LED:  return 9;   // PWMB3
	}
}


INLINE struct REG_TIMER_STRUCT* bld_get_timer(enum BLD_DEVICE device)
{
	switch (device)
	{
	default: ASSERT(0);
	case BLD_GREEN_LED: return &REG_TIMER_B[2];
	case BLD_RED_LED: return &REG_TIMER_B[1];
	case BLD_YELLOW_LED: return &REG_TIMER_B[3];
	}
}

INLINE void bld_hw_init(void)
{
}

INLINE void bld_hw_set(enum BLD_DEVICE device, bool enable)
{
	if (bld_is_inverted_intensity(device))
		enable = !enable;

	// Handle a BLD connected to a PWM
	if (bld_is_pwm(device))
	{
		struct PWM* pwm = pwm_get_handle(bld_get_pwm(device));

		pwm_set_enable(pwm, false);
		pwm_set_dutycycle_percent(pwm, (enable ? 50 : 0));
		pwm_set_enable(pwm, true);
	}
	else if (bld_is_timer(device))
	{
		struct REG_TIMER_STRUCT* timer = bld_get_timer(device);

		// Check that the timer is currently stopped, and the OFLAG is not
		//  controlled by another timer. Otherwise, the led is already 
		//  controlled by the timer, and we cannot correctly set it 
		//  on/off without reprogramming the timer.
		ASSERT((timer->CTRL & REG_TIMER_CTRL_MODE_MASK) == REG_TIMER_CTRL_MODE_STOP);
		ASSERT(!(timer->SCR & REG_TIMER_SCR_EEOF));

		// Check also that polarity is correct
		ASSERT(!(timer->SCR & REG_TIMER_SCR_OPS));

		// Without programming the timer, we have a way to manually force a certain
		//  value on the external pin. We also need to enable the output pin.
		timer->SCR &= ~REG_TIMER_SCR_VAL_1;
		timer->SCR |= REG_TIMER_SCR_OEN |
		              REG_TIMER_SCR_FORCE |
		              (!enable ? REG_TIMER_SCR_VAL_0 : REG_TIMER_SCR_VAL_1);
	}
	else
		ASSERT(0);
}

#elif ARCH & ARCH_SC

// We do not need inline functions here, because constant propagation is not big deal here
void bld_hw_init(void);
void bld_hw_set(enum BLD_DEVICE device, bool enable);

#endif

#endif /* DRV_BUZZERLED_DSP56K_H */
