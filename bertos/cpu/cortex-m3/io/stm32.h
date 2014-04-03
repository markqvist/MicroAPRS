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
 * \brief STM32F10XX registers definition.
 */

#ifndef STM32_H
#define STM32_H

#include <cfg/compiler.h>
#include <cfg/macros.h>

#include <cpu/types.h>

#include <io/stm32_memmap.h>

#include "stm32_nvic.h"
#include "stm32_ints.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_adc.h"
#include "stm32_i2c.h"
#include "stm32_flash.h"

#if CPU_CM3_STM32F101C4
	#define GPIO_USART1_TX_PIN	BV(9)
	#define GPIO_USART1_RX_PIN	BV(10)
	#define GPIO_USART2_TX_PIN	BV(2)
	#define GPIO_USART2_RX_PIN	BV(3)
#elif CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE || CPU_CM3_STM32F100RB
	#define GPIO_USART1_TX_PIN	BV(9)
	#define GPIO_USART1_RX_PIN	BV(10)
	#define GPIO_USART2_TX_PIN	BV(2)
	#define GPIO_USART2_RX_PIN	BV(3)
	#define GPIO_USART3_TX_PIN	BV(10)
	#define GPIO_USART3_RX_PIN	BV(11)
#else
	#error No USART pins are defined for select cpu
#endif

#if CPU_CM3_STM32F101C4
	#define GPIO_I2C1_SCL_PIN	BV(6)
	#define GPIO_I2C1_SDA_PIN	BV(7)
#elif CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE || CPU_CM3_STM32F100RB
	#define GPIO_I2C1_SCL_PIN	BV(6)
	#define GPIO_I2C1_SDA_PIN	BV(7)
	#define GPIO_I2C2_SCL_PIN	BV(10)
	#define GPIO_I2C2_SDA_PIN	BV(11)
#else
	#error No i2c pins are defined for select cpu
#endif

#if CPU_CM3_STM32F101C4 || CPU_CM3_STM32F103RB || CPU_CM3_STM32F103RE || CPU_CM3_STM32F100RB
	#define FLASH_PAGE_SIZE   1024
#else
	#error No embedded definition for select cpu
#endif

#endif /* STM32_H */
