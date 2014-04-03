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
 * \brief STM32F103xx I2C definition.
 */

#ifndef STM32_I2C_H
#define STM32_I2C_H

#include <cpu/types.h>


/* I2C_modes */
#define I2C_MODE_I2C                    ((uint16_t)0x0000)
#define I2C_MODE_SMBUSDEVICE            ((uint16_t)0x0002)
#define I2C_MODE_SMBUSHOST              ((uint16_t)0x000A)

/* I2C_duty_cycle_in_fast_mode */
#define I2C_DUTYCYCLE_16_9              ((uint16_t)0x4000)
#define I2C_DUTYCYCLE_2                 ((uint16_t)0xBFFF)

/* I2C_cknowledgementy */
#define I2C_ACK_ENABLE                  ((uint16_t)0x0400)
#define I2C_ACK_DISABLE                 ((uint16_t)0x0000)

/* I2C_transfer_direction */
#define I2C_DIRECTION_TRANSMITTER      ((uint8_t)0x00)
#define I2C_DIRECTION_RECEIVER         ((uint8_t)0x01)

/* I2C_acknowledged_address_defines  */
#define I2C_ACKNOWLEDGEDADDRESS_7BIT    ((uint16_t)0x4000)
#define I2C_ACKNOWLEDGEDADDRESS_10BIT   ((uint16_t)0xC000)

/* I2C_registers */
#define I2C_REGISTER_CR1                ((uint8_t)0x00)
#define I2C_REGISTER_CR2                ((uint8_t)0x04)
#define I2C_REGISTER_OAR1               ((uint8_t)0x08)
#define I2C_REGISTER_OAR2               ((uint8_t)0x0C)
#define I2C_REGISTER_DR                 ((uint8_t)0x10)
#define I2C_REGISTER_SR1                ((uint8_t)0x14)
#define I2C_REGISTER_SR2                ((uint8_t)0x18)
#define I2C_REGISTER_CCR                ((uint8_t)0x1C)
#define I2C_REGISTER_TRISE              ((uint8_t)0x20)

/* I2C_SMBus_alert_pin_level  */
#define I2C_SMBUSALERT_LOW              ((uint16_t)0x2000)
#define I2C_SMBUSALERT_HIGH             ((uint16_t)0xDFFF)

/* I2C_PEC_position  */
#define I2C_PECPOSITION_NEXT            ((uint16_t)0x0800)
#define I2C_PECPOSITION_CURRENT         ((uint16_t)0xF7FF)

/* I2C_interrupts_definition  */
#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)

/* I2C_interrupts_definition  */
#define I2C_IT_SMBALERT                 ((uint32_t)0x01008000)
#define I2C_IT_TIMEOUT                  ((uint32_t)0x01004000)
#define I2C_IT_PECERR                   ((uint32_t)0x01001000)
#define I2C_IT_OVR                      ((uint32_t)0x01000800)
#define I2C_IT_AF                       ((uint32_t)0x01000400)
#define I2C_IT_ARLO                     ((uint32_t)0x01000200)
#define I2C_IT_BERR                     ((uint32_t)0x01000100)
#define I2C_IT_TXE                      ((uint32_t)0x06000080)
#define I2C_IT_RXNE                     ((uint32_t)0x06000040)
#define I2C_IT_STOPF                    ((uint32_t)0x02000010)
#define I2C_IT_ADD10                    ((uint32_t)0x02000008)
#define I2C_IT_BTF                      ((uint32_t)0x02000004)
#define I2C_IT_ADDR                     ((uint32_t)0x02000002)
#define I2C_IT_SB                       ((uint32_t)0x02000001)

/* SR2 register flags  */
#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)

/* SR1 register flags  */
#define I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)
#define I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)
#define I2C_FLAG_PECERR                 ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                    ((uint32_t)0x10000800)
#define I2C_FLAG_AF                     ((uint32_t)0x10000400)
#define I2C_FLAG_ARLO                   ((uint32_t)0x10000200)
#define I2C_FLAG_BERR                   ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                    ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define I2C_FLAG_ADD10                  ((uint32_t)0x10000008)
#define I2C_FLAG_BTF                    ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define I2C_FLAG_SB                     ((uint32_t)0x10000001)

/* EV1  */
#define I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA, BUSY, TXE and ADDR flags */
#define I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY and ADDR flags */
#define I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF, TRA, BUSY and TXE flags */
#define I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF and BUSY flags */
#define I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL and BUSY flags */

/* EV2  */
#define I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY and RXNE flags */

/* EV3 */
#define I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA, BUSY, TXE and BTF flags */

/* EV4 */
#define I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF flag */

/* EV5 */
#define I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */


/* EV6 */
#define I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */

/* EV7 */
#define I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/* EV8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */

/* EV8_2 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

/* EV9 */
#define I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */

/* EV3_2 */
#define I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF flag */



/* I2C SPE mask */
#define CR1_PE_SET              ((uint16_t)0x0001)
#define CR1_PE_RESET            ((uint16_t)0xFFFE)

/* I2C START mask */
#define CR1_START_SET           ((uint16_t)0x0100)
#define CR1_START_RESET         ((uint16_t)0xFEFF)

/* I2C STOP mask */
#define CR1_STOP_SET            ((uint16_t)0x0200)
#define CR1_STOP_RESET          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_SET             ((uint16_t)0x0400)
#define CR1_ACK_RESET           ((uint16_t)0xFBFF)

/* I2C ENGC mask */
#define CR1_ENGC_SET            ((uint16_t)0x0040)
#define CR1_ENGC_RESET          ((uint16_t)0xFFBF)

/* I2C SWRST mask */
#define CR1_SWRST_SET           ((uint16_t)0x8000)
#define CR1_SWRST_RESET         ((uint16_t)0x7FFF)

/* I2C PEC mask */
#define CR1_PEC_SET             ((uint16_t)0x1000)
#define CR1_PEC_RESET           ((uint16_t)0xEFFF)

/* I2C ENPEC mask */
#define CR1_ENPEC_SET           ((uint16_t)0x0020)
#define CR1_ENPEC_RESET         ((uint16_t)0xFFDF)

/* I2C ENARP mask */
#define CR1_ENARP_SET           ((uint16_t)0x0010)
#define CR1_ENARP_RESET         ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CR1_NOSTRETCH_SET       ((uint16_t)0x0080)
#define CR1_NOSTRETCH_RESET     ((uint16_t)0xFF7F)

/* I2C registers Masks */
#define CR1_CLEAR_MASK          ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CR2_DMAEN_SET           ((uint16_t)0x0800)
#define CR2_DMAEN_RESET         ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CR2_LAST_SET            ((uint16_t)0x1000)
#define CR2_LAST_RESET          ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CR2_FREQ_RESET          ((uint16_t)0xFFC0)

#define CR2_FREQ_36MHZ          ((uint16_t)0x100100)

/* I2C ADD0 mask */
#define OAR1_ADD0_SET           ((uint16_t)0x0001)
#define OAR1_ADD0_RESET         ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OAR2_ENDUAL_SET         ((uint16_t)0x0001)
#define OAR2_ENDUAL_RESET       ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OAR2_ADD2_RESET         ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CCR_FS_SET              ((uint16_t)0x8000)

/* I2C CCR mask */
#define CCR_CCR_SET             ((uint16_t)0x0FFF)


/* I2C CR1 */
#define CR1_POS_SET                ((uint16_t)0x800)
#define CR1_POS_RESET                 (~CR1_POS_SET)

/* I2C Status */
#define SR1_PECERR                              12
#define SR1_OVR                                 11
#define SR1_AF                                  10
#define SR1_ARLO                                 9
#define SR1_BERR                                 8
#define SR1_TXE                                  7
#define SR1_RXNE                                 6
#define SR1_ADDR                                 1
#define SR1_BTF                                  2
#define SR1_SB                                   0

#define SR2_TRA                                  2
#define SR2_BUSY                                 1
#define SR2_MSL                                  0


#define SR1_ERR_MASK        (BV(SR1_PECERR) | \
							 BV(SR1_OVR)    | \
							 BV(SR1_AF)     | \
							 BV(SR1_ARLO)   | \
							 BV(SR1_BERR))

struct stm32_i2c
{
	reg16_t CR1;
	uint16_t  RESERVED0;
	reg16_t CR2;
	uint16_t  RESERVED1;
	reg16_t OAR1;
	uint16_t  RESERVED2;
	reg16_t OAR2;
	uint16_t  RESERVED3;
	reg16_t DR;
	uint16_t  RESERVED4;
	reg16_t SR1;
	uint16_t  RESERVED5;
	reg16_t SR2;
	uint16_t  RESERVED6;
	reg16_t CCR;
	uint16_t  RESERVED7;
	reg16_t TRISE;
	uint16_t  RESERVED8;
};

#endif /* STM32_I2C_H */
