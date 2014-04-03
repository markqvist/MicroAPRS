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
 * \brief LM3S debug support (implementation).
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/cfg_debug.h>
#include <cfg/macros.h> /* for BV() */
#include <drv/clock_lm3s.h> /* lm3s_busyWait() */
#include <drv/gpio_lm3s.h>
#include <drv/ser_lm3s.h>
#include "kdebug_lm3s.h"

#if CONFIG_KDEBUG_PORT == 0
	#define UART_BASE UART0_BASE
	#define UART_GPIO_BASE GPIO_PORTA_BASE
	#define UART_PINS (BV(1) | BV(0))
	#define UART_REG_SYSCTL SYSCTL_RCGC2_GPIOA
#elif CONFIG_KDEBUG_PORT == 1
	#define UART_BASE UART1_BASE
	#define UART_GPIO_BASE GPIO_PORTD_BASE
	#define UART_PINS (BV(3) | BV(2))
	#define UART_REG_SYSCTL SYSCTL_RCGC2_GPIOD
#elif CONFIG_KDEBUG_PORT == 2
	#define UART_BASE UART2_BASE
	#define UART_GPIO_BASE GPIO_PORTG_BASE
	#define UART_PINS (BV(1) | BV(0))
	#define UART_REG_SYSCTL SYSCTL_RCGC2_GPIOG
#else
	#error "UART port not supported in this board"
#endif

#define KDBG_WAIT_READY()     while (!lm3s_uartReady(UART_BASE)) {}
#define KDBG_WAIT_TXDONE()    while (!lm3s_uartTxDone(UART_BASE)) {}

#define KDBG_WRITE_CHAR(c)    do { lm3s_uartPutCharNonBlocking(UART_BASE, c); } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)    do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old) do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;

INLINE void uart_hw_config(void)
{
	unsigned long div, baud = CONFIG_KDEBUG_BAUDRATE;
	bool hi_speed = false;

	if (baud * 16 > CPU_FREQ)
	{
		hi_speed = true;
		baud /= 2;
	}
	div = (CPU_FREQ * 8 / baud + 1) / 2;

	lm3s_uartDisable(UART_BASE);
	if (hi_speed)
		HWREG(UART_BASE + UART_O_CTL) |= UART_CTL_HSE;
	else
		HWREG(UART_BASE + UART_O_CTL) &= ~UART_CTL_HSE;
	/* Set the baud rate */
	HWREG(UART_BASE + UART_O_IBRD) = div / 64;
	HWREG(UART_BASE + UART_O_FBRD) = div % 64;
	/* Set word lenght and parity */
	HWREG(UART_BASE + UART_O_LCRH) = UART_LCRH_WLEN_8;
	lm3s_uartClear(UART_BASE);
	lm3s_uartEnable(UART_BASE);
}

INLINE void kdbg_hw_init(void)
{
	uint32_t reg_clock = 1 << CONFIG_KDEBUG_PORT;

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= reg_clock;
	SYSCTL_RCGC2_R |= UART_REG_SYSCTL;
	lm3s_busyWait(512);

	/* Configure GPIO pins to work as UART pins */
	lm3s_gpioPinConfig(UART_GPIO_BASE, UART_PINS,
			GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	/* Low-level UART configuration */
	uart_hw_config();
}
