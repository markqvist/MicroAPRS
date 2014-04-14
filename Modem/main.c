
//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#include <cpu/irq.h>		// Interrupt functionality from BertOS
#include "cfg/debug.h"		// Debug configuration from BertOS

#include <drv/ser.h>		// Serial driver from BertOS
#include <drv/timer.h>		// Timer driver from BertOS

#include <stdio.h>			// Standard input/output
#include <string.h>			// String operations

#include "afsk.h"			// Header for AFSK modem
#include "protocol/mp1.h"	// Header for MP.1 protocol

#include "compression/heatshrink_encoder.h"
#include "compression/heatshrink_decoder.h"

//////////////////////////////////////////////////////
// A few definitions				                //
//////////////////////////////////////////////////////

static Afsk afsk;			// Declare a AFSK modem struct
static MP1 mp1;				// Declare a protocol struct
static Serial ser;			// Declare a serial interface struct

#define ADC_CH 0			// Define which channel (pin) we want
							// for the ADC (this is A0 on arduino)

#define TEST_TX false		// Whether we should send test packets
							// periodically, plus what to send:
#define TEST_PACKET "Test MP1 AFSK Packet. Test123."
#define TEST_TX_INTERVAL 10000L


static uint8_t serialBuffer[MP1_MAX_FRAME_LENGTH];	// This is a buffer for incoming serial data
static int sbyte;									// For holding byte read from serial port
static int serialLen = 0;							// Counter for counting length of data from serial
static bool sertx = false;							// Flag signifying whether it's time to send data
													// Received on the serial port.

static uint8_t compressedData[MP1_MAX_FRAME_LENGTH];
static uint8_t decompressedData[MP1_MAX_FRAME_LENGTH];


static int freeRam () {
   extern int __heap_start, *__brkval; 
   int v; 
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

static size_t compress(uint8_t *input, size_t length) {
	heatshrink_encoder *hse = heatshrink_encoder_alloc(8, 4);
	if (hse == NULL) {
		kprintf("Could not allocate encoder");
		return 0;
	}

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_encoder_sink(hse, input, length, &sunk);
	int status = heatshrink_encoder_finish(hse);

	if (sunk < length) {
		kprintf("Not all data was sunk into encoder\n");
		heatshrink_encoder_free(hse);
		return 0;
	} else {
		kprintf("Bytes sunk into HSE: %d\n", length);
		if (status == HSER_FINISH_MORE) {
			heatshrink_encoder_poll(hse, compressedData, MP1_MAX_FRAME_LENGTH, &written);
			kprintf("Bytes written into buffer: %d\n", written);
		} else {
			kprintf("All input data was sunk, but encoder doesn't have any data for us.");
		}
	}

	heatshrink_encoder_free(hse);
	return written;
}

static size_t decompress(uint8_t *input, size_t length) {
	heatshrink_decoder *hsd = heatshrink_decoder_alloc(MP1_MAX_FRAME_LENGTH, 8, 4);
	if (hsd == NULL) {
		kprintf("Could not allocate decoder");
		return 0;
	}

	kprintf("\nDecoder allocated. Free RAM: %d bytes\n", freeRam());

	size_t written = 0;
	size_t sunk = 0;
	heatshrink_decoder_sink(hsd, input, length, &sunk);
	int status = heatshrink_decoder_finish(hsd);

	if (sunk < length) {
		kprintf("Not all data was sunk into decoder\n");
		heatshrink_decoder_free(hsd);
		return 0;
	} else {
		kprintf("Bytes sunk into HSD: %d\n", length);
		if (status == HSER_FINISH_MORE) {
			heatshrink_decoder_poll(hsd, decompressedData, MP1_MAX_FRAME_LENGTH, &written);
			kprintf("Bytes written into decompression buffer: %d\n", written);
		} else {
			kprintf("All input data was sunk, but the decoder doesn't have any data for us.");
		}
	}

	heatshrink_decoder_free(hsd);
	return written;
}

static void hseTest() { 
	kprintf("\nFree RAM: %d bytes\n", freeRam());
	size_t compressed_size = compress(serialBuffer, serialLen);
	size_t decompressed_size = decompress(compressedData, compressed_size);
	kprintf("\n-------------------\nInput size: %d\nCompressed size: %d\nDecompressed size: %d\n", serialLen, compressed_size, decompressed_size);

	// heatshrink_encoder *hse = heatshrink_encoder_alloc(8, 4);
	// kprintf("\nFree RAM: %d bytes\n", freeRam());
	
	// size_t out_sz = 50;
	// uint8_t out_buf[out_sz];
	// size_t written = 0;
	// kprintf("\nFree RAM: %d bytes\n", freeRam());

	// size_t length = serialLen;

	// heatshrink_encoder_sink(hse, serialBuffer, serialLen, &length);

	

	// int returnv = heatshrink_encoder_finish(hse);
	// kprintf("Encoder finish returned: %d\n", returnv);

	// if (length < serialLen) {
	// 	kprintf("Not all data was sunk into encoder\n");
	// } else {
	// 	// All data delivered
	// 	kprintf("Bytes sunk into HSE: %d\n", length);

	// 	heatshrink_encoder_poll(hse, out_buf, out_sz, &written);

	// 	kprintf("2: Bytes written into buffer: %d\n", written);
	// }

	// heatshrink_encoder_free(hse);
}

//////////////////////////////////////////////////////
// And here comes the actual program :)             //
//////////////////////////////////////////////////////

// This is a callback we register with the protocol,
// so we can process each packet as they are decoded.
// Right now it just prints the packet to the serial port.
static void mp1Callback(struct MP1Packet *packet) {
	kfile_printf(&ser.fd, "%.*s\n", packet->dataLength, packet->data);
	//kprintf("%.*s\n", packet->dataLength, packet->data);
}

// Simple initialization function.
static void init(void)
{
	// Enable interrupts
	IRQ_ENABLE;
	// Initialize BertOS debug bridge
	// kdbg_init();

    // Initialize hardware timers
	timer_init();

	// Initialize serial comms on UART0,
	// which is the hardware serial on arduino
	ser_init(&ser, SER_UART0);
	ser_setbaudrate(&ser, 115200);

	// Create a modem context
	afsk_init(&afsk, ADC_CH);
	// ... and a protocol context with the modem
	mp1Init(&mp1, &afsk.fd, mp1Callback);

	// That's all!
}

int main(void)
{
	// Start by running the main initialization
	init();
	// Record the current tick count for time-keeping
	ticks_t start = timer_clock();
	
	// Go into ye good ol' infinite loop
	while (1)
	{	
		// First we instruct the protocol to check for
		// incoming data
		mp1Poll(&mp1);

		
		// We then read a byte from the serial port.
		// Notice that we use "_nowait" since we can't
		// have this blocking execution until a byte
		// comes in.
		sbyte = ser_getchar_nowait(&ser);
		// If there was actually some data waiting for us
		// there, let's se what it tastes like :)
		if (sbyte != EOF) {
			// If we have not yet surpassed the maximum frame length
			// and the byte is not a "transmit" (newline) character,
			// we should store it for transmission.
			if ((serialLen < MP1_MAX_FRAME_LENGTH) && (sbyte != 138)) {
				// Put the read byte into the buffer;
				serialBuffer[serialLen] = sbyte;
				// Increment the read length counter
				serialLen++;
			} else {
				// If one of the above conditions were actually the
				// case, it means we have to transmit, se we set
				// transmission flag to true.
				sertx = true;
			}
		}

		// Check whether we should send data in our serial buffer
		if (sertx) {
			// If we should, pass the buffer to the protocol's
			// send function.
			
			hseTest();
			// mp1Send(&mp1, serialBuffer, serialLen);

			// Reset the transmission flag and length counter
			sertx = false;
			serialLen = 0;
		}

		// Periodically send test data if we should do so
		if (TEST_TX && timer_clock() - start > ms_to_ticks(TEST_TX_INTERVAL)) {
			// Reset the timer counter;
			start = timer_clock();
			// And send a test packet!
			mp1Send(&mp1, TEST_PACKET, sizeof(TEST_PACKET));
		}
	}
	return 0;
}