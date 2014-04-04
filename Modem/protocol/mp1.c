#include "mp1.h"
#include <string.h>
#include <drv/ser.h>
//#include <ctype.h>

static void mp1Decode(MP1 *mp1) {
	MP1Packet packet;				// A decoded packet struct
	uint8_t *buffer	= mp1->buffer;	// Get the buffer from the protocol context

	packet.dataLength = mp1->packetLength - 1;
	packet.data = buffer;

	if (mp1->callback) mp1->callback(&packet);
}

void mp1Poll(MP1 *mp1) {
	int byte;

	// Read bytes from the modem until we reach EOF
	while ((byte = kfile_getc(mp1->modem)) != EOF) {
		if (!mp1->escape && byte == HDLC_FLAG) {
			// We are not in an escape sequence and we
			// found a HDLC_FLAG. This can mean two things:
			if (mp1->packetLength >= MP1_MIN_FRAME_LENGTH) {
				// We already have more data than the minimum
				// frame length, which means the flag signifies
				// the end of the packet. Pass control to the
				// decoder.
				kprintf("Got checksum: %d.\n", mp1->buffer[mp1->packetLength-1]);
				if ((mp1->checksum_in & 0xff) == 0x00) {
					//kprintf("Correct checksum. Found %d.\n", mp1->buffer[mp1->packetLength-1]);
					mp1Decode(mp1);
				} else {
					// Checksum was incorrect
					mp1Decode(mp1);
					//kprintf("Incorrect checksum. Found %d.\n", mp1->buffer[mp1->packetLength]);
					//kprintf("should be %d", mp1->checksum_in);
				}
			}
			// If the above is not the case, this must be the
			// beginning of a frame
			mp1->reading = true;
			mp1->packetLength = 0;
			mp1->checksum_in = MP1_CHECKSUM_INIT;
			//kprintf("Checksum init with %d\n", mp1->checksum_in);

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
				//kprintf("Checksum is now %d\n", mp1->checksum_in);
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
	}
	kfile_putc(byte, mp1->modem);
}

void mp1Send(MP1 *mp1, const void *_buffer, size_t length) {
	// Get the transmit data buffer
	const uint8_t *buffer = (const uint8_t *)_buffer;

	// Initialize checksum
	mp1->checksum_out = MP1_CHECKSUM_INIT;
	//kprintf("Checksum init with %d\n", mp1->checksum_out);

	// Transmit the HDLC_FLAG to signify start of TX
	kfile_putc(HDLC_FLAG, mp1->modem);

	// Continously increment the pointer address
	// of the buffer while passing it to the byte
	// output function
	while (length--) {
		mp1->checksum_out = mp1->checksum_out ^ *buffer;
		//kprintf("Checksum is now %d\n", mp1->checksum_out);
		mp1Putbyte(mp1, *buffer++);
	}

	// Write checksum to end of packet
	kprintf("Checksum of this packet is %d\n", mp1->checksum_out);
	mp1Putbyte(mp1, mp1->checksum_out);

	// Transmit a HDLC_FLAG to signify end of TX
	kfile_putc(HDLC_FLAG, mp1->modem);
}

void mp1Init(MP1 *mp1, KFile *modem, mp1_callback_t callback) {
	// Allocate memory for our protocol "object"
	memset(mp1, 0, sizeof(*mp1));
	mp1->modem = modem;
	mp1->callback = callback;
}