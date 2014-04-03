
#ifndef FSK_MODEM_HW
#define FSK_MODEM_HW

#include "cfg/cfg_arch.h"

#include <avr/io.h>

struct Afsk;
void hw_afsk_adcInit(int ch, struct Afsk *_ctx);
void hw_afsk_dacInit(int ch, struct Afsk *_ctx);


#define AFSK_ADC_INIT(ch, ctx) hw_afsk_adcInit(ch, ctx)

#define AFSK_STROBE_INIT() do { DDRB |= BV(5); } while (0)

#define AFSK_STROBE_ON()   do { PORTB |= BV(5); } while (0)

#define AFSK_STROBE_OFF()  do { PORTB &= ~BV(5); } while (0)

// Initialization, start and stop for DAC
#define AFSK_DAC_INIT(ch, ctx)   do { (void)ch, (void)ctx; DDRD |= 0xF0; DDRB |= BV(3); } while (0)
#define AFSK_DAC_IRQ_START(ch)   do { (void)ch; extern bool hw_afsk_dac_isr; PORTB |= BV(3); hw_afsk_dac_isr = true; } while (0)
#define AFSK_DAC_IRQ_STOP(ch)    do { (void)ch; extern bool hw_afsk_dac_isr; PORTB &= ~BV(3); hw_afsk_dac_isr = false; } while (0)

#endif
