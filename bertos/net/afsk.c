/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief AFSK1200 modem.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "afsk.h"
#include <net/ax25.h>

#include "cfg/cfg_afsk.h"
#include "hw/hw_afsk.h"

#include <drv/timer.h>

#include <cfg/module.h>

#define LOG_LEVEL   AFSK_LOG_LEVEL
#define LOG_FORMAT  AFSK_LOG_FORMAT
#include <cfg/log.h>

#include <cpu/power.h>
#include <cpu/pgm.h>
#include <struct/fifobuf.h>

#include <string.h> /* memset */

#define PHASE_BIT    8
#define PHASE_INC    1

#define PHASE_MAX    (SAMPLEPERBIT * PHASE_BIT)
#define PHASE_THRES  (PHASE_MAX / 2) // - PHASE_BIT / 2)

// Modulator constants
#define MARK_FREQ  1200
#define MARK_INC   (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)MARK_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

#define SPACE_FREQ 2200
#define SPACE_INC  (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)SPACE_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

//Ensure sample rate is a multiple of bit rate
STATIC_ASSERT(!(CONFIG_AFSK_DAC_SAMPLERATE % BITRATE));

#define DAC_SAMPLEPERBIT (CONFIG_AFSK_DAC_SAMPLERATE / BITRATE)

/**
 * Sine table for the first quarter of wave.
 * The rest of the wave is computed from this first quarter.
 * This table is used to generate the modulated data.
 */
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
};

#define SIN_LEN 512 ///< Full wave length

STATIC_ASSERT(sizeof(sin_table) == SIN_LEN / 4);


/**
 * Given the index, this function computes the correct sine sample
 * based only on the first quarter of wave.
 */
INLINE uint8_t sin_sample(uint16_t idx)
{
	ASSERT(idx < SIN_LEN);
	uint16_t new_idx = idx % (SIN_LEN / 2);
	new_idx = (new_idx >= (SIN_LEN / 4)) ? (SIN_LEN / 2 - new_idx - 1) : new_idx;

	uint8_t data = pgm_read8(&sin_table[new_idx]);

	return (idx >= (SIN_LEN / 2)) ? (255 - data) : data;
}


#define BIT_DIFFER(bitline1, bitline2) (((bitline1) ^ (bitline2)) & 0x01)
#define EDGE_FOUND(bitline)            BIT_DIFFER((bitline), (bitline) >> 1)

/**
 * High-Level Data Link Control parsing function.
 * Parse bitstream in order to find characters.
 *
 * \param hdlc HDLC context.
 * \param bit  current bit to be parsed.
 * \param fifo FIFO buffer used to push characters.
 *
 * \return true if all is ok, false if the fifo is full.
 */
static bool hdlc_parse(Hdlc *hdlc, bool bit, FIFOBuffer *fifo)
{
	bool ret = true;

	hdlc->demod_bits <<= 1;
	hdlc->demod_bits |= bit ? 1 : 0;

	/* HDLC Flag */
	if (hdlc->demod_bits == HDLC_FLAG)
	{
		if (!fifo_isfull(fifo))
		{
			fifo_push(fifo, HDLC_FLAG);
			hdlc->rxstart = true;
		}
		else
		{
			ret = false;
			hdlc->rxstart = false;
		}

		hdlc->currchar = 0;
		hdlc->bit_idx = 0;
		return ret;
	}

	/* Reset */
	if ((hdlc->demod_bits & HDLC_RESET) == HDLC_RESET)
	{
		hdlc->rxstart = false;
		return ret;
	}

	if (!hdlc->rxstart)
		return ret;

	/* Stuffed bit */
	if ((hdlc->demod_bits & 0x3f) == 0x3e)
		return ret;

	if (hdlc->demod_bits & 0x01)
		hdlc->currchar |= 0x80;

	if (++hdlc->bit_idx >= 8)
	{
		if ((hdlc->currchar == HDLC_FLAG
			|| hdlc->currchar == HDLC_RESET
			|| hdlc->currchar == AX25_ESC))
		{
			if (!fifo_isfull(fifo))
				fifo_push(fifo, AX25_ESC);
			else
			{
				hdlc->rxstart = false;
				ret = false;
			}
		}

		if (!fifo_isfull(fifo))
			fifo_push(fifo, hdlc->currchar);
		else
		{
			hdlc->rxstart = false;
			ret = false;
		}

		hdlc->currchar = 0;
		hdlc->bit_idx = 0;
	}
	else
		hdlc->currchar >>= 1;

	return ret;
}


/**
 * ADC ISR callback.
 * This function has to be called by the ADC ISR when a sample of the configured
 * channel is available.
 * \param af Afsk context to operate on.
 * \param curr_sample current sample from the ADC.
 */
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

	af->iir_x[0] = af->iir_x[1];

	#if (CONFIG_AFSK_FILTER == AFSK_BUTTERWORTH)
		af->iir_x[1] = ((int8_t)fifo_pop(&af->delay_fifo) * curr_sample) >> 2;
		//af->iir_x[1] = ((int8_t)fifo_pop(&af->delay_fifo) * curr_sample) / 6.027339492;
	#elif (CONFIG_AFSK_FILTER == AFSK_CHEBYSHEV)
		af->iir_x[1] = ((int8_t)fifo_pop(&af->delay_fifo) * curr_sample) >> 2;
		//af->iir_x[1] = ((int8_t)fifo_pop(&af->delay_fifo) * curr_sample) / 3.558147322;
	#else
		#error Filter type not found!
	#endif

	af->iir_y[0] = af->iir_y[1];

	#if CONFIG_AFSK_FILTER == AFSK_BUTTERWORTH
		/*
		 * This strange sum + shift is an optimization for af->iir_y[0] * 0.668.
		 * iir * 0.668 ~= (iir * 21) / 32 =
		 * = (iir * 16) / 32 + (iir * 4) / 32 + iir / 32 =
		 * = iir / 2 + iir / 8 + iir / 32 =
		 * = iir >> 1 + iir >> 3 + iir >> 5
		 */
		af->iir_y[1] = af->iir_x[0] + af->iir_x[1] + (af->iir_y[0] >> 1) + (af->iir_y[0] >> 3) + (af->iir_y[0] >> 5);
		//af->iir_y[1] = af->iir_x[0] + af->iir_x[1] + af->iir_y[0] * 0.6681786379;
	#elif CONFIG_AFSK_FILTER == AFSK_CHEBYSHEV
		/*
		 * This should be (af->iir_y[0] * 0.438) but
		 * (af->iir_y[0] >> 1) is a faster approximation :-)
		 */
		af->iir_y[1] = af->iir_x[0] + af->iir_x[1] + (af->iir_y[0] >> 1);
		//af->iir_y[1] = af->iir_x[0] + af->iir_x[1] + af->iir_y[0] * 0.4379097269;
	#endif

	/* Save this sampled bit in a delay line */
	af->sampled_bits <<= 1;
	af->sampled_bits |= (af->iir_y[1] > 0) ? 1 : 0;

	/* Store current ADC sample in the af->delay_fifo */
	fifo_push(&af->delay_fifo, curr_sample);

	/* If there is an edge, adjust phase sampling */
	if (EDGE_FOUND(af->sampled_bits))
	{
		if (af->curr_phase < PHASE_THRES)
			af->curr_phase += PHASE_INC;
		else
			af->curr_phase -= PHASE_INC;
	}
	af->curr_phase += PHASE_BIT;

	/* sample the bit */
	if (af->curr_phase >= PHASE_MAX)
	{
		af->curr_phase %= PHASE_MAX;

		/* Shift 1 position in the shift register of the found bits */
		af->found_bits <<= 1;

		/*
		 * Determine bit value by reading the last 3 sampled bits.
		 * If the number of ones is two or greater, the bit value is a 1,
		 * otherwise is a 0.
		 * This algorithm presumes that there are 8 samples per bit.
		 */
		STATIC_ASSERT(SAMPLEPERBIT == 8);
		uint8_t bits = af->sampled_bits & 0x07;
		if (bits == 0x07 // 111, 3 bits set to 1
		 || bits == 0x06 // 110, 2 bits
		 || bits == 0x05 // 101, 2 bits
		 || bits == 0x03 // 011, 2 bits
		)
			af->found_bits |= 1;

		/*
		 * NRZI coding: if 2 consecutive bits have the same value
		 * a 1 is received, otherwise it's a 0.
		 */
		if (!hdlc_parse(&af->hdlc, !EDGE_FOUND(af->found_bits), &af->rx_fifo))
			af->status |= AFSK_RXFIFO_OVERRUN;
	}


	AFSK_STROBE_OFF();
}

static void afsk_txStart(Afsk *af)
{
	if (!af->sending)
	{
		af->phase_inc = MARK_INC;
		af->phase_acc = 0;
		af->stuff_cnt = 0;
		af->sending = true;
		af->preamble_len = DIV_ROUND(CONFIG_AFSK_PREAMBLE_LEN * BITRATE, 8000);
		AFSK_DAC_IRQ_START(af->dac_ch);
	}
	ATOMIC(af->trailer_len  = DIV_ROUND(CONFIG_AFSK_TRAILER_LEN  * BITRATE, 8000));
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
	if (af->sample_count == 0)
	{
		if (af->tx_bit == 0)
		{
			/* We have just finished transimitting a char, get a new one. */
			if (fifo_isempty(&af->tx_fifo) && af->trailer_len == 0)
			{
				AFSK_DAC_IRQ_STOP(af->dac_ch);
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
				if (!af->bit_stuff)
					af->stuff_cnt = 0;

				af->bit_stuff = true;

				/*
				 * Handle preamble and trailer
				 */
				if (af->preamble_len == 0)
				{
					if (fifo_isempty(&af->tx_fifo))
					{
						af->trailer_len--;
						af->curr_out = HDLC_FLAG;
					}
					else
						af->curr_out = fifo_pop(&af->tx_fifo);
				}
				else
				{
					af->preamble_len--;
					af->curr_out = HDLC_FLAG;
				}

				/* Handle char escape */
				if (af->curr_out == AX25_ESC)
				{
					if (fifo_isempty(&af->tx_fifo))
					{
						AFSK_DAC_IRQ_STOP(af->dac_ch);
						af->sending = false;
						AFSK_STROBE_OFF();
						return 0;
					}
					else
						af->curr_out = fifo_pop(&af->tx_fifo);
				}
				else if (af->curr_out == HDLC_FLAG || af->curr_out == HDLC_RESET)
					/* If these chars are not escaped disable bit stuffing */
					af->bit_stuff = false;
			}
			/* Start with LSB mask */
			af->tx_bit = 0x01;
		}

		/* check for bit stuffing */
		if (af->bit_stuff && af->stuff_cnt >= BIT_STUFF_LEN)
		{
			/* If there are more than 5 ones in a row insert a 0 */
			af->stuff_cnt = 0;
			/* switch tone */
			af->phase_inc = SWITCH_TONE(af->phase_inc);
		}
		else
		{
			/*
			 * NRZI: if we want to transmit a 1 the modulated frequency will stay
			 * unchanged; with a 0, there will be a change in the tone.
			 */
			if (af->curr_out & af->tx_bit)
			{
				/*
				 * Transmit a 1:
				 * - Stay on the previous tone
				 * - Increase bit stuff counter
				 */
				af->stuff_cnt++;
			}
			else
			{
				/*
				 * Transmit a 0:
				 * - Reset bit stuff counter
				 * - Switch tone
				 */
				af->stuff_cnt = 0;
				af->phase_inc = SWITCH_TONE(af->phase_inc);
			}

			/* Go to the next bit */
			af->tx_bit <<= 1;
		}
		af->sample_count = DAC_SAMPLEPERBIT;
	}

	/* Get new sample and put it out on the DAC */
	af->phase_acc += af->phase_inc;
	af->phase_acc %= SIN_LEN;

	af->sample_count--;
	AFSK_STROBE_OFF();
	return sin_sample(af->phase_acc);
}


static size_t afsk_read(KFile *fd, void *_buf, size_t size)
{
	Afsk *af = AFSK_CAST(fd);
	uint8_t *buf = (uint8_t *)_buf;

	#if CONFIG_AFSK_RXTIMEOUT == 0
	while (size-- && !fifo_isempty_locked(&af->rx_fifo))
	#else
	while (size--)
	#endif
	{
		#if CONFIG_AFSK_RXTIMEOUT != -1
		ticks_t start = timer_clock();
		#endif

		while (fifo_isempty_locked(&af->rx_fifo))
		{
			cpu_relax();
			#if CONFIG_AFSK_RXTIMEOUT != -1
			if (timer_clock() - start > ms_to_ticks(CONFIG_AFSK_RXTIMEOUT))
				return buf - (uint8_t *)_buf;
			#endif
		}

		*buf++ = fifo_pop_locked(&af->rx_fifo);
	}

	return buf - (uint8_t *)_buf;
}

static size_t afsk_write(KFile *fd, const void *_buf, size_t size)
{
	Afsk *af = AFSK_CAST(fd);
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size--)
	{
		while (fifo_isfull_locked(&af->tx_fifo))
			cpu_relax();

		fifo_push_locked(&af->tx_fifo, *buf++);
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


/**
 * Initialize an AFSK1200 modem.
 * \param af Afsk context to operate on.
 * \param adc_ch  ADC channel used by the demodulator.
 * \param dac_ch  DAC channel used by the modulator.
 */
void afsk_init(Afsk *af, int adc_ch, int dac_ch)
{
	#if CONFIG_AFSK_RXTIMEOUT != -1
	MOD_CHECK(timer);
	#endif
	memset(af, 0, sizeof(*af));
	af->adc_ch = adc_ch;
	af->dac_ch = dac_ch;

	fifo_init(&af->delay_fifo, (uint8_t *)af->delay_buf, sizeof(af->delay_buf));
	fifo_init(&af->rx_fifo, af->rx_buf, sizeof(af->rx_buf));

	/* Fill sample FIFO with 0 */
	for (int i = 0; i < SAMPLEPERBIT / 2; i++)
		fifo_push(&af->delay_fifo, 0);

	fifo_init(&af->tx_fifo, af->tx_buf, sizeof(af->tx_buf));

	AFSK_ADC_INIT(adc_ch, af);
	AFSK_DAC_INIT(dac_ch, af);
	AFSK_STROBE_INIT();
	LOG_INFO("MARK_INC %d, SPACE_INC %d\n", MARK_INC, SPACE_INC);

	DB(af->fd._type = KFT_AFSK);
	af->fd.write = afsk_write;
	af->fd.read = afsk_read;
	af->fd.flush = afsk_flush;
	af->fd.error = afsk_error;
	af->fd.clearerr = afsk_clearerr;
	af->phase_inc = MARK_INC;
}
