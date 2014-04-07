//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#include "afsk.h"			// We need the header file for the modem
#include "config.h"			// This stores basic configuration
#include "hardware.h"		// Hardware functions are nice to have too :)

#include <drv/timer.h>		// Timer driver from BertOS
//FIXME: is this needed ? #include <cfg/module.h>		

#include <cpu/power.h>		// Power management from BertOS
#include <cpu/pgm.h>		// Access to PROGMEM from BertOS
#include <struct/fifobuf.h>	// FIFO buffer implementation from BertOS
#include <string.h>			// String operations, primarily used for memset function


//////////////////////////////////////////////////////
// Definitions and some useful macros               //
//////////////////////////////////////////////////////

// Sine table for Direct Digital Synthesis DAC
// Since it would be inefficient to calculate a sine value each
// time we process a sample, we store the values in program memory
// as a look-up table. We only need to store values for a quarter
// wave, since we can easily reconstruct the entire 512 values
// from only these 128 values.
#define SIN_LEN 512
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


// Calculate any sine value from quarter wave sine table
// The reason we declare this inline is to eliminate an extra
// call for the code. The code is essentially inserted directly
// in the calling functions code. This makes stuff faster :)
INLINE uint8_t sinSample(uint16_t i) {
	// Make sure that the index asked for is in the correct range
	ASSERT(i < SIN_LEN);
	// First we make a new index value, and restrict it to only
	// the first half-wave of the sine.
	uint16_t newI = i % (SIN_LEN/2);
	// We then check if this new index is larger than the first
	// quarter wave. If it is, we don't have the value for this
	// index directly, but we can figure it out by subtracting
	// the new index from a half wave, effectively wrapping us
	// back into the same place on the wave, whithin the quarter
	// wave we have data for, only with the inverse sign. If the
	// index was actually in the first quarter, we don't need to
	// do anything.
	newI = (newI >= (SIN_LEN/4)) ? (SIN_LEN/2 - newI -1) : newI;
	// Now we just need to read the value from program memory
	uint8_t sine = pgm_read8(&sin_table[newI]);
	// And flip the sign (+/-) if the original index was greater
	// than a half wave.
	return (i >= (SIN_LEN/2)) ? (255 - sine) : sine;
}

// A very basic macro that just checks whether the last bit
// of a whatever is passed into it differ. This is used in the
// next macro.
#define BITS_DIFFER(bits1, bits2) (((bits1)^(bits2)) & 0x01)

// This macro is used to look for signal transitions. We need
// to identify these to keep the phase of our demodulator in
// sync with the incoming signal. Each time we find a signal
// transition on the physical medium, we adjust the phase of
// the demodulator.
// The macro effectively looks at the two least significant
// bits in a stream and returns true if they differ.
#define TRANSITION_FOUND(bits) BITS_DIFFER((bits), (bits) >> 1)

// Phase sync constants
#define PHASE_BITS    8
#define PHASE_INC    1 										// FIXME: originally 1
#define PHASE_MAX    (SAMPLESPERBIT * PHASE_BITS)
#define PHASE_THRESHOLD  (PHASE_MAX / 2) + (PHASE_MAX / 8)  // FIXME: originally /2

// Modulation constants
#define MARK_FREQ  1200
#define MARK_INC   (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)MARK_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))
#define SPACE_FREQ 2200
#define SPACE_INC  (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)SPACE_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

// HDLC flag bytes
#define HDLC_FLAG  0x7E		// An HDLC_FLAG is used to signify the start or end of a frame
#define HDLC_RESET 0x7F		// An HDLC_RESET is used to abruptly stop or reset a transmission
#define AX25_ESC   0x1B		// We use the AX.25 escape character for escaping bit sequences in
							// the actual data. This is similar to escaping an " character in a
							// string enclosed by "s.

// Check that sample rate is divisible by bitrate.
// If this is not the case, all of our algorithms will
// fail horribly and we will cry.
STATIC_ASSERT(!(CONFIG_AFSK_DAC_SAMPLERATE % BITRATE));

// How many samples it takes to encode or decode one bit
// on the physical medium.
#define DAC_SAMPLESPERBIT (CONFIG_AFSK_DAC_SAMPLERATE / BITRATE)

//////////////////////////////////////////////////////
// Link Layer Control and Demodulation              //
//////////////////////////////////////////////////////

// hdlcParse /////////////////////////////////////////
// This function looks at the raw bits demodulated from
// the physical medium and tries to parse actual data
// packets from the bitstream. Note that at this level,
// we don't really try to discriminate when a packet
// starts or ends, or where the payload is. We only try
// to detect that a transmission is taking place, then
// synchronise to the start and end of the transmitted
// bytes, and push these up to the data-link layer, in
// this example the MP.x protocol. It is then the
// protocols job to actually recreate the full packet.
// Also note that the data is not "pushed" per se, but
// stored in a FIFO buffer, that the protocol must
// continously read to recreate the received packets.
static bool hdlcParse(Hdlc *hdlc, bool bit, FIFOBuffer *fifo) {
	// Initialise a return value. We start with the
	// assumption that all is going to end well :)
	bool ret = true;

	// Bitshift our byte of demodulated bits to
	// the left by one bit, to make room for the
	// next incoming bit
	hdlc->demodulatedBits <<= 1;
	// And then put the newest bit from the 
	// demodulator into the byte.
	hdlc->demodulatedBits |= bit ? 1 : 0;

	// Now we'll look at the last 8 received bits, and
	// check if we have received a HDLC flag (01111110)
	if (hdlc->demodulatedBits == HDLC_FLAG) {
		// If we have, check that our output buffer is
		// not full.
		if (!fifo_isfull(fifo)) {
			// If it isn't, we'll push the HDLC_FLAG into
			// the buffer and indicate that we are now
			// receiving data. For bling we also turn
			// on the RX LED.
			fifo_push(fifo, HDLC_FLAG);
			hdlc->receiving = true;
			LED_RX_ON();
		} else {
			// If the buffer is full, we have a problem
			// and abort by setting the return value to
			// false and stopping the here.
			ret = false;
			hdlc->receiving = false;
			LED_RX_OFF();
		}

		// Everytime we receive a HDLC_FLAG, we reset the
		// storage for our current incoming byte and bit
		// position in that byte. This effectively
		// synchronises our parsing to  the start and end
		// of the received bytes.
		hdlc->currentByte = 0;
		hdlc->bitIndex = 0;
		return ret;
	}

	// Check if we have received a RESET flag (01111111)
	if ((hdlc->demodulatedBits & HDLC_RESET) == HDLC_RESET) {
		// If we have, something probably went wrong at the
		// transmitting end, and we abort the reception.
		hdlc->receiving = false;
		LED_RX_OFF();
		return ret;
	}

	// If we have not yet seen a HDLC_FLAG indicating that
	// a transmission is actually taking place, don't bother
	// with anything.
	if (!hdlc->receiving)
		return ret;

	// First check if what we are seeing is a stuffed bit.
	// Since the different HDLC control characters like
	// HDLC_FLAG, HDLC_RESET and such could also occur in
	// a normal data stream, we employ a method known as
	// "bit stuffing". All control characters have more than
	// 5 ones in a row, so if the transmitting party detects
	// this sequence in the _data_ to be transmitted, it inserts
	// a zero to avoid the receiving party interpreting it as
	// a control character. Therefore, if we detect such a
	// "stuffed bit", we simply ignore it and wait for the
	// next bit to come in.
	// 
	// We do the detection by applying an AND bit-mask to the
	// stream of demodulated bits. This mask is 00111111 (0x3f)
	// if the result of the operation is 00111110 (0x3e), we
	// have detected a stuffed bit.
	if ((hdlc->demodulatedBits & 0x3f) == 0x3e)
		return ret;

	// If we have an actual 1 bit, push this to the current byte
	// If it's a zero, we don't need to do anything, since the
	// bit is initialized to zero when we bitshifted earlier.
	if (hdlc->demodulatedBits & 0x01)
		hdlc->currentByte |= 0x80;

	// Increment the bitIndex and check if we have a complete byte
	if (++hdlc->bitIndex >= 8) {
		// If we have a HDLC control character, put a AX.25 escape
		// in the received data. We know we need to do this,
		// because at this point we must have already seen a HDLC
		// flag, meaning that this control character is the result
		// of a bitstuffed byte that is equal to said control
		// character, but is actually part of the data stream.
		// By inserting the escape character, we tell the protocol
		// layer that this is not an actual control character, but
		// data.
		if ((hdlc->currentByte == HDLC_FLAG ||
			 hdlc->currentByte == HDLC_RESET ||
			 hdlc->currentByte == AX25_ESC)) {
			// We also need to check that our received data buffer
			// is not full before putting more data in
			if (!fifo_isfull(fifo)) {
				fifo_push(fifo, AX25_ESC);
			} else {
				// If it is, abort and return false
				hdlc->receiving = false;
				LED_RX_OFF();
				ret = false;
			}
		}

		// Push the actual byte to the received data FIFO,
		// if it isn't full.
		if (!fifo_isfull(fifo)) {
			fifo_push(fifo, hdlc->currentByte);
		} else {
			// If it is, well, you know by now!
			hdlc->receiving = false;
			LED_RX_OFF();
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

// adcISR ////////////////////////////////////////////
// This is the Interrupt Service Routine for the
// Analog to Digital Conversion. It is called 9600
// times each second to analyze the sample taken from
// the physical medium. The job of this routine is
// to detect whether we have a "mark" or "space"
// frequency present on the baseband (the physical
// medium). The result of this analysis will then
// be passed to the HDLC parser in form of a 1 or a 0
void afsk_adc_isr(Afsk *afsk, int8_t currentSample) {
	// To determine the received frequency, and thereby
	// the bit of the sample, we multiply the sample by
	// a sample delayed by (samples per bit / 2).
	// We then lowpass-filter the sample with a first
	// order 600Hz filter. This is a Chebyshev filter.

	afsk->iirX[0] = afsk->iirX[1];
	afsk->iirX[1] = ((int8_t)fifo_pop(&afsk->delayFifo) * currentSample) >> 2;

	afsk->iirY[0] = afsk->iirY[1];
	afsk->iirY[1] = afsk->iirX[0] + afsk->iirX[1] + (afsk->iirY[0] >> 1);

	// We put the sampled bit in a delay-line:
	// First we bitshift everything 1 left
	afsk->sampledBits <<= 1;
	// And then add the sampled bit to our delay line
	afsk->sampledBits |= (afsk->iirY[1] > 0) ? 1 : 0;

	// Put the current raw sample in the delay FIFO
	fifo_push(&afsk->delayFifo, currentSample);

	// We need to check whether there is a signal transition.
	// If there is, we can recalibrate the phase of our 
	// sampler to stay in sync with the transmitter. A bit of
	// explanation is required to understand how this works.
	// Since we have PHASE_MAX/PHASE_BITS = 8 samples per bit,
	// we employ a phase counter (currentPhase), that increments
	// by PHASE_BITS everytime a sample is captured. When this
	// counter reaches PHASE_MAX, it wraps around by modulus
	// PHASE_MAX. We then look at the last three samples we
	// captured and determine if the bit was a one or a zero.
	//
	// This gives us a "window" looking into the stream of
	// samples coming from the ADC. Sort of like this:
	//
	//   Past                                      Future
	//       0000000011111111000000001111111100000000
	//                   |________|
	//                       ||     
	//                     Window
	//
	// Every time we detect a signal transition, we adjust
	// where this window is positioned little. How much we
	// adjust it is defined by PHASE_INC. If our current phase
	// phase counter value is less than half of PHASE_MAX (ie, 
	// the window size) when a signal transition is detected,
	// add PHASE_INC to our phase counter, effectively moving
	// the window a little bit backward (to the left in the
	// illustration), inversely, if the phase counter is greater
	// than half of PHASE_MAX, we move it forward a little.
	// This way, our "window" is constantly seeking to position
	// it's center at the bit transitions. Thus, we synchronise
	// our timing to the transmitter, even if it's timing is
	// a little off compared to our own.
	if (TRANSITION_FOUND(afsk->sampledBits)) {
		if (afsk->currentPhase < PHASE_THRESHOLD) {
			afsk->currentPhase += PHASE_INC;
		} else {
			afsk->currentPhase -= PHASE_INC;
		}
	}

	// We incroment our phase counter
	afsk->currentPhase += PHASE_BITS;

	// Check if we have reached the end of
	// our sampling window.
	if (afsk->currentPhase >= PHASE_MAX) {
		// If we have, wrap around our phase
		// counter by modulus
		afsk->currentPhase %= PHASE_MAX;

		// Bitshift to make room for the next
		// bit in our stream of demodulated bits
		afsk->actualBits <<= 1;

		// We determine the actual bit value by reading
		// the last 3 sampled bits. If there is two or
		// more 1's, we will assume that the transmitter
		// sent us a one, otherwise we assume a zero
		uint8_t bits = afsk->sampledBits & 0x07;
		if (bits == 0x07 || // 111
			bits == 0x06 || // 110
			bits == 0x05 || // 101
			bits == 0x03	// 011
			) {
			afsk->actualBits |= 1;
		}

		// Now we can pass the actual bit to the HDLC parser.
		// We are using NRZ coding, so if 2 consecutive bits
		// have the same value, we have a 1, otherwise a 0.
		// We use the TRANSITION_FOUND function to determine this.
		//
		// This is smart in combination with bit stuffing,
		// since it ensures a transmitter will never send more
		// than five consecutive 1's. When sending consecutive
		// ones, the signal stays at the same level, and if
		// this happens for longer periods of time, we would
		// not be able to synchronize our phase to the transmitter
		// and would start experiencing "bit slip".
		//
		// By combining bit-stuffing with NRZ coding, we ensure
		// that the signal will regularly make transitions
		// that we can use to synchronize our phase.
		//
		// We also check the return of the Link Control parser
		// to check if an error occured.
		if (!hdlcParse(&afsk->hdlc, !TRANSITION_FOUND(afsk->actualBits), &afsk->rxFifo)) {
			afsk->status |= RX_OVERRUN;
		}
	}
}

//////////////////////////////////////////////////////
// Signal modulation and DAC                        //
//////////////////////////////////////////////////////

// Defines how many consecutive ones we send
// before we need to "stuff" in a zero
#define BIT_STUFF_LEN 5

// A macro for switching what tone is being
// synthesized by the DAC.
#define SWITCH_TONE(inc)  (((inc) == MARK_INC) ? SPACE_INC : MARK_INC)

// This function starts the transmission
static void afsk_txStart(Afsk *afsk) {
	if (!afsk->sending) {
		// Initialize the phase increment to
		// that of the mark frequency
		afsk->phaseInc = MARK_INC;
		// Reset the phase accumulator to 0
		afsk->phaseAcc = 0;
		// And also the bitstuff counter
		afsk->bitstuffCount = 0;
		// Indicate we are now sending
		afsk->sending = true;
		// And turn on the blingy LED
		LED_TX_ON();
		// We also need to calculate how many HDLC_FLAG
		// bytes we need to send in preamble
		afsk->preambleLength = DIV_ROUND(CONFIG_AFSK_PREAMBLE_LEN * BITRATE, 8000);
		AFSK_DAC_IRQ_START();
	}
	// We make the same calculation for the tail length,
	// but this needs to be atomic, since the txStart
	// function could potentially be called while we
	// are already transmitting.
	ATOMIC(afsk->tailLength = DIV_ROUND(CONFIG_AFSK_TRAILER_LEN * BITRATE, 8000));
}

// This is the DAC ISR, called at sampling rate whenever the DAC IRQ is on.
// It modulates the data to be transmitted and returns a value directly
// for output on the DAC
uint8_t afsk_dac_isr(Afsk *afsk) {
	// Check whether we are at the beginning of a sample
	if (afsk->sampleIndex == 0) {
		// If we are, we should figure out what we are
		// actually going to modulate and transmit :)
		if (afsk->txBit == 0) {
			// txBit is a bitmask that is ANDed to the
			// byte we are sending. It is bitshifted one
			// position left each time we shift the next
			// bit. If it is 0, we know we are at the
			// beginning of the next byte, and nothing
			// has been transmitted yet.

			// If TX FIFO is empty and tail-length has decremented to 0
			// we are done, stop the IRQ and reset
			if (fifo_isempty(&afsk->txFifo) && afsk->tailLength == 0) {
				AFSK_DAC_IRQ_STOP();
				afsk->sending = false;
				LED_TX_OFF();
				return 0;
			} else {
				// Reset the bitstuff counter if we have just sent
				// a bitstuffed byte
				if (!afsk->bitStuff) afsk->bitstuffCount = 0;
				// Reset bitstuff indicator to true, signifying
				// that it's ok to bit stuff.
				afsk->bitStuff = true;

				// Check if we are in preamble or tail
				if (afsk->preambleLength == 0) {
					// We are not in preamble
					if (fifo_isempty(&afsk->txFifo)) {
						// If the TX buffer is empty, we must
						// be in the TX tail then.
						// Decrement the tail counter and send
						// a HDLC_FLAG
						afsk->tailLength--;
						afsk->currentOutputByte = HDLC_FLAG;
					} else {
						// If preamble is already transmitted and TX
						// buffer is not empty, we should get a byte
						// for transmission
						afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
					}
				} else {
					// We are in preamble. We'll decrement
					// the preamble counter and transmit a
					// HDLC_FLAG
					afsk->preambleLength--;
					afsk->currentOutputByte = HDLC_FLAG;
				}

				// This handles escape sequences and control
				// characters. First we check if the current
				// byte is an escape character. If it is, we
				// know the next byte, even though it looks
				// like an HDLC control character, in fact is
				// not. Therefore we'll fetch it and transmit
				// it as data using bit stuffing.
				if (afsk->currentOutputByte == AX25_ESC) {
					// First make sure that the TX buffer is
					// not empty for some strange reason
					if (fifo_isempty(&afsk->txFifo)) {
						AFSK_DAC_IRQ_STOP();
						afsk->sending = false;
						LED_TX_OFF();
						return 0;
					} else {
						// If it is not, fetch the next byte
						afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
					}
				} else if (afsk->currentOutputByte == HDLC_FLAG || afsk->currentOutputByte == HDLC_RESET) {
					// If there was not an escape character and
					// this byte is an HDLC control character,
					// we know that it is an _actual_ control
					// character, and we indicate that it should
					// not be bitstuffed.
					afsk->bitStuff = false;
				}
			}
			// Since we are at the beginning of a byte,
			// we'll initialize the txBit mask to:
			// 00000001. It will then be bit-shifted one
			// position to the left each time we send the
			// next bit. By ANDing this mask to the byte
			// we are sending, we can quickly figure out
			// what tone we should transmit. For example:
			// 
			// If we are sending bit number 4 of the
			// byte:                      01101011
			// The bit mask would be:     00001000
			// If we AND the byte and the
			// mask, we get:			  00001000
			// Since this is not zero, we know we should
			// transmit a one.
			afsk->txBit = 0x01;
		}

		// First we need to check for bit-stuffing
		if (afsk->bitStuff && afsk->bitstuffCount >= BIT_STUFF_LEN) {
			// If we are allowed to bit-stuff, and we have
			// reached the maximum number of consecutive
			// ones, we'll reset the bit-stuff counter and
			// insert a zero into the bitstream
			afsk->bitstuffCount = 0;
			afsk->phaseInc = SWITCH_TONE(afsk->phaseInc);
		} else {
			// If we don't need to bit-stuff now, we can get
			// on with the actual transmission.
			//
			// We are using NRZ so if we want to transmit a 1
			// the modulated signal will stay the same. For a 0
			// we make the signal transition.
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

			// Bitshift the mast to allow for the next
			// bit in the byte to be transmitted
			afsk->txBit <<= 1;
		}

		// We set sampleIndex to DAC_SAMPLESPERBIT,
		// so we will transmit this bit for the number
		// of samples one bit requires to transmit at
		// the chosen bitrate.
		afsk->sampleIndex = DAC_SAMPLESPERBIT;
	}

	// We increment the phase accumulator
	// by the amount needed for the tone
	afsk->phaseAcc += afsk->phaseInc;
	// We then make sure that we have not
	// exceeded the length of our sine table
	afsk->phaseAcc %= SIN_LEN;
	// Finally we decrement the sample counter
	afsk->sampleIndex--;
	// ... and return the sample to for it to
	// be written out
	return sinSample(afsk->phaseAcc);
}


//////////////////////////////////////////////////////
// File operation functions for read/write          //
// These functions make the "class" act like a file //
// pointer, which can be read from or written to.   //
// Handy for sending and receiving data :)          //
//////////////////////////////////////////////////////

// Read from the modem
static size_t afsk_read(KFile *fd, void *_buf, size_t size) {
	Afsk *afsk = AFSK_CAST(fd);
	uint8_t *buffer = (uint8_t *)_buf;

	#if CONFIG_AFSK_RXTIMEOUT == 0
	while (size-- && !fifo_isempty_locked(&afsk->rxFifo))
	#else
	while (size--)
	#endif
	{
		#if CONFIG_AFSK_RXTIMEOUT != -1
		ticks_t start = timer_clock();
		#endif

		while (fifo_isempty_locked(&afsk->rxFifo)) {
			cpu_relax();
			#if CONFIG_AFSK_RXTIMEOUT != -1
			if (timer_clock() - start > ms_to_ticks(CONFIG_AFSK_RXTIMEOUT)) {
				return buffer - (uint8_t *)_buf;
			}
			#endif
		}
		*buffer++ = fifo_pop_locked(&afsk->rxFifo);
	}

	return buffer - (uint8_t *)_buf;
}

// Write to the modem
static size_t afsk_write(KFile *fd, const void *_buf, size_t size) {
	Afsk *afsk = AFSK_CAST(fd);
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size--) {
		while (fifo_isfull_locked(&afsk->txFifo)) {
			cpu_relax();
		}

		fifo_push_locked(&afsk->txFifo, *buf++);
		afsk_txStart(afsk);
	}

	return buf - (const uint8_t *)_buf;
}

// Waits for the write operation to finish
static int afsk_flush(KFile *fd) {
	Afsk *afsk = AFSK_CAST(fd);
	while (afsk->sending) {
		cpu_relax();
	}
	return 0;
}

// Check whether there was any errors
// while reading or writing
static int afsk_error(KFile *fd) {
	Afsk *afsk = AFSK_CAST(fd);
	int err;
	ATOMIC(err = afsk->status);
	return err;
}

// Allows resetting the error-state
static void afsk_clearerr(KFile *fd) {
	Afsk *afsk = AFSK_CAST(fd);
	ATOMIC(afsk->status = 0);
}


//////////////////////////////////////////////////////
// Modem Initialization                             //
//////////////////////////////////////////////////////

void afsk_init(Afsk *afsk, int _adcPin) {
	// Allocate memory for struct
	memset(afsk, 0, sizeof(*afsk));

	// Configure ADC pin
	afsk->adcPin = _adcPin;

	// Initialise phase increment to that
	// of the mark frequency
	afsk->phaseInc = MARK_INC;

	// Initialize FIFO buffers
	fifo_init(&afsk->delayFifo, (uint8_t *)afsk->delayBuf, sizeof(afsk->delayBuf));
	fifo_init(&afsk->rxFifo, afsk->rxBuf, sizeof(afsk->rxBuf));
	fifo_init(&afsk->txFifo, afsk->txBuf, sizeof(afsk->txBuf));

	// Fill delay FIFO with zeroes
	for (int i = 0; i<SAMPLESPERBIT / 2; i++) {
		fifo_push(&afsk->delayFifo, 0);
	}

	// Initialize hardware
	AFSK_ADC_INIT(_adcPin, afsk);
	AFSK_DAC_INIT();
	LED_TX_INIT();
	LED_RX_INIT();

	// And register the modem file-pointer
	// functions for reading from and 
	// writing to it.
	DB(afsk->fd._type = KFT_AFSK);
	afsk->fd.write = afsk_write;
	afsk->fd.read = afsk_read;
	afsk->fd.flush = afsk_flush;
	afsk->fd.error = afsk_error;
	afsk->fd.clearerr = afsk_clearerr;	
}