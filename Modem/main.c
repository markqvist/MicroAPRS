
#include <cpu/irq.h>
#include <cfg/debug.h>

#include "afsk.h"			// Header for AFSK modem
#include "protocol/mp1.h"	// Header for MP.1 protocol

#include <drv/ser.h>
#include <drv/timer.h>

#include <stdio.h>
#include <string.h>

static Afsk afsk;
static Serial ser;

static MP1 mp1;

#define ADC_CH 0

#define TEST_PACKET "Test MP1 AFSK Packet! This one is longer and probably more prone to errors..."

static uint8_t serialBuffer[MP1_MAX_FRAME_LENGTH];
static int sbyte;
static uint8_t serialLen = 0;
static bool sertx = false;

static void mp1Callback(struct MP1Packet *packet) {
	kfile_printf(&ser.fd, "%.*s\r\n", packet->dataLength, packet->data);
}

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
    kprintf("Init\n");

	timer_init();

	afsk_init(&afsk, ADC_CH, 0);
	mp1Init(&mp1, &afsk.fd, mp1Callback);

	ser_init(&ser, SER_UART0);
	ser_setbaudrate(&ser, 57600);
	//ser_settimeouts(&ser, 0, 0);
}

int main(void)
{
	init();
	ticks_t start = timer_clock();
	
	while (1)
	{
		mp1Poll(&mp1);

		sbyte = ser_getchar_nowait(&ser);
		if (sbyte != EOF) {
			if (serialLen < MP1_MAX_FRAME_LENGTH && sbyte != 138) {
				serialBuffer[serialLen] = sbyte;
				serialLen++;
			} else {
				sertx = true;
			}
		}

		if (sertx) {
			mp1Send(&mp1, serialBuffer, serialLen);
			sertx = false;
			serialLen = 0;
		}

		// Periodically send test data
		if (false && timer_clock() - start > ms_to_ticks(4000L))
		{
			start = timer_clock();
			mp1Send(&mp1, TEST_PACKET, sizeof(TEST_PACKET));
		}
	}
	return 0;
}