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
 * \brief Low-level clocking driver for Cortex-M3 STM32.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef CLOCK_STM32_H
#define CLOCK_STM32_H


#include <cfg/compiler.h>

/* RCC registers bit address */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/**
 * CR Register
 */
/*\{*/
/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BITNUMBER           0x00
#define CR_HSION_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BITNUMBER * 4)))

/* Alias word address of PLLON bit */
#define PLLON_BITNUMBER           0x18
#define CR_PLLON_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BITNUMBER * 4)))

/* Alias word address of CSSON bit */
#define CSSON_BITNUMBER           0x13
#define CR_CSSON_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BITNUMBER * 4)))
/*\}*/

/**
 * CFGR Register
 */
/*\{*/
/* Alias word address of USBPRE bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x04)
#define USBPRE_BITNUMBER          0x16
#define CFGR_USBPRE_BB            ((reg32_t *)(PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BITNUMBER * 4)))
/*\}*/

/**
 * BDCR Register
 */
/*\{*/
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x20)
#define RTCEN_BITNUMBER           0x0F
#define BDCR_RTCEN_BB             ((reg32_t *)(PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BITNUMBER * 4)))

/* Alias word address of BDRST bit */
#define BDRST_BITNUMBER           0x10
#define BDCR_BDRST_BB             ((reg32_t *)(PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BITNUMBER * 4)))
/*\}*/

/**
 * CSR Register
 */
/*\{*/
/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x24)
#define LSION_BITNUMBER           0x00
#define CSR_LSION_BB              ((reg32_t *)(PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BITNUMBER * 4)))
/*\}*/

/**
 * RCC registers bit mask
 */
/*\{*/
/* CR register bit mask */
#define CR_HSEBYP_RESET           (0xFFFBFFFF)
#define CR_HSEBYP_SET             (0x00040000)
#define CR_HSEON_RESET            (0xFFFEFFFF)
#define CR_HSEON_SET              (0x00010000)
#define CR_HSITRIM_MASK           (0xFFFFFF07)

/* CFGR register bit mask */
#define CFGR_PLL_MASK             (0xFFC0FFFF)
#define CFGR_PLLMull_MASK         (0x003C0000)
#define CFGR_PLLSRC_MASK          (0x00010000)
#define CFGR_PLLXTPRE_MASK        (0x00020000)
#define CFGR_SWS_MASK             (0x0000000C)
#define CFGR_SW_MASK              (0xFFFFFFFC)
#define CFGR_HPRE_RESET_MASK      (0xFFFFFF0F)
#define CFGR_HPRE_SET_MASK        (0x000000F0)
#define CFGR_PPRE1_RESET_MASK     (0xFFFFF8FF)
#define CFGR_PPRE1_SET_MASK       (0x00000700)
#define CFGR_PPRE2_RESET_MASK     (0xFFFFC7FF)
#define CFGR_PPRE2_SET_MASK       (0x00003800)
#define CFGR_ADCPRE_RESET_MASK    (0xFFFF3FFF)
#define CFGR_ADCPRE_SET_MASK      (0x0000C000)

/* CSR register bit mask */
#define CSR_RVMF_SET              (0x01000000)

/* RCC Flag MASK */
#define FLAG_MASK                 (0x1F)

/* Typical VALUE of the HSI in Hz */
#define HSI_VALUE                 (8000000)

/* BDCR register base address */
#define BDCR_BASE                 (PERIPH_BASE + BDCR_OFFSET)

/* RCC Flag */
#define RCC_FLAG_HSIRDY                  (0x20)
#define RCC_FLAG_HSERDY                  (0x31)
#define RCC_FLAG_PLLRDY                  (0x39)
#define RCC_FLAG_LSERDY                  (0x41)
#define RCC_FLAG_LSIRDY                  (0x61)
#define RCC_FLAG_PINRST                  (0x7A)
#define RCC_FLAG_PORRST                  (0x7B)
#define RCC_FLAG_SFTRST                  (0x7C)
#define RCC_FLAG_IWDGRST                 (0x7D)
#define RCC_FLAG_WWDGRST                 (0x7E)
#define RCC_FLAG_LPWRRST                 (0x7F)

/* System clock source */
#define RCC_SYSCLK_HSI             (0x00000000)
#define RCC_SYSCLK_HSE             (0x00000001)
#define RCC_SYSCLK_PLLCLK          (0x00000002)

/* PLL entry clock source */
#define RCC_PLL_HSI_DIV2           (0x00000000)
#define RCC_PLL_HSE_DIV1           (0x00010000)
#define RCC_PLL_HSE_DIV2           (0x00030000)

/* PLL multiplication factor */
#define RCC_PLLMUL_2               (0x00000000)
#define RCC_PLLMUL_3               (0x00040000)
#define RCC_PLLMUL_4               (0x00080000)
#define RCC_PLLMUL_5               (0x000C0000)
#define RCC_PLLMUL_6               (0x00100000)
#define RCC_PLLMUL_7               (0x00140000)
#define RCC_PLLMUL_8               (0x00180000)
#define RCC_PLLMUL_9               (0x001C0000)
#define RCC_PLLMUL_10              (0x00200000)
#define RCC_PLLMUL_11              (0x00240000)
#define RCC_PLLMUL_12              (0x00280000)
#define RCC_PLLMUL_13              (0x002C0000)
#define RCC_PLLMUL_14              (0x00300000)
#define RCC_PLLMUL_15              (0x00340000)
#define RCC_PLLMUL_16              (0x00380000)

/* APB1/APB2 clock source */
#define RCC_HCLK_DIV1              (0x00000000)
#define RCC_HCLK_DIV2              (0x00000400)
#define RCC_HCLK_DIV4              (0x00000500)
#define RCC_HCLK_DIV8              (0x00000600)
#define RCC_HCLK_DIV16             (0x00000700)

/* USB clock source */
#define RCC_USBCLK_PLLCLK_1DIV5    (0x00)
#define RCC_USBCLK_PLLCLK_DIV1     (0x01)

/* ADC clock source */
#define RCC_PCLK2_DIV2             (0x00000000)
#define RCC_PCLK2_DIV4             (0x00004000)
#define RCC_PCLK2_DIV6             (0x00008000)
#define RCC_PCLK2_DIV8             (0x0000C000)

/* AHB clock source */
#define RCC_SYSCLK_DIV1            (0x00000000)
#define RCC_SYSCLK_DIV2            (0x00000080)
#define RCC_SYSCLK_DIV4            (0x00000090)
#define RCC_SYSCLK_DIV8            (0x000000A0)
#define RCC_SYSCLK_DIV16           (0x000000B0)
#define RCC_SYSCLK_DIV64           (0x000000C0)
#define RCC_SYSCLK_DIV128          (0x000000D0)
#define RCC_SYSCLK_DIV256          (0x000000E0)
#define RCC_SYSCLK_DIV512          (0x000000F0)
/*\}*/

/**
 * RCC register: APB1 peripheral
 */
/*\{*/
#define RCC_APB1_TIM2              (0x00000001)
#define RCC_APB1_TIM3              (0x00000002)
#define RCC_APB1_TIM4              (0x00000004)
#define RCC_APB1_WWDG              (0x00000800)
#define RCC_APB1_SPI2              (0x00004000)
#define RCC_APB1_USART2            (0x00020000)
#define RCC_APB1_USART3            (0x00040000)
#define RCC_APB1_I2C1              (0x00200000)
#define RCC_APB1_I2C2              (0x00400000)
#define RCC_APB1_USB               (0x00800000)
#define RCC_APB1_CAN               (0x02000000)
#define RCC_APB1_BKP               (0x08000000)
#define RCC_APB1_PWR               (0x10000000)
#define RCC_APB1_ALL               (0x1AE64807)
/*\}*/

/**
 * RCC register: APB2 peripheral
 */
/*\{*/
#define RCC_APB2_AFIO              (0x00000001)
#define RCC_APB2_GPIOA             (0x00000004)
#define RCC_APB2_GPIOB             (0x00000008)
#define RCC_APB2_GPIOC             (0x00000010)
#define RCC_APB2_GPIOD             (0x00000020)
#define RCC_APB2_GPIOE             (0x00000040)
#define RCC_APB2_ADC1              (0x00000200)
#define RCC_APB2_ADC2              (0x00000400)
#define RCC_APB2_TIM1              (0x00000800)
#define RCC_APB2_SPI1              (0x00001000)
#define RCC_APB2_USART1            (0x00004000)
#define RCC_APB2_ALL               (0x00005E7D)

/**
 * RCC register:  BCDR
 */
#define RCC_BDCR_LSEON             (0x00000001)
#define RCC_BDCR_LSERDY            (0x00000002)
#define RCC_BDCR_RTCSEL            (0x00000300)
#define RCC_BDCR_RTCEN             (0x00008000)
/*\}*/

/* Crystal frequency of the main oscillator (8MHz) */
#define PLL_VCO                    8000000

/* Reset and Clock Controller registers */
struct RCC
{
	reg32_t CR;
	reg32_t CFGR;
	reg32_t CIR;
	reg32_t APB2RSTR;
	reg32_t APB1RSTR;
	reg32_t AHBENR;
	reg32_t APB2ENR;
	reg32_t APB1ENR;
	reg32_t BDCR;
	reg32_t CSR;
};

/* RCC registers base */
extern struct RCC *RCC;

void clock_init(void);

#endif /* CLOCK_STM32_h */
