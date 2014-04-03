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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * SAM3 Analog to Digital Converter.
 *
 * $WIZ$
 *
 */


#ifndef SAM3_ADC_H
#define SAM3_ADC_H

/** ADC registers base. */
#define ADC_BASE                0x400C0000

/**
 * ADC control register
 * \{
 */
#define ADC_CR_OFF              0x00000000     ///< Control register offeset.
#define ADC_CR          (*((reg32_t *)(ADC_BASE + ADC_CR_OFF))) ///< Control register address.
#define ADC_SWRST                        0      ///< Software reset.
#define ADC_START                        1      ///< Start conversion.
/* \} */

/**
 * ADC mode register
 * \{
 */
#define ADC_MR_OFF              0x00000004     ///< Mode register offeset.
#define ADC_MR          (*((reg32_t *)(ADC_BASE + ADC_MR_OFF))) ///< Mode register address.
#define ADC_TRGEN                        0     ///< Trigger enable.

#define ADC_TRGSEL_TIOA0         0x00000000    ///< TIOA output of the timer counter channel 0.
#define ADC_TRGSEL_TIOA1         0x00000002    ///< TIOA output of the timer counter channel 1.
#define ADC_TRGSEL_TIOA2         0x00000004    ///< TIOA output of the timer counter channel 2.
#define ADC_TRGSEL_PWM0          0x0000000A    ///< PWM Event Line 0.
#define ADC_TRGSEL_PWM1          0x0000000C    ///< PWM Event Line 1.

#define ADC_LOWRES                        4   ///< Resolution 0: 12-bit, 1: 10-bit.
#define ADC_SLEEP                         5   ///< Sleep mode.
#define ADC_FREERUN                       7   ///< Freerun.

/**
 * Prescaler rate selection.
 * ADCClock = MCK / ((ADC_PRESCALER_VALUE + 1) * 2)
 */
#define ADC_PRESCALER_MASK       0x0000FF00   ///< Prescaler rate selection mask.
#define ADC_PRESCALER_SHIFT               8   ///< Prescale  rate selection shift.

/**
 * Start up timer.
 */
#define ADC_STARTUP_MASK         0x000F0000    ///< Start up timer mask.
#define ADC_STARTUP_SHIFT                16    ///< Start up timer shift.

/**
 * Start up timer.
 * $WIZ$ sam3_adc_sut = "ADC_SUT0","ADC_SUT8","ADC_SUT16","ADC_SUT24","ADC_SUT64","DC_SUT80","ADC_SUT96","ADC_SUT112","ADC_SUT512","ADC_SUT576","ADC_SUT640","ADC_SUT704","ADC_SUT768","ADC_SUT832","ADC_SUT896","ADC_SUT960"
 * \{
 */
#define ADC_SUT0                          0    ///< 0 period of ADCClock.
#define ADC_SUT8                          1    ///< 8 period of ADCClock.
#define ADC_SUT16                         2    ///< 16 period of ADCClock.
#define ADC_SUT24                         3    ///< 24 period of ADCClock.
#define ADC_SUT64                         4    ///< 64 period of ADCClock.
#define ADC_SUT80                         5    ///< 80 period of ADCClock.
#define ADC_SUT96                         6    ///< 96 period of ADCClock.
#define ADC_SUT112                        7    ///< 112 period of ADCClock.
#define ADC_SUT512                        8    ///< 512 period of ADCClock.
#define ADC_SUT576                        9    ///< 576 period of ADCClock.
#define ADC_SUT640                        10   ///< 640 period of ADCClock.
#define ADC_SUT704                        11   ///< 704 period of ADCClock.
#define ADC_SUT768                        12   ///< 768 period of ADCClock.
#define ADC_SUT832                        13   ///< 832 period of ADCClock.
#define ADC_SUT896                        14   ///< 896 period of ADCClock.
#define ADC_SUT960                        15   ///< 896 period of ADCClock.
/** \} */

/**
 * Analog Settling Time.
 * $WIZ$ sam3_adc_stt = "ADC_AST3", "ADC_AST5", "ADC_AST9", "ADC_AST17"
 */
#define ADC_SETTLING_MASK        0x00300000    ///< Analog Settling Time mask.
#define ADC_SETTLING_SHIFT               20    ///< Analog Settling Time shift.
#define ADC_AST3                          0    ///< 3 period of ADCClock
#define ADC_AST5                          1    ///< 5 period of ADCClock
#define ADC_AST9                          2    ///< 9 period of ADCClock
#define ADC_AST17                         3    ///< 17 period of ADCClock

/**
 * Tracking Time.
 * Tracking Time = (TRACKTIM + 1) * ADCClock periods.
 */
#define ADC_TRACKTIM_MASK          0x0F000000   ///< Tracking Time mask.
#define ADC_TRACKTIM_SHIFT                 24   ///< Tracking Time shift.

/**
 * Transfer Period.
 * Transfer Period = (TRANSFER * 2 + 3) ADCClock periods.
 */
#define ADC_TRANSFER_MASK          0x30000000   ///< Transfer Period mask.
#define ADC_TRANSFER_SHIFT                 28   ///< Transfer Period shift.
/* \} */


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


/**
 * ADC status register
 */
#define ADC_SR_OFF               0x0000001C     ///< Status register offeset.
#define ADC_SR          (*((reg32_t *)(ADC_BASE + ADC_SR_OFF))) ///< Status register address.


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

/**
 * ADC Interrupt status register.
 */
#define ADC_ISR_OFF              0x00000030     ///< Interrupt status register offeset.
#define ADC_ISR          (*((reg32_t *)(ADC_BASE + ADC_ISR_OFF))) ///<  Interrupt status register.

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

#define ADC_DRDY                         24    ///< Data ready.
#define ADC_GOVRE                        25    ///< General overrun error.
#define ADC_COMPE                        26    ///< Comparition event interrupt mask.
#define ADC_ENDRX                        27    ///< End of RX buffer.
#define ADC_RXBUFF                       28    ///< Rx buffer full.

/**
 * ADC last convert data register.
 * \{
 */
#define ADC_LCDR_OFF             0x00000020     ///< Last converted data register offeset.
#define ADC_LCDR          (*((reg32_t *)(ADC_BASE + ADC_LCDR_OFF))) ///< Last converted RAW data register.
#define ADC_LDATA         (ADC_LCDR & 0xFFF)  ///< Last data converted register.
#define ADC_CHNB          ((ADC_LCDR & 0xF000) >> 12) ///< Channel number.
/* \} */


/**
 * ADC Channel data register.
 * \{
 */
#define ADC_CDR_OFF             0x00000050     ///< Channel data register offeset.
#define ADC_CDR          (*((reg32_t *)(ADC_BASE + ADC_CDR_OFF))) ///< Channel data register.
/* \} */


/**
 * ADC Analog Control register.
 * \{
 */
#define ADC_ACR_OFF              0x00000094     ///< Analog control register offeset.
#define ADC_ACR          (*((reg32_t *)(ADC_BASE + ADC_ACR_OFF))) ///< Analog control register.
#define ADC_TSON                          4     ///< Temperature Sensor On.
#define ADC_TEMPERATURE_CH               15     ///< Channel where is the internal sensor temperature
/* \} */

#endif /* SAM3_ADC_H */
