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
 * \brief STM32 debug support (implementation).
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "kdebug_stm32.h"

#include <cfg/cfg_debug.h>

#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h> /* RCC */

#include <io/stm32.h>



#if CONFIG_KDEBUG_PORT == 0
	#define UART_BASE ((struct stm32_usart *)USART1_BASE)
#elif CONFIG_KDEBUG_PORT == 1
	#define UART_BASE ((struct stm32_usart *)USART2_BASE)
#elif CONFIG_KDEBUG_PORT == 2
	#define UART_BASE ((struct stm32_usart *)USART3_BASE)
#else
	#error "UART port not supported in this board"
#endif

#define KDBG_WAIT_READY()	while (!(UART_BASE->SR & USART_FLAG_TXE))
#define KDBG_WAIT_TXDONE()	while (!(UART_BASE->SR & USART_FLAG_TC))

#define KDBG_WRITE_CHAR(c)	do { UART_BASE->DR = (c) & 0x1ff; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)	do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old)	do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;


/* Initialize UART debug port */
INLINE void kdbg_hw_init(void)
{
	/* Enable clocking on AFIO */
	RCC->APB2ENR |= RCC_APB2_AFIO;
	/* Configure USART pins */
#if CONFIG_KDEBUG_PORT == 0
	RCC->APB2ENR |= RCC_APB2_GPIOA;
	RCC->APB2ENR |= RCC_APB2_USART1;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOA_BASE, GPIO_USART1_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#elif CONFIG_KDEBUG_PORT == 1
	RCC->APB2ENR |= RCC_APB2_GPIOA;
	RCC->APB1ENR |= RCC_APB1_USART2;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOA_BASE, GPIO_USART2_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#elif  CONFIG_KDEBUG_PORT == 2
	RCC->APB2ENR |= RCC_APB2_GPIOB;
	RCC->APB1ENR |= RCC_APB1_USART3;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOB_BASE, GPIO_USART3_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#else
	#error "UART port not supported in this board"
#endif
	/* Enable the USART by writing the UE bit */
	UART_BASE->CR1 |= CR1_RUN_SET;
	/* Configure the desired baud rate */
	UART_BASE->BRR = (uint16_t)evaluate_brr(UART_BASE, CPU_FREQ, CONFIG_KDEBUG_BAUDRATE);
	/* Set the Transmitter Enable bit in CR1 */
	UART_BASE->CR1 |= USART_MODE_TX;
}
