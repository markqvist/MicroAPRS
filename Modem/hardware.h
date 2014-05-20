//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#ifndef FSK_MODEM_HW
#define FSK_MODEM_HW

#include "cfg/cfg_arch.h"    // Architecture configuration

#include <avr/io.h>          // AVR IO functions from BertOS

//////////////////////////////////////////////////////
// Definitions and some useful macros               //
//////////////////////////////////////////////////////

// Forward declaration of our modem "object"
struct Afsk;

// Function declarations
void hw_afsk_adcInit(int ch, struct Afsk *_ctx);
void hw_afsk_dacInit(int ch, struct Afsk *_ctx);

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

// A shorthand macro for initializing the ADC.
// It just calls the actual ADC initialization code
// in "hardware.c"
#define AFSK_ADC_INIT(ch, ctx) hw_afsk_adcInit(ch, ctx)

// Initialization of the DAC pins. The DDRD register
// configures pins 0 through 7 for input or output.
// DDR stands for Data Direction Register. By setting
// it to 0xF8 we set 11111000, which means the pins
// 3, 4, 5, 6 and 7 will be set to output.
#define AFSK_DAC_INIT()   do { DDRD |= 0xF8; } while (0)

// These two macros start and stop the DAC routine
// being called in our timer interrupt. For starting
// it, we set a boolean flag to true, and false for
// stopping it. We also turn on and off pin 3 to trigger
// the PTT of the radio.
#define AFSK_DAC_IRQ_START()   do { extern bool hw_afsk_dac_isr; PORTD |= BV(3); hw_afsk_dac_isr = true; } while (0)
#define AFSK_DAC_IRQ_STOP()    do { extern bool hw_afsk_dac_isr; PORTD &= ~BV(3); hw_afsk_dac_isr = false; } while (0)

#define AFSK_HW_PTT_ON()     do { extern bool hw_ptt_on; hw_ptt_on = true; } while (0)
#define AFSK_HW_PTT_OFF()    do { extern bool hw_ptt_on; hw_ptt_on = false; } while (0)

#endif
