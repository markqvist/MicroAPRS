
#include "hardware.h"

#include <net/afsk.h>
#include <cpu/irq.h>

#include <avr/io.h>
#include <avr/interrupt.h>


static Afsk *ctx;

void hw_afsk_adcInit(int ch, Afsk *_ctx)
{
	ctx = _ctx;
	ASSERT(ch <= 5);

	AFSK_STROBE_INIT();
	AFSK_STROBE_OFF();
	/* Set prescaler to clk/8 (2 MHz), CTC, top = ICR1 */
	TCCR1A = 0;
	TCCR1B = BV(CS11) | BV(WGM13) | BV(WGM12);
	/* Set max value to obtain a 9600Hz freq */
	ICR1 = ((CPU_FREQ / 8) / 9600) - 1;

	/* Set reference to AVCC (5V), select CH */
	ADMUX = BV(REFS0) | ch;

	DDRC &= ~BV(ch);
	PORTC &= ~BV(ch);
	DIDR0 |= BV(ch);

	/* Set autotrigger on Timer1 Input capture flag */
	ADCSRB = BV(ADTS2) | BV(ADTS1) | BV(ADTS0);
	/* Enable ADC, autotrigger, 1MHz, IRQ enabled */
	/* We are using the ADC a bit out of specifications otherwise it's not fast enough for our
	 * purposes */
	ADCSRA = 	BV(ADEN) |	// ADC Enable
				BV(ADSC) |	// ADC Start converting
				BV(ADATE) | // Enable autotriggering
				BV(ADIE) |  // ADC Interrupt enable
				BV(ADPS2);  // Enable prescaler flag 2 (1-0-0 = division by 16 = 1MHz)
}


bool hw_afsk_dac_isr;

/*
 * This is how you declare an ISR.
 */
DECLARE_ISR(ADC_vect)
{
	TIFR1 = BV(ICF1);
	afsk_adc_isr(ctx, ((int16_t)((ADC) >> 2) - 128));
	if (hw_afsk_dac_isr)
		PORTD = afsk_dac_isr(ctx) & 0xF0;
	else
		PORTD = 128;
}
