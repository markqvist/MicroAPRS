//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#ifndef FSK_MODEM_HW
#define FSK_MODEM_HW

#include "cfg/cfg_arch.h"	// Architecture configuration

#include <avr/io.h>			// AVR IO functions from BertOS

//////////////////////////////////////////////////////
// Definitions and some useful macros               //
//////////////////////////////////////////////////////

// Forward declaration of our modem "object"
struct Afsk;

// Function declarations
void hw_afsk_adcInit(int ch, struct Afsk *_ctx);
void hw_afsk_dacInit(int ch, struct Afsk *_ctx);

// A shorthand macro for initializing the ADC.
// It just calls the actual ADC initialization code
// in "hardware.c"
#define AFSK_ADC_INIT(ch, ctx) hw_afsk_adcInit(ch, ctx)

// Here's some macros for controlling the RX/TX LEDs
// THE _INIT() functions writes to the DDRB register
// to configure the pins as output pins, and the _ON()
// and _OFF() functions writes to the PORT registers
// to turn the pins on or off.
#define LED_TX_INIT() do { DDRB |= BV(1); } while (0)
#define LED_TX_ON()   do { PORTB |= BV(1); } while (0)
#define LED_TX_OFF()  do { PORTB &= ~BV(1); } while (0)

#define LED_RX_INIT() do { DDRB |= BV(2); } while (0)
#define LED_RX_ON()   do { PORTB |= BV(2); } while (0)
#define LED_RX_OFF()  do { PORTB &= ~BV(2); } while (0)


// FIXME: remove these, they're in the DAC writes now
#define PTT_INIT() do { DDRD |= BV(3); } while (0)
#define PTT_ON()   do { PORTD |= BV(3); } while (0)
#define PTT_OFF()  do { PORTD &= ~BV(3); } while (0)

// Initialization of the DAC pins. The DDRD register
// configures pins 0 through 7 for input or output.
// DDR stands for Data Direction Register. By setting
// it to 0xF8 we set 11111000, which means the pins
// 3, 4, 5, 6 and 7 will be set to output.
// FIXME: remove ch and ctx
#define AFSK_DAC_INIT(ch, ctx)   do { (void)ch, (void)ctx; DDRD |= 0xF8; DDRB |= BV(3); } while (0)

// These two macros start and stop the DAC being
// triggered in our timer interrupt. For starting
// it, we set a boolean flag to true, and false for
// stopping it. We also turn on and off pin 3 to trigger
// the PTT of the radio.
#define AFSK_DAC_IRQ_START(ch)   do { (void)ch; extern bool hw_afsk_dac_isr; PORTB |= BV(3); hw_afsk_dac_isr = true; } while (0)
#define AFSK_DAC_IRQ_STOP(ch)    do { (void)ch; extern bool hw_afsk_dac_isr; PORTB &= ~BV(3); hw_afsk_dac_isr = false; } while (0)

#endif
