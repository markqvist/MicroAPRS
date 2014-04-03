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
 * \brief STM32F10xx interrupts definition.
 */

#ifndef STM32_INTS_H
#define STM32_INTS_H

/**
 * The following are defines for the interrupt assignments.
 */
/*\{*/
#define WWDG_IRQHANDLER                 16  /* Window WatchDog Interrupt */
#define PVD_IRQHANDLER                  17  /* PVD through EXTI Line detection Interrupt */
#define TAMPER_IRQHANDLER               18  /* Tamper Interrupt */
#define RTC_IRQHANDLER                  19  /* RTC global Interrupt */
#define FLASH_IRQHANDLER                20  /* FLASH global Interrupt */
#define RCC_IRQHANDLER                  21  /* RCC global Interrupt */
#define EXTI0_IRQHANDLER                22  /* EXTI Line0 Interrupt */
#define EXTI1_IRQHANDLER                23  /* EXTI Line1 Interrupt */
#define EXTI2_IRQHANDLER                24  /* EXTI Line2 Interrupt */
#define EXTI3_IRQHANDLER                25  /* EXTI Line3 Interrupt */
#define EXTI4_IRQHANDLER                26  /* EXTI Line4 Interrupt */
#define DMACHANNEL1_IRQHANDLER          27  /* DMA Channel 1 global Interrupt */
#define DMACHANNEL2_IRQHANDLER          28  /* DMA Channel 2 global Interrupt */
#define DMACHANNEL3_IRQHANDLER          29  /* DMA Channel 3 global Interrupt */
#define DMACHANNEL4_IRQHANDLER          30  /* DMA Channel 4 global Interrupt */
#define DMACHANNEL5_IRQHANDLER          31  /* DMA Channel 5 global Interrupt */
#define DMACHANNEL6_IRQHANDLER          32  /* DMA Channel 6 global Interrupt */
#define DMACHANNEL7_IRQHANDLER          33  /* DMA Channel 7 global Interrupt */
#define ADC_IRQHANDLER                  34  /* ADC global Interrupt */
#define USB_HP_CAN_TX_IRQHANDLER        35  /* USB High Priority or CAN TX Interrupts */
#define USB_LP_CAN_RX0_IRQHANDLER       36  /* USB Low Priority or CAN RX0 Interrupts */
#define CAN_RX1_IRQHANDLER              37  /* CAN RX1 Interrupt */
#define CAN_SCE_IRQHANDLER              38  /* CAN SCE Interrupt */
#define EXTI9_5_IRQHANDLER              39  /* External Line[9:5] Interrupts */
#define TIM1_BRK_IRQHANDLER             40  /* TIM1 Break Interrupt */
#define TIM1_UP_IRQHANDLER              41  /* TIM1 Update Interrupt */
#define TIM1_TRG_COM_IRQHANDLER         42  /* TIM1 Trigger and Commutation Interrupt */
#define TIM1_CC_IRQHANDLER              43  /* TIM1 Capture Compare Interrupt */
#define TIM2_IRQHANDLER                 44  /* TIM2 global Interrupt */
#define TIM3_IRQHANDLER                 45  /* TIM3 global Interrupt */
#define TIM4_IRQHANDLER                 46  /* TIM4 global Interrupt */
#define I2C1_EV_IRQHANDLER              47  /* I2C1 Event Interrupt */
#define I2C1_ER_IRQHANDLER              48  /* I2C1 Error Interrupt */
#define I2C2_EV_IRQHANDLER              49  /* I2C2 Event Interrupt */
#define I2C2_ER_IRQHANDLER              50  /* I2C2 Error Interrupt */
#define SPI1_IRQHANDLER                 51  /* SPI1 global Interrupt */
#define SPI2_IRQHANDLER                 52  /* SPI2 global Interrupt */
#define USART1_IRQHANDLER               53  /* USART1 global Interrupt */
#define USART2_IRQHANDLER               54  /* USART2 global Interrupt */
#define USART3_IRQHANDLER               55  /* USART3 global Interrupt */
#define EXTI15_10_IRQHANDLER            56  /* External Line[15:10] Interrupts */
#define RTCALARM_IRQHANDLER             57  /* RTC Alarm through EXTI Line Interrupt */
#define USBWAKEUP_IRQHANDLER            58  /* USB WakeUp from suspend through EXTI Line Interrupt */
/*\}*/

#define NUM_INTERRUPTS  66

#endif /* STM32_INTS_H */
