
#include <cpu/irq.h>
#include <cfg/debug.h>

#include "afsk.h"

#include <drv/ser.h>
#include <drv/timer.h>

#include <stdio.h>
#include <string.h>

static Afsk afsk;
static Serial ser;

#define ADC_CH 0

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();

	afsk_init(&afsk, ADC_CH, 0);
	
	ser_init(&ser, SER_UART0);
	ser_setbaudrate(&ser, 115200);
}

int main(void)
{
	init();
	//ticks_t start = timer_clock();

	while (1)
	{
		if (!fifo_isempty(&afsk.rx_fifo)) {
			char c = fifo_pop(&afsk.rx_fifo);
			kprintf("%c", c);
		}
	}
	return 0;
}