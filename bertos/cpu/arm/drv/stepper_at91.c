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
 * \brief Stepper driver interface implementation.
 *
 * This module use the three timer on the at91 family, to generate a
 * six periodic variable pwm waveform. The pulse width is fix, and could
 * change by setting the STEPPER_DELAY_ON_COMPARE_C define, but you make
 * an attention to do this, becouse the pulse width is not exactly 
 * STEPPER_DELAY_ON_COMPARE_C. The pulse width depend also to latency 
 * time of cpu to serve an interrupt, this generate an pwm waveform affect
 * to noise. This noise not effect the period but only the pulse width,
 * becouse the raising edge is generate by hardware comply with the our
 * period settings.
 *
 * Note: is most important to set STEPPER_DELAY_ON_COMPARE_C value minor
 * than a interrupt time service, becouse the falling edge must be happen
 * inside to inerrupt service to guarantee a correct functionaly of pwm
 * generator.
 * 
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "stepper_at91.h"

#include "cfg/cfg_stepper.h"
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/types.h>
#include <cpu/irq.h>

#include <io/arm.h>


/*
 * Delay to set C compare to clear output
 * on select TIO output
 */
#define STEPPER_DELAY_ON_COMPARE_C 20

/*
 * Forward declaration for interrupt handler
 */
static ISR_PROTO(stepper_tc0_irq);
static ISR_PROTO(stepper_tc1_irq);
static ISR_PROTO(stepper_tc2_irq);

///< Static array of timer counter struct for stepper.
static struct TimerCounter stepper_timers[CONFIG_TC_STEPPER_MAX_NUM] =
{
	{ //Timer Counter settings for TIOA0 output pin
		.timer_id = TC0_ID,
		.blk_ctrl_set = TC_NONEXC0,
		.chl_mode_reg = &TC0_CMR,
		.chl_ctrl_reg = &TC0_CCR,
		.comp_effect_mask = TC_ACPA_MASK,
		.comp_effect_set = TC_ACPA_SET_OUTPUT,
		.comp_effect_clear = TC_ACPA_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_ACPC_MASK,
		.comp_effect_c_clear = TC_ACPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC0,
		.comp_reg = &TC0_RA,
		.comp_c_reg = &TC0_RC,
		.count_val_reg = &TC0_CV,
		.irq_enable_reg = &TC0_IER,
		.irq_disable_reg = &TC0_IDR,
		.irq_set_mask = BV(TC_CPAS),
		.irq_mask_reg = &TC0_IMR,
		.isr = stepper_tc0_irq,
		.status_reg = &TC0_SR,
		.tio_pin = TIOA0,
		.callback = NULL,
		.motor = NULL,
	},
	{ //Timer Counter settings for TIOB0 output pin
		.timer_id = TC0_ID,
		.blk_ctrl_set = TC_NONEXC0,
		.chl_mode_reg = &TC0_CMR,
		.chl_ctrl_reg = &TC0_CCR,
		.comp_reg = &TC0_RB,
		.comp_c_reg = &TC0_RC,
		.count_val_reg = &TC0_CV,
		.comp_effect_mask = TC_BCPB_MASK,
		.comp_effect_set = TC_BCPB_SET_OUTPUT,
		.comp_effect_clear = TC_BCPB_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_BCPC_MASK,
		.comp_effect_c_clear = TC_BCPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC0,
		.irq_enable_reg = &TC0_IER,
		.irq_disable_reg = &TC0_IDR,
		.irq_set_mask = BV(TC_CPBS),
		.irq_mask_reg = &TC0_IMR,
		.isr = stepper_tc0_irq,
		.status_reg = &TC0_SR,
		.tio_pin = TIOB0,
		.callback = NULL,
		.motor = NULL,
	},
	{ //Timer Counter settings for TIOA1 output pin
		.timer_id = TC1_ID,
		.blk_ctrl_set = TC_NONEXC1,
		.chl_mode_reg = &TC1_CMR,
		.chl_ctrl_reg = &TC1_CCR,
		.comp_reg = &TC1_RA,
		.comp_c_reg = &TC1_RC,
		.count_val_reg = &TC1_CV,
		.comp_effect_mask = TC_ACPA_MASK,
		.comp_effect_set = TC_ACPA_SET_OUTPUT,
		.comp_effect_clear = TC_ACPA_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_ACPC_MASK,
		.comp_effect_c_clear = TC_ACPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC1,
		.irq_enable_reg = &TC1_IER,
		.irq_disable_reg = &TC1_IDR,
		.irq_set_mask = BV(TC_CPAS),
		.irq_mask_reg = &TC1_IMR,
		.isr = stepper_tc1_irq,
		.status_reg = &TC1_SR,
		.tio_pin = TIOA1,
		.callback = NULL,
		.motor = NULL,
	},
	{ //Timer Counter settings for TIOB1 output pin
		.timer_id = TC1_ID,
		.blk_ctrl_set = TC_NONEXC1,
		.chl_mode_reg = &TC1_CMR,
		.chl_ctrl_reg = &TC1_CCR,
		.comp_reg = &TC1_RB,
		.comp_c_reg = &TC1_RC,
		.count_val_reg = &TC1_CV,
		.comp_effect_mask = TC_BCPB_MASK,
		.comp_effect_set = TC_BCPB_SET_OUTPUT,
		.comp_effect_clear = TC_BCPB_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_BCPC_MASK,
		.comp_effect_c_clear = TC_BCPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC1,
		.irq_enable_reg = &TC1_IER,
		.irq_disable_reg = &TC1_IDR,
		.irq_set_mask = BV(TC_CPBS),
		.irq_mask_reg = &TC1_IMR,
		.isr = stepper_tc1_irq,
		.status_reg = &TC1_SR,
		.tio_pin = TIOB1,
		.callback = NULL,
		.motor = NULL,
	},
	{ //Timer Counter settings for TIOA2 output pin
		.timer_id = TC2_ID,
		.blk_ctrl_set = TC_NONEXC2,
		.chl_mode_reg = &TC2_CMR,
		.chl_ctrl_reg = &TC2_CCR,
		.comp_reg = &TC2_RA,
		.comp_c_reg = &TC2_RC,
		.count_val_reg = &TC2_CV,
		.comp_effect_mask = TC_ACPA_MASK,
		.comp_effect_set = TC_ACPA_SET_OUTPUT,
		.comp_effect_clear = TC_ACPA_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_ACPC_MASK,
		.comp_effect_c_clear = TC_ACPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC2,
		.irq_enable_reg = &TC2_IER,
		.irq_disable_reg = &TC2_IDR,
		.irq_set_mask = BV(TC_CPAS),
		.irq_mask_reg = &TC2_IMR,
		.isr = stepper_tc2_irq,
		.status_reg = &TC2_SR,
		.tio_pin = TIOA2,
		.callback = NULL,
		.motor = NULL,
	},
	{ //Timer Counter settings for TIOB2 output pin
		.timer_id = TC2_ID,
		.blk_ctrl_set = TC_NONEXC2,
		.chl_mode_reg = &TC2_CMR,
		.chl_ctrl_reg = &TC2_CCR,
		.comp_reg = &TC2_RB,
		.comp_c_reg = &TC2_RC,
		.count_val_reg = &TC2_CV,
		.comp_effect_mask = TC_BCPB_MASK,
		.comp_effect_set = TC_BCPB_SET_OUTPUT,
		.comp_effect_clear = TC_BCPB_CLEAR_OUTPUT,
		.comp_effect_c_mask = TC_BCPC_MASK,
		.comp_effect_c_clear = TC_BCPC_CLEAR_OUTPUT,
		.ext_event_set = TC_EEVT_XC2,
		.irq_enable_reg = &TC2_IER,
		.irq_disable_reg = &TC2_IDR,
		.irq_set_mask = BV(TC_CPBS),
		.irq_mask_reg = &TC2_IMR,
		.isr = stepper_tc2_irq,
		.status_reg = &TC2_SR,
		.tio_pin = TIOB2,
		.callback = NULL,
		.motor = NULL,
	}
};

/**
 * Generic TIO interrupt handler.
 */
INLINE void stepper_tc_tio_irq(struct TimerCounter * t)
{
	//
	*t->chl_mode_reg &= ~t->comp_effect_c_mask;
	*t->chl_mode_reg |= t->comp_effect_c_clear;

	/*
	 * Cleat TIO output on c register compare.
	 * This generate an pulse with variable lenght, this
	 * depend to delay that interrupt is realy service.
	 */
	*t->comp_c_reg = *t->count_val_reg + STEPPER_DELAY_ON_COMPARE_C;

	//Call the associate callback
	t->callback(t->motor);

	*t->chl_mode_reg &= ~t->comp_effect_c_mask;
}


/*
 * Interrupt handler for timer counter TCKL0
 */
DECLARE_ISR(stepper_tc0_irq)
{
	/*
	 * Warning: when we read the status_reg register, we reset it.
	 * That mean if is occur an interrupt event we can read only
	 * the last that has been occur. To not miss an interrupt event
	 * we save the status_reg register and then we read it.
	 */
	uint32_t  status_reg = TC0_SR & TC0_IMR;

	if (status_reg & BV(TC_CPAS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOA0]);

	if (status_reg & BV(TC_CPBS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOB0]);

	/* Inform hw that we have served the IRQ */
	AIC_EOICR = 0;

}

/*
 * Interrupt handler for timer counter TCKL1
 */
DECLARE_ISR(stepper_tc1_irq)
{
	/*
	 * Warning: when we read the status_reg register, we reset it.
	 * That mean if is occur an interrupt event we can read only
	 * the last that has been occur. To not miss an interrupt event
	 * we save the status_reg register and then we read it.
	 */
	uint32_t  status_reg = TC1_SR & TC1_IMR;

	if (status_reg & BV(TC_CPAS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOA1]);

	if (status_reg & BV(TC_CPBS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOB1]);


	/* Inform hw that we have served the IRQ */
	AIC_EOICR = 0;
}


/*
 * Interrupt handler for timer counter TCKL2
 */
DECLARE_ISR(stepper_tc2_irq)
{

	/*
	 * Warning: when we read the status_reg register, we reset it.
	 * That mean if is occur an interrupt event we can read only
	 * the last that has been occur. To not miss an interrupt event
	 * we save the status_reg register and then we read it.
	 */
	uint32_t  status_reg = TC2_SR & TC2_IMR;

	if (status_reg & BV(TC_CPAS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOA2]);

	if (status_reg & BV(TC_CPBS))
		stepper_tc_tio_irq(&stepper_timers[TC_TIOB2]);

	/* Inform hw that we have served the IRQ */
	AIC_EOICR = 0;

}

/**
 * Timer couter setup.
 *
 * This function apply to select timer couter all needed settings.
 * Every settings are stored in stepper_timers[].
 */
void stepper_tc_setup(int index, stepper_isr_t callback, struct Stepper *motor)
{
	ASSERT(index < CONFIG_TC_STEPPER_MAX_NUM);

	motor->timer = &stepper_timers[index];

	//Disable PIO controller and enable TIO function
	TIO_PIO_PDR = BV(motor->timer->tio_pin);
	TIO_PIO_ABSR = BV(motor->timer->tio_pin);

	/*
	 * Sets timer counter in waveform mode.
	 * We set as default:
	 * - Waveform mode 00 (see datasheet for more detail.)
	 * - Master clock prescaler to STEPPER_MCK_PRESCALER
	 * - Set none external event
	 * - Clear pin output on comp_reg
	 * - None effect on reg C compare
	 */
	*motor->timer->chl_mode_reg = BV(TC_WAVE);
	*motor->timer->chl_mode_reg |= motor->timer->ext_event_set;
	*motor->timer->chl_mode_reg &= ~TC_WAVSEL_MASK;
	*motor->timer->chl_mode_reg |= TC_WAVSEL_UP;
	*motor->timer->chl_mode_reg |= STEPPER_MCK_PRESCALER;
	*motor->timer->chl_mode_reg |= motor->timer->comp_effect_clear;
	*motor->timer->chl_mode_reg &= ~motor->timer->comp_effect_c_mask;

	//Reset comp_reg and C compare register
	*motor->timer->comp_reg = 0;
	*motor->timer->comp_c_reg = 0;

	//Register interrupt vector
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/*
	 * Warning: To guarantee a correct management of interrupt event, we must
	 * trig the interrupt on level sensitive. This becouse, we have only a common
	 * line for interrupt request, and if we have at the same time two interrupt
	 * request could be that the is service normaly but the second will never
	 *  been detected and interrupt will stay active but never serviced.
	 */
	AIC_SVR(motor->timer->timer_id) = motor->timer->isr;
	AIC_SMR(motor->timer->timer_id) = AIC_SRCTYPE_INT_LEVEL_SENSITIVE;
	AIC_IECR = BV(motor->timer->timer_id);

	// Disable interrupt on select timer counter
	stepper_tc_irq_disable(motor->timer);

	IRQ_RESTORE(flags);

	//Register callback
	motor->timer->callback = callback;
	motor->timer->motor = motor;
}

/**
 * Timer counter init.
 */
void stepper_tc_init(void)
{
	STEPPER_STROBE_INIT;

	ASSERT(CONFIG_NUM_STEPPER_MOTORS <= CONFIG_TC_STEPPER_MAX_NUM);

	/*
	 * Enable timer counter:
	 * - power on all timer counter
	 * - disable all interrupt
	 * - disable all external event/timer source
	 */
	for (int i = 0; i < CONFIG_TC_STEPPER_MAX_NUM; i++)
	{
		PMC_PCER = BV(stepper_timers[i].timer_id);
		*stepper_timers[i].irq_disable_reg = 0xFFFFFFFF;
		TC_BMR = stepper_timers[i].blk_ctrl_set;
	}

	/*
	 * Enable timer counter and start it.
	 */
	for (int i = 0; i < CONFIG_TC_STEPPER_MAX_NUM; i++)
		*stepper_timers[i].chl_ctrl_reg = (BV(TC_CLKEN) | BV(TC_SWTRG));

}

