
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
#define TEST_PACKET "Test MP1 AFSK Packet. Test123"


static uint8_t serialBuffer[MP1_MAX_FRAME_LENGTH];	// This is a buffer for incoming serial data
static int sbyte;									// For holding byte read from serial port
static int serialLen = 0;							// Counter for counting length of data from serial
static bool sertx = false;							// Flag signifying whether it's time to send data
													// Received on the serial port.


//////////////////////////////////////////////////////
// And here comes the actual program :)             //
//////////////////////////////////////////////////////

// This is a callback we register with the protocol,
// so we can process each packet as they are decoded.
// Right now it just prints the packet to the serial port.
static void mp1Callback(struct MP1Packet *packet) {
	//kfile_printf(&ser.fd, "%.*s\r\n", packet->dataLength, packet->data);
	kprintf("%.*s\r\n", packet->dataLength, packet->data);
}

// Simple initialization function.
static void init(void)
{
	// Enable interrupts
	IRQ_ENABLE;
	// Initialize BertOS debug bridge
	kdbg_init();
    kprintf("Init\n");

    // Initialize hardware timers
	timer_init();

	// Initialize serial comms on UART0,
	// which is the hardware serial on arduino
	ser_init(&ser, SER_UART0);
	ser_setbaudrate(&ser, 57600);

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
				kprintf("%d\n", sbyte);
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
			mp1Send(&mp1, serialBuffer, serialLen);
			// Reset the transmission flag and length counter
			sertx = false;
			serialLen = 0;
		}

		// Periodically send test data if we should do so
		if (TEST_TX && timer_clock() - start > ms_to_ticks(4000L)) {
			// Reset the timer counter;
			start = timer_clock();
			// And send a test packet!
			mp1Send(&mp1, TEST_PACKET, sizeof(TEST_PACKET));
		}
	}
	return 0;
}