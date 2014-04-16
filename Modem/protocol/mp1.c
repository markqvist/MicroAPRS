#include "mp1.h"
#include "hardware.h"
#include "config.h"
#include <string.h>
#include <drv/ser.h>

#include "compression/heatshrink_encoder.h"
#include "compression/heatshrink_decoder.h"

// We need an indicator to tell us whether we
// should send a parity byte. This happens
// whenever two normal bytes of data has been
// sent. We also keep the last sent byte in
// memory because we need it to calculate the
// parity byte.
static bool sendParityBlock = false;
static uint8_t lastByte = 0x00;

// We also need a buffer for compressing and
// decompressing packet data.
static uint8_t compressionBuffer[MP1_MAX_FRAME_LENGTH+10];

// The GET_BIT macro is used in the interleaver
// and deinterleaver to access single bits of a
// byte.
INLINE bool GET_BIT(uint8_t byte, int n) { return (byte & (1 << (8-n))) == (1 << (8-n)); }

// This function calculates and returns a parity
// byte for two input bytes. The parity byte is
// used for correcting errors in the transmission.
// The error correction algorithm is a standard
// (12,8) Hamming code.
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

// This deode function retrieves the buffer of
// received, deinterleaved and error-corrected
// bytes, inspects the header and determines
// whether there is padding to be removed, and
// whether the packet is compressed. If it is
// it is decompressed before being passed to
// the registered callback.
static void mp1Decode(MP1 *mp1) {
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
		// If we have, we decompress it and use the
		// decompressed data for the packet
		size_t decompressedSize = decompress(buffer, packet.dataLength);
		packet.dataLength = decompressedSize;
		memcpy(buffer, compressionBuffer, decompressedSize);
	}

	// Set the data field of the packet to our buffer
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

		/////////////////////////////////////////////
		// This following block handles forward    //
		// error correction using an interleaved   //
		// (12,8) Hamming code                     //
		/////////////////////////////////////////////

		// If we have started reading (received an
		// HDLC_FLAG), we will start looking at the
		// incoming data and perform forward error
		// correction on it.
		if (mp1->reading && (byte != AX25_ESC) ) {
			mp1->readLength++;

			// Check if we have read three bytes. If we
			// have, we should now have a block of two
			// data bytes and a parity byte. This block
			if (mp1->readLength % 3 == 0) {
				// The block is interleaved, so we will
				// first put the received bytes in the
				// deinterleaving buffer
				mp1->interleaveIn[0] = mp1->buffer[mp1->packetLength-2];
				mp1->interleaveIn[1] = mp1->buffer[mp1->packetLength-1];
				mp1->interleaveIn[2] = byte;

				// We then deinterleave the block
				mp1Deinterleave(mp1);

				// And write the deinterleaved data
				// back into the buffer
				mp1->buffer[mp1->packetLength-2] = mp1->interleaveIn[0];
				mp1->buffer[mp1->packetLength-1] = mp1->interleaveIn[1];

				// We now calculate a parity byte on the
				// received data.
				mp1->calculatedParity = mp1ParityBlock(mp1->buffer[mp1->packetLength-2], mp1->buffer[mp1->packetLength-1]);

				// By XORing the calculated parity byte
				// with the received parity byte, we get
				// what is called the "syndrome". This
				// number will tell us if we had any
				// errors during transmission, and if so
				// where they are. Using Hamming code, we
				// can only detect single bit errors in a
				// byte though, which is why we interleave
				// the data, since most errors will usually
				// occur in bursts of more than one bit.
				// With 2 data byte interleaving we can
				// correct 2 consecutive bit errors.
				uint8_t syndrome = mp1->calculatedParity ^ mp1->interleaveIn[2];
				if (syndrome == 0x00) {
					// If the syndrome equals 0, we either
					// don't have any errors, or the error
					// is unrecoverable, so we don't do
					// anything
				} else {
					// If the syndrome is not equal to 0,
					// there is a problem, and we will try
					// to correct it. We first need to split
					// the syndrome byte up into the two
					// actual syndrome numbers, one for
					// each data byte.
					uint8_t syndromes[2];
					syndromes[0] = syndrome & 0x0f;
					syndromes[1] = (syndrome & 0xf0) >> 4;

					// Then we look at each syndrome number
					// to determine what bit in the data
					// bytes to correct.
					for (int i = 0; i < 2; i++) {
						uint8_t s = syndromes[i];
						uint8_t correction = 0x00;
						if (s == 1 || s == 2 || s == 4 || s == 8) {
							// This signifies an error in the
							// parity block, so we actually
							// don't need any correction
							continue;
						}

						// The following determines what
						// bit to correct according to
						// the syndrome value.
						if (s == 3)  correction = 0x01;
						if (s == 5)  correction = 0x02;
						if (s == 6)  correction = 0x04;
						if (s == 7)  correction = 0x08;
						if (s == 9)  correction = 0x10;
						if (s == 10) correction = 0x20;
						if (s == 11) correction = 0x40;
						if (s == 12) correction = 0x80;

						// And finally we apply the correction
						mp1->buffer[mp1->packetLength-(2-i)] ^= correction;

						// This is just for testing purposes.
						// Nice to know when corrections were
						// actually made.
						if (s != 0) mp1->correctionsMade += 1;
					}
				}

				// We now update the checksum of the packet
				// with the deinterleaved and possibly
				// corrected bytes.
				mp1->checksum_in ^= mp1->buffer[mp1->packetLength-2];
				mp1->checksum_in ^= mp1->buffer[mp1->packetLength-1];

				continue;
			}
		}
		/////////////////////////////////////////////
		// End of forward error correction block   //
		/////////////////////////////////////////////
		
		// This next part of the poll function handles
		// the reading from the modem, and looks for
		// starts and ends of transmissions. It also
		// handles escape characters by discarding them
		// so they don't get put into the output data.

		// Let's first check if we have read an HDLC_FLAG.
		if (!mp1->escape && byte == HDLC_FLAG) {
			// We are not in an escape sequence and we
			// found a HDLC_FLAG. This can mean two things:
			if (mp1->packetLength >= MP1_MIN_FRAME_LENGTH) {
				// We already have more data than the minimum
				// frame length, which means the flag signifies
				// the end of the packet. Pass control to the
				// decoder.
				if ((mp1->checksum_in & 0xff) == 0x00) {
					if (SERIAL_DEBUG) kprintf("[CHK-OK] [C=%d] ", mp1->correctionsMade);
					mp1Decode(mp1);
				} else {
					// Checksum was incorrect, we don't do anything,
					// but you can enable the decode anyway, if you
					// need it for testing or debugging
					if (PASSALL) {
						if (SERIAL_DEBUG) kprintf("[CHK-ER] [C=%d] ", mp1->correctionsMade);
						mp1Decode(mp1);
					}
				}
			}
			// If the above is not the case, this must be the
			// beginning of a frame
			mp1->reading = true;
			mp1->packetLength = 0;
			mp1->readLength = 0;
			mp1->checksum_in = MP1_CHECKSUM_INIT;
			mp1->correctionsMade = 0;

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
			// We then continue reading the next byte.
			continue;
		}

		// Now let's get to the actual reading of the data
		if (mp1->reading) {
			if (mp1->packetLength < MP1_MAX_FRAME_LENGTH) {
				// If the length of the current incoming frame is
				// still less than our max length, put the incoming
				// byte in the buffer. When we have collected 3
				// bytes, they will be processed by the error
				// correction part above.

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

// This is called to actually send the bytes
// after they have been interleaved
static void mp1WriteByte(MP1 *mp1, uint8_t byte) {
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

// This is an intermediary function that
// receives outgoing bytes, and adds
// interleaving and a parity byte to the
// outgoing data in blocks of two data
// bytes. The actual transmitted block will
// be 3 bytes long due to the added parity
// byte.
static void mp1Putbyte(MP1 *mp1, uint8_t byte) {
	mp1Interleave(mp1, byte);

	if (sendParityBlock) {
		uint8_t p = mp1ParityBlock(lastByte, byte);
		//kfile_putc(p, mp1->modem);
		mp1Interleave(mp1, p);
	}

	lastByte = byte;
	sendParityBlock ^= true;
}

// This function accepts a buffer with data
// to be transmitted, and structures it into
// a valid packet.
void mp1Send(MP1 *mp1, const void *_buffer, size_t length) {
	// Get the transmit data buffer
	const uint8_t *buffer = (const uint8_t *)_buffer;

	// Initialize checksum to zero
	mp1->checksum_out = MP1_CHECKSUM_INIT;

	// We also reset the interleave counter to zero
	mp1->interleaveCounter = 0;

	// Transmit the HDLC_FLAG to signify start of TX
	kfile_putc(HDLC_FLAG, mp1->modem);

	// We start out assuming we should not use
	// compression.
	bool packetCompression = false;

	// We then try to compress the data to see
	// if we can save some space with compression.
	size_t compressedSize = compress(buffer, length);
	if (compressedSize != 0 && compressedSize < length) {
		// Compression saved us some space, we'll
		// send the paket compressed
		packetCompression = true;
		// Write the compressed data into the
		// outgoing data buffer
		memcpy(buffer, compressionBuffer, compressedSize);
		// Make sure to set the length of the
		// data to the new (compressed) length
		length = compressedSize;
	} else {
		// We are not going to use compression,
		// so we don't do anything.
	}

	
	// We now need to construct a header, that
	// can tell the receiving end whether the
	// packet is compressed. Since a packet must
	// have an even number of total payload bytes
	// (including the header), we check the length
	// of the outgoing data, and if it is not even,
	// we add a single byte of padding to the
	// packet. Remember that we also send a single
	// byte checksum at the end of the packet, so
	// the header and checksum bytes together don't
	// change whether the payload length is even
	// or not. The payload length needs to be even
	// since we are sending a parity byte for every
	// two data bytes sent, and because interleaving
	// happens in blocks of three bytes.
	uint8_t header = 0xf0;

	// If we are using compression, set the
	// appropriate header flag to true.
	if (packetCompression) header ^= MP1_HEADER_COMPRESSION;

	// We check if the data length is even
	if (length % 2 != 0) {
		// If it is not, we set the appropriate
		// header flag to indicate that we are
		// padding this packet with one byte.
		header ^= MP1_HEADER_PADDED;

		// We then update the checksum with the
		// header byte and queue it for transmit
		mp1->checksum_out = mp1->checksum_out ^ header;
		mp1Putbyte(mp1, header);

		// We now update the checksum with the
		// padding byte, and queue that for
		// transmission as well. At this point,
		// we will have pushed out two bytes of
		// data. The output function will detect
		// this, and a parity byte will be
		// calculated. The 3-byte block is then
		// actually transmitted.
		mp1->checksum_out = mp1->checksum_out ^ MP1_PADDING;
		mp1Putbyte(mp1, MP1_PADDING);
	} else {
		// If the length was already even, we
		// just update the checksum with the
		// header byte and queue it.
		mp1->checksum_out = mp1->checksum_out ^ header;
		mp1Putbyte(mp1, header);
	}

	// Now we'll transmit the actual data of
	// the packet. We continously increment the
	// pointer address of the buffer while
	// passing it to the intermediary output
	// function. Everytime the interleaving
	// counter reaches 3, a block will be
	// transmitted.
	while (length--) {
			mp1->checksum_out = mp1->checksum_out ^ *buffer;
			mp1Putbyte(mp1, *buffer++);
	}

	// Finally we write the checksum to the
	// end of the packet.
	mp1Putbyte(mp1, mp1->checksum_out);

	// And transmit a HDLC_FLAG to signify
	// end of the transmission.
	kfile_putc(HDLC_FLAG, mp1->modem);
}

// This function will simply initialize
// the protocol context and allocate the
// needed memory.
void mp1Init(MP1 *mp1, KFile *modem, mp1_callback_t callback) {
	// Allocate memory for our protocol "object"
	memset(mp1, 0, sizeof(*mp1));
	// Set references to our modem "object" and
	// a callback for when a packet has been decoded
	mp1->modem = modem;
	mp1->callback = callback;
}

// A handy debug function that can determine
// how much available memory we have left.
int freeRam(void) {
   extern int __heap_start, *__brkval; 
   int v; 
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

// Following is the functions responsible
// for interleaving and deinterleaving
// blocks of data. The interleaving table
// is also included.

///////////////////////////////
// Interleave-table          //
///////////////////////////////
//
// Non-interleaved:
// aaaaaaaa bbbbbbbb cccccccc
// 12345678 12345678 12345678
// M      L
// S      S
// B      B
//
// Interleaved:
// abcabcab cabcabca bcabcabc
// 11144477 22255578 63336688
// 
//
// 3bit burst error patterns:
// X||||||| X||||||| X|||||||
// |||X|||| X||||||| X|||||||
// |||X|||| |||X|||| X|||||||
// |||X|||| |||X|||| |||X||||
// ||||||X| |||X|||| |||X||||
// ||||||X| ||||||X| |||X||||
// ||||||X| ||||||X| |X||||||
// |X|||||| ||||||X| |X||||||
// |X|||||| |X|||||| |X||||||
// |X|||||| |X|||||| ||||X|||
// ||||X||| |X|||||| ||||X|||
// ||||X||| ||||X||| ||||X|||
// ||||X||| ||||X||| ||||||X|
// |||||||X ||||X||| ||||||X|
// |||||||X |||||X|| ||||||X|
// |||||||X |||||X|| ||X|||||
// ||X||||| |||||X|| ||X|||||
// ||X||||| ||X||||| ||X|||||
// ||X||||| ||X||||| |||||X||
// |||||X|| ||X||||| |||||X||
// |||||X|| |||||||X |||||X||
// |||||X|| |||||||X |||||||X
//
///////////////////////////////

void mp1Interleave(MP1 *mp1, uint8_t byte) {
	mp1->interleaveOut[mp1->interleaveCounter] = byte;
	mp1->interleaveCounter++;
	if (mp1->interleaveCounter == 3) {
		// We have three bytes in the buffer and
		// are ready to interleave them.

		uint8_t a = (GET_BIT(mp1->interleaveOut[0], 1) << 7) +
					(GET_BIT(mp1->interleaveOut[1], 1) << 6) +
					(GET_BIT(mp1->interleaveOut[2], 1) << 5) +
					(GET_BIT(mp1->interleaveOut[0], 4) << 4) +
					(GET_BIT(mp1->interleaveOut[1], 4) << 3) +
					(GET_BIT(mp1->interleaveOut[2], 4) << 2) +
					(GET_BIT(mp1->interleaveOut[0], 7) << 1) +
					(GET_BIT(mp1->interleaveOut[1], 7));
		mp1WriteByte(mp1, a);

		uint8_t b = (GET_BIT(mp1->interleaveOut[2], 2) << 7) +
					(GET_BIT(mp1->interleaveOut[0], 2) << 6) +
					(GET_BIT(mp1->interleaveOut[1], 2) << 5) +
					(GET_BIT(mp1->interleaveOut[2], 5) << 4) +
					(GET_BIT(mp1->interleaveOut[0], 5) << 3) +
					(GET_BIT(mp1->interleaveOut[1], 5) << 2) +
					(GET_BIT(mp1->interleaveOut[2], 7) << 1) +
					(GET_BIT(mp1->interleaveOut[0], 8));
		mp1WriteByte(mp1, b);

		uint8_t c = (GET_BIT(mp1->interleaveOut[1], 6) << 7) +
					(GET_BIT(mp1->interleaveOut[2], 3) << 6) +
					(GET_BIT(mp1->interleaveOut[0], 3) << 5) +
					(GET_BIT(mp1->interleaveOut[1], 3) << 4) +
					(GET_BIT(mp1->interleaveOut[2], 6) << 3) +
					(GET_BIT(mp1->interleaveOut[0], 6) << 2) +
					(GET_BIT(mp1->interleaveOut[1], 8) << 1) +
					(GET_BIT(mp1->interleaveOut[2], 8));

		mp1WriteByte(mp1, c);
		// mp1WriteByte(mp1, a);
		// mp1WriteByte(mp1, b);
		// mp1WriteByte(mp1, c);

		mp1->interleaveCounter = 0;
	}
}


void mp1Deinterleave(MP1 *mp1) {
	uint8_t a = (GET_BIT(mp1->interleaveIn[0], 1) << 7) +
				(GET_BIT(mp1->interleaveIn[1], 2) << 6) +
				(GET_BIT(mp1->interleaveIn[2], 3) << 5) +
				(GET_BIT(mp1->interleaveIn[0], 4) << 4) +
				(GET_BIT(mp1->interleaveIn[1], 5) << 3) +
				(GET_BIT(mp1->interleaveIn[2], 6) << 2) +
				(GET_BIT(mp1->interleaveIn[0], 7) << 1) +
				(GET_BIT(mp1->interleaveIn[1], 8));

	uint8_t b = (GET_BIT(mp1->interleaveIn[0], 2) << 7) +
				(GET_BIT(mp1->interleaveIn[1], 3) << 6) +
				(GET_BIT(mp1->interleaveIn[2], 4) << 5) +
				(GET_BIT(mp1->interleaveIn[0], 5) << 4) +
				(GET_BIT(mp1->interleaveIn[1], 6) << 3) +
				(GET_BIT(mp1->interleaveIn[2], 1) << 2) +
				(GET_BIT(mp1->interleaveIn[0], 8) << 1) +
				(GET_BIT(mp1->interleaveIn[2], 7));

	uint8_t c = (GET_BIT(mp1->interleaveIn[0], 3) << 7) +
				(GET_BIT(mp1->interleaveIn[1], 1) << 6) +
				(GET_BIT(mp1->interleaveIn[2], 2) << 5) +
				(GET_BIT(mp1->interleaveIn[0], 6) << 4) +
				(GET_BIT(mp1->interleaveIn[1], 4) << 3) +
				(GET_BIT(mp1->interleaveIn[2], 5) << 2) +
				(GET_BIT(mp1->interleaveIn[1], 7) << 1) +
				(GET_BIT(mp1->interleaveIn[2], 8));

	mp1->interleaveIn[0] = a;
	mp1->interleaveIn[1] = b;
	mp1->interleaveIn[2] = c;
}

// This function compresses data using
// the Heatshrink library
size_t compress(uint8_t *input, size_t length) {
	heatshrink_encoder *hse = heatshrink_encoder_alloc(8, 4);
	if (hse == NULL) {
		return 0;
	}

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_encoder_sink(hse, input, length, &sunk);
	int status = heatshrink_encoder_finish(hse);

	if (sunk < length) {
		heatshrink_encoder_free(hse);
		return 0;
	} else {
		if (status == HSER_FINISH_MORE) {
			heatshrink_encoder_poll(hse, compressionBuffer, MP1_MAX_FRAME_LENGTH, &written);
		}
	}

	heatshrink_encoder_free(hse);
	return written;
}

// This function decompresses data using
// the Heatshrink library
size_t decompress(uint8_t *input, size_t length) {
	heatshrink_decoder *hsd = heatshrink_decoder_alloc(MP1_MAX_FRAME_LENGTH, 8, 4);
	if (hsd == NULL) {
		return 0;
	}

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_decoder_sink(hsd, input, length, &sunk);
	int status = heatshrink_decoder_finish(hsd);

	if (sunk < length) {
		heatshrink_decoder_free(hsd);
		return 0;
	} else {
		if (status == HSER_FINISH_MORE) {
			heatshrink_decoder_poll(hsd, compressionBuffer, MP1_MAX_FRAME_LENGTH, &written);
		}
	}

	heatshrink_decoder_free(hsd);
	return written;
}
