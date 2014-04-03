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
#define PHASE_BIT    8
#define PHASE_INC    1
#define PHASE_MAX    (SAMPLESPERBIT * PHASE_BIT)
#define PHASE_THRES  (PHASE_MAX / 2)

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



void afsk_adc_isr(Afsk *af, int8_t curr_sample)
{
	AFSK_STROBE_ON();

	/*
	 * Frequency discriminator and LP IIR filter.
	 * This filter is designed to work
	 * at the given sample rate and bit rate.
	 */
	STATIC_ASSERT(SAMPLERATE == 9600);
	STATIC_ASSERT(BITRATE == 1200);

	/*
	 * Frequency discrimination is achieved by simply multiplying
	 * the sample with a delayed sample of (samples per bit) / 2.
	 * Then the signal is lowpass filtered with a first order,
	 * 600 Hz filter. The filter implementation is selectable
	 * through the CONFIG_AFSK_FILTER config variable.
	 */

	af->iirX[0] = af->iirX[1];

	#if (CONFIG_AFSK_FILTER == AFSK_BUTTERWORTH)
		af->iirX[1] = ((int8_t)fifo_pop(&af->delayFifo) * curr_sample) >> 2;
		//af->iirX[1] = ((int8_t)fifo_pop(&af->delayFifo) * curr_sample) / 6.027339492;
	#elif (CONFIG_AFSK_FILTER == AFSK_CHEBYSHEV)
		af->iirX[1] = ((int8_t)fifo_pop(&af->delayFifo) * curr_sample) >> 2;
		//af->iirX[1] = ((int8_t)fifo_pop(&af->delayFifo) * curr_sample) / 3.558147322;
	#else
		#error Filter type not found!
	#endif

	af->iirY[0] = af->iirY[1];

	#if CONFIG_AFSK_FILTER == AFSK_BUTTERWORTH
		/*
		 * This strange sum + shift is an optimization for af->iirY[0] * 0.668.
		 * iir * 0.668 ~= (iir * 21) / 32 =
		 * = (iir * 16) / 32 + (iir * 4) / 32 + iir / 32 =
		 * = iir / 2 + iir / 8 + iir / 32 =
		 * = iir >> 1 + iir >> 3 + iir >> 5
		 */
		af->iirY[1] = af->iirX[0] + af->iirX[1] + (af->iirY[0] >> 1) + (af->iirY[0] >> 3) + (af->iirY[0] >> 5);
		//af->iirY[1] = af->iirX[0] + af->iirX[1] + af->iirY[0] * 0.6681786379;
	#elif CONFIG_AFSK_FILTER == AFSK_CHEBYSHEV
		/*
		 * This should be (af->iirY[0] * 0.438) but
		 * (af->iirY[0] >> 1) is a faster approximation :-)
		 */
		af->iirY[1] = af->iirX[0] + af->iirX[1] + (af->iirY[0] >> 1);
		//af->iirY[1] = af->iirX[0] + af->iirX[1] + af->iirY[0] * 0.4379097269;
	#endif

	/* Save this sampled bit in a delay line */
	af->sampledBits <<= 1;
	af->sampledBits |= (af->iirY[1] > 0) ? 1 : 0;

	/* Store current ADC sample in the af->delayFifo */
	fifo_push(&af->delayFifo, curr_sample);

	/* If there is an edge, adjust phase sampling */
	if (EDGE_FOUND(af->sampledBits))
	{
		if (af->currentPhase < PHASE_THRES)
			af->currentPhase += PHASE_INC;
		else
			af->currentPhase -= PHASE_INC;
	}
	af->currentPhase += PHASE_BIT;

	/* sample the bit */
	if (af->currentPhase >= PHASE_MAX)
	{
		af->currentPhase %= PHASE_MAX;

		/* Shift 1 position in the shift register of the found bits */
		af->actualBits <<= 1;

		/*
		 * Determine bit value by reading the last 3 sampled bits.
		 * If the number of ones is two or greater, the bit value is a 1,
		 * otherwise is a 0.
		 * This algorithm presumes that there are 8 samples per bit.
		 */
		STATIC_ASSERT(SAMPLESPERBIT == 8);
		uint8_t bits = af->sampledBits & 0x07;
		if (bits == 0x07 // 111, 3 bits set to 1
		 || bits == 0x06 // 110, 2 bits
		 || bits == 0x05 // 101, 2 bits
		 || bits == 0x03 // 011, 2 bits
		)
			af->actualBits |= 1;

		/*
		 * NRZI coding: if 2 consecutive bits have the same value
		 * a 1 is received, otherwise it's a 0.
		 */
		if (!hdlcParse(&af->hdlc, !EDGE_FOUND(af->actualBits), &af->rxFifo))
			af->status |= AFSK_RXFIFO_OVERRUN;
	}


	AFSK_STROBE_OFF();
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

void afsk_init(Afsk *af, int adcPin, int dacPin)
{
	#if CONFIG_AFSK_RXTIMEOUT != -1
	MOD_CHECK(timer);
	#endif
	memset(af, 0, sizeof(*af));
	af->adcPin = adcPin;
	af->dacPin = dacPin;

	fifo_init(&af->delayFifo, (uint8_t *)af->delay_buf, sizeof(af->delay_buf));
	fifo_init(&af->rxFifo, af->rx_buf, sizeof(af->rx_buf));

	/* Fill sample FIFO with 0 */
	for (int i = 0; i < SAMPLESPERBIT / 2; i++)
		fifo_push(&af->delayFifo, 0);

	fifo_init(&af->txFifo, af->tx_buf, sizeof(af->tx_buf));

	AFSK_ADC_INIT(adcPin, af);
	AFSK_DAC_INIT(dacPin, af);
	AFSK_STROBE_INIT();
	//LOG_INFO("MARK_INC %d, SPACE_INC %d\n", MARK_INC, SPACE_INC);

	DB(af->fd._type = KFT_AFSK);
	af->fd.write = afsk_write;
	af->fd.read = afsk_read;
	af->fd.flush = afsk_flush;
	af->fd.error = afsk_error;
	af->fd.clearerr = afsk_clearerr;
	af->phaseInc = MARK_INC;
}
