
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

	// We need to do some configuration on the Timer/Counter Control
	// Register 1, aka Timer1
	// The following bits are set:
	// CS11: ClockSource 11, sets the prescaler to 8, ie 2MHz
	// WGM13 and WGM12 together enables Timer Mode 12, which
	// is Clear Timer on Compare, compare set to TOP, and the
	// source for the TOP value is ICR1 (Input Capture Register1).
	TCCR1A = 0;									
	TCCR1B = BV(CS11) | BV(WGM13) | BV(WGM12);

	// Then we set the ICR1 register to what count value we want to
	// reset (and thus trigger the interrupt) at.
	// Since the prescaler is set to 2MHz, the counter will be
	// incremented two million times each second, and we want the
	// interrupt to trigger 9600 time each second. The formula for
	// calculating the value of ICR1 (the TOP value) is:
	//    (CPUClock / Prescaler) / desired frequency - 1
	// So that's what well put in this register to set up our
	// 9.6KHz sampling rate.
	ICR1 = ((CPU_FREQ / 8) / 9600) - 1;

	// Set reference to AVCC (5V), select pin
	// Set the ADMUX register. The first part (BV(REFS0)) sets
	// the reference voltage to VCC (5V), and the next selects
	// the ADC channel (basically what pin we are capturing on)
	ADMUX = BV(REFS0) | ch;

	DDRC &= ~BV(ch);	// Set the selected channel (pin) to input
	PORTC &= ~BV(ch);	// Initialize the selected pin to LOW
	DIDR0 |= BV(ch);	// Disable the Digital Input Buffer on selected pin

	// Now a little more configuration to get the ADC working
	// the way we want
	ADCSRB = 	BV(ADTS2) |	// Setting these three on (1-1-1) sets the ADC to
				BV(ADTS1) |	// "Timer1 capture event". That means we can declare
				BV(ADTS0);	// an ISR in the ADC Vector, that will then get called
							// everytime the ADC has a sample ready, which will
							// happen at the 9.6Khz sampling rate we set up earlier
				
	ADCSRA = 	BV(ADEN) |	// ADC Enable - Yes, we need to turn it on :)
				BV(ADSC) |	// ADC Start Converting - Tell it to start doing conversions
				BV(ADATE) | // Enable autotriggering - Enables the autotrigger on complete
				BV(ADIE) |  // ADC Interrupt enable - Enables an interrupt to be called
				BV(ADPS2);  // Enable prescaler flag 2 (1-0-0 = division by 16 = 1MHz)
							// This sets the ADC to run at 1MHz. This is out of spec,
							// Since it's normal operating range is only up to 200KHz.
							// But don't worry, it's not dangerous! I promise it wont
							// blow up :) There is a downside to running at this speed
							// though, hence the "out of spec", which is that we get
							// a much lower resolution on the output. In this case,
							// it's not a problem though, since we don't need the full
							// 10-bit resolution, so we'll take fast and less precise!
}


// This declares the Interrupt Service routine that will
// get called everytime the ADC finishes taking a sample.
// What actually happens here is that we take a piece of
// code, store it somewhere in memory, and then put the
// address of that "somewhere" into the Interrupt Vector
// Table of the processor, in this case the position 
// "ADC_vect". This lets the processor know what to do
// when all the timing and configuration we just set up
// finally* ends up triggering the interrupt.
bool hw_afsk_dac_isr;
DECLARE_ISR(ADC_vect) {
	TIFR1 = BV(ICF1);

	// Call the routine for analysing the captured sample
	// Notice that we read the ADC sample, and then bitshift
	// by two places to the right, effectively eliminating
	// two bits of precision. But we didn't have those
	// anyway, because the ADC is running at high speed.
	// We then subtract 128 from the value, to get the
	// representation to match an AC waveform. We need to
	// do this because the AC waveform (from the audio input)
	// is biased by +2.5V, which is nessecary, since the ADC
	// can't read negative voltages. By doing this simple
	// math, we bring it back to an AC representation
	// we can do further calculations on.
	afsk_adc_isr(context, ((int16_t)((ADC) >> 2) - 128));

	// We also need to check if we're supposed to spit
	// out some modulated data to the DAC.
	if (hw_afsk_dac_isr)
		// If there is, it's easy to actually do so. We
		// calculate what the sample should be in the
		// DAC ISR, and apply the bitmask 11110000. This
		// simoultaneously spits out our 4-bit digital
		// sample to the four pins connected to our DAC
		// circuit, which then converts it to an analog
		// waveform. The reason for the " | BV(3)" is that
		// we also need to trigger another pin controlled
		// by the PORTD register. This is the PTT pin
		// which tells the radio to open it transmitter.
		PORTD = (afsk_dac_isr(context) & 0xF0) | BV(3); 
	else
		// If we're not supposed to transmit anything, we
		// keep quiet by continously sending 128, which
		// when converted to an AC waveform by the DAC,
		// equates to a steady, unchanging 0 volts.
		PORTD = 128;
}


// * "finally" is probably the wrong description here.
// "All the f'ing time" is probably more accurate :)
// but it felt like it was a long way down here,
// writing all the explanations. I think this is a
// nice testament to how efficient and smart these
// processors are. The actual code to set up what
// took a long time to explain, is really very short.