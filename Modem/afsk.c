#include "afsk.h"
#include "config.h"
#include "hardware.h"

#include <drv/timer.h>
#include <cfg/module.h>

#define HDLC_FLAG  0x7E
#define HDLC_RESET 0x7F
#define AX25_ESC   0x1B

#include <cfg/log.h>

#include <cpu/power.h>
#include <cpu/pgm.h>
#include <struct/fifobuf.h>

#include <string.h> /* memset */

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

// Check that sample rate is divisible by bitrate
STATIC_ASSERT(!(CONFIG_AFSK_DAC_SAMPLERATE % BITRATE));

#define DAC_SAMPLESPERBIT (CONFIG_AFSK_DAC_SAMPLERATE / BITRATE)

static bool hdlcParse(Hdlc *hdlc, bool bit, FIFOBuffer *fifo)
{
	bool ret = true;

	hdlc->demodulatedBits <<= 1;
	hdlc->demodulatedBits |= bit ? 1 : 0;

	/* HDLC Flag */
	if (hdlc->demodulatedBits == HDLC_FLAG)
	{
		if (!fifo_isfull(fifo))
		{
			fifo_push(fifo, HDLC_FLAG);
			hdlc->receiving = true;
		}
		else
		{
			ret = false;
			hdlc->receiving = false;
		}

		hdlc->currentByte = 0;
		hdlc->bitIndex = 0;
		return ret;
	}

	/* Reset */
	if ((hdlc->demodulatedBits & HDLC_RESET) == HDLC_RESET)
	{
		hdlc->receiving = false;
		return ret;
	}

	if (!hdlc->receiving)
		return ret;

	/* Stuffed bit */
	if ((hdlc->demodulatedBits & 0x3f) == 0x3e)
		return ret;

	if (hdlc->demodulatedBits & 0x01)
		hdlc->currentByte |= 0x80;

	if (++hdlc->bitIndex >= 8)
	{
		if ((hdlc->currentByte == HDLC_FLAG
			|| hdlc->currentByte == HDLC_RESET
			|| hdlc->currentByte == AX25_ESC))
		{
			if (!fifo_isfull(fifo))
				fifo_push(fifo, AX25_ESC);
			else
			{
				hdlc->receiving = false;
				ret = false;
			}
		}

		if (!fifo_isfull(fifo))
			fifo_push(fifo, hdlc->currentByte);
		else
		{
			hdlc->receiving = false;
			ret = false;
		}

		hdlc->currentByte = 0;
		hdlc->bitIndex = 0;
	}
	else
		hdlc->currentByte >>= 1;

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

static void afsk_txStart(Afsk *af)
{
	if (!af->sending)
	{
		af->phaseInc = MARK_INC;
		af->phaseAcc = 0;
		af->bitstuffCount = 0;
		af->sending = true;
		af->preambleLength = DIV_ROUND(CONFIG_AFSK_PREAMBLE_LEN * BITRATE, 8000);
		AFSK_DAC_IRQ_START(af->dacPin);
	}
	ATOMIC(af->tailLength  = DIV_ROUND(CONFIG_AFSK_TRAILER_LEN  * BITRATE, 8000));
}

#define BIT_STUFF_LEN 5

#define SWITCH_TONE(inc)  (((inc) == MARK_INC) ? SPACE_INC : MARK_INC)

/**
 * DAC ISR callback.
 * This function has to be called by the DAC ISR when a sample of the configured
 * channel has been converted out.
 *
 * \param af Afsk context to operate on.
 *
 * \return The next DAC output sample.
 */
uint8_t afsk_dac_isr(Afsk *af)
{
	AFSK_STROBE_ON();

	/* Check if we are at a start of a sample cycle */
	if (af->sampleIndex == 0)
	{
		if (af->txBit == 0)
		{
			/* We have just finished transimitting a char, get a new one. */
			if (fifo_isempty(&af->txFifo) && af->tailLength == 0)
			{
				AFSK_DAC_IRQ_STOP(af->dacPin);
				af->sending = false;
				AFSK_STROBE_OFF();
				return 0;
			}
			else
			{
				/*
				 * If we have just finished sending an unstuffed byte,
				 * reset bitstuff counter.
				 */
				if (!af->bitStuff)
					af->bitstuffCount = 0;

				af->bitStuff = true;

				/*
				 * Handle preamble and trailer
				 */
				if (af->preambleLength == 0)
				{
					if (fifo_isempty(&af->txFifo))
					{
						af->tailLength--;
						af->currentOutputByte = HDLC_FLAG;
					}
					else
						af->currentOutputByte = fifo_pop(&af->txFifo);
				}
				else
				{
					af->preambleLength--;
					af->currentOutputByte = HDLC_FLAG;
				}

				/* Handle char escape */
				if (af->currentOutputByte == AX25_ESC)
				{
					if (fifo_isempty(&af->txFifo))
					{
						AFSK_DAC_IRQ_STOP(af->dacPin);
						af->sending = false;
						AFSK_STROBE_OFF();
						return 0;
					}
					else
						af->currentOutputByte = fifo_pop(&af->txFifo);
				}
				else if (af->currentOutputByte == HDLC_FLAG || af->currentOutputByte == HDLC_RESET)
					/* If these chars are not escaped disable bit stuffing */
					af->bitStuff = false;
			}
			/* Start with LSB mask */
			af->txBit = 0x01;
		}

		/* check for bit stuffing */
		if (af->bitStuff && af->bitstuffCount >= BIT_STUFF_LEN)
		{
			/* If there are more than 5 ones in a row insert a 0 */
			af->bitstuffCount = 0;
			/* switch tone */
			af->phaseInc = SWITCH_TONE(af->phaseInc);
		}
		else
		{
			/*
			 * NRZI: if we want to transmit a 1 the modulated frequency will stay
			 * unchanged; with a 0, there will be a change in the tone.
			 */
			if (af->currentOutputByte & af->txBit)
			{
				/*
				 * Transmit a 1:
				 * - Stay on the previous tone
				 * - Increase bit stuff counter
				 */
				af->bitstuffCount++;
			}
			else
			{
				/*
				 * Transmit a 0:
				 * - Reset bit stuff counter
				 * - Switch tone
				 */
				af->bitstuffCount = 0;
				af->phaseInc = SWITCH_TONE(af->phaseInc);
			}

			/* Go to the next bit */
			af->txBit <<= 1;
		}
		af->sampleIndex = DAC_SAMPLESPERBIT;
	}

	/* Get new sample and put it out on the DAC */
	af->phaseAcc += af->phaseInc;
	af->phaseAcc %= SIN_LEN;

	af->sampleIndex--;
	AFSK_STROBE_OFF();
	return sinSample(af->phaseAcc);
}


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