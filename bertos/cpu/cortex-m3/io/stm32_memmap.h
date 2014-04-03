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
 * \brief STM32 memory map.
 */

#ifndef STM32_MEMMAP_H
#define STM32_MEMMAP_H

/* Peripheral and SRAM base address in the alias region */
#define PERIPH_BB_BASE        (0x42000000)
#define SRAM_BB_BASE          (0x22000000)

/* Peripheral and SRAM base address in the bit-band region */
#define SRAM_BASE             (0x20000000)
#define PERIPH_BASE           (0x40000000)

/* Flash refisters base address */
#define FLASH_BASE            (0x40022000)
/* Flash Option Bytes base address */
#define OB_BASE               (0x1FFFF800)

/* Peripheral memory map */
#define APB1PERIPH_BASE       (PERIPH_BASE)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)
#define CEC_BASE              (APB1PERIPH_BASE + 0x7800)

#define AFIO_BASE             (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x1800)
#define GPIOF_BASE            (APB2PERIPH_BASE + 0x1C00)
#define GPIOG_BASE            (APB2PERIPH_BASE + 0x2000)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x3400)
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x3C00)
#define TIM15_BASE            (APB2PERIPH_BASE + 0x4000)
#define TIM16_BASE            (APB2PERIPH_BASE + 0x4400)
#define TIM17_BASE            (APB2PERIPH_BASE + 0x4800)

#define SDIO_BASE             (PERIPH_BASE + 0x18000)


#define DMA1_BASE             (AHBPERIPH_BASE + 0X0000)
#define DMA1_CHANNEL1_BASE    (AHBPERIPH_BASE + 0X0008)
#define DMA1_CHANNEL2_BASE    (AHBPERIPH_BASE + 0X001C)
#define DMA1_CHANNEL3_BASE    (AHBPERIPH_BASE + 0X0030)
#define DMA1_CHANNEL4_BASE    (AHBPERIPH_BASE + 0X0044)
#define DMA1_CHANNEL5_BASE    (AHBPERIPH_BASE + 0X0058)
#define DMA1_CHANNEL6_BASE    (AHBPERIPH_BASE + 0X006C)
#define DMA1_CHANNEL7_BASE    (AHBPERIPH_BASE + 0X0080)
#define DMA2_BASE             (AHBPERIPH_BASE + 0X0400)
#define DMA2_CHANNEL1_BASE    (AHBPERIPH_BASE + 0X0408)
#define DMA2_CHANNEL2_BASE    (AHBPERIPH_BASE + 0X041C)
#define DMA2_CHANNEL3_BASE    (AHBPERIPH_BASE + 0X0430)
#define DMA2_CHANNEL4_BASE    (AHBPERIPH_BASE + 0X0444)
#define DMA2_CHANNEL5_BASE    (AHBPERIPH_BASE + 0X0458)
#define RCC_BASE              (AHBPERIPH_BASE + 0X1000)
#define CRC_BASE              (AHBPERIPH_BASE + 0X3000)

#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x2000) ///< Flash registers base address

#define ETH_BASE              (AHBPERIPH_BASE + 0x8000)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000)

#define FSMC_BANK1_R_BASE     (FSMC_R_BASE + 0x0000) ///< FSMC Bank1 registers base address
#define FSMC_BANK1E_R_BASE    (FSMC_R_BASE + 0x0104) ///< FSMC Bank1E registers base address
#define FSMC_BANK2_R_BASE     (FSMC_R_BASE + 0x0060) ///< FSMC Bank2 registers base address
#define FSMC_BANK3_R_BASE     (FSMC_R_BASE + 0x0080) ///< FSMC Bank3 registers base address
#define FSMC_BANK4_R_BASE     (FSMC_R_BASE + 0x00A0) ///< FSMC Bank4 registers base address

#define DBGMCU_BASE          ((uint32_t)0xE0042000) ///< Debug MCU registers base address

/* System Control Space memory map */
#define SCS_BASE              (0xE000E000)

#define SYSTICK_BASE          (SCS_BASE + 0x0010)
#define NVIC_BASE             (SCS_BASE + 0x0100)
#define SCB_BASE              (SCS_BASE + 0x0D00)

#endif /* STM32_MEMMAP_H */
