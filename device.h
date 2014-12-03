#ifndef DEVICE_CONFIGURATION
#define DEVICE_CONFIGURATION

// CPU settings
#define F_CPU 16000000
#define FREQUENCY_CORRECTION 0

// Sampling & timer setup
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

// Serial settings
#define BAUD 9600

// Port settings
#define DAC_PORT PORTB
#define DAC_DDR  DDRB
#define ADC_PORT PORTC
#define ADC_DDR   DDRC

#endif