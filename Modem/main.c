
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


///////////////////////// AX25 for testing ////
#include <net/ax25.h>
static AX25Ctx ax25;
static AX25Call path[] = AX25_PATH(AX25_CALL("apzbrt", 0), AX25_CALL("nocall", 0), AX25_CALL("wide1", 1), AX25_CALL("wide2", 2));
#define APRS_MSG    ">Test BeRTOS APRS http://www.bertos.org"
static void message_callback(struct AX25Msg *msg)
{
	kfile_printf(&ser.fd, "\n\nSRC[%.6s-%d], DST[%.6s-%d]\r\n", msg->src.call, msg->src.ssid, msg->dst.call, msg->dst.ssid);

	for (int i = 0; i < msg->rpt_cnt; i++)
		kfile_printf(&ser.fd, "via: [%.6s-%d]\r\n", msg->rpt_lst[i].call, msg->rpt_lst[i].ssid);

	kfile_printf(&ser.fd, "DATA: %.*s\r\n", msg->len, msg->info);
}
///////////////////////////////////////////////

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
	ax25_init(&ax25, &afsk.fd, message_callback);
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
		// Raw read, no protocol
		// if (!fifo_isempty(&afsk.rxFifo)) {
		// 	char c = fifo_pop(&afsk.rxFifo);
		// 	kprintf("%c", c);
		// }

		mp1Poll(&mp1);

		// Use AX.25 to send test data
		if (timer_clock() - start > ms_to_ticks(4000L))
		{
			kputs("Test TX\n");
			start = timer_clock();
			ax25_sendVia(&ax25, path, countof(path), APRS_MSG, sizeof(APRS_MSG));
		}
	}
	return 0;
}