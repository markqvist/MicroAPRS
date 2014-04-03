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
 * All Rights Reserved.
 * -->
 *
 * \brief Stepper hardware-specific definitions
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <drv/stepper.h>
#include <io/arm.h>

/**
 * Setting master clock prescaler for all timer couter
 *
 * You could choise one of these:
 * - TC_CLKS_MCK2: Selects MCK / 2
 * - TC_CLKS_MCK8: Selects MCK / 8
 * - TC_CLKS_MCK32: Selects MCK / 32
 * - TC_CLKS_MCK128: Selects MCK / 128
 * - TC_CLKS_MCK1024: Selects MCK / 1024
 */
#if STEPPER_PRESCALER_LOG2 == 1
	#define STEPPER_MCK_PRESCALER TC_CLKS_MCK2
#elif STEPPER_PRESCALER_LOG2 == 3
	#define STEPPER_MCK_PRESCALER TC_CLKS_MCK8
#elif STEPPER_PRESCALER_LOG2 == 5
	#define STEPPER_MCK_PRESCALER TC_CLKS_MCK32
#elif STEPPER_PRESCALER_LOG2 == 7
	#define STEPPER_MCK_PRESCALER TC_CLKS_MCK128
#elif STEPPER_PRESCALER_LOG2 == 10
	#define STEPPER_MCK_PRESCALER TC_CLKS_MCK1024
#else
	#error Unsupported stepper prescaler value.
#endif

/**
 * Timer counter hw enumeration.
 */
enum
{
	TC_TIOA0 = 0,
	TC_TIOB0,
	TC_TIOA1,
	TC_TIOB1,
	TC_TIOA2,
	TC_TIOB2,

	TC_CNT
};

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
	uint32_t blk_ctrl_set;         ///< Control block setting for this timer
	reg32_t *chl_mode_reg;         ///< Channel mode register
	reg32_t *chl_ctrl_reg;         ///< Channel control register
	reg32_t *comp_reg;             ///< Compare register
	reg32_t *comp_c_reg;           ///< C compare register
	reg32_t *count_val_reg;        ///< Current timer counter value
	uint32_t comp_effect_mask;     ///< Bit mask for TIO register compare effects
	uint32_t comp_effect_set;      ///< Set TIO on register compare event
	uint32_t comp_effect_clear;    ///< Clear TIO on register compare event
	uint32_t comp_effect_c_mask;   ///< Bit mask for TIO on C register compare effects
	uint32_t comp_effect_c_clear;  ///< Clear TIO on C register compare event
	uint32_t ext_event_set;        ///< Setting for extern event trigger for TIOB
	reg32_t *irq_enable_reg;       ///< Enable interrupt register
	reg32_t *irq_disable_reg;      ///< Disable interrupt register
	uint32_t irq_set_mask;         ///< IRQ flag bit for select TIO
	reg32_t *irq_mask_reg;         ///< IRQ mask register
	irq_t isr;                     ///< IRQ handler
	reg32_t *status_reg;           ///< Timer status register
	int tio_pin;                   ///< Timer I/O pin
	stepper_isr_t callback;        ///< Interrupt callback pointer
	struct Stepper *motor;         ///< Stepper context structure

} TimerCounter;

/**
 * Enable interrupt for timer counter compare event.
 */
INLINE void stepper_tc_irq_enable(struct TimerCounter *timer)
{
	*timer->irq_enable_reg = timer->irq_set_mask;
}

/**
 * Disable interrupt for timer counter compare event.
 */
INLINE void  stepper_tc_irq_disable(struct TimerCounter *timer)
{
	*timer->irq_disable_reg = timer->irq_set_mask;
}

/**
 * Set delay for next interrupt compare event.
 */
INLINE void  stepper_tc_setDelay(struct TimerCounter *timer, stepper_time_t delay)
{
	*timer->comp_reg += delay;
}


/**
 * Set delay for next interrupt compare event.
 */
INLINE void  stepper_tc_resetTimer(struct TimerCounter *timer)
{
	*timer->comp_reg = 0;
}

/**
 * Programm timer counter to generate a pulse on select TIO output.
 */
INLINE void FAST_FUNC stepper_tc_doPulse(struct TimerCounter *timer)
{
	*timer->chl_mode_reg &= ~timer->comp_effect_mask;
	*timer->chl_mode_reg |= timer->comp_effect_set;
}

/**
 * Programm timer counter to not generate a pulse on select TIO output.
 */
INLINE void FAST_FUNC stepper_tc_skipPulse(struct TimerCounter *timer)
{
	*timer->chl_mode_reg &= ~timer->comp_effect_mask;
}

void stepper_tc_setup(int index, stepper_isr_t callback, struct Stepper *motor);
void stepper_tc_init(void);

/*
 * Test the hardware timer counter on board.
 * This test generate a square waveform through irq, setting
 * the timer register.
 */
void stepper_timer_test_brute(void);
/*
 * Test the timer driver structure.
 * This test generate a square waveform through irq.
 * The irq callback is programmable, and all timer setting
 * are save in one data structure. Every pulse is generate through
 * a call of this irq callback.
 */
void stepper_timer_test_prestepper(struct Stepper *local_motor, struct StepperConfig *local_cfg, int index);
