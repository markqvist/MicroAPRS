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
 * \brief Low-level stepper timer module for AVR (inplementation).
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#ifndef DRV_STEPPER_AVR_H
#define DRV_STEPPER_AVR_H


#include <cfg/compiler.h>
#include <cfg/macros.h>

#include <drv/stepper.h>

#warning TODO:This is an example, you must implement it!

/**
 * IRQ callback function type definition.
 */
typedef void (*irq_t)(void);

/**
 * Timer contex structure.
 */
typedef struct TimerCounter
{
	int timer_id;                  ///< Timer counter ID
	irq_t isr;                     ///< IRQ handler
	stepper_isr_t callback;        ///< Interrupt callback pointer
	struct Stepper *motor;         ///< Stepper context structure

} TimerCounter;

/**
 * Enable interrupt for timer counter compare event.
 */
INLINE void stepper_tc_irq_enable(struct TimerCounter *timer)
{
		/* put here code to enable timer irq */

		//Only for test remove when implement this function
		(void)timer;
}


/**
 * Disable interrupt for timer counter compare event.
 */
INLINE void  stepper_tc_irq_disable(struct TimerCounter *timer)
{
		/* put here code to disable timer irq */

		//Only for test remove when implement this function
		(void)timer;
}

/**
 * Set delay for next interrupt compare event.
 */
INLINE void  stepper_tc_setDelay(struct TimerCounter *timer, stepper_time_t delay)
{
		/* put here code to set the delay for next irq */

		//Only for test remove when implement this function
		(void)timer;
		(void)delay;
}


/**
 * Set delay for next interrupt compare event.
 */
INLINE void  stepper_tc_resetTimer(struct TimerCounter *timer)
{
		/* put here code to reset the timer */

		//Only for test remove when implement this function
		(void)timer;
}

/**
 * Programm timer counter to generate a pulse on select TIO output.
 */
INLINE void FAST_FUNC stepper_tc_doPulse(struct TimerCounter *timer)
{
		/* put here code to generate a pulse */

		//Only for test remove when implement this function
		(void)timer;
}

/**
 * Programm timer counter to not generate a pulse on select TIO output.
 */
INLINE void FAST_FUNC stepper_tc_skipPulse(struct TimerCounter *timer)
{
		/* put here code to skip a pulse */

		//Only for test remove when implement this function
		(void)timer;
}

void stepper_tc_setup(int index, stepper_isr_t callback, struct Stepper *motor);
void stepper_tc_init(void);

#endif /*DRV_STEPPER_AVR_H */
