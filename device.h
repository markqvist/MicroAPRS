#include "util/constants.h"

#ifndef DEVICE_CONFIGURATION
#define DEVICE_CONFIGURATION

// CPU settings
#define TARGET_CPU m328p
#define F_CPU 16000000
#define FREQUENCY_CORRECTION 0

// ADC settings
#define OPEN_SQUELCH true
#define ADC_REFERENCE REF_3V3
// OR
//#define ADC_REFERENCE REF_5V

// Sampling & timer setup
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

// Serial protocol settings
#define SERIAL_PROTOCOL PROTOCOL_KISS
// OR
//#define SERIAL_PROTOCOL PROTOCOL_SIMPLE_SERIAL

// AX25 settings
#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
    #define CUSTOM_FRAME_SIZE 330
#endif

// Serial settings
#define BAUD 9600
#define SERIAL_DEBUG false
#define TX_MAXWAIT 2UL

// Port settings
#if TARGET_CPU == m328p
    #define DAC_PORT PORTD
    #define DAC_DDR  DDRD
    #define LED_PORT PORTB
    #define LED_DDR  DDRB
    #define ADC_PORT PORTC
    #define ADC_DDR  DDRC
#endif

#endif