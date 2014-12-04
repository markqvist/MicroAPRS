#ifndef DEVICE_CONFIGURATION
#define DEVICE_CONFIGURATION

// CPU settings
#define F_CPU 16000000
#define FREQUENCY_CORRECTION 0

// Sampling & timer setup
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

// Serial settings
#define BAUD 115200

// Port settings
#define DAC_PORT PORTD
#define DAC_DDR  DDRD
#define LED_PORT PORTB
#define LED_DDR  DDRB
#define ADC_PORT PORTC
#define ADC_DDR  DDRC

#endif