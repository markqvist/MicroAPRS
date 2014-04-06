
#ifndef FSK_MODEM_HW
#define FSK_MODEM_HW

#include "cfg/cfg_arch.h"

#include <avr/io.h>

struct Afsk;
void hw_afsk_adcInit(int ch, struct Afsk *_ctx);
void hw_afsk_dacInit(int ch, struct Afsk *_ctx);

// ADC initialization
#define AFSK_ADC_INIT(ch, ctx) hw_afsk_adcInit(ch, ctx)

// Here's some macros for controlling the RX/TX LEDs
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

// Initialization, start and stop for DAC
#define AFSK_DAC_INIT(ch, ctx)   do { (void)ch, (void)ctx; DDRD |= 0xF4; DDRB |= BV(3); } while (0)
#define AFSK_DAC_IRQ_START(ch)   do { (void)ch; extern bool hw_afsk_dac_isr; PORTB |= BV(3); hw_afsk_dac_isr = true; } while (0)
#define AFSK_DAC_IRQ_STOP(ch)    do { (void)ch; extern bool hw_afsk_dac_isr; PORTB &= ~BV(3); hw_afsk_dac_isr = false; } while (0)

#endif
