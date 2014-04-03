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
 * \brief STM32 UART interface driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "ser_stm32.h"

#include "cfg/cfg_ser.h"

#include <cfg/macros.h> /* for BV() */
#include <cfg/debug.h>

#include <drv/gpio_stm32.h>
#include <drv/irq_cm3.h>
#include <drv/clock_stm32.h>
#include <drv/ser_p.h>
#include <drv/ser.h>


/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

struct CM3Serial
{
	struct SerialHardware hw;
	volatile bool sending;
	uint32_t base;
	sysirq_t irq;
};

/* Forward declaration */
static struct CM3Serial UARTDesc[SER_CNT];

/* GPIO descriptor for UART pins */
struct gpio_uart_info
{
	/* GPIO base address register */
	uint32_t base;
	/* Pin(s) bitmask */
	uint32_t rx_pin;
	uint32_t tx_pin;
	/* Sysctl */
	uint32_t sysctl_gpio;
	uint32_t sysctl_usart;

};

/* Table to retrieve GPIO pins configuration to work as UART pins */
static const struct gpio_uart_info gpio_uart[SER_CNT] =
{
	/* UART1 */
	{
		.base = GPIOA_BASE,
		.rx_pin = GPIO_USART1_RX_PIN,
		.tx_pin = GPIO_USART1_TX_PIN,
		.sysctl_gpio = RCC_APB2_GPIOA,
		.sysctl_usart = RCC_APB2_USART1,
	},
	/* UART2 */
	{
		.base = GPIOA_BASE,
		.rx_pin = GPIO_USART2_RX_PIN,
		.tx_pin = GPIO_USART2_TX_PIN,
		.sysctl_gpio = RCC_APB2_GPIOA,
		.sysctl_usart = RCC_APB1_USART2,
	},
#if CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE
	/* UART3 */
	{
		.base = GPIOB_BASE,
		.rx_pin = GPIO_USART3_RX_PIN,
		.tx_pin = GPIO_USART3_TX_PIN,
		.sysctl_gpio = RCC_APB2_GPIOB,
		.sysctl_usart = RCC_APB1_USART3,
	},
#endif
};

#define USART1_PORT                0
#define USART2_PORT                1
#define USART3_PORT                2

void stm32_uartSetBaudRate(uint32_t base, unsigned long baud)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	_base->BRR = evaluate_brr(_base, CPU_FREQ, baud);
}

void stm32_uartSetParity(uint32_t base, int parity)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;

	/*  USART_WORD_LEN_8B */
	_base->CR1 &= ~BV(CR1_M);

	switch(parity)
	{
	case SER_PARITY_NONE:
		_base->CR1 &= ~BV(CR1_PCE);
		break;
	case SER_PARITY_ODD:
		_base->CR1 |= (BV(CR1_PCE) | BV(CR1_PS));
		break;
	case SER_PARITY_EVEN:
		_base->CR1 |= BV(CR1_PCE);
		_base->CR1 &= ~BV(CR1_PS);
		break;
	default:
		ASSERT(0);
		return;
	}
}

void stm32_uartInit(int port)
{
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;

	ASSERT(port >= 0 && port < SER_CNT);

	/* Enable clocking on AFIO */
	RCC->APB2ENR |= RCC_APB2_AFIO;
	RCC->APB2ENR |= gpio_uart[port].sysctl_gpio;

	/* Configure USART pins */
	if (port == USART1_PORT)
	{
		RCC->APB2ENR |=  gpio_uart[port].sysctl_usart;
	}
	else
	{
		RCC->APB1ENR |=  gpio_uart[port].sysctl_usart;
	}

	stm32_gpioPinConfig((struct stm32_gpio *)gpio_uart[port].base,  gpio_uart[port].tx_pin,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);

	stm32_gpioPinConfig((struct stm32_gpio *)gpio_uart[port].base,  gpio_uart[port].rx_pin,
				GPIO_MODE_IN_FLOATING, GPIO_SPEED_50MHZ);

	/* Clear control registry */
	base->CR2 = 0;
	base->CR1 = 0;
	base->CR3 = 0;
	base->SR = 0;

	/* Set serial param: 115.200 bps, no parity */
	stm32_uartSetBaudRate(UARTDesc[port].base, 115200);
	stm32_uartSetParity(UARTDesc[port].base, SER_PARITY_NONE);


	/* Enable trasmision and receiver */
	base->CR1 |= (BV(CR1_TE) | BV(CR1_RE));

}

static bool tx_sending(struct SerialHardware *_hw)
{
	struct CM3Serial *hw = (struct CM3Serial *)_hw;
	return hw->sending;
}

static void uart_irq_rx(int port)
{
	struct FIFOBuffer *rxfifo = &ser_handles[port]->rxfifo;
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;
	char c;

	while (stm32_uartRxReady(UARTDesc[port].base))
	{
		c = base->DR;
		if (fifo_isfull(rxfifo))
			ser_handles[port]->status |= SERRF_RXFIFOOVERRUN;
		else
			fifo_push(rxfifo, c);
	}
}

static void uart_irq_tx(int port)
{
	struct FIFOBuffer *txfifo = &ser_handles[port]->txfifo;
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;

	if (fifo_isempty(txfifo))
	{
		/*
		 * Disable TX empty interrupts if there're no more
		 * characters to transmit.
		 */
		base->CR1 &= ~BV(CR1_TXEIE);
		UARTDesc[port].sending = false;
	}
	else
	{
		base->DR = fifo_pop(txfifo);
	}
}

static void uart_common_irq_handler(int port)
{
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;
	uint32_t status;

	/* Read and clear the IRQ status */
	status = base->SR;

	/* Check hw errors */
	ser_handles[port]->status = status &
		(BV(SR_ORE) | BV(SR_FE) | BV(SR_PE) | BV(SR_NE));

	/* Process the IRQ */
	if (status & BV(CR1_RXNEIE))
	{
		uart_irq_rx(port);
	}
	if (status & (BV(CR1_TXEIE) | BV(CR1_TCIE)))
	{
		uart_irq_tx(port);
	}
}

static void stm32_uartIRQEnable(int port, sysirq_handler_t handler)
{
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;

	/* Register the IRQ handler */
	sysirq_setHandler(UARTDesc[port].irq, handler);

	base->CR1 |= BV(CR1_RXNEIE);
}

static void stm32_uartIRQDisable(int port)
{
	struct stm32_usart *base = (struct stm32_usart *)UARTDesc[port].base;

	base->CR1 &= ~(BV(CR1_RXNEIE) | USART_FLAG_TXE);
}


/* UART class definition */
#define UART_PORT(port)			 \
	/* UART TX and RX buffers */ \
	static unsigned char uart ## port ## _txbuffer[CONFIG_UART ## port ## _TXBUFSIZE]; \
	static unsigned char uart ## port ## _rxbuffer[CONFIG_UART ## port ## _RXBUFSIZE];	\
										\
	/* UART interrupt handler */						\
	static DECLARE_ISR(uart ## port ## _irq_handler)	\
	{									\
		uart_common_irq_handler(USART ## port ## _PORT);	\
	}									\
										\
	/* UART public methods */ \
	static void	uart ## port ## _txStart(struct SerialHardware *_hw) \
	{																 \
		struct FIFOBuffer *txfifo = &ser_handles[USART ## port ## _PORT]->txfifo;		 \
		struct CM3Serial *hw = (struct CM3Serial *)_hw;			     \
		struct stm32_usart *base = (struct stm32_usart *)USART## port ## _BASE; \
		if (hw->sending)						\
			return;							\
		stm32_uartPutChar(USART ## port ## _BASE, fifo_pop(txfifo));	\
		if (!fifo_isempty(txfifo))					\
		{								\
			hw->sending = true;	 \
			base->CR1 |= BV(CR1_TXEIE); \
		} \
	}									\
										\
	static void	uart ## port ## _setbaudrate(UNUSED_ARG(struct SerialHardware *, hw), \
						unsigned long baud)		\
	{									\
		stm32_uartSetBaudRate(USART## port ## _BASE, baud);		\
	}									\
										\
	static void	uart ## port ## _setparity(UNUSED_ARG(struct SerialHardware *, hw),	\
						int parity)			\
	{									\
		stm32_uartSetParity(USART## port ## _BASE, parity);		\
	}									\
										\
	static void	uart ## port ## _cleanup(struct SerialHardware *_hw)			\
	{									\
		struct CM3Serial *hw = (struct CM3Serial *)_hw;			\
		hw->sending = false; \
		stm32_uartIRQDisable(USART ## port ## _PORT);					\
		stm32_uartClear(USART## port ## _BASE);				\
		stm32_uartDisable(USART## port ## _BASE);			\
	}									\
										\
	static void	uart ## port ## _init(UNUSED_ARG(struct SerialHardware *, hw),		\
				UNUSED_ARG(struct Serial *, ser))		\
	{									\
		stm32_uartInit(USART ## port ## _PORT);						\
		stm32_uartEnable(USART## port ## _BASE);				\
		stm32_uartIRQEnable(USART ## port ## _PORT, uart ## port ## _irq_handler);		\
	}									\
										\
	/* UART operations */							\
	static const struct SerialHardwareVT USART ## port ## _VT =		\
	{									\
		.init = uart ## port ## _init,					\
		.cleanup = uart ## port ## _cleanup,				\
		.setBaudrate = uart ## port ## _setbaudrate,			\
		.setParity = uart ## port ## _setparity,			\
		.txStart = uart ## port ## _txStart,				\
		.txSending = tx_sending,					\
	};

/* UART port instances */
UART_PORT(1)
UART_PORT(2)
#if CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE
UART_PORT(3)
#endif

static struct CM3Serial UARTDesc[SER_CNT] =
{
	{
		.hw = {
			.table = &USART1_VT,
			.txbuffer = uart1_txbuffer,
			.rxbuffer = uart1_rxbuffer,
			.txbuffer_size = sizeof(uart1_txbuffer),
			.rxbuffer_size = sizeof(uart1_rxbuffer),
		},
		.sending = false,
		.base = USART1_BASE,
		.irq = USART1_IRQHANDLER,
	},
	{
		.hw = {
			.table = &USART2_VT,
			.txbuffer = uart2_txbuffer,
			.rxbuffer = uart2_rxbuffer,
			.txbuffer_size = sizeof(uart2_txbuffer),
			.rxbuffer_size = sizeof(uart2_rxbuffer),
		},
		.sending = false,
		.base = USART2_BASE,
		.irq = USART2_IRQHANDLER,
	},
#if CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE
	{
		.hw = {
			.table = &USART3_VT,
			.txbuffer = uart3_txbuffer,
			.rxbuffer = uart3_rxbuffer,
			.txbuffer_size = sizeof(uart3_txbuffer),
			.rxbuffer_size = sizeof(uart3_rxbuffer),
		},
		.sending = false,
		.base = USART3_BASE,
		.irq = USART3_IRQHANDLER,
	},
#endif
};

struct SerialHardware *ser_hw_getdesc(int port)
{
	ASSERT(port >= 0 && port < SER_CNT);
	return &UARTDesc[port].hw;
}
