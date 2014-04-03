
#ifndef FSK_MODEM_H
#define FSK_MODEM_H

#include "config.h"
#include "hardware.h"

#include <cfg/compiler.h>
#include <io/kfile.h>
#include <struct/fifobuf.h>


#define SAMPLERATE 9600
#define BITRATE    1200
#define SAMPLESPERBIT (SAMPLERATE / BITRATE)

typedef struct Hdlc
{
	uint8_t demod_bits; ///< Bitstream from the demodulator.
	uint8_t bit_idx;    ///< Current received bit.
	uint8_t currchar;   ///< Current received character.
	bool rxstart;       ///< True if an HDLC_FLAG char has been found in the bitstream.
} Hdlc;

#define AFSK_RXFIFO_OVERRUN BV(0)

typedef struct Afsk
{
		// I/O hardware pins
	int adcPin;				// Pin for incoming signal
	int dacPin;				// Pin for outgoing signal

	// General values
	Hdlc hdlc; 					// We need a link control structure
	uint16_t preambleLength;	// Length of sync preamble
	uint16_t tailLength;		// Length of transmission tail

	// Modulation values
	uint8_t sample_index;		// Current sample index for outgoing bit 
	uint8_t currentOutputByte;	// Current byte to be modulated
	uint8_t txBit;				// Mask of current modulated bit

	uint8_t bitstuffCount;		// Counter for bit-stuffing

	uint16_t phaseAcc;			// Phase accumulator
	uint16_t phaseInc;			// Phase increment per sample

	FIFOBuffer txFifo;			// FIFO for transmit data
	uint8_t txBuffer[TX_BUFLEN];// Actial data storage for said FIFO

	volatile bool sending;		// Set when modem is sending

	// Demodulation values
	FIFOBuffer delayFifo;		// Delayed FIFO for frequency discrimination
	int8_t delayBuffer[5];			// Actual data storage for said FIFO

	FIFOBuffer rxFifo;			// FIFO for received data
	uint8_t rxBuffer[RX_BUFLEN];// Actual data storage for said FIFO

	int16_t iirX[2];			// IIR Filter X cells
	int16_t iirY[2];			// IIR Filter Y cells

	uint8_t sampledBits;		// Bits sampled by the demodulator (at ADC speed)
	int8_t currentPhase;		// Current phase of the demodulator
	uint8_t actualBits;			// Actual found bits at correct bitrate

	volatile int status;		// Status of the modem, 0 means OK

/*
	KFile fd;
	int adc_ch;
	int dac_ch;
	uint8_t sample_count;
	uint8_t curr_out;
	uint8_t tx_bit;
	bool bit_stuff;
	uint8_t stuff_cnt;
	uint16_t phase_acc;
	uint16_t phase_inc;
	FIFOBuffer delay_fifo;
	int8_t delay_buf[SAMPLESPERBIT / 2 + 1];
	FIFOBuffer rx_fifo;
	uint8_t rx_buf[CONFIG_AFSK_RX_BUFLEN];
	FIFOBuffer tx_fifo;
	uint8_t tx_buf[CONFIG_AFSK_TX_BUFLEN];
	int16_t iir_x[2];
	int16_t iir_y[2];
	uint8_t sampled_bits;
	int8_t curr_phase;
	uint8_t found_bits;
	volatile bool sending;
	volatile int status;
	Hdlc hdlc;
	uint16_t preamble_len;
	uint16_t trailer_len;
	*/
} Afsk;

#define KFT_AFSK MAKE_ID('F', 'S', 'K', 'M')

INLINE Afsk *AFSK_CAST(KFile *fd)
{
  ASSERT(fd->_type == KFT_AFSK);
  return (Afsk *)fd;
}


void afsk_adc_isr(Afsk *af, int8_t sample);
uint8_t afsk_dac_isr(Afsk *af);
void afsk_init(Afsk *af, int adc_ch, int dac_ch);


#define AFSK_BUTTERWORTH  0
#define AFSK_CHEBYSHEV    1

int afsk_testSetup(void);
int afsk_testRun(void);
int afsk_testTearDown(void);

#endif
