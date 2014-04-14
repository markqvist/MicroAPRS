#include "mp1.h"
#include "hardware.h"
#include <string.h>
#include <drv/ser.h>

#include "compression/heatshrink_encoder.h"
#include "compression/heatshrink_decoder.h"

// FIXME: Describe these
static uint8_t lastByte = 0x00;
static bool sendParityBlock = false;


// FIXME: Describe this
INLINE bool BIT(uint8_t byte, int n) { return ((byte & BV(n-1))>>(n-1)); }
static uint8_t mp1ParityBlock(uint8_t first, uint8_t other) {
	uint8_t parity = 0x00;

	parity = 	((BIT(first, 1) ^ BIT(first, 2) ^ BIT(first, 4) ^ BIT(first, 5) ^ BIT(first, 7))) +
				((BIT(first, 1) ^ BIT(first, 3) ^ BIT(first, 4) ^ BIT(first, 6) ^ BIT(first, 7))<<1) +
				((BIT(first, 2) ^ BIT(first, 3) ^ BIT(first, 4) ^ BIT(first, 8))<<2) +
				((BIT(first, 5) ^ BIT(first, 6) ^ BIT(first, 7) ^ BIT(first, 8))<<3) +

				((BIT(other, 1) ^ BIT(other, 2) ^ BIT(other, 4) ^ BIT(other, 5) ^ BIT(other, 7))<<4) +
				((BIT(other, 1) ^ BIT(other, 3) ^ BIT(other, 4) ^ BIT(other, 6) ^ BIT(other, 7))<<5) +
				((BIT(other, 2) ^ BIT(other, 3) ^ BIT(other, 4) ^ BIT(other, 8))<<6) +
				((BIT(other, 5) ^ BIT(other, 6) ^ BIT(other, 7) ^ BIT(other, 8))<<7);

	return parity;
}

static void mp1Decode(MP1 *mp1) {
	// This decode function is basic and bare minimum.
	// It does nothing more than extract the data
	// payload from the buffer and put it into a struct
	// for further processing.
	MP1Packet packet;				// A decoded packet struct
	uint8_t *buffer	= mp1->buffer;	// Get the buffer from the protocol context
	
	// Get the header and "remove" it from the buffer
	uint8_t header = buffer[0];
	buffer++;

	// If header indicates a padded packet, remove
	// padding
	if (header & MP1_HEADER_PADDED) {
		buffer++;
	}

	// Set the payload length of the packet to the counted
	// length minus 1, so we remove the checksum
	packet.dataLength = mp1->packetLength - 2 - (header & 0x01);

	// Check if we have received a compressed packet
	if (header & MP1_HEADER_COMPRESSION) {
		size_t decompressedSize = decompress(buffer, packet.dataLength);
		packet.dataLength = decompressedSize;
		memcpy(buffer, compressionBuffer, decompressedSize);
	}

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
	sendParityBlock = false; // Reset our parity tx indicator

	// Read bytes from the modem until we reach EOF
	while ((byte = kfile_getc(mp1->modem)) != EOF) {
		// We have a byte, increment our read counter

		// FIXME: Describe error correction
		if (mp1->reading && (byte != AX25_ESC) ) {
			mp1->readLength++;


			if (mp1->readLength % 3 == 0) {
				mp1->calculatedParity = mp1ParityBlock(mp1->buffer[mp1->packetLength-2], mp1->buffer[mp1->packetLength-1]);
				uint8_t syndrome = mp1->calculatedParity ^ byte;
				if (syndrome == 0x00) {
					// No problems!
				} else {
					uint8_t syndromes[2];
					syndromes[0] = syndrome & 0x0f;
					syndromes[1] = (syndrome & 0xf0) >> 4;

					for (int i = 0; i < 2; i++) {
						uint8_t s = syndromes[i];
						uint8_t correction = 0x00;
						if (s == 1 || s == 2 || s == 4 || s == 8) {
							// Error in parity bit, no correction needed
							continue;
						}
						if (s == 3)  correction = 0x01;
						if (s == 5)  correction = 0x02;
						if (s == 6)  correction = 0x04;
						if (s == 7)  correction = 0x08;
						if (s == 9)  correction = 0x10;
						if (s == 10) correction = 0x20;
						if (s == 11) correction = 0x40;
						if (s == 12) correction = 0x80;

						if (correction != 0x00) {
							mp1->checksum_in ^= correction;
						}
						mp1->buffer[mp1->packetLength-(2-i)] ^= correction;
					}
				}
				continue;
			}
		}
		// FIXME: Describe error correction //////////
		
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
					// kprintf("[ER] [%d] ", mp1->checksum_in);
					//mp1Decode(mp1);
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
				mp1->checksum_in = mp1->checksum_in ^ byte;
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

// FIXME: Desribe additions here
static void mp1Putbyte(MP1 *mp1, uint8_t byte) {
	// If we are sending something that looks
	// like an HDLC special byte, send an escape
	// character first
	if (byte == HDLC_FLAG ||
		byte == HDLC_RESET ||
		byte == AX25_ESC) {
		kfile_putc(AX25_ESC, mp1->modem);
		lastByte = AX25_ESC;
		//sendParityBlock ^= true;
	}

	kfile_putc(byte, mp1->modem);

	if (sendParityBlock) {
		uint8_t p = mp1ParityBlock(lastByte, byte);
		kfile_putc(p, mp1->modem);
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

	bool packetCompression = false;
	size_t compressedSize = compress(buffer, length);
	if (compressedSize != 0 && compressedSize < length) {
		//kprintf("Using compression\n");
		// Compression saved us some space, we'll
		// send the paket compressed
		packetCompression = true;
		memcpy(buffer, compressionBuffer, compressedSize);
		length = compressedSize;
	} else {
		// We are not going to use compression
	}

	// Write header and possibly padding
	// Remember we also write a header and
	// a checksum. This ensures that we will
	// always end our packet with a checksum
	// and a parity byte.

	uint8_t header = 0xf0;
	if (packetCompression) header ^= MP1_HEADER_COMPRESSION;

	if (length % 2 != 0) {
		header ^= MP1_HEADER_PADDED;
		mp1->checksum_out = mp1->checksum_out ^ header;
		mp1Putbyte(mp1, header);
		mp1->checksum_out = mp1->checksum_out ^ MP1_PADDING;
		mp1Putbyte(mp1, MP1_PADDING);
	} else {
		mp1->checksum_out = mp1->checksum_out ^ header;
		mp1Putbyte(mp1, header);
	}

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

int freeRam(void) {
   extern int __heap_start, *__brkval; 
   int v; 
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

size_t compress(uint8_t *input, size_t length) {
	heatshrink_encoder *hse = heatshrink_encoder_alloc(8, 4);
	if (hse == NULL) {
		//kprintf("Could not allocate encoder\n");
		return 0;
	}

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_encoder_sink(hse, input, length, &sunk);
	int status = heatshrink_encoder_finish(hse);

	if (sunk < length) {
		//kprintf("Not all data was sunk into encoder\n");
		heatshrink_encoder_free(hse);
		return 0;
	} else {
		//kprintf("Bytes sunk into HSE: %d\n", length);
		if (status == HSER_FINISH_MORE) {
			heatshrink_encoder_poll(hse, compressionBuffer, MP1_MAX_FRAME_LENGTH, &written);
			//kprintf("Bytes written into buffer: %d\n", written);
		} else {
			//kprintf("All input data was sunk, but encoder doesn't have any data for us.");
		}
	}

	heatshrink_encoder_free(hse);
	return written;
}

size_t decompress(uint8_t *input, size_t length) {
	heatshrink_decoder *hsd = heatshrink_decoder_alloc(MP1_MAX_FRAME_LENGTH, 8, 4);
	if (hsd == NULL) {
		//kprintf("Could not allocate decoder\n");
		return 0;
	}

	//kprintf("\nDecoder allocated. Free RAM: %d bytes\n", freeRam());

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_decoder_sink(hsd, input, length, &sunk);
	int status = heatshrink_decoder_finish(hsd);

	if (sunk < length) {
		//kprintf("Not all data was sunk into decoder\n");
		heatshrink_decoder_free(hsd);
		return 0;
	} else {
		//kprintf("Bytes sunk into HSD: %d\n", length);
		if (status == HSER_FINISH_MORE) {
			heatshrink_decoder_poll(hsd, compressionBuffer, MP1_MAX_FRAME_LENGTH, &written);
			//kprintf("Bytes written into decompression buffer: %d\n", written);
		} else {
			//kprintf("All input data was sunk, but the decoder doesn't have any data for us.");
		}
	}

	heatshrink_decoder_free(hsd);
	return written;
}
