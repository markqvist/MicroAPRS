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
 * \brief Low level test for stepper driver interface implementation.
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


#warning FIXME:This test is incomplete.. you MUST review..

#if 0
static void stepper_test_irq_schedule(struct Stepper *motor, stepper_time_t delay)
{
	stepper_tc_doPulse(motor->timer);
	stepper_tc_setDelay(motor->timer, delay);
}

static void stepper_test_irq(struct Stepper *motor)
{

	stepper_test_irq_schedule(motor, 300);
}
/*
 * Test a timer couter driver
 */
void stepper_timer_test_prestepper(struct Stepper *local_motor, struct StepperConfig *local_cfg, int index)
{
	local_cfg->pulse = 300;

	local_motor->cfg = local_cfg;
	stepper_tc_init(index, &stepper_test_irq, local_motor);
	stepper_tc_irq_enable(local_motor->timer);
}


bool su = true;
bool sub = true;
uint16_t periodo_st0 = 100;
uint16_t periodo_st1 = 233;

static void tc_irq(void) __attribute__ ((interrupt));
static void tc_irq(void)
{
	uint32_t status_reg = TC2_SR & TC2_IMR;

	if (status_reg & BV(TC_CPAS))
	{
		TC2_CMR &= ~TC_ACPA_MASK;
		if (su)
		{
			TC2_CMR |= TC_ACPA_CLEAR_OUTPUT;
			TC2_RA += periodo_st0;
		}
		else
		{
			TC2_CMR |= TC_ACPA_SET_OUTPUT;
			TC2_RA += periodo_st1;
		}
		su = !su;
	}
	if (status_reg & BV(TC_CPBS))
	{
		TC2_CMR &= ~TC_BCPB_MASK ;
		if (sub)
		{
			TC2_CMR |= TC_BCPB_CLEAR_OUTPUT;
			TC2_RB += periodo_st0;
		}
		else
		{
			TC2_CMR |= TC_BCPB_SET_OUTPUT;
			TC2_RB += periodo_st1;
		}
		sub = !sub;
	}
	/* Inform hw that we have served the IRQ */
	AIC_EOICR = 0;
}

/*
 * Test a timer couter hardware
 */
void stepper_timer_test_brute(void)
{
	PIOA_PDR |= BV(26) | BV(27);
	PIOA_BSR |= BV(26) | BV(27);

	// Power on TCLK0
	PMC_PCER |= BV(TC2_ID);// | BV(TC1_ID) | BV(TC2_ID);

	TC_BCR = 1;
	TC_BMR |= TC_NONEXC2;

	// Select waveform mode
	TC2_CMR = BV(TC_WAVE);

	TC2_CMR |= TC_EEVT_XC2;
	TC2_CMR |= TC_WAVSEL_UP;
	TC2_CMR |= TC_CLKS_MCK8;

	//Set waveform on TIOA and TIOB
	TC2_CMR |= TC_ACPA_SET_OUTPUT;
	TC2_CMR |= TC_BCPB_SET_OUTPUT;


	//Reset all comp_reg register
	TC2_RA = 0;
	TC2_RB = 0;

	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Set the vector. */
	AIC_SVR(TC2_ID) = tc_irq;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(TC2_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED;
	/* Enable the USART IRQ */
	AIC_IECR = BV(TC2_ID);

	IRQ_RESTORE(flags);

	// Disable all interrupt
	TC2_IDR = 0xFFFFFFFF;

	//Enable interrupt on RA, RB
	TC2_IER = BV(TC_CPAS) | BV(TC_CPBS);

	//Enable timer and trig it
	TC2_CCR = BV(TC_CLKEN) | BV(TC_SWTRG);
}
#endif

