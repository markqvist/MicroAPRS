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
 * Copyright 2005, 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * \brief Low-level timer module for AVR MEGA (implementation).
 *
 * This module is automatically included so no need to include
 * in test list.
 * notest: avr
 */

#include <drv/timer_mega.h>
#include <cfg/macros.h> // BV()

#include <cpu/types.h>
#include <cpu/irq.h>

#include <avr/io.h>

#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA2560
	#define REG_TIFR0 TIFR0
	#define REG_TIFR1 TIFR1
	#define REG_TIFR2 TIFR2
	#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		#define REG_TIFR3 TIFR3
	#endif

	#define REG_TIMSK0 TIMSK0
	#define REG_TIMSK1 TIMSK1
	#define REG_TIMSK2 TIMSK2
	#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		#define REG_TIMSK3 TIMSK3
	#endif

	#define REG_TCCR0A TCCR0A
	#define REG_TCCR0B TCCR0B

	#define REG_TCCR2A TCCR2A
	#define REG_TCCR2B TCCR2B

	#define REG_OCR0A  OCR0A
	#define REG_OCR2A  OCR2A

	#define BIT_OCF0A  OCF0A
	#define BIT_OCF2A  OCF2A

	#define BIT_OCIE0A OCIE0A
	#define BIT_OCIE2A OCIE2A
#else
	#define REG_TIFR0 TIFR
	#define REG_TIFR1 TIFR
	#define REG_TIFR2 TIFR
	#define REG_TIFR3 TIFR

	#define REG_TIMSK0 TIMSK
	#define REG_TIMSK1 TIMSK
	#define REG_TIMSK2 TIMSK
	#define REG_TIMSK3 ETIMSK

	#define REG_TCCR0A TCCR0
	#define REG_TCCR0B TCCR0

	#define REG_TCCR2A TCCR2
	#define REG_TCCR2B TCCR2

	#define REG_OCR0A  OCR0
	#define REG_OCR2A  OCR2

	#define BIT_OCF0A  OCF0
	#define BIT_OCF2A  OCF2

	#define BIT_OCIE0A OCIE0
	#define BIT_OCIE2A OCIE2
#endif

#if CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA103
    /* These ATMega have different prescaler options. */
    #define TIMER0_PRESCALER_64 BV(CS02)
    #define TIMER2_PRESCALER_64 (BV(CS21) | BV(CS20))
#else
    #define TIMER0_PRESCALER_64 (BV(CS01) | BV(CS00))
    #define TIMER2_PRESCALER_64 BV(CS22)
#endif

/** HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	void timer_hw_init(void)
	{
		cpu_flags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer flags */
		REG_TIFR0 = BV(BIT_OCF0A) | BV(TOV0);

		/* Setup Timer/Counter interrupt */
		REG_TCCR0A = 0;	// TCCR2 reg could be separate or a unique register with both A & B values, this is needed to
		REG_TCCR0B = 0;

		REG_TCCR0A = BV(WGM01);             /* Clear on Compare match */
			#if TIMER_PRESCALER == 64
			REG_TCCR0B |= TIMER0_PRESCALER_64;
			#else
				#error Unsupported value of TIMER_PRESCALER
			#endif

		TCNT0 = 0x00;                 /* Initialization of Timer/Counter */
		REG_OCR0A = OCR_DIVISOR;           /* Timer/Counter Output Compare Register */

		/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */
		REG_TIMSK0 &= ~BV(TOIE0);
		REG_TIMSK0 |= BV(BIT_OCIE0A);

		IRQ_RESTORE(flags);
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	void timer_hw_init(void)
	{
		cpu_flags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer overflow flag */
		REG_TIFR1 |= BV(TOV1);

		/* Fast PWM mode, 9 bit, 24 kHz, no prescaling. */
		#if (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 9)
			TCCR1A |= BV(WGM11);
			TCCR1A &= ~BV(WGM10);
			TCCR1B |= BV(WGM12) | BV(CS10);
			TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12));
		/* Fast PWM mode, 8 bit, 24 kHz, no prescaling. */
		#elif (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 8)
			TCCR1A |= BV(WGM10);
			TCCR1A &= ~BV(WGM11);
			TCCR1B |= BV(WGM12) | BV(CS10);
			TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12));
		#else
			#error Unsupported value of TIMER_PRESCALER or TIMER_HW_BITS
		#endif

		TCNT1 = 0x00;         /* initialization of Timer/Counter */

		/* Enable timer interrupt: Timer/Counter1 Overflow */
		REG_TIMSK1 |= BV(TOIE1);

		IRQ_RESTORE(flags);
	}

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)
	void timer_hw_init(void)
	{
		cpu_flags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer flags */
		REG_TIFR2 = BV(BIT_OCF2A) | BV(TOV2);

		/* Setup Timer/Counter interrupt */
		REG_TCCR2A = 0;	// TCCR2 reg could be separate or a unique register with both A & B values, this is needed to
		REG_TCCR2B = 0; // ensure correct initialization.

		REG_TCCR2A = BV(WGM21);
		#if TIMER_PRESCALER == 64
			REG_TCCR2B |= TIMER2_PRESCALER_64;
		#else
			#error Unsupported value of TIMER_PRESCALER
		#endif

		/* Clear on Compare match & prescaler = 64, internal sys clock.
		   When changing prescaler change TIMER_HW_HPTICKS_PER_SEC too */
		TCNT2 = 0x00;         /* initialization of Timer/Counter */
		REG_OCR2A = (uint8_t)OCR_DIVISOR;   /* Timer/Counter Output Compare Register */

		/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */
		REG_TIMSK2 &= ~BV(TOIE2);
		REG_TIMSK2 |= BV(BIT_OCIE2A);

		IRQ_RESTORE(flags);
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW3)

	#if CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P || CPU_AVR_ATMEGA32
		#error For select target there is not TIMER_ON_OVERFLOW3, please select an other one.
	#endif

	void timer_hw_init(void)
	{
		cpu_flags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer overflow flag */
		REG_TIFR3 |= BV(TOV3);

		/* Fast PWM mode, 9 bit, 24 kHz, no prescaling. */
		#if (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 9)
			TCCR3A |= BV(WGM31);
			TCCR3A &= ~BV(WGM30);
			TCCR3B |= BV(WGM32) | BV(CS30);
			TCCR3B &= ~(BV(WGM33) | BV(CS31) | BV(CS32));
		/* Fast PWM mode, 8 bit, 24 kHz, no prescaling. */
		#elif (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 8)
			TCCR3A |= BV(WGM30);
			TCCR3A &= ~BV(WGM31);
			TCCR3B |= BV(WGM32) | BV(CS30);
			TCCR3B &= ~(BV(WGM33) | BV(CS31) | BV(CS32));
		#else
			#error Unsupported value of TIMER_PRESCALER or TIMER_HW_BITS
		#endif

		/* initialization of Timer/Counter */
		TCNT3 = 0x00;

		/* Enable timer interrupt: Timer/Counter3 Overflow */
		REG_TIMSK3 |= BV(TOIE3);

		IRQ_RESTORE(flags);
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

