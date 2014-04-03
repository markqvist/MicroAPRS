/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM3S1968 UART interface driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/macros.h> /* for BV() */
#include <drv/gpio_lm3s.h>
#include <drv/ser_p.h>
#include <drv/ser.h>
#include <drv/irq_cm3.h>
#include "cfg/cfg_ser.h"
#include "ser_lm3s.h"

/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

struct CM3Serial
{
	struct SerialHardware hw;
	bool sending;
	uint32_t base;
	sysirq_t irq;
};

/* Forward declaration */
static struct CM3Serial UARTDesc[SER_CNT];

/* GPIO descriptor for UART pins */
struct gpio_uart_info
{
	/* Sysctl */
	uint32_t sysctl;
	/* GPIO base address register */
	uint32_t base;
	/* Pin(s) bitmask */
	uint8_t pins;
};

/* Table to retrieve GPIO pins configuration to work as UART pins */
static const struct gpio_uart_info gpio_uart[SER_CNT] =
{
	/* UART0 */
	{
		.base = GPIO_PORTA_BASE,
		.pins = BV(1) | BV(0),
		.sysctl = SYSCTL_RCGC2_GPIOA,
	},
	/* UART1 */
	{
		.base = GPIO_PORTD_BASE,
		.pins = BV(3) | BV(2),
		.sysctl = SYSCTL_RCGC2_GPIOD,
	},
	/* UART2 */
	{
		.base = GPIO_PORTG_BASE,
		.pins = BV(1) | BV(0),
		.sysctl = SYSCTL_RCGC2_GPIOG,
	},
};

void lm3s_uartSetBaudRate(uint32_t base, unsigned long baud)
{
	unsigned long div;
	bool hi_speed;

	if (baud * 16 > CPU_FREQ)
	{
		hi_speed = true;
		baud /= 2;
	}
	div = (CPU_FREQ * 8 / baud + 1) / 2;

	lm3s_uartDisable(base);
	if (hi_speed)
		HWREG(base + UART_O_CTL) |= UART_CTL_HSE;
	else
		HWREG(base + UART_O_CTL) &= ~UART_CTL_HSE;
	/* Set the baud rate */
	HWREG(base + UART_O_IBRD) = div / 64;
	HWREG(base + UART_O_FBRD) = div % 64;
	lm3s_uartClear(base);
	lm3s_uartEnable(base);
}

void lm3s_uartSetParity(uint32_t base, int parity)
{
	/* Set 8-bit word, one stop bit by default */
	uint32_t config = UART_LCRH_WLEN_8;

	switch(parity)
	{
	case SER_PARITY_NONE:
		break;
	case SER_PARITY_ODD:
		config |= UART_LCRH_PEN;
		break;
	case SER_PARITY_EVEN:
		config |= UART_LCRH_EPS | UART_LCRH_PEN;
		break;
	default:
		ASSERT(0);
		return;
	}
	lm3s_uartDisable(base);
	HWREG(base + UART_O_LCRH) = config;
	lm3s_uartClear(base);
	lm3s_uartEnable(base);
}

void lm3s_uartInit(int port)
{
	uint32_t reg_clock, base;

	ASSERT(port >= 0 && port < SER_CNT);

	base = UARTDesc[port].base;
	reg_clock = 1 << port;

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= reg_clock;
	SYSCTL_RCGC2_R |= gpio_uart[port].sysctl;
	lm3s_busyWait(512);

	/* Configure GPIO pins to work as UART pins */
	lm3s_gpioPinConfig(gpio_uart[port].base, gpio_uart[port].pins,
			GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	/* Set serial param: 115.200 bps, no parity */
	lm3s_uartSetBaudRate(base, 115200);
	lm3s_uartSetParity(base, SER_PARITY_NONE);
}

static bool tx_sending(struct SerialHardware *_hw)
{
	struct CM3Serial *hw = (struct CM3Serial *)_hw;
	return hw->sending;
}

static void uart_irq_rx(int port)
{
	struct FIFOBuffer *rxfifo = &ser_handles[port]->rxfifo;
	uint32_t base = UARTDesc[port].base;
	char c;

	while (lm3s_uartRxReady(base))
	{
		c = HWREG(base + UART_O_DR);
		if (fifo_isfull(rxfifo))
			ser_handles[port]->status |= SERRF_RXFIFOOVERRUN;
		else
			fifo_push(rxfifo, c);
	}
}

static void uart_irq_tx(int port)
{
	struct FIFOBuffer *txfifo = &ser_handles[port]->txfifo;
	uint32_t base = UARTDesc[port].base;

	while (lm3s_uartTxReady(base))
	{
		if (fifo_isempty(txfifo))
		{
			/*
			 * Disable TX empty interrupts if there're no more
			 * characters to transmit.
			 */
			HWREG(base + UART_O_IM) &= ~UART_IM_TXIM;
			UARTDesc[port].sending = false;
			break;
		}
		HWREG(base + UART_O_DR) = fifo_pop(txfifo);
	}
}

static void uart_common_irq_handler(int port)
{
	uint32_t base = UARTDesc[port].base;
	uint32_t status;

	/* Read and clear the IRQ status */
	status = HWREG(base + UART_O_RIS);

	/* Process the IRQ */
	if (status & (UART_RIS_RXRIS | UART_RIS_RTRIS))
		uart_irq_rx(port);
	if (status & UART_RIS_TXRIS)
		uart_irq_tx(port);
}

static void lm3s_uartIRQEnable(int port, sysirq_handler_t handler)
{
	uint32_t base = UARTDesc[port].base;
	sysirq_t irq = UARTDesc[port].irq;

	/* Register the IRQ handler */
	sysirq_setHandler(irq, handler);
	/* Enable RX interrupt in the UART interrupt mask register */
	HWREG(base + UART_O_IM) |= UART_IM_RXIM | UART_IM_RTIM;
}

static void lm3s_uartIRQDisable(int port)
{
	uint32_t base = UARTDesc[port].base;

	HWREG(base + UART_O_IM) &=
			~(UART_IM_TXIM | UART_IM_RXIM | UART_IM_RTIM);
}

/* UART class definition */
#define UART_PORT(port)								\
	/* UART TX and RX buffers */						\
	static unsigned char							\
		uart ## port ## _txbuffer[CONFIG_UART ## port ## _TXBUFSIZE];	\
	static unsigned char							\
		uart ## port ## _rxbuffer[CONFIG_UART ## port ## _RXBUFSIZE];	\
										\
	/* UART interrupt handler */						\
	static DECLARE_ISR(uart ## port ## _irq_handler)			\
	{									\
		uart_common_irq_handler(port);					\
	}									\
										\
	/* UART public methods */						\
	static void								\
	uart ## port ## _txStart(struct SerialHardware *_hw)			\
	{									\
		struct FIFOBuffer *txfifo = &ser_handles[port]->txfifo;		\
		struct CM3Serial *hw = (struct CM3Serial *)_hw;			\
										\
                if (hw->sending)						\
                        return;							\
		lm3s_uartPutChar(UART ## port ## _BASE, fifo_pop(txfifo));	\
		if (!fifo_isempty(txfifo))					\
		{								\
			HWREG(UART ## port ## _BASE + UART_O_IM) |=		\
						 UART_IM_TXIM;			\
			hw->sending = true;					\
		}								\
	}									\
										\
	static void								\
	uart ## port ## _setbaudrate(UNUSED_ARG(struct SerialHardware *, hw),	\
						unsigned long baud)		\
	{									\
		lm3s_uartSetBaudRate(UART ## port ## _BASE, baud);		\
	}									\
										\
	static void								\
	uart ## port ## _setparity(UNUSED_ARG(struct SerialHardware *, hw),	\
						int parity)			\
	{									\
		lm3s_uartSetParity(UART ## port ## _BASE, parity);		\
	}									\
										\
	static void								\
	uart ## port ## _cleanup(struct SerialHardware *_hw)			\
	{									\
		struct CM3Serial *hw = (struct CM3Serial *)_hw;			\
										\
		hw->sending = false;						\
		lm3s_uartIRQDisable(port);					\
		lm3s_uartClear(UART ## port ## _BASE);				\
		lm3s_uartDisable(UART ## port ## _BASE);			\
	}									\
										\
	static void								\
	uart ## port ## _init(UNUSED_ARG(struct SerialHardware *, hw),		\
				UNUSED_ARG(struct Serial *, ser))		\
	{									\
		lm3s_uartInit(port);						\
		lm3s_uartEnable(UART ## port ## _BASE);				\
		lm3s_uartIRQEnable(port, uart ## port ## _irq_handler);		\
	}									\
										\
	/* UART operations */							\
	static const struct SerialHardwareVT UART ## port ## _VT =		\
	{									\
		.init = uart ## port ## _init,					\
		.cleanup = uart ## port ## _cleanup,				\
		.setBaudrate = uart ## port ## _setbaudrate,			\
		.setParity = uart ## port ## _setparity,			\
		.txStart = uart ## port ## _txStart,				\
		.txSending = tx_sending,					\
	};

/* UART port instances */
UART_PORT(0)
UART_PORT(1)
UART_PORT(2)

static struct CM3Serial UARTDesc[SER_CNT] =
{
	{
		.hw = {
			.table = &UART0_VT,
			.txbuffer = uart0_txbuffer,
			.rxbuffer = uart0_rxbuffer,
			.txbuffer_size = sizeof(uart0_txbuffer),
			.rxbuffer_size = sizeof(uart0_rxbuffer),
		},
		.sending = false,
		.base = UART0_BASE,
		.irq = INT_UART0,
	},
	{
		.hw = {
			.table = &UART1_VT,
			.txbuffer = uart1_txbuffer,
			.rxbuffer = uart1_rxbuffer,
			.txbuffer_size = sizeof(uart1_txbuffer),
			.rxbuffer_size = sizeof(uart1_rxbuffer),
		},
		.sending = false,
		.base = UART1_BASE,
		.irq = INT_UART1,
	},
	{
		.hw = {
			.table = &UART2_VT,
			.txbuffer = uart2_txbuffer,
			.rxbuffer = uart2_rxbuffer,
			.txbuffer_size = sizeof(uart2_txbuffer),
			.rxbuffer_size = sizeof(uart2_rxbuffer),
		},
		.sending = false,
		.base = UART2_BASE,
		.irq = INT_UART2,
	},
};

struct SerialHardware *ser_hw_getdesc(int port)
{
	ASSERT(port >= 0 && port < SER_CNT);
	return &UARTDesc[port].hw;
}
