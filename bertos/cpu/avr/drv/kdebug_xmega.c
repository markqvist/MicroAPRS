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
 * Copyright 2003, 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernie Innocenti <bernie@codewiz.org>
 * Copyright 2011 Onno <developer@gorgoz.org>
 *
 * -->
 *
 * \brief AVR XMega debug support (implementation).
 *
 * This file is heavily inspired by the AVR implementation for BeRTOS,
 * but uses a different approach for implementing the different debug
 * ports, by using the USART_t structs.
 *
 * \author Onno <developer@gorgoz.org>
 * notest:all
 */

#include <hw/hw_cpufreq.h>	/* for CPU_FREQ */
#include "hw/hw_ser.h"		/* Required for bus macros overrides */

#include "cfg/cfg_debug.h"	/* for debugging configuration settings */
#include <cfg/macros.h> 	/* for BV(), DIV_ROUND */

#include <cpu/types.h>
#include <cpu/attr.h>

#include <avr/io.h>

/* Set KDBG_USART, KDBG_USART_PORT and KDBG_USART_TX_PIN_bm
 * according to the CONFIG_KDEBUG_PORT setting
 * The Xmega A and D families support at least 2 UARTS
 */
#if CONFIG_KDEBUG_PORT == 0
	#define KDBG_USART              USARTC0
	#define KDBG_USART_PORT         PORTC
	#define KDBG_USART_TX_PIN_bm    PIN3_bm
#elif CONFIG_KDEBUG_PORT == 1
	#define KDBG_USART              USARTD0
	#define KDBG_USART_PORT         PORTD
	#define KDBG_USART_TX_PIN_bm    PIN3_bm
#endif
/* Allow the configuration of the extra 3 UARTS for the
 * Xmega A family
 */
#ifdef CPU_AVR_XMEGA_A
	#if CONFIG_KDEBUG_PORT == 2
		#define KDBG_USART              USARTC1
		#define KDBG_USART_PORT         PORTC
		#define KDBG_USART_TX_PIN_bm    PIN7_bm
	#elif CONFIG_KDEBUG_PORT == 3
		#define KDBG_USART              USARTD1
		#define KDBG_USART_PORT         PORTD
		#define KDBG_USART_TX_PIN_bm    PIN7_bm
	#elif CONFIG_KDEBUG_PORT == 4
		#define KDBG_USART              USARTE0
		#define KDBG_USART_PORT         PORTE
		#define KDBG_USART_TX_PIN_bm    PIN3_bm
	#endif
#endif
/* Check if all required KDBG_ macros are defined
 */
#ifndef KDBG_USART
	#if CPU_AVR_XMEGA_D
		#error CONFIG_KDEBUG_PORT should be either 0 or 1
	#elif CPU_AVR_XMEGA_A
		#error CONFIG_KDEBUG_PORT should be either 0, 1, 2, 3 or 4
	#endif
#endif

/*
 * Scalefactor to use for computing the baudrate
 * this scalefactor should be an integer value between -7
 * and 7
 */
#ifndef KDBG_USART_SCALE_FACTOR
	#define KDBG_USART_SCALE_FACTOR (-7)
#else
	#if KDBG_USART_SCALE_FACTOR > 7 || KDBG_USART_SCALE_FACTOR < -7
		#error KDBG_USART_SCALE_FACTOR should be an integer between -7 and 7
	#endif
#endif

/*
 * \name KDBG macros
 *
 * Used to set or alter the KDB_USART operation,
 * enable the usart or send a byte.
 * Some of these methods are called/included from kdbg_hw_init()
 * others are called/included from the cpu independ kdebug implementation
 * These macros are heavily imspired by the examples provided by atmel
 *
 * \{
 */
#define KDBG_SET_FORMAT(_charSize, _parityMode, _twoStopBits)         \
	(KDBG_USART).CTRLC = (uint8_t) _charSize | _parityMode |                      \
	                  (_twoStopBits ? USART_SBMODE_bm : 0)

#define KDBG_SET_BAUDRATE(_bselValue, _bScaleFactor)                  \
	(KDBG_USART).BAUDCTRLA =(uint8_t)_bselValue;                                           \
	(KDBG_USART).BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)

#define KDBG_TX_ENABLE()	((KDBG_USART).CTRLB |= USART_TXEN_bm)

#define KDBG_SET_MODE(_usartMode)                                      \
	((KDBG_USART).CTRLC = ((KDBG_USART).CTRLC & (~USART_CMODE_gm)) | _usartMode)

#define KDBG_WAIT_READY()   do{ loop_until_bit_is_set((KDBG_USART).STATUS, USART_DREIF_bp); } while(0)
#define KDBG_WAIT_TXDONE()  do { loop_until_bit_is_set((KDBG_USART).STATUS, USART_TXCIF_bp); } while(0)
#define KDBG_WRITE_CHAR(c)  do { (KDBG_USART).DATA = (c); } while(0)

#define KDBG_SET_TX_INTERRUPTLEVEL(_txdIntLevel)                      \
	(KDBG_USART).CTRLA = ((KDBG_USART).CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel

#define KDBG_SET_DRE_INTERRUPTLEVEL(_dreIntLevel)                      \
	(KDBG_USART).CTRLA = ((KDBG_USART).CTRLA & ~USART_DREINTLVL_gm) | _dreIntLevel

/*\}*/

/*
 * To restore the USART state, to registers need to be restored
 * These registers (CTRLA and CTRLB) can be saved to the
 * kdbg_avr_xmaga_irqsave structure
 */
struct kdbg_avr_xmega_irqsave
{
	uint8_t ctrlb;
	uint8_t ctrla;
};
typedef struct kdbg_avr_xmega_irqsave kdbg_irqsave_t;

/*
 * param is the kdbg_irqsave_t structure
 *
 * * Stores the current state of the USART.CTRLA and
 * the USART.CTRLB registers
 * * Disables Transmit Complete and Date Register Empty interrupts
 * * Enabled the transmitter
 */
#define KDBG_MASK_IRQ(old)    do { \
	(old).ctrlb = KDBG_USART.CTRLB; \
	(old).ctrla = KDBG_USART.CTRLA; \
	KDBG_SET_TX_INTERRUPTLEVEL(USART_TXCINTLVL_OFF_gc); \
	KDBG_SET_DRE_INTERRUPTLEVEL(USART_DREINTLVL_OFF_gc); \
	KDBG_TX_ENABLE(); \
} while(0)

/*
 * param is the kdbg_irqsave_t structure
 *
 * * waits until all data has been transmitted
 * * restores the USART.CTRLA and USART.CTRLB registers
 */
#define KDBG_RESTORE_IRQ(old) do { \
	KDBG_WAIT_TXDONE(); \
	KDBG_USART.CTRLB = (old).ctrlb; \
	KDBG_USART.CTRLA = (old).ctrla; \
} while(0)


/*
 * method included from the cpu independent kdebug.c file.
 * it initializes KDBG_USART by:
 * * Setting the KDBG_USART_TX_PIN_bm as an outputpin
 * * Setting KDBG_USART to use 8 bits, No parity, 1 stopbit
 * * Setting the baudrate to 115200
 * * Enabeling the transmitter
 */
INLINE void kdbg_hw_init(void)
{
		//set transmit pin as output
		KDBG_USART_PORT.OUT = KDBG_USART_PORT.OUT & ~KDBG_USART_TX_PIN_bm;
		KDBG_USART_PORT.DIRSET = KDBG_USART_TX_PIN_bm;
		//set 8 bits, no parity, 1 stop bit
		KDBG_SET_FORMAT(USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
		//compute and set the baud rate
		/* Compute baud-rate period, this requires a valid USART_SCALE_FACTOR */
		#if KDBG_USART_SCALE_FACTOR < 0
			uint16_t bsel = DIV_ROUND((1 << (-(KDBG_USART_SCALE_FACTOR))) * (CPU_FREQ - (16 * CONFIG_KDEBUG_BAUDRATE)), 16 * CONFIG_KDEBUG_BAUDRATE);
		#else
			uint16_t bsel = DIV_ROUND(CPU_FREQ, (1 << (KDBG_USART_SCALE_FACTOR)) * 16 * CONFIG_KDEBUG_BAUDRATE) - 1;
		#endif
		KDBG_SET_BAUDRATE(bsel, KDBG_USART_SCALE_FACTOR);
		//enable the Transmitter
		KDBG_TX_ENABLE();
}
