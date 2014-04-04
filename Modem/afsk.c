#include "afsk.h"
#include "config.h"
#include "hardware.h"

#include <drv/timer.h>
#include <cfg/module.h>

#include <cfg/log.h>

#include <cpu/power.h>
#include <cpu/pgm.h>
#include <struct/fifobuf.h>

#include <string.h>

// Sine table for DAC DDS
#define SIN_LEN 512 // Length of a full wave. Table is 1/4 wave.
static const uint8_t PROGMEM sin_table[] =
{
	128, 129, 131, 132, 134, 135, 137, 138, 140, 142, 143, 145, 146, 148, 149, 151,
	152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 167, 169, 170, 172, 173, 175,
	176, 178, 179, 181, 182, 183, 185, 186, 188, 189, 190, 192, 193, 194, 196, 197,
	198, 200, 201, 202, 203, 205, 206, 207, 208, 210, 211, 212, 213, 214, 215, 217,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
	234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245,
	245, 246, 246, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252,
	253, 253, 253, 253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255,
}; STATIC_ASSERT(sizeof(sin_table) == SIN_LEN / 4);


// Calculate Sine value from quarter sine table
INLINE uint8_t sinSample(uint16_t i) {
	ASSERT(i < SIN_LEN);
	uint16_t newI = i % (SIN_LEN/2);
	newI = (newI >= (SIN_LEN/4)) ? (SIN_LEN/2 - newI -1) : newI;
	uint8_t sine = pgm_read8(&sin_table[newI]);
	return (i >= (SIN_LEN/2)) ? (255 - sine) : sine;
}

// Look for signal transition. Used for phase sync.
#define BITS_DIFFER(bits1, bits2) (((bits1)^(bits2)) & 0x01)
#define EDGE_FOUND(bits) BITS_DIFFER((bits), (bits) >> 1)

// Phase sync constants
#define PHASE_BITS    8
#define PHASE_INC    1
#define PHASE_MAX    (SAMPLESPERBIT * PHASE_BITS)
#define PHASE_THRESHOLD  (PHASE_MAX / 2)

// Modulation constants
#define MARK_FREQ  1200
#define MARK_INC   (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)MARK_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))
#define SPACE_FREQ 2200
#define SPACE_INC  (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)SPACE_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

// HDLC flag bytes
#define HDLC_FLAG  0x7E
#define HDLC_RESET 0x7F
#define AX25_ESC   0x1B

// Check that sample rate is divisible by bitrate
STATIC_ASSERT(!(CONFIG_AFSK_DAC_SAMPLERATE % BITRATE));

#define DAC_SAMPLESPERBIT (CONFIG_AFSK_DAC_SAMPLERATE / BITRATE)

//////////////////////////////////////////////////////
// Link Layer Control and Demodulation              //
//////////////////////////////////////////////////////

static bool hdlcParse(Hdlc *hdlc, bool bit, FIFOBuffer *fifo) {
	bool ret = true;

	hdlc->demodulatedBits <<= 1;
	hdlc->demodulatedBits |= bit ? 1 : 0;

	// Check if we have received a HDLC flag (01111110)
	if (hdlc->demodulatedBits == HDLC_FLAG) {
		if (!fifo_isfull(fifo)) {
			fifo_push(fifo, HDLC_FLAG);
			hdlc->receiving = true;
		} else {
			ret = false;
			hdlc->receiving = false;
		}

		hdlc->currentByte = 0;
		hdlc->bitIndex = 0;
		return ret;
	}

	// Check if we have received a RESET flag (01111111)
	if ((hdlc->demodulatedBits & HDLC_RESET) == HDLC_RESET) {
		hdlc->receiving = false;
		return ret;
	}

	// If we are just receiving noise, don't bother with anything
	if (!hdlc->receiving)
		return ret;

	// First check if what we are seeing is a stuffed bit
	if ((hdlc->demodulatedBits & 0x3f) == 0x3e)
		return ret;

	// If we have an actual 1 bit, push this to the current byte
	if (hdlc->demodulatedBits & 0x01)
		hdlc->currentByte |= 0x80;

	// Increment the bitIndex and check if we have a complete byte
	if (++hdlc->bitIndex >= 8) {
		// If we have a HDLC control character,
		// put a AX.25 escape in the received data
		if ((hdlc->currentByte == HDLC_FLAG ||
			 hdlc->currentByte == HDLC_RESET ||
			 hdlc->currentByte == AX25_ESC)) {
			if (!fifo_isfull(fifo)) {
				fifo_push(fifo, AX25_ESC);
			} else {
				hdlc->receiving = false;
				ret = false;
			}
		}

		// Push the actual byte to the received data FIFO
		if (!fifo_isfull(fifo)) {
			fifo_push(fifo, hdlc->currentByte);
		} else {
			hdlc->receiving = false;
			ret = false;
		}

		// Wipe received byte and reset bit index to 0
		hdlc->currentByte = 0;
		hdlc->bitIndex = 0;

	} else {
		// We don't have a full byte yet, bitshift the byte
		// to make room for the next bit
		hdlc->currentByte >>= 1;
	}

	return ret;
}

void afsk_adc_isr(Afsk *afsk, int8_t currentSample) {
	// To determine the received frequency, and thereby
	// the bit of the sample, we multiply the sample by
	// a sample delayed by (samples per bit / 2).
	// We then lowpass-filter the sample with a first
	// order 600Hz filter

	afsk->iirX[0] = afsk->iirX[1];
	afsk->iirX[1] = ((int8_t)fifo_pop(&afsk->delayFifo) * currentSample) >> 2;

	afsk->iirY[0] = afsk->iirY[1];
	afsk->iirY[1] = afsk->iirX[0] + afsk->iirX[1] + (afsk->iirY[0] >> 1);

	// Put the sampled bit in a delay-line
	afsk->sampledBits <<= 1; // Bitshift everything 1 left
	afsk->sampledBits |= (afsk->iirY[1] > 0) ? 1 : 0;

	// Put the current raw sample in the delay FIFO
	fifo_push(&afsk->delayFifo, currentSample);

	// If there is a signal transition, recalibrate
	// sampling phase
	
	if (EDGE_FOUND(afsk->sampledBits)) {
		if (afsk->currentPhase < PHASE_THRESHOLD) {
			afsk->currentPhase += PHASE_INC;
		} else {
			afsk->currentPhase -= PHASE_INC;
		}
	}
	afsk->currentPhase += PHASE_BITS;

	// Look at the raw samples to determine the transmitted bit
	if (afsk->currentPhase >= PHASE_MAX) {
		afsk->currentPhase %= PHASE_MAX;

		// Bitshift to make room for next bit
		afsk->actualBits <<= 1;

		// Determine the actual bit value by reading the last
		// 3 sampled bits. If there is two ore more 1's, the
		// actual bit is a 1, otherwise a 0.
		uint8_t bits = afsk->sampledBits & 0x07;
		if (bits == 0x07 || // 111
			bits == 0x06 || // 110
			bits == 0x05 || // 101
			bits == 0x03	// 011
			) {
			afsk->actualBits |= 1;
		}

		// Now we can pass the actual bit to the HDLC parser.
		// We are using NRZI coding, so if 2 consecutive bits
		// have the same value, we have a 1, otherwise a 0.
		// We use the EDGE_FOUND function to determine this.
		// We also check the return of the Link Control parser
		// to check if an error occured.
		if (!hdlcParse(&afsk->hdlc, !EDGE_FOUND(afsk->actualBits), &afsk->rxFifo)) {
			afsk->status |= RX_OVERRUN;
		}
	}
}

//////////////////////////////////////////////////////
// Signal modulation and DAC                        //
//////////////////////////////////////////////////////

#define BIT_STUFF_LEN 5
#define SWITCH_TONE(inc)  (((inc) == MARK_INC) ? SPACE_INC : MARK_INC)

static void afsk_txStart(Afsk *afsk) {
	if (!afsk->sending) {
		afsk->phaseInc = MARK_INC;
		afsk->phaseAcc = 0;
		afsk->bitstuffCount = 0;
		afsk->sending = true;
		afsk->preambleLength = DIV_ROUND(CONFIG_AFSK_PREAMBLE_LEN * BITRATE, 8000);
		AFSK_DAC_IRQ_START(afsk->dacPin);
	}
	ATOMIC(afsk->tailLength = DIV_ROUND(CONFIG_AFSK_TRAILER_LEN * BITRATE, 8000));
}

// This is the DAC ISR, called at sampling ratewhenever the DAC IRQ is on.
// It modulates the data to be transmitted and returns a value directly
// for output on the DAC
uint8_t afsk_dac_isr(Afsk *afsk) {
	// Check whether we are at the beginning of a sample
	if (afsk->sampleIndex == 0) {
		if (afsk->txBit == 0) {
			// If TX FIFO is empty and tail-length has decremented to 0
			// we are done, stop the IRQ and reset
			if (fifo_isempty(&afsk->txFifo) && afsk->tailLength == 0) {
				AFSK_DAC_IRQ_STOP(afsk->dacPin);
				afsk->sending = false;
				return 0;
			} else {
				// Reset the bitstuff counter if we have just sent
				// a bitstuffed byte
				if (!afsk->bitStuff) afsk->bitstuffCount = 0;
				// Reset bitstuff indicator to true
				afsk->bitStuff = true;

				// Check if we are in preamble or tail
				if (afsk->preambleLength == 0) {
					if (fifo_isempty(&afsk->txFifo)) {
						afsk->tailLength--;
						afsk->currentOutputByte = HDLC_FLAG;
					} else {
						// If preamble is already transmitted and TX
						// buffer is not empty, we should get a byte
						// for transmission
						afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
					}
				} else {
					afsk->preambleLength--;
					afsk->currentOutputByte = HDLC_FLAG;
				}

				// Handle escape sequences
				if (afsk->currentOutputByte == AX25_ESC) {
					if (fifo_isempty(&afsk->txFifo)) {
						AFSK_DAC_IRQ_STOP(afsk->dacPin);
						afsk->sending = false;
						return 0;
					} else {
						afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
					}
				} else if (afsk->currentOutputByte == HDLC_FLAG || afsk->currentOutputByte == HDLC_RESET) {
					afsk->bitStuff = false;
				}
			}
			// Start with LSB mask
			afsk->txBit = 0x01;
		}

		// Check for bit stuffing
		if (afsk->bitStuff && afsk->bitstuffCount >= BIT_STUFF_LEN) {
			afsk->bitstuffCount = 0;
			afsk->phaseInc = SWITCH_TONE(afsk->phaseInc);
		} else {
			// We are using NRZI so if we want to transmit a 1
			// the modulated signal will stay the same. For a 0
			// we make the signal transition
			if (afsk->currentOutputByte & afsk->txBit) {
				// We don't do anything, aka stay on the same
				// tone as before. We have sent one 1, so we
				// increment the bitstuff counter.
				afsk->bitstuffCount++;
			} else {
				// We switch the tone, and reset the bitstuff
				// counter, since we have now transmitted a
				// zero
				afsk->bitstuffCount = 0;
				afsk->phaseInc = SWITCH_TONE(afsk->phaseInc);
			}

			// Move on to the next bit
			afsk->txBit <<= 1;
		}

		afsk->sampleIndex = DAC_SAMPLESPERBIT;
	}

	// Retrieve af new sample index and DAC it
	afsk->phaseAcc += afsk->phaseInc;
	afsk->phaseAcc %= SIN_LEN;
	afsk->sampleIndex--;
	return sinSample(afsk->phaseAcc);
}



//////////////////////////////////////////////////////
// File operation overwrites for read/write         //
//////////////////////////////////////////////////////

static size_t afsk_read(KFile *fd, void *_buf, size_t size)
{
	Afsk *af = AFSK_CAST(fd);
	uint8_t *buf = (uint8_t *)_buf;

	#if CONFIG_AFSK_RXTIMEOUT == 0
	while (size-- && !fifo_isempty_locked(&af->rxFifo))
	#else
	while (size--)
	#endif
	{
		#if CONFIG_AFSK_RXTIMEOUT != -1
		ticks_t start = timer_clock();
		#endif

		while (fifo_isempty_locked(&af->rxFifo))
		{
			cpu_relax();
			#if CONFIG_AFSK_RXTIMEOUT != -1
			if (timer_clock() - start > ms_to_ticks(CONFIG_AFSK_RXTIMEOUT))
				return buf - (uint8_t *)_buf;
			#endif
		}

		*buf++ = fifo_pop_locked(&af->rxFifo);
	}

	return buf - (uint8_t *)_buf;
}

static size_t afsk_write(KFile *fd, const void *_buf, size_t size)
{
	Afsk *af = AFSK_CAST(fd);
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size--)
	{
		while (fifo_isfull_locked(&af->txFifo))
			cpu_relax();

		fifo_push_locked(&af->txFifo, *buf++);
		afsk_txStart(af);
	}

	return buf - (const uint8_t *)_buf;
}

static int afsk_flush(KFile *fd)
{
	Afsk *af = AFSK_CAST(fd);
	while (af->sending)
		cpu_relax();
	return 0;
}

static int afsk_error(KFile *fd)
{
	Afsk *af = AFSK_CAST(fd);
	int err;

	ATOMIC(err = af->status);
	return err;
}

static void afsk_clearerr(KFile *fd)
{
	Afsk *af = AFSK_CAST(fd);
	ATOMIC(af->status = 0);
}

//////////////////////////////////////////////////////
// Modem Initialization                             //
//////////////////////////////////////////////////////

void afsk_init(Afsk *afsk, int _adcPin, int _dacPin) {
	// Allocate memory for struct
	memset(afsk, 0, sizeof(*afsk));

	// Configure pins
	afsk->adcPin = _adcPin;
	afsk->dacPin = _dacPin;
	afsk->phaseInc = MARK_INC;

	// Init FIFO buffers
	fifo_init(&afsk->delayFifo, (uint8_t *)afsk->delayBuf, sizeof(afsk->delayBuf));
	fifo_init(&afsk->rxFifo, afsk->rxBuf, sizeof(afsk->rxBuf));
	fifo_init(&afsk->txFifo, afsk->txBuf, sizeof(afsk->txBuf));

	// Fill delay FIFO with zeroes
	for (int i = 0; i<SAMPLESPERBIT / 2; i++) {
		fifo_push(&afsk->delayFifo, 0);
	}

	// Init DAC & ADC
	AFSK_ADC_INIT(_adcPin, afsk);
	AFSK_DAC_INIT(_dacPin, afsk);
	AFSK_STROBE_INIT();

	DB(afsk->fd._type = KFT_AFSK);
	afsk->fd.write = afsk_write;
	afsk->fd.read = afsk_read;
	afsk->fd.flush = afsk_flush;
	afsk->fd.error = afsk_error;
	afsk->fd.clearerr = afsk_clearerr;	
}