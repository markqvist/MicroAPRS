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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * AT91SAM7 Analog to Digital Converter.
 *
 */


#ifndef AT91_ADC_H
#define AT91_ADC_H


/**
 * ADC control register
 */
#define ADC_CR_OFF              0x00000000     ///< Control register offeset.
#define ADC_CR          (*((reg32_t *)(ADC_BASE + ADC_CR_OFF))) ///< Control register address.
#define ADC_SWRST                        0      ///< Software reset.
#define ADC_START                        1      ///< Start conversion.


/**
 * ADC mode register
 */
#define ADC_MR_OFF              0x00000004     ///< Mode register offeset.
#define ADC_MR          (*((reg32_t *)(ADC_BASE + ADC_MR_OFF))) ///< Mode register address.
#define ADC_TRGEN                        0     ///< Trigger enable.

#define ADC_TRGSEL_TIOA0         0x00000000    ///< TIOA output of the timer counter channel 0.
#define ADC_TRGSEL_TIOA1         0x00000002    ///< TIOA output of the timer counter channel 1.
#define ADC_TRGSEL_TIOA2         0x00000004    ///< TIOA output of the timer counter channel 2.
#define ADC_TRGSEL_EXT           0x0000000C    ///< External trigger.

#define ADC_LOWRES                        4   ///< Resolution 0: 10-bit, 1: 8-bit.
#define ADC_SLEEP                         5   ///< Sleep mode.

/**
 * Prescaler rate selection.
 * ADCClock = MCK / ((ADC_PRESCALER_VALUE + 1) * 2)
 */
#define ADC_PRESCALER_MASK       0x00003F00   ///< Prescaler rate selection mask.
#define ADC_PRESCALER_SHIFT               8   ///< Prescale  rate selection shift.

/**
 * Start up timer.
 * Startup time = (ADC_STARTUP_VALUE + 1) * 8 /ADCClock
 */
#define ADC_STARTUP_MASK         0x001F0000   ///< Start up timer mask.
#define ADC_STARTUP_SHIFT                16   ///< Start up timer shift.


/**
 * Sample & hold time.
 * Sample & hold time = (ADC_SHTIM_VALUE + 1) * 8 /ADCClock
 */
#define ADC_SHTIME_MASK          0x0F000000   ///< Sample & hold time mask.
#define ADC_SHTIME_SHIFT                 24   ///< Sample & hold time shift.


/**
 * ADC channel enable register
 */
#define ADC_CHER_OFF             0x00000010     ///< Channel enable register offeset.
#define ADC_CHER          (*((reg32_t *)(ADC_BASE + ADC_CHER_OFF))) ///<  Channel enable register address.

/**
 * ADC channel disable register
 */
#define ADC_CHDR_OFF             0x00000014     ///< Channel disable register offeset.
#define ADC_CHDR          (*((reg32_t *)(ADC_BASE + ADC_CHDR_OFF))) ///<  Channel disable register address.

/**
 * ADC channel status register
 */
#define ADC_CHSR_OFF             0x00000018     ///< Channel status register offeset.
#define ADC_CHSR          (*((reg32_t *)(ADC_BASE + ADC_CHSR_OFF))) ///<  Channel status register address.

#define ADC_CH_MASK              0x000000FF    ///< Channel mask.
#define ADC_CH0                           0    ///< Channel 0
#define ADC_CH1                           1    ///< Channel 1
#define ADC_CH2                           2    ///< Channel 2
#define ADC_CH3                           3    ///< Channel 3
#define ADC_CH4                           4    ///< Channel 4
#define ADC_CH5                           5    ///< Channel 5
#define ADC_CH6                           6    ///< Channel 6
#define ADC_CH7                           7    ///< Channel 7

/**
 * ADC status register
 */
#define ADC_SR_OFF               0x0000001C     ///< Status register offeset.
#define ADC_SR          (*((reg32_t *)(ADC_BASE + ADC_SR_OFF))) ///< Status register address.

/**
 * ADC Interrupt enable register.
 */
#define ADC_IER_OFF              0x00000024     ///< Interrupt enable register offeset.
#define ADC_IER          (*((reg32_t *)(ADC_BASE + ADC_IER_OFF))) ///<  Interrupt enable register.

/**
 * ADC Interrupt disable register.
 */
#define ADC_IDR_OFF              0x00000028     ///< Interrupt disable register offeset.
#define ADC_IDR          (*((reg32_t *)(ADC_BASE + ADC_IDR_OFF))) ///<  Interrupt disable register.

/**
 * ADC Interrupt mask register.
 */
#define ADC_IMR_OFF              0x0000002C     ///< Interrupt mask register offeset.
#define ADC_IMR          (*((reg32_t *)(ADC_BASE + ADC_IMR_OFF))) ///<  Interrupt mask register.

#define ADC_EOC_MASK             0x000000FF    ///< End of converison mask.
#define ADC_EOC0                          0    ///< End of conversion channel 0.
#define ADC_EOC1                          1    ///< End of conversion channel 1.
#define ADC_EOC2                          2    ///< End of conversion channel 2.
#define ADC_EOC3                          3    ///< End of conversion channel 3.
#define ADC_EOC4                          4    ///< End of conversion channel 4.
#define ADC_EOC5                          5    ///< End of conversion channel 5.
#define ADC_EOC6                          6    ///< End of conversion channel 6.
#define ADC_EOC7                          7    ///< End of conversion channel 7.

#define ADC_OVRE0                         8    ///< Overrun error channel 0.
#define ADC_OVRE1                         9    ///< Overrun error channel 1.
#define ADC_OVRE2                        10    ///< Overrun error channel 2.
#define ADC_OVRE3                        11    ///< Overrun error channel 3.
#define ADC_OVRE4                        12    ///< Overrun error channel 4.
#define ADC_OVRE5                        13    ///< Overrun error channel 5.
#define ADC_OVRE6                        14    ///< Overrun error channel 6.
#define ADC_OVRE7                        15    ///< Overrun error channel 7.

#define ADC_DRDY                         16    ///< Data ready.
#define ADC_GOVRE                        17    ///< General overrun error.
#define ADC_ENDRX                        18    ///< End of RX buffer.
#define ADC_RXBUFF                       19    ///< Rx buffer full.

/**
 * ADC last convert data register.
 */
#define ADC_LCDR_OFF             0x00000020     ///< Last converted data register offeset.
#define ADC_LCDR          (*((reg32_t *)(ADC_BASE + ADC_LCDR_OFF))) ///< Last converted data register.

/**
 * ADC channel data register.
 *
 * \{
 */
#define ADC_CDR0_OFF              0x00000030    ///< Channel data register 0 offeset.
#define ADC_CDR1_OFF              0x00000034    ///< Channel data register 1 offeset.
#define ADC_CDR2_OFF              0x00000038    ///< Channel data register 2 offeset.
#define ADC_CDR3_OFF              0x0000003C    ///< Channel data register 3 offeset.
#define ADC_CDR4_OFF              0x00000040    ///< Channel data register 4 offeset.
#define ADC_CDR5_OFF              0x00000044    ///< Channel data register 5 offeset.
#define ADC_CDR6_OFF              0x00000048    ///< Channel data register 6 offeset.
#define ADC_CDR7_OFF              0x0000004C    ///< Channel data register 7 offeset.

#define ADC_CDR0    (*((reg32_t *)(ADC_BASE + ADC_CDR0_OFF)))  ///< Channel data register 0.
#define ADC_CDR1    (*((reg32_t *)(ADC_BASE + ADC_CDR1_OFF)))  ///< Channel data register 1.
#define ADC_CDR2    (*((reg32_t *)(ADC_BASE + ADC_CDR2_OFF)))  ///< Channel data register 2.
#define ADC_CDR3    (*((reg32_t *)(ADC_BASE + ADC_CDR3_OFF)))  ///< Channel data register 3.
#define ADC_CDR4    (*((reg32_t *)(ADC_BASE + ADC_CDR4_OFF)))  ///< Channel data register 4.
#define ADC_CDR5    (*((reg32_t *)(ADC_BASE + ADC_CDR5_OFF)))  ///< Channel data register 5.
#define ADC_CDR6    (*((reg32_t *)(ADC_BASE + ADC_CDR6_OFF)))  ///< Channel data register 6.
#define ADC_CDR7    (*((reg32_t *)(ADC_BASE + ADC_CDR7_OFF)))  ///< Channel data register 7.
/* \} */

#endif /* AT91_ADC_H */
