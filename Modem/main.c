
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

#define TEST_PACKET "Test MP1 AFSK Packet!"

static void mp1Callback(struct MP1Packet *packet) {
	kfile_printf(&ser.fd, "\nMP1 Packet Received:\n");
	kfile_printf(&ser.fd, "%.*s\r\n", packet->dataLength, packet->data);
}

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();

	afsk_init(&afsk, ADC_CH, 0);
	mp1Init(&mp1, &afsk.fd, mp1Callback);

	
	ser_init(&ser, SER_UART0);
	ser_setbaudrate(&ser, 115200);
}

int main(void)
{
	init();
	ticks_t start = timer_clock();

	while (1)
	{
		mp1Poll(&mp1);

		// Periodically send test data
		if (timer_clock() - start > ms_to_ticks(4000L))
		{
			kputs("Test TX\n");
			start = timer_clock();
			mp1Send(&mp1, TEST_PACKET, sizeof(TEST_PACKET));
		}
	}
	return 0;
}