#ifndef PROTOCOL_MP1
#define PROTOCOL_MP1

#include <cfg/compiler.h>
#include <io/kfile.h>

// Frame sizing
#define MP1_MIN_FRAME_LENGTH 1
#define MP1_MAX_FRAME_LENGTH 300

// We need to know some basic HDLC flag bytes
#define HDLC_FLAG  0x7E
#define HDLC_RESET 0x7F
#define AX25_ESC   0x1B

// Just a forward declaration that this struct exists
struct MP1Packet;

// The type of a callback function for passing
// back a decoded packet
typedef void (*mp1_callback_t)(struct MP1Packet *packet);

// Struct for a protocol context
typedef struct MP1 {
	uint8_t buffer[MP1_MAX_FRAME_LENGTH];	// A buffer for incoming packets
	KFile *modem;							// KFile access to the modem
	size_t packetLength;					// Counter for received packet length
	mp1_callback_t callback;				// The function to call when a packet has been received
	bool reading;							// True when we have seen a HDLC flag
	bool escape;							// We need to know if we are in an escape sequence
} MP1;

// A struct encapsulating a network packet
typedef struct MP1Packet {
	const uint8_t *data;					// Pointer to the actual data in the packet
	size_t dataLength;						// The length of the received data
} MP1Packet;

void mp1Init(MP1 *mp1, KFile *modem, mp1_callback_t callback);
void mp1Poll(MP1 *mp1);
void mp1Send(MP1 *mp1, const void *_buffer, size_t length);

#endif