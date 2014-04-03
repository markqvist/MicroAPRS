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
 * \brief Test for PWM driver (implementation)
 *
 * This is a simple test for PWM driver. This module
 * is target independent, so you can test all target that
 * BeRTOS support.
 * To use this test you should include a pwm_map.h header where
 * are defined the PWM channels for your target. Then you should add
 * or remove a test setting in pwm_test_cfg array, and edit a value for
 * your specific test.
 * Afther this, all is ready and you can test PWM driver.
 *
 * The test check first if all PWM channel starts, and then try
 * to change a PWM duty cicle for all channel.
 * The change of duty cycle is operate when a PWM channel is enable,
 * in this way you can see if a pwm signal is clean and work properly.
 * The duty value is change incrementaly, and when it arrive to 100% or 0%,
 * we reset the duty value and restart the test.
 * Further the duty test, we check also a PWM polarity, infact when we
 * reach a reset duty value, we invert a polary of PWM wavform.
 * So you can see if the hardware manage correctly this situation.
 *
 * Note: To be simple and target independently we not use a timer module,
 * and so the delay is do with a for cycle.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "hw/pwm_map.h" // For PwmDev and channel avaible on thi target
#include "cfg/cfg_pwm.h"
#include <cfg/macros.h>
#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PWM_LOG_LEVEL
#define LOG_FORMAT        PWM_LOG_FORMAT
#include <cfg/log.h>   // for logging system

#include <cpu/types.h>
#include <cpu/irq.h>

#include <drv/pwm.h>
#include CPU_HEADER(pwm)

#define DELAY_TIME   10000  // This is a number of for cycle before to set a new value of duty
#define PWM_DUTY_INC   200  // Incremental value for duty


/*
 * Simple struct to store
 * the testing value.
 */
typedef struct PwmTest
{
        int ch;
        bool pol;
        pwm_freq_t freq;
        pwm_duty_t duty;
} PwmTest;

/*
 * Test settings for each channel.
 *
 * Frequency value is in Hz.
 *
 * Esample of value for duty cycle"
 *
 * - 100% => 0xFFFFFFFF
 * - 80%  => 0xCCCCCCCC
 * - 75%  => 0xBFFFFFFF
 * - 50%  => 0x7FFFFFFF
 * - 25%  => 0x3FFFFFFF
 * - 33%  => 0x55555555
 * - 16%  => 0x2AAAAAAA
 */
static PwmTest pwm_test_cfg[PWM_CNT] =
{
        /* Channel, polarity, frequecy,   duty */
        {        0,    false,    100UL,      0 }, /*     100Hz,  0% duty */
        {        1,    false,   1000UL, 0x7FFF }, /*      1KHz, 50% duty */
        {        2,    false,  12356UL, 0x5555 }, /* 12,356KHz, 33% duty */
        {        3,    false, 100000UL, 0xCCCC }  /*    100KHz, 80% duty */
};

/*
 * Setup all needed to test PWM on AT91
 *
 */
int pwm_testSetup(void)
{
	LOG_INFO("Init pwm..");
	pwm_init();
	LOG_INFO("done.\n");

	return 0;
}


/*
 * Test suit for genation of pwm waveform.
 *
 */
void NORETURN pwm_testRun(void)
{
        pwm_duty_t duty = 0;
        int delay = 0;

        pwm_testSetup();

        LOG_INFO("\n\n===== BeRTOS PWM test =====\n\n");

        for (int i = 0; i < PWM_CNT; i++)
        {
                LOG_INFO("PWM test ch[%d]\n", pwm_test_cfg[i].ch);
                LOG_INFO("--> set pol[%d]", pwm_test_cfg[i].pol);
                LOG_INFO("\n(Note: if polarity is false the output waveform start at high level,\n see low level implentation for detail)i\n");
                pwm_setPolarity(pwm_test_cfg[i].ch, pwm_test_cfg[i].pol);
                LOG_INFO("..ok\n");

                LOG_INFO("--> set freq[%ld]", pwm_test_cfg[i].freq);
                pwm_setFrequency(pwm_test_cfg[i].ch, pwm_test_cfg[i].freq);
                LOG_INFO("..ok\n");

                LOG_INFO("--> set duty[%d]", pwm_test_cfg[i].duty);
                pwm_setDuty(pwm_test_cfg[i].ch, pwm_test_cfg[i].duty);
                LOG_INFO("..ok\n");

                LOG_INFO("--> Enable pwm");
                pwm_enable(pwm_test_cfg[i].ch, true);
                LOG_INFO("..ok\n");
        }

        LOG_INFO("\n-------------------------- Dinamic PWM test --------------------------\n");
        LOG_INFO("We test if driver change correctly the duty cycle durind it working.\n");
        LOG_INFO("On your oscilloscope you should see the pwm singal that increase until\n");
        LOG_INFO("the duty value is 100%%. After this value we invert a polarity of pwm,\n");
        LOG_INFO("and repeat the test. But now you should see that pwm duty decreasing until\n");
        LOG_INFO("0%% duty value.\nAfter that, we repeat the test from beginning.\n\n");

        for (;;)
        {
                if (delay == DELAY_TIME)
                {
                        for (int i = 0; i < PWM_CNT; i++)
                        {
                                LOG_INFO("PWM test ch[%d]\n", pwm_test_cfg[i].ch);
                                LOG_INFO("--> set duty[%d]", duty);
                                pwm_setDuty(pwm_test_cfg[i].ch, duty);
                                LOG_INFO("..ok\n");
                        }
                        LOG_INFO("\n++++++++++++++++++++\n");
                        duty += PWM_DUTY_INC;
                        delay = 0;
                }

                //Reset duty cycle overflow
                if (duty >= (pwm_duty_t)0xFFFF)
                {
                        duty = 0;
                        for (int i = 0; i < PWM_CNT; i++)
                        {
                                LOG_INFO("Duty reset, swap polarity:\n");
                                LOG_INFO("--> pol from [%d] to [%d]", pwm_test_cfg[i].pol, !pwm_test_cfg[i].pol);

                                pwm_test_cfg[i].pol = !pwm_test_cfg[i].pol;
                                pwm_setPolarity(pwm_test_cfg[i].ch, pwm_test_cfg[i].pol);

                                LOG_INFO("..ok\n");
                        }
                        LOG_INFO("\n++++++++++++++++++++\n");
                }
                delay++;
        }
}

/*
 * End a PWM Test.
 * (Unused)
 */
int pwm_testTearDown(void)
{
	/*    */
	return 0;
}

/*
 * Empty main.
 *
 * Look it as exmple or use it if
 * you want test a PWM driver stand alone.
 */
#if 0
int main(void)
{
	IRQ_ENABLE;
	kdbg_init();

	pwm_testRun();

	for(;;)
	{
	}

}
#endif


