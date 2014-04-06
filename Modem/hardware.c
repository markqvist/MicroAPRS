
#include "hardware.h"
#include "afsk.h"

#include <cpu/irq.h>

#include <avr/io.h>
#include <avr/interrupt.h>


static Afsk *context;

void hw_afsk_adcInit(int ch, Afsk *_context)
{
	context = _context;
	ASSERT(ch <= 5);

	// Timer/Counter Control Register 1 (Timer1 settings, for short)
	// Set prescaler to clk/8 (2 MHz), CTC, top = ICR1
	TCCR1A = 0;
	TCCR1B = BV(CS11) | BV(WGM13) | BV(WGM12);
	// Configure ICR1 to get 9.6KHz sampling rate
	ICR1 = ((CPU_FREQ / 8) / 9600) - 1;	// Input capture register

	// Set reference to AVCC (5V), select pin
	ADMUX = BV(REFS0) | ch;

	DDRC &= ~BV(ch);
	PORTC &= ~BV(ch);
	DIDR0 |= BV(ch);

	// Set autotrigger on Timer1 Input capture flag
	ADCSRB = 	BV(ADTS2) |	//  Setting these three on (1-1-1) sets the ADC to
				BV(ADTS1) |	//  "Timer1 capture event"
				BV(ADTS0);	// 
				
	ADCSRA = 	BV(ADEN) |	// ADC Enable
				BV(ADSC) |	// ADC Start converting
				BV(ADATE) | // Enable autotriggering
				BV(ADIE) |  // ADC Interrupt enable
				BV(ADPS2);  // Enable prescaler flag 2 (1-0-0 = division by 16 = 1MHz)
}


// Declare ADC ISR
bool hw_afsk_dac_isr;
DECLARE_ISR(ADC_vect) {
	TIFR1 = BV(ICF1);
	afsk_adc_isr(context, ((int16_t)((ADC) >> 2) - 128));
	if (hw_afsk_dac_isr)
		PORTD = (afsk_dac_isr(context) & 0xF0) | BV(3); 
	else
		PORTD = 128;
}