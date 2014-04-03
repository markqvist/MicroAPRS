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
 * \brief LPC23xx UART driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * notest:arm
 */

#include "ser_lpc2.h"

#include "cfg/cfg_ser.h"

#include <cfg/macros.h> /* for BV() */

#include <cpu/power.h> /* cpu_relax() */

#include <drv/ser_p.h>
#include <drv/ser.h>
#include <drv/vic_lpc2.h> /* vic_handler_t */

#include <io/lpc23xx.h>


/* Register offsets */
#define RBR	0x00
#define THR	0x00
#define DLL	0x00
#define DLM	0x04
#define IER	0x04
#define IIR	0x08
#define FCR	0x08
#define LCR	0x0c
#define LSR	0x14
#define SCR	0x1c
#define ACR	0x20
#define ICR	0x24
#define FDR	0x28
#define TER	0x30

/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

struct LPC2Serial
{
	struct SerialHardware hw;
	bool sending;
	int irq;
};

/* Forward declaration */
static struct LPC2Serial UARTDesc[SER_CNT];

struct uart_config
{
	uint32_t base;
	uint32_t pconp;
	uint32_t pclksel0;
	uint32_t pclksel0_mask;
	uint32_t pclksel1;
	uint32_t pclksel1_mask;
	uint32_t pinsel0;
	uint32_t pinsel0_mask;
	uint32_t pinsel4;
	uint32_t pinsel4_mask;
};

/* UART registers configuration */
static const struct uart_config uart_param[] =
{
	/* UART0 */
	{
		.base = UART0_BASE_ADDR,
		.pconp = BV(3),

		.pclksel0 = BV(6),
		.pclksel0_mask = BV(7) | BV(6),

		.pclksel1 = 0,
		.pclksel1_mask = 0,

		.pinsel0 = BV(6) | BV(4),
		.pinsel0_mask = BV(7) | BV(6) | BV(5) | BV(4),

		.pinsel4 = 0,
		.pinsel4_mask = 0,
	},
	/* UART1 */
	{
		.base = UART1_BASE_ADDR,
		.pconp = BV(4),

		.pclksel0 = BV(8),
		.pclksel0_mask = BV(9) | BV(8),

		.pclksel1 = 0,
		.pclksel1_mask = 0,

		.pinsel0 = 0,
		.pinsel0_mask = 0,

		.pinsel4 = BV(3) | BV(1),
		.pinsel4_mask = BV(3) | BV(2) | BV(1) | BV(0),
	},
	/* UART2 */
	{
		.base = UART2_BASE_ADDR,
		.pconp = BV(24),

		.pclksel0 = 0,
		.pclksel0_mask = 0,

		.pclksel1 = BV(16),
		.pclksel1_mask = BV(17) | BV(16),

		.pinsel0 = 0,
		.pinsel0_mask = 0,

		.pinsel4 = BV(19) | BV(17),
		.pinsel4_mask = BV(19) | BV(18) | BV(17) | BV(16),
	},
	/* UART3 */
	{
		.base = UART3_BASE_ADDR,
		.pconp = BV(25),

		.pclksel0 = 0,
		.pclksel0_mask = 0,

		.pclksel1 = BV(18),
		.pclksel1_mask = BV(19) | BV(18),

		.pinsel0 = BV(3) | BV(1),
		.pinsel0_mask = BV(3) | BV(2) | BV(1) | BV(0),

		.pinsel4 = 0,
		.pinsel4_mask = 0,
	},
};

static void lpc2_uartSetBaudRate(int port, unsigned long baud)
{
	cpu_flags_t flags;

	IRQ_SAVE_DISABLE(flags);

	/* LCR: DLAB = 1 (enable divisor modify) */
	*(reg8_t *)(uart_param[port].base + LCR) |= 0x80;
	/* DLL */
	*(reg8_t *)(uart_param[port].base + DLL) =
		DIV_ROUND(CPU_FREQ, 16 * baud) & 0xFF;
	/* DLM */
	*(reg8_t *)(uart_param[port].base + DLM) =
		(DIV_ROUND(CPU_FREQ, 16 * baud) >> 8) & 0xFF;
	*(reg32_t *)(uart_param[port].base + LCR) &= ~0x80;
	/* LCR: DLAB = 0 (disable divisor modify) */
	*(reg8_t *)(uart_param[port].base + LCR) &= ~0x80;

	IRQ_RESTORE(flags);
}

static void lpc2_uartSetParity(int port, int parity)
{
        /* Set 8-bit word, one stop bit by default */
        uint32_t config = BV(1) | BV(0);

	cpu_flags_t flags;

	IRQ_SAVE_DISABLE(flags);

        switch(parity)
        {
        case SER_PARITY_NONE:
                break;
        case SER_PARITY_ODD:
                config |= BV(3);
                break;
        case SER_PARITY_EVEN:
                config |= BV(4) | BV(3);
                break;
        default:
                ASSERT(0);
		IRQ_RESTORE(flags);
                return;
        }
	/* LCR */
	*(reg8_t *)(uart_param[port].base + LCR) = config;

	IRQ_RESTORE(flags);
}

static void lpc2_uartPutChar(uint32_t base, uint8_t c)
{
	reg8_t *lsr = (reg8_t *)base + LSR;
	reg8_t *thr = (reg8_t *)base + THR;

	while (!(*lsr & BV(6)))
		cpu_relax();
	*thr = c;
}

void lpc2_uartInit(int port)
{
	cpu_flags_t flags;

	IRQ_SAVE_DISABLE(flags);

	/* Power-on the device */
	PCONP |= uart_param[port].pconp;
	/* Set UART clk to CPU_FREQ */
	PCLKSEL0 &= ~uart_param[port].pclksel0_mask;
	PCLKSEL0 |= uart_param[port].pclksel0;
	PCLKSEL1 &= ~uart_param[port].pclksel1_mask;
	PCLKSEL1 |= uart_param[port].pclksel1;

	/* LCR: 8bit, 1 stop bit, no parity, DLAB = 1 (enable divisor modify) */
	*(reg8_t *)(uart_param[port].base + LCR) = 0x83;
	/* DLL */
	*(reg8_t *)(uart_param[port].base + DLL) =
		DIV_ROUND(CPU_FREQ, 16 * CONFIG_KDEBUG_BAUDRATE) & 0xFF;
	/* DLM */
	*(reg8_t *)(uart_param[port].base + DLM) =
		(DIV_ROUND(CPU_FREQ, 16 * CONFIG_KDEBUG_BAUDRATE) >> 8) & 0xFF;
	/* FDR */
	*(reg32_t *)(uart_param[port].base + FDR) = 0x10;

	/* Assign TX pin to UART0*/
	PINSEL0 &= ~uart_param[port].pinsel0_mask;
	PINSEL0 |= uart_param[port].pinsel0;
	PINSEL4 &= ~uart_param[port].pinsel4_mask;
	PINSEL4 |= uart_param[port].pinsel4;
	/* LCR: set 8bit, 1 stop bit, no parity, DLAB = 0 (disable divisor modify) */
	*(reg8_t *)(uart_param[port].base + LCR) = 0x03;

	/* TER: Enable transmitter */
	*(reg8_t *)(uart_param[port].base + TER) = BV(7);
	/* IER: Enable RBR interrupt */
	*(reg8_t *)(uart_param[port].base + IER) = BV(0);

	IRQ_RESTORE(flags);
}

static bool tx_sending(struct SerialHardware *_hw)
{
	struct LPC2Serial *hw = (struct LPC2Serial *)_hw;
	return hw->sending;
}

INLINE bool lpc2_uartRxReady(int port)
{
	/* LSR: check Receiver Data Ready (RDR) bit */
	return *(reg8_t *)(uart_param[port].base + LSR) & BV(0) ? true : false;
}

static void uart_irq_rx(int port)
{
	struct FIFOBuffer *rxfifo = &ser_handles[port]->rxfifo;
	char c;

	while (lpc2_uartRxReady(port))
	{
		/* RBR: read a character from the Receiver Buffer Register */
		c = *(reg8_t *)(uart_param[port].base + RBR);
		if (fifo_isfull(rxfifo))
			ser_handles[port]->status |= SERRF_RXFIFOOVERRUN;
		else
			fifo_push(rxfifo, c);
	}
}

INLINE bool lpc2_uartTxReady(int port)
{
	/* LSR: check Transmitter Holding Register Empty (THRE) bit */
	return *(reg8_t *)(uart_param[port].base + LSR) & BV(5) ? true : false;
}

static void uart_irq_tx(int port)
{
	struct FIFOBuffer *txfifo = &ser_handles[port]->txfifo;

	while (lpc2_uartTxReady(port))
	{
		/*
		 * Disable TX empty interrupts if there're no more
		 * characters to transmit.
		 */
		if (fifo_isempty(txfifo))
		{
			/* IER: Disable THRE interrupt */
			*(reg8_t *)(uart_param[port].base + IER) &= ~BV(1);
			UARTDesc[port].sending = false;
			break;
		}
		/* THR: put a character to the Transmit Holding Register */
		*(reg8_t *)(uart_param[port].base + THR) = fifo_pop(txfifo);
	}
}

static void uart_common_irq_handler(int port)
{
	/* IIR: identify the interrupt source */
	uint32_t status = *(reg32_t *)(uart_param[port].base + IIR) >> 1 & 0x7;

	/* Receiver Data Ready (RDR) */
	if (status == 0x02)
		uart_irq_rx(port);
	/* Transmit Holding Register Empty (THRE) */
	else if (status == 0x01)
		uart_irq_tx(port);
	/* Signal the VIC we have completed the ISR */
	VICVectAddr = 0;
}

static void lpc2_uartIRQEnable(int port, vic_handler_t handler)
{
	vic_setVector(UARTDesc[port].irq, handler);
	vic_enable(UARTDesc[port].irq);
}

static void lpc2_uartIRQDisable(int port)
{
	vic_disable(UARTDesc[port].irq);
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
		struct LPC2Serial *hw = (struct LPC2Serial *)_hw;		\
										\
                if (hw->sending)						\
                        return;							\
		lpc2_uartPutChar(UART ## port ## _BASE_ADDR, fifo_pop(txfifo));	\
		if (!fifo_isempty(txfifo))					\
		{								\
			hw->sending = true;					\
			/* IER: Enable THRE interrupt */			\
			*(reg8_t *)(uart_param[port].base + IER) |= BV(1);	\
		}								\
	}									\
										\
	static void								\
	uart ## port ## _setbaudrate(UNUSED_ARG(struct SerialHardware *, hw),	\
						unsigned long baud)		\
	{									\
		lpc2_uartSetBaudRate(port, baud);				\
	}									\
										\
	static void								\
	uart ## port ## _setparity(UNUSED_ARG(struct SerialHardware *, hw),	\
						int parity)			\
	{									\
		lpc2_uartSetParity(port, parity);				\
	}									\
										\
	static void								\
	uart ## port ## _cleanup(struct SerialHardware *_hw)			\
	{									\
		struct LPC2Serial *hw = (struct LPC2Serial *)_hw;		\
										\
		hw->sending = false;						\
		lpc2_uartIRQDisable(port);					\
	}									\
										\
	static void								\
	uart ## port ## _init(UNUSED_ARG(struct SerialHardware *, hw),		\
				UNUSED_ARG(struct Serial *, ser))		\
	{									\
		lpc2_uartInit(port);						\
		lpc2_uartIRQEnable(port, uart ## port ## _irq_handler);		\
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
UART_PORT(3)

static struct LPC2Serial UARTDesc[SER_CNT] =
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
		.irq = INT_UART2,
	},
	{
		.hw = {
			.table = &UART3_VT,
			.txbuffer = uart3_txbuffer,
			.rxbuffer = uart3_rxbuffer,
			.txbuffer_size = sizeof(uart3_txbuffer),
			.rxbuffer_size = sizeof(uart3_rxbuffer),
		},
		.sending = false,
		.irq = INT_UART3,
	},
};

struct SerialHardware *ser_hw_getdesc(int port)
{
	ASSERT(port >= 0 && port < SER_CNT);
	return &UARTDesc[port].hw;
}
