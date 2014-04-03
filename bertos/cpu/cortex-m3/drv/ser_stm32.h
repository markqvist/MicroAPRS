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
 * \brief STM32F10xx UART interface driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef SER_STM32_H
#define SER_STM32_H

#include <cfg/debug.h>

#include <cpu/power.h> /* cpu_relax() */

#include <io/stm32.h>

/* Serial hardware numbers */
enum
{
	SER_UART1 = 0,
	SER_UART2,
#if CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE
	SER_UART3,
#endif
	SER_CNT //< Number of serial ports
};

/* Software errors */
#define SERRF_RXFIFOOVERRUN  BV(6) //< Rx FIFO buffer overrun
#define SERRF_RXTIMEOUT      BV(5) //< Receive timeout
#define SERRF_TXTIMEOUT      BV(4) //< Transmit timeout

/*
 * Hardware errors.
 */
#define SERRF_RXSROVERRUN    SR_ORE	 //< Input overrun
#define SERRF_FRAMEERROR     SR_FE   //< Stop bit missing
#define SERRF_PARITYERROR    SR_PE   //< Parity error
#define SERRF_NOISEERROR     SR_NE   //< Noise error

/* Serial error/status flags */
typedef uint32_t serstatus_t;

INLINE void stm32_uartDisable(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	_base->CR1 &= ~CR1_RUN_RESET;
}

INLINE void stm32_uartEnable(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	_base->CR1 |= CR1_RUN_SET;
}

/* Clear the flags register */
INLINE void stm32_uartClear(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	_base->SR &= ~USART_FLAG_MASK;
}

INLINE bool stm32_uartTxDone(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	return (_base->SR & USART_FLAG_TC);
}

INLINE bool stm32_uartTxReady(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	return (_base->SR & (BV(CR1_TXEIE) | BV(CR1_TCIE)));
}

INLINE bool stm32_uartRxReady(uint32_t base)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	return (_base->SR & BV(CR1_RXNEIE));
}

INLINE int stm32_uartPutChar(uint32_t base, unsigned char c)
{
	struct stm32_usart *_base = (struct stm32_usart *)base;
	while (!stm32_uartTxReady(base))
		cpu_relax();
	_base->DR = c;
	return c;
}

INLINE int stm32_uartGetChar(uint32_t base)
{
	struct stm32_usart * _base = (struct stm32_usart *)base;
	return _base->DR;
}

void stm32_uartSetBaudRate(uint32_t base, unsigned long baud);
void stm32_uartSetParity(uint32_t base, int parity);
void stm32_uartInit(int port);

#endif /* SER_STM32_H */
