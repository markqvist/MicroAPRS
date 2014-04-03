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
 * \brief STM32F103xx adc definition.
 */

#ifndef STM32_ADC_H
#define STM32_ADC_H

#include <cpu/types.h>

/* ADC dual mode */
#define ADC_MODE_INDEPENDENT                       ((uint32_t)0x00000000)
#define ADC_MODE_REGINJECSIMULT                    ((uint32_t)0x00010000)
#define ADC_MODE_REGSIMULT_ALTERTRIG               ((uint32_t)0x00020000)
#define ADC_MODE_INJECSIMULT_FASTINTERL            ((uint32_t)0x00030000)
#define ADC_MODE_INJECSIMULT_SLOWINTERL            ((uint32_t)0x00040000)
#define ADC_MODE_INJECSIMULT                       ((uint32_t)0x00050000)
#define ADC_MODE_REGSIMULT                         ((uint32_t)0x00060000)
#define ADC_MODE_FASTINTERL                        ((uint32_t)0x00070000)
#define ADC_MODE_SLOWINTERL                        ((uint32_t)0x00080000)
#define ADC_MODE_ALTERTRIG                         ((uint32_t)0x00090000)

/* ADC extrenal trigger sources for regular channels conversion */
#define ADC_EXTERNALTRIGCONV_T1_CC1                ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONV_T1_CC2                ((uint32_t)0x00020000)
#define ADC_EXTERNALTRIGCONV_T1_CC3                ((uint32_t)0x00040000)
#define ADC_EXTERNALTRIGCONV_T2_CC2                ((uint32_t)0x00060000)
#define ADC_EXTERNALTRIGCONV_T3_TRGO               ((uint32_t)0x00080000)
#define ADC_EXTERNALTRIGCONV_T4_CC4                ((uint32_t)0x000A0000)
#define ADC_EXTERNALTRIGCONV_EXT_IT11              ((uint32_t)0x000C0000)
#define ADC_EXTERNALTRIGCONV_NONE                  ((uint32_t)0x000E0000)

/* ADC data align */
#define ADC_DATAALIGN_RIGHT                        ((uint32_t)0x00000000)
#define ADC_DATAALIGN_LEFT                         ((uint32_t)0x00000800)

/* ADC channels */
#define ADC_CHANNEL_0                               ((uint8_t)0x00)
#define ADC_CHANNEL_1                               ((uint8_t)0x01)
#define ADC_CHANNEL_2                               ((uint8_t)0x02)
#define ADC_CHANNEL_3                               ((uint8_t)0x03)
#define ADC_CHANNEL_4                               ((uint8_t)0x04)
#define ADC_CHANNEL_5                               ((uint8_t)0x05)
#define ADC_CHANNEL_6                               ((uint8_t)0x06)
#define ADC_CHANNEL_7                               ((uint8_t)0x07)
#define ADC_CHANNEL_8                               ((uint8_t)0x08)
#define ADC_CHANNEL_9                               ((uint8_t)0x09)
#define ADC_CHANNEL_10                              ((uint8_t)0x0A)
#define ADC_CHANNEL_11                              ((uint8_t)0x0B)
#define ADC_CHANNEL_12                              ((uint8_t)0x0C)
#define ADC_CHANNEL_13                              ((uint8_t)0x0D)
#define ADC_CHANNEL_14                              ((uint8_t)0x0E)
#define ADC_CHANNEL_15                              ((uint8_t)0x0F)
#define ADC_CHANNEL_16                              ((uint8_t)0x10)
#define ADC_CHANNEL_17                              ((uint8_t)0x11)

/* ADC sampling times */
#define ADC_SAMPLETIME_1CYCLES5                    ((uint8_t)0x00)
#define ADC_SAMPLETIME_7CYCLES5                    ((uint8_t)0x01)
#define ADC_SAMPLETIME_13CYCLES5                   ((uint8_t)0x02)
#define ADC_SAMPLETIME_28CYCLES5                   ((uint8_t)0x03)
#define ADC_SAMPLETIME_41CYCLES5                   ((uint8_t)0x04)
#define ADC_SAMPLETIME_55CYCLES5                   ((uint8_t)0x05)
#define ADC_SAMPLETIME_71CYCLES5                   ((uint8_t)0x06)
#define ADC_SAMPLETIME_239CYCLES5                  ((uint8_t)0x07)

/* ADC extrenal trigger sources for injected channels conversion */
#define ADC_EXTERNALTRIGINJECCONV_T1_TRGO          ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGINJECCONV_T1_CC4           ((uint32_t)0x00001000)
#define ADC_EXTERNALTRIGINJECCONV_T2_TRGO          ((uint32_t)0x00002000)
#define ADC_EXTERNALTRIGINJECCONV_T2_CC1           ((uint32_t)0x00003000)
#define ADC_EXTERNALTRIGINJECCONV_T3_CC4           ((uint32_t)0x00004000)
#define ADC_EXTERNALTRIGINJECCONV_T4_TRGO          ((uint32_t)0x00005000)
#define ADC_EXTERNALTRIGINJECCONV_EXT_IT15         ((uint32_t)0x00006000)
#define ADC_EXTERNALTRIGINJECCONV_NONE             ((uint32_t)0x00007000)

/* ADC injected channel selection */
#define ADC_INJECTEDCHANNEL_1                       ((uint8_t)0x14)
#define ADC_INJECTEDCHANNEL_2                       ((uint8_t)0x18)
#define ADC_INJECTEDCHANNEL_3                       ((uint8_t)0x1C)
#define ADC_INJECTEDCHANNEL_4                       ((uint8_t)0x20)

/* ADC analog watchdog selection */
#define ADC_ANALOGWATCHDOG_SINGLEREGENABLE         ((uint32_t)0x00800200)
#define ADC_ANALOGWATCHDOG_SINGLEINJECENABLE       ((uint32_t)0x00400200)
#define ADC_ANALOGWATCHDOG_SINGLEREGORINJECENABLE  ((uint32_t)0x00C00200)
#define ADC_ANALOGWATCHDOG_ALLREGENABLE            ((uint32_t)0x00800000)
#define ADC_ANALOGWATCHDOG_ALLINJECENABLE          ((uint32_t)0x00400000)
#define ADC_ANALOGWATCHDOG_ALLREGALLINJECENABLE    ((uint32_t)0x00C00000)
#define ADC_ANALOGWATCHDOG_NONE                    ((uint32_t)0x00000000)

/* ADC interrupts definition */
#define ADC_IT_EOC                                 ((uint16_t)0x0220)
#define ADC_IT_AWD                                 ((uint16_t)0x0140)
#define ADC_IT_JEOC                                ((uint16_t)0x0480)

/* ADC flags definition */
#define ADC_FLAG_AWD                               ((uint8_t)0x01)
#define ADC_FLAG_EOC                               ((uint8_t)0x02)
#define ADC_FLAG_JEOC                              ((uint8_t)0x04)
#define ADC_FLAG_JSTRT                             ((uint8_t)0x08)
#define ADC_FLAG_STRT                              ((uint8_t)0X10)


/* ADC ADON mask */
#define CR2_ADON_SET               ((uint32_t)0x00000001)
#define CR2_ADON_RESET             ((uint32_t)0xFFFFFFFE)

/* ADC DMA mask */
#define CR2_DMA_SET                ((uint16_t)0x0100)
#define CR2_DMA_RESET              ((uint16_t)0xFEFF)

/* ADC RSTCAL mask */
#define CR2_RSTCAL_SET             ((uint16_t)0x0008)

/* ADC CAL mask */
#define CR2_CAL_SET                ((uint16_t)0x0004)

/* ADC SWSTRT mask */
#define CR2_SWSTRT_SET             ((uint32_t)0x00400000)

/* ADC DISCNUM mask */
#define CR1_DISCNUM_RESET          ((uint32_t)0xFFFF1FFF)

/* ADC DISCEN mask */
#define CR1_DISCEN_SET             ((uint32_t)0x00000800)
#define CR1_DISCEN_RESET           ((uint32_t)0xFFFFF7FF)

/* ADC EXTTRIG mask */
#define CR2_EXTTRIG_SET            ((uint32_t)0x00100000)
#define CR2_EXTTRIG_RESET          ((uint32_t)0xFFEFFFFF)

/* ADC Software start mask */
#define CR2_EXTTRIG_SWSTRT_SET     ((uint32_t)0x00500000)
#define CR2_EXTTRIG_SWSTRT_RESET   ((uint32_t)0xFFAFFFFF)

/* ADC JAUTO mask */
#define CR1_JAUTO_SET              ((uint32_t)0x00000400)
#define CR1_JAUTO_RESET            ((uint32_t)0xFFFFFBFF)

/* ADC JDISCEN mask */
#define CR1_JDISCEN_SET            ((uint32_t)0x00001000)
#define CR1_JDISCEN_RESET          ((uint32_t)0xFFFFEFFF)

/* ADC JEXTSEL mask */
#define CR2_JEXTSEL_RESET          ((uint32_t)0xFFFF8FFF)

/* ADC JEXTTRIG mask */
#define CR2_JEXTTRIG_SET           ((uint32_t)0x00008000)
#define CR2_JEXTTRIG_RESET         ((uint32_t)0xFFFF7FFF)

/* ADC JSWSTRT mask */
#define CR2_JSWSTRT_SET            ((uint32_t)0x00200000)

/* ADC injected software start mask */
#define CR2_JEXTTRIG_JSWSTRT_SET   ((uint32_t)0x00208000)
#define CR2_JEXTTRIG_JSWSTRT_RESET ((uint32_t)0xFFDF7FFF)

/* ADC AWDCH mask */
#define CR1_AWDCH_RESET            ((uint32_t)0xFFFFFFE0)

/* ADC SQx mask */
#define SQR3_SQ_MASK                ((uint8_t)0x1F)
#define SQR2_SQ_MASK                ((uint8_t)0x1F)
#define SQR1_SQ_MASK                ((uint8_t)0x1F)
#define SQR1_SQ_LEN_MASK                       0xF
#define SQR1_SQ_LEN_SHIFT                       20

/* ADC JSQx mask */
#define JSQR_JSQ_SET               ((uint8_t)0x1F)

/* ADC JL mask */
#define JSQR_JL_RESET              ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SMPR1_SMP_SET              ((uint8_t)0x07)
#define SMPR2_SMP_SET              ((uint8_t)0x07)

/* ADC Analog watchdog enable mode mask */
#define CR1_AWDMODE_RESET          ((uint32_t)0xFF3FFDFF)

/* ADC TSPD mask */
#define CR2_TSVREFE_SET            ((uint32_t)0x00800000)
#define CR2_TSVREFE_RESET          ((uint32_t)0xFF7FFFFF)

/* ADC JDRx registers= offset */
#define JDR_OFFSET                 ((uint8_t)0x28)

/* ADC CR1 register */
#define CR1_EOCIE                                      5
#define CR1_AWDIE                                      6
#define CR1_JEOCIE                                     7
#define CR1_SCAN                                       8
#define CR1_AWDSGL                                     9
#define CR1_JAUTO                                     10
#define CR1_DISCEN                                    11
#define CR1_JDISCEN                                   12
#define CR1_JAWDEN                                    22
#define CR1_AWDEN                                     23

/* ADC CR2 register */
#define CR2_ADON                                       0
#define CR2_CONT                                       1
#define CR2_CAL                                        2
#define CR2_RTSCAL                                     3
#define CR2_DMA                                        8
#define CR2_ALIGN                                     11
#define CR2_JEXTTRIG                                  15
#define CR2_EXTTRIG                                   20
#define CR2_JSWSTART                                  21
#define CR2_SWSTART                                   22
#define CR2_TSVREFE                                   23

/* ADC status */
#define SR_AWD                                         0
#define SR_EOC                                         1
#define SR_JEOC                                        2
#define SR_JSTRT                                       3
#define SR_STRT                                        4

/* ADC sample time */
#define SMPR1_CH17                                    21
#define SMPR1_CH16                                    18
#define SMPR1_CH15                                    15
#define SMPR1_CH14                                    12
#define SMPR1_CH13                                     9
#define SMPR1_CH12                                     6
#define SMPR1_CH11                                     3
#define SMPR1_CH10                                     0

#define SMPR2_CH9                                     27
#define SMPR2_CH8                                     24
#define SMPR2_CH7                                     21
#define SMPR2_CH6                                     18
#define SMPR2_CH5                                     15
#define SMPR2_CH4                                     12
#define SMPR2_CH3                                      9
#define SMPR2_CH2                                      6
#define SMPR2_CH1                                      3
#define SMPR2_CH0                                      0

/* ADC registers Masks */
#define CR1_ADC_CLEAR_MASK             ((uint32_t)0xFFF0FEFF)
#define CR2_ADC_CLEAR_MASK             ((uint32_t)0xFFF1F7FD)
#define SQR1_CLEAR_MASK                ((uint32_t)0xFF0FFFFF)



/* ADC defines for SMT32F103Bxx */
#define ADC_TEMP_V25         4300  // uV / C
#define ADC_TEMP_SLOPE       1430  // mV
#define ADC_TEMP_CONST      25000
#define ADC_TEMP_CH            16
#define ADC_VREFINT_CH         17

struct stm32_adc
{
	reg32_t SR;
	reg32_t CR1;
	reg32_t CR2;
	reg32_t SMPR1;
	reg32_t SMPR2;
	reg32_t JOFR1;
	reg32_t JOFR2;
	reg32_t JOFR3;
	reg32_t JOFR4;
	reg32_t HTR;
	reg32_t LTR;
	reg32_t SQR1;
	reg32_t SQR2;
	reg32_t SQR3;
	reg32_t JSQR;
	reg32_t JDR1;
	reg32_t JDR2;
	reg32_t JDR3;
	reg32_t JDR4;
	reg32_t DR;
};

#endif /* STM32_ADC_H */
