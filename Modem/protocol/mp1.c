#include "mp1.h"
#include "hardware.h"
#include <string.h>
#include <drv/ser.h>

static uint8_t lastByte = 0x00;
static bool sendParityBlock = false;

INLINE bool BIT(uint8_t byte, int n) { return (byte & BV(n)); }
static uint8_t mp1ParityBlock(uint8_t first, uint8_t other) {
	uint8_t parity = 0x00;
	parity ^= (BIT(first, 1) ^ BIT(first, 2) ^ BIT(first, 4) ^ BIT(first, 5) ^ BIT(first, 7)) << 7;
	parity ^= (BIT(first, 1) ^ BIT(first, 3) ^ BIT(first, 4) ^ BIT(first, 6) ^ BIT(first, 7)) << 6;
	parity ^= (BIT(first, 2) ^ BIT(first, 3) ^ BIT(first, 4) ^ BIT(first, 8)) << 5;
	parity ^= (BIT(first, 5) ^ BIT(first, 6) ^ BIT(first, 7) ^ BIT(first, 8)) << 4;

	parity ^= BIT(other, 1) ^ BIT(other, 2) ^ BIT(other, 4) ^ BIT(other, 5) ^ BIT(other, 7) << 3;
	parity ^= BIT(other, 1) ^ BIT(other, 3) ^ BIT(other, 4) ^ BIT(other, 6) ^ BIT(other, 7) << 2;
	parity ^= BIT(other, 2) ^ BIT(other, 3) ^ BIT(other, 4) ^ BIT(other, 8) << 1;
	parity ^= BIT(other, 5) ^ BIT(other, 6) ^ BIT(other, 7) ^ BIT(other, 8);

	return parity;
}

static void mp1Decode(MP1 *mp1) {
	// This decode function is basic and bare minimum.
	// It does nothing more than extract the data
	// payload from the buffer and put it into a struct
	// for further processing.
	MP1Packet packet;				// A decoded packet struct
	uint8_t *buffer	= mp1->buffer;	// Get the buffer from the protocol context

	// Set the payload length of the packet to the counted
	// length minus 1, so we remove the checksum
	packet.dataLength = mp1->packetLength - 1;
	packet.data = buffer;

	// If a callback have been specified, let's
	// call it and pass the decoded packet
	if (mp1->callback) mp1->callback(&packet);
}

////////////////////////////////////////////////////////////
// The Poll function reads data from the modem, handles   //
// frame recognition and passes data on to higher layers  //
// if valid packets are found                             //
////////////////////////////////////////////////////////////
void mp1Poll(MP1 *mp1) {
	int byte; // A place to store our read byte

	// Read bytes from the modem until we reach EOF
	while ((byte = kfile_getc(mp1->modem)) != EOF) {
		// We have a byte, increment our read counter
		mp1->readLength++;

		if (mp1->readLength % 3 != 0) {
			// This is not a parity byte
			if (!mp1->escape && byte == HDLC_FLAG) {
				// We are not in an escape sequence and we
				// found a HDLC_FLAG. This can mean two things:
				if (mp1->packetLength >= MP1_MIN_FRAME_LENGTH) {
					// We already have more data than the minimum
					// frame length, which means the flag signifies
					// the end of the packet. Pass control to the
					// decoder.
					if ((mp1->checksum_in & 0xff) == 0x00) {
						mp1Decode(mp1);
					} else {
						// Checksum was incorrect, we don't do anything,
						// but you can enable the decode anyway, if you
						// need it for testing or debugging
						// mp1Decode(mp1);
					}
				}
				// If the above is not the case, this must be the
				// beginning of a frame
				mp1->reading = true;
				mp1->packetLength = 0;
				mp1->readLength = 0;
				mp1->checksum_in = MP1_CHECKSUM_INIT;

				// We have indicated that we are reading,
				// and reset the length counter. Now we'll
				// continue to the next byte.
				continue;
			}

			if (!mp1->escape && byte == HDLC_RESET) {
				// Not good, we got a reset. The transmitting
				// party may have encountered an error. We'll
				// stop receiving this packet immediately.
				mp1->reading = false;
				continue;
			}

			// This should be a parity byte
			if (mp1->readLength % 3 == 0) {
				uint8_t calculatedParity = mp1ParityBlock(mp1->buffer[mp1->packetLength-2], mp1->buffer[mp1->packetLength-1]);
				if (byte == calculatedParity) {
					// Parity match, block is correct
				} else {
					// Parity differ, transmission error ocurred
					kprintf("Parity mismatch");
				}
				mp1->readLength = 0;
			}

			if (!mp1->escape && byte == AX25_ESC) {
				// We found an escape character. We'll set
				// the escape seqeunce indicator so we don't
				// interpret the next byte as a reset or flag
				mp1->escape = true;
				continue;
			}

			// Now let's get to the actual reading of the data
			if (mp1->reading) {
				if (mp1->packetLength < MP1_MAX_FRAME_LENGTH) {
					// If the length of the current incoming frame is
					// still less than our max length, put the incoming
					// byte in the buffer.
					if (!mp1->escape) mp1->checksum_in = mp1->checksum_in ^ byte;
					mp1->buffer[mp1->packetLength++] = byte;
				} else {
					// If not, we have a problem: The buffer has overrun
					// We need to stop receiving, and the packet will be
					// dropped :(
					mp1->reading = false;
				}
			}
			// We need to set the escape sequence indicator back
			// to false after each byte.
			mp1->escape = false;
		} else {
			
		}
	}

	if (kfile_error(mp1->modem)) {
		// If there was an error from the modem, we'll be rude
		// and just reset it. No error handling is done for now.
		kfile_clearerr(mp1->modem);
	}
}

static void mp1Putbyte(MP1 *mp1, uint8_t byte) {
	// If we are sending something that looks
	// like an HDLC special byte, send an escape
	// character first
	if (byte == HDLC_FLAG ||
		byte == HDLC_RESET ||
		byte == AX25_ESC) {
		kfile_putc(AX25_ESC, mp1->modem);
		lastByte = AX25_ESC;
		sendParityBlock ^= true;
	}

	kfile_putc(byte, mp1->modem);

	if (sendParityBlock) {
		kfile_putc(mp1ParityBlock(lastByte, byte), mp1->modem);
	}

	lastByte = byte;
	sendParityBlock ^= true;
}


void mp1Send(MP1 *mp1, const void *_buffer, size_t length) {
	// Get the transmit data buffer
	const uint8_t *buffer = (const uint8_t *)_buffer;

	// Initialize checksum
	mp1->checksum_out = MP1_CHECKSUM_INIT;

	// Transmit the HDLC_FLAG to signify start of TX
	kfile_putc(HDLC_FLAG, mp1->modem);

	// Continously increment the pointer address
	// of the buffer while passing it to the byte
	// output function
	while (length--) {
		mp1->checksum_out = mp1->checksum_out ^ *buffer;
		mp1Putbyte(mp1, *buffer++);
	}

	// Write checksum to end of packet
	mp1Putbyte(mp1, mp1->checksum_out);

	// Transmit a HDLC_FLAG to signify end of TX
	kfile_putc(HDLC_FLAG, mp1->modem);
}

void mp1Init(MP1 *mp1, KFile *modem, mp1_callback_t callback) {
	// Allocate memory for our protocol "object"
	memset(mp1, 0, sizeof(*mp1));
	// Set references to our modem "object" and
	// a callback for when a packet has been decoded
	mp1->modem = modem;
	mp1->callback = callback;
}