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
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef LM3S_H
#define LM3S_H

#include <cpu/detect.h>
#include <cfg/compiler.h>

#include "lm3s_com.h"
#include "lm3s_types.h"
#include "lm3s_ints.h"
#include "lm3s_nvic.h"
#include "lm3s_sysctl.h"
#include "lm3s_gpio.h"
#include "lm3s_memmap.h"
#include "lm3s_uart.h"
#include "lm3s_ssi.h"
#include "lm3s_i2c.h"
#include "lm3s_adc.h"

#if CPU_CM3_LM3S1968
    #define GPIO_I2C0_SCL_PIN   BV(2)
    #define GPIO_I2C0_SDA_PIN   BV(3)

    #define GPIO_I2C1_SCL_PIN   BV(6)
    #define GPIO_I2C1_SDA_PIN   BV(7)
#elif CPU_CM3_LM3S8962
    #define GPIO_I2C0_SCL_PIN   BV(2)
    #define GPIO_I2C0_SDA_PIN   BV(3)
#else
    #error No i2c pins are defined for select cpu
#endif

/* Flash memory mapping */
#if CPU_CM3_LM3S1968 || CPU_CM3_LM3S8962
    #define FLASH_SIZE               0x40000 //< 256KiB
    #define FLASH_PAGE_SIZE          0x400   //< 1KiB
#else
    #error No embedded definition for select cpu
#endif

#endif /* LM3S_H */
