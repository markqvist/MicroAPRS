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
 * \brief UART STM32F10xx registers definition.
 */

#ifndef STM32_UART_H
#define STM32_UART_H

/*
 * USART Word Length
 */
#define USART_WORD_LEN_8B                  ((uint16_t)0x0000)
#define USART_WORD_LEN_9B                  ((uint16_t)0x1000)

/*
 * USART Stop Bits
 */
#define USART_STOP_BIT_1                     ((uint16_t)0x0000)
#define USART_STOP_BIT_0_5                   ((uint16_t)0x1000)
#define USART_STOP_BIT_2                     ((uint16_t)0x2000)
#define USART_STOP_BIT_1_5                   ((uint16_t)0x3000)

/*
 * USART Parity
 */
#define USART_PARITY_NO                      ((uint16_t)0x0000)
#define USART_PARITY_EVEN                    ((uint16_t)0x0400)
#define USART_PARITY_ODD                     ((uint16_t)0x0600)


/*
 * USART Hardware Flow Control
 */
#define USART_HW_FLOW_NONE       ((uint16_t)0x0000)
#define USART_HW_FLOW_RTS        ((uint16_t)0x0100)
#define USART_HW_FLOW_CTS        ((uint16_t)0x0200)
#define USART_HW_FLOW_RTS_CTS    ((uint16_t)0x0300)


/*
 * USART Clock
 */
#define USART_CLK_DIS                  ((uint16_t)0x0000)
#define USART_CLK_EN                   ((uint16_t)0x0800)

/*
 * USART Clock Polarity
 */
#define USART_CPOL_LOW                       ((uint16_t)0x0000)
#define USART_CPOL_HIGHT                      ((uint16_t)0x0400)

/*
 * USART Clock Phase
 */
#define USART_CPHA_1EDGE                     ((uint16_t)0x0000)
#define USART_CPHA_2EDGE                     ((uint16_t)0x0200)



/*
 * USART Interrupt definition
 */
#define USART_IT_PE                          ((uint16_t)0x0028)
#define USART_IT_TXE                         ((uint16_t)0x0727)
#define USART_IT_TC                          ((uint16_t)0x0626)
#define USART_IT_RXNE                        ((uint16_t)0x0525)
#define USART_IT_IDLE                        ((uint16_t)0x0424)
#define USART_IT_LBD                         ((uint16_t)0x0846)
#define USART_IT_CTS                         ((uint16_t)0x096A)
#define USART_IT_ERR                         ((uint16_t)0x0060)
#define USART_IT_ORE                         ((uint16_t)0x0360)
#define USART_IT_NE                          ((uint16_t)0x0260)
#define USART_IT_FE                          ((uint16_t)0x0160)


#define INT_UART0_CR1                                      0x0C
#define INT_UART0_CR2                                      0x10
#define INT_UART0_CR3                                      0x14

/*
 * USART DMA Requests
 */
#define USART_DMA_REQ_TX                      ((uint16_t)0x0080)
#define USART_DMA_REQ_RX                      ((uint16_t)0x0040)

/*
 * USART WakeUp methods
 */
#define USART_WAKEUP_IDLELINE                ((uint16_t)0x0000)
#define USART_WAKEUP_ADRESSMARK             ((uint16_t)0x0800)


/*
 * USART LIN Break Detection Length
 */
#define USART_LIN_BREAK_DET_LEN_10B      ((uint16_t)0x0000)
#define USART_LIN_BREAK_DET_LEN_11B      ((uint16_t)0x0020)

/*
 * USART IrDA Low Power
 */
#define USART_IRDA_MODE_LOWPOWER              ((uint16_t)0x0004)
#define USART_IRDA_MODE_NORMAL                ((uint16_t)0x0000)


#define CR1_RUN_SET               ((uint16_t)0x2000)  /* USART Enable MASK */
#define CR1_RUN_RESET             ((uint16_t)0xDFFF)  /* USART Disable MASK */
#define CR1_RWU_SET               ((uint16_t)0x0002)  /* USART mute mode Enable MASK */
#define CR1_RWU_RESET             ((uint16_t)0xFFFD)  /* USART mute mode Enable MASK */
#define CR1_SBK_SET               ((uint16_t)0x0001)  /* USART Break Character send MASK */
#define CR1_USART_CLEAR_MASK      ((uint16_t)0xE9F3)  /* USART CR1 MASK */
#define CR1_UE                                   13
#define CR1_M                                    12
#define CR1_WAKE                                 11
#define CR1_PCE                                  10
#define CR1_PS                                    9
#define CR1_PEIE                                  8
#define CR1_TXEIE                                 7
#define CR1_TCIE                                  6
#define CR1_RXNEIE                                5
#define CR1_IDLEIE                                4
#define CR1_TE                                    3
#define CR1_RE                                    2
#define CR1_RWU                                   1
#define CR1_SBK                                   0

#define CR2_MASK                  ((uint16_t)0xFFF0)  /* USART address MASK */
#define CR2_LINE_SET              ((uint16_t)0x4000)  /* USART LIN Enable MASK */
#define CR2_LINE_RESET            ((uint16_t)0xBFFF)  /* USART LIN Disable MASK */
#define CR2_USART_CLEAR_MASK      ((uint16_t)0xC0FF)  /* USART CR2 MASK */

#define CR3_SCEN_SET              ((uint16_t)0x0020)  /* USART SC Enable MASK */
#define CR3_SCEN_RESET            ((uint16_t)0xFFDF)  /* USART SC Disable MASK */
#define CR3_NACK_SET              ((uint16_t)0x0010)  /* USART SC NACK Enable MASK */
#define CR3_NACK_RESET            ((uint16_t)0xFFEF)  /* USART SC NACK Disable MASK */
#define CR3_HDSEL_SET             ((uint16_t)0x0008)  /* USART Half-Duplex Enable MASK */
#define CR3_HDSEL_RESET           ((uint16_t)0xFFF7)  /* USART Half-Duplex Disable MASK */
#define CR3_IRLP_MASK             ((uint16_t)0xFFFB)  /* USART IrDA LowPower mode MASK */
#define CR3_LBDL_MASK             ((uint16_t)0xFFDF)  /* USART LIN Break detection MASK */
#define CR3_WAKE_MASK             ((uint16_t)0xF7FF)  /* USART WakeUp Method MASK */
#define CR3_IREN_SET              ((uint16_t)0x0002)  /* USART IrDA Enable MASK */
#define CR3_IREN_RESET            ((uint16_t)0xFFFD)  /* USART IrDA Disable MASK */
#define CR3_CLEAR_MASK            ((uint16_t)0xFCFF)  /* USART CR3 MASK */


/* Status */
#define SR_CTS                                    9
#define SR_LBD                                    8
#define SR_TXE                                    7
#define SR_TC                                     6
#define SR_RXNE                                   5
#define SR_IDLE                                   4
#define SR_ORE                                    3
#define SR_NE                                     2
#define SR_FE                                     1
#define SR_PE                                     0

#define GTPR_LSB_MASK             ((uint16_t)0x00FF)  /* Guard Time Register LSB MASK */
#define GTPR_MSB_MASK             ((uint16_t)0xFF00)  /* Guard Time Register MSB MASK */

#define USART_IT_MASK             ((uint16_t)0x001F)  /* USART Interrupt MASK */

/* USART flags */
#define USART_FLAG_CTS            ((uint16_t)0x0200)
#define USART_FLAG_LBD            ((uint16_t)0x0100)
#define USART_FLAG_TXE            ((uint16_t)0x0080)
#define USART_FLAG_TC             ((uint16_t)0x0040)
#define USART_FLAG_RXNE           ((uint16_t)0x0020)
#define USART_FLAG_IDLE           ((uint16_t)0x0010)
#define USART_FLAG_ORE            ((uint16_t)0x0008)
#define USART_FLAG_NE             ((uint16_t)0x0004)
#define USART_FLAG_FE             ((uint16_t)0x0002)
#define USART_FLAG_PE             ((uint16_t)0x0001)
#define USART_FLAG_MASK           ((uint16_t)0x03FF)


/* USART registers */
struct stm32_usart
{
	reg16_t SR;
	uint16_t _RESERVED0;
	reg16_t DR;
	uint16_t _RESERVED1;
	reg16_t BRR;
	uint16_t _RESERVED2;
	reg16_t CR1;
	uint16_t _RESERVED3;
	reg16_t CR2;
	uint16_t _RESERVED4;
	reg16_t CR3;
	uint16_t _RESERVED5;
	reg16_t GTPR;
	uint16_t _RESERVED6;
};

/* USART mode */
#define USART_MODE_RX             ((uint16_t)0x0004)
#define USART_MODE_TX             ((uint16_t)0x0008)

/* USART last bit */
#define USART_LASTBIT_DISABLE     ((uint16_t)0x0000)
#define USART_LASTBIT_ENABLE      ((uint16_t)0x0100)


INLINE uint16_t evaluate_brr(struct stm32_usart *base, unsigned long cpu_freq, unsigned long baud)
{
	uint32_t freq, reg, div, frac;

	/* NOTE: PCLK1 has been configured as CPU_FREQ / 2 */
	freq = (base == (struct stm32_usart *)USART1_BASE) ? cpu_freq : cpu_freq / 2;
	div = (0x19 * freq) / (0x04 * baud);
	reg = (div / 0x64) << 0x04;
	frac = div - (0x64 * (reg >> 0x04));
	reg |= ((frac * 0x10 + 0x32) / 0x64) & 0x0f;

	return (uint16_t)reg;
}

#endif /* STM32_UART_H */

