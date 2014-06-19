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
 * Copyright 2003, 2004, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief AVR MEGA UART and SPI I/O driver (Implementation)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "hw/hw_ser.h"  /* Required for bus macros overrides */
#include <hw/hw_cpufreq.h>  /* CPU_FREQ */

#include "cfg/cfg_ser.h"

#include <cfg/macros.h> /* DIV_ROUND */
#include <cfg/debug.h>
#include <cfg/cfg_arch.h> // ARCH_NIGHTTEST

#include <drv/ser.h>
#include <drv/ser_p.h>
#include <drv/timer.h>

#include <struct/fifobuf.h>

#include <avr/io.h>

#if defined(__AVR_LIBC_VERSION__) && (__AVR_LIBC_VERSION__ >= 10400UL)
	#include <avr/interrupt.h>
#else
	#include <avr/signal.h>
#endif


#if !CONFIG_SER_HWHANDSHAKE
	/**
	 * \name Hardware handshake (RTS/CTS).
	 * \{
	 */
	#define RTS_ON      do {} while (0)
	#define RTS_OFF     do {} while (0)
	#define IS_CTS_ON   true
	#define EIMSKF_CTS  0 /**< Dummy value, must be overridden */
	/*\}*/
#endif

#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	#define BIT_RXCIE0 RXCIE0
	#define BIT_RXEN0  RXEN0
	#define BIT_TXEN0  TXEN0
	#define BIT_UDRIE0 UDRIE0

	#define BIT_RXCIE1 RXCIE1
	#define BIT_RXEN1  RXEN1
	#define BIT_TXEN1  TXEN1
	#define BIT_UDRIE1 UDRIE1
	#if CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
		#define BIT_RXCIE2 RXCIE2
		#define BIT_RXEN2  RXEN2
		#define BIT_TXEN2  TXEN2
		#define BIT_UDRIE2 UDRIE2

		#define BIT_RXCIE3 RXCIE3
		#define BIT_RXEN3  RXEN3
		#define BIT_TXEN3  TXEN3
		#define BIT_UDRIE3 UDRIE3
	#endif
#elif CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P
	#define BIT_RXCIE0 RXCIE0
	#define BIT_RXEN0  RXEN0
	#define BIT_TXEN0  TXEN0
	#define BIT_UDRIE0 UDRIE0

	#define BIT_RXCIE1 RXCIE0
	#define BIT_RXEN1  RXEN0
	#define BIT_TXEN1  TXEN0
	#define BIT_UDRIE1 UDRIE0
#else
	#define BIT_RXCIE0 RXCIE
	#define BIT_RXEN0  RXEN
	#define BIT_TXEN0  TXEN
	#define BIT_UDRIE0 UDRIE

	#define BIT_RXCIE1 RXCIE
	#define BIT_RXEN1  RXEN
	#define BIT_TXEN1  TXEN
	#define BIT_UDRIE1 UDRIE
#endif


/**
 * \name Overridable serial bus hooks
 *
 * These can be redefined in hw.h to implement
 * special bus policies such as half-duplex, 485, etc.
 *
 *
 * \code
 *  TXBEGIN      TXCHAR      TXEND  TXOFF
 *    |   __________|__________ |     |
 *    |   |   |   |   |   |   | |     |
 *    v   v   v   v   v   v   v v     v
 * ______  __  __  __  __  __  __  ________________
 *       \/  \/  \/  \/  \/  \/  \/
 * ______/\__/\__/\__/\__/\__/\__/
 *
 * \endcode
 *
 * \{
 */
#ifndef SER_UART0_BUS_TXINIT
	/**
	 * Default TXINIT macro - invoked in uart0_init()
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	#define SER_UART0_BUS_TXINIT do { \
		UCSR0A = 0; /* The Arduino Uno bootloader turns on U2X0 */ \
		UCSR0B = BV(BIT_RXCIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXBEGIN
	/**
	 * Invoked before starting a transmission
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable both the RX complete and UDR empty interrupts
	 */
	#define SER_UART0_BUS_TXBEGIN do { \
		UCSR0B = BV(BIT_RXCIE0) | BV(BIT_UDRIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXCHAR
	/**
	 * Invoked to send one character.
	 */
	#define SER_UART0_BUS_TXCHAR(c) do { \
		UDR0 = (c); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXEND
	/**
	 * Invoked as soon as the txfifo becomes empty
	 *
	 * - Keep both the receiver and the transmitter enabled
	 * - Keep the RX complete interrupt enabled
	 * - Disable the UDR empty interrupt
	 */
	#define SER_UART0_BUS_TXEND do { \
		UCSR0B = BV(BIT_RXCIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXOFF
	/**
	 * \def SER_UART0_BUS_TXOFF
	 *
	 * Invoked after the last character has been transmitted
	 *
	 * The default is no action.
	 */
	#ifdef __doxygen__
	#define SER_UART0_BUS_TXOFF
	#endif
#endif

#ifndef SER_UART1_BUS_TXINIT
	/** \sa SER_UART0_BUS_TXINIT */
	#define SER_UART1_BUS_TXINIT do { \
		UCSR1B = BV(BIT_RXCIE1) | BV(BIT_RXEN1) | BV(BIT_TXEN1); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXBEGIN
	/** \sa SER_UART0_BUS_TXBEGIN */
	#define SER_UART1_BUS_TXBEGIN do { \
		UCSR1B = BV(BIT_RXCIE1) | BV(BIT_UDRIE1) | BV(BIT_RXEN1) | BV(BIT_TXEN1); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXCHAR
	/** \sa SER_UART0_BUS_TXCHAR */
	#define SER_UART1_BUS_TXCHAR(c) do { \
		UDR1 = (c); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXEND
	/** \sa SER_UART0_BUS_TXEND */
	#define SER_UART1_BUS_TXEND do { \
		UCSR1B = BV(BIT_RXCIE1) | BV(BIT_RXEN1) | BV(BIT_TXEN1); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXOFF
	/**
	 * \def SER_UART1_BUS_TXOFF
	 *
	 * \see SER_UART0_BUS_TXOFF
	 */
	#ifdef __doxygen__
	#define SER_UART1_BUS_TXOFF
	#endif
#endif

#ifndef SER_UART2_BUS_TXINIT
	/** \sa SER_UART0_BUS_TXINIT */
	#define SER_UART2_BUS_TXINIT do { \
		UCSR2B = BV(BIT_RXCIE2) | BV(BIT_RXEN2) | BV(BIT_TXEN2); \
	} while (0)
#endif
#ifndef SER_UART2_BUS_TXBEGIN
	/** \sa SER_UART0_BUS_TXBEGIN */
	#define SER_UART2_BUS_TXBEGIN do { \
		UCSR2B = BV(BIT_RXCIE2) | BV(BIT_UDRIE2) | BV(BIT_RXEN2) | BV(BIT_TXEN2); \
	} while (0)
#endif
#ifndef SER_UART2_BUS_TXCHAR
	/** \sa SER_UART0_BUS_TXCHAR */
	#define SER_UART2_BUS_TXCHAR(c) do { \
		UDR2 = (c); \
	} while (0)
#endif
#ifndef SER_UART2_BUS_TXEND
	/** \sa SER_UART0_BUS_TXEND */
	#define SER_UART2_BUS_TXEND do { \
		UCSR2B = BV(BIT_RXCIE2) | BV(BIT_RXEN2) | BV(BIT_TXEN2); \
	} while (0)
#endif
#ifndef SER_UART2_BUS_TXOFF
	/**
	 * \def SER_UART2_BUS_TXOFF
	 *
	 * \see SER_UART0_BUS_TXOFF
	 */
	#ifdef __doxygen__
	#define SER_UART2_BUS_TXOFF
	#endif
#endif

#ifndef SER_UART3_BUS_TXINIT
	/** \sa SER_UART0_BUS_TXINIT */
	#define SER_UART3_BUS_TXINIT do { \
		UCSR3B = BV(BIT_RXCIE3) | BV(BIT_RXEN3) | BV(BIT_TXEN3); \
	} while (0)
#endif
#ifndef SER_UART3_BUS_TXBEGIN
	/** \sa SER_UART0_BUS_TXBEGIN */
	#define SER_UART3_BUS_TXBEGIN do { \
		UCSR3B = BV(BIT_RXCIE3) | BV(BIT_UDRIE3) | BV(BIT_RXEN3) | BV(BIT_TXEN3); \
	} while (0)
#endif
#ifndef SER_UART3_BUS_TXCHAR
	/** \sa SER_UART0_BUS_TXCHAR */
	#define SER_UART3_BUS_TXCHAR(c) do { \
		UDR3 = (c); \
	} while (0)
#endif
#ifndef SER_UART3_BUS_TXEND
	/** \sa SER_UART0_BUS_TXEND */
	#define SER_UART3_BUS_TXEND do { \
		UCSR3B = BV(BIT_RXCIE3) | BV(BIT_RXEN3) | BV(BIT_TXEN3); \
	} while (0)
#endif
#ifndef SER_UART3_BUS_TXOFF
	/**
	 * \def SER_UART3_BUS_TXOFF
	 *
	 * \see SER_UART0_BUS_TXOFF
	 */
	#ifdef __doxygen__
	#define SER_UART3_BUS_TXOFF
	#endif
#endif
/*\}*/


/**
 * \name Overridable SPI hooks
 *
 * These can be redefined in hw.h to implement
 * special bus policies such as slave select pin handling, etc.
 *
 * \{
 */
#ifndef SER_SPI_BUS_TXINIT
	/**
	 * Default TXINIT macro - invoked in spi_init()
	 * The default is no action.
	 */
	#define SER_SPI_BUS_TXINIT
#endif

#ifndef SER_SPI_BUS_TXCLOSE
	/**
	 * Invoked after the last character has been transmitted.
	 * The default is no action.
	 */
	#define SER_SPI_BUS_TXCLOSE
#endif
/*\}*/


/* SPI port and pin configuration */
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA1281 \
    || CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	#define SPI_PORT      PORTB
	#define SPI_DDR       DDRB
	#define SPI_SS_BIT    PB0
	#define SPI_SCK_BIT   PB1
	#define SPI_MOSI_BIT  PB2
	#define SPI_MISO_BIT  PB3
// TODO: these bits are the same as ATMEGA8 but the defines in avr-gcc are different.
// They should be the same!
#elif CPU_AVR_ATMEGA328P
	#define SPI_PORT      PORTB
	#define SPI_DDR       DDRB
	#define SPI_SS_BIT    PORTB2
	#define SPI_SCK_BIT   PORTB5
	#define SPI_MOSI_BIT  PORTB3
	#define SPI_MISO_BIT  PORTB4
#elif CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA168
	#define SPI_PORT      PORTB
	#define SPI_DDR       DDRB
	#define SPI_SS_BIT    PB2
	#define SPI_SCK_BIT   PB5
	#define SPI_MOSI_BIT  PB3
	#define SPI_MISO_BIT  PB4
#elif CPU_AVR_ATMEGA32
	#define SPI_PORT      PORTB
	#define SPI_DDR       DDRB
	#define SPI_SS_BIT    PB4
	#define SPI_SCK_BIT   PB7
	#define SPI_MOSI_BIT  PB5
	#define SPI_MISO_BIT  PB6
#else
	#error Unknown architecture
#endif

/* USART register definitions */
#if CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	#define AVR_HAS_UART1 1
	#define AVR_HAS_UART2 1
	#define AVR_HAS_UART3 1
#elif CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281
	#define AVR_HAS_UART1 1
	#define AVR_HAS_UART2 0
	#define AVR_HAS_UART3 0
#elif CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P
	#define AVR_HAS_UART1 0
	#define AVR_HAS_UART2 0
	#define AVR_HAS_UART3 0
	#define USART0_UDRE_vect USART_UDRE_vect
	#define USART0_RX_vect USART_RX_vect
	#define USART0_TX_vect USART_TX_vect
#elif CPU_AVR_ATMEGA8 || CPU_AVR_ATMEGA32
	#define AVR_HAS_UART1 0
	#define AVR_HAS_UART2 0
	#define AVR_HAS_UART3 0
	#define UCSR0A UCSRA
	#define UCSR0B UCSRB
	#define UCSR0C UCSRC
	#define UDR0   UDR
	#define UBRR0L UBRRL
	#define UBRR0H UBRRH
	#define UPM01  UPM1
	#define UPM00  UPM0
	#define USART0_UDRE_vect USART_UDRE_vect
	#define USART0_RX_vect USART_RXC_vect
	#define USART0_TX_vect USART_TXC_vect
#elif CPU_AVR_ATMEGA103
	#define AVR_HAS_UART1 0
	#define AVR_HAS_UART2 0
	#define AVR_HAS_UART3 0
	#define UCSR0B UCR
	#define UDR0   UDR
	#define UCSR0A USR
	#define UBRR0L UBRR
	#define USART0_UDRE_vect USART_UDRE_vect
	#define USART0_RX_vect USART_RX_vect
	#define USART0_TX_vect USART_TX_vect
#else
	#error Unknown architecture
#endif


/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
#if AVR_HAS_UART1
	static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
	static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];
#endif
#if AVR_HAS_UART2
	static unsigned char uart2_txbuffer[CONFIG_UART2_TXBUFSIZE];
	static unsigned char uart2_rxbuffer[CONFIG_UART2_RXBUFSIZE];
#endif
#if AVR_HAS_UART3
	static unsigned char uart3_txbuffer[CONFIG_UART3_TXBUFSIZE];
	static unsigned char uart3_rxbuffer[CONFIG_UART3_RXBUFSIZE];
#endif
static unsigned char spi_txbuffer[CONFIG_SPI_TXBUFSIZE];
static unsigned char spi_rxbuffer[CONFIG_SPI_RXBUFSIZE];


/**
 * Internal hardware state structure
 *
 * The \a sending variable is true while the transmission
 * interrupt is retriggering itself.
 *
 * For the USARTs the \a sending flag is useful for taking specific
 * actions before sending a burst of data, at the start of a trasmission
 * but not before every char sent.
 *
 * For the SPI, this flag is necessary because the SPI sends and receives
 * bytes at the same time and the SPI IRQ is unique for send/receive.
 * The only way to start transmission is to write data in SPDR (this
 * is done by spi_starttx()). We do this *only* if a transfer is
 * not already started.
 */
struct AvrSerial
{
	struct SerialHardware hw;
	volatile bool sending;
};

static uint16_t uart_period(unsigned long bps)
{
	uint16_t period = DIV_ROUND(CPU_FREQ / 16UL, bps) - 1;

	#ifdef _DEBUG
		//long skew = bps - (long)(period + 1) * (CPU_FREQ / 16);
		/* 8N1 is reliable within 3% skew */
		//if ((unsigned long)ABS(skew) > bps / (100 / 3)) kprintf("Baudrate off by %ldbps\n", skew);
	#endif

	//DB(kprintf("uart_period(bps=%lu): period=%u\n", bps, period);)
	return period;
}

/*
 * Callbacks
 */
static void uart0_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART0_BUS_TXINIT;
	RTS_ON;
	SER_STROBE_INIT;
}

static void uart0_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	UCSR0B = 0;
}

static void uart0_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt sets hw->sending to false
	 * when it runs with an empty fifo.  The order of statements in the
	 * if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART0_BUS_TXBEGIN;
	}
}

static void uart0_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	uint16_t period = uart_period(rate);

#if !CPU_AVR_ATMEGA103
	UBRR0H = period >> 8;
#endif
	UBRR0L = period;
}

static void uart0_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
#if !CPU_AVR_ATMEGA103
	UCSR0C = (UCSR0C & ~(BV(UPM01) | BV(UPM00))) | ((parity) << UPM00);
#endif
}

#if AVR_HAS_UART1

static void uart1_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART1_BUS_TXINIT;
	RTS_ON;
	SER_STROBE_INIT;
}

static void uart1_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	UCSR1B = 0;
}

static void uart1_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt
	 * sets hw->sending to false when it runs with
	 * an empty fifo.  The order of the statements
	 * in the if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART1_BUS_TXBEGIN;
	}
}

static void uart1_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	uint16_t period = uart_period(rate);
	UBRR1H = period >> 8;
	UBRR1L = period;
}

static void uart1_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	UCSR1C = (UCSR1C & ~(BV(UPM11) | BV(UPM10))) | ((parity) << UPM10);
}

#endif // AVR_HAS_UART1

#if AVR_HAS_UART2

static void uart2_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART2_BUS_TXINIT;
	RTS_ON;
	SER_STROBE_INIT;
}

static void uart2_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	UCSR2B = 0;
}

static void uart2_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt
	 * sets hw->sending to false when it runs with
	 * an empty fifo.  The order of the statements
	 * in the if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART2_BUS_TXBEGIN;
	}
}

static void uart2_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	uint16_t period = uart_period(rate);
	UBRR2H = period >> 8;
	UBRR2L = period;
}

static void uart2_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	UCSR2C = (UCSR2C & ~(BV(UPM21) | BV(UPM20))) | ((parity) << UPM20);
}

#endif // AVR_HAS_UART2

#if AVR_HAS_UART3

static void uart3_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART3_BUS_TXINIT;
	RTS_ON;
	SER_STROBE_INIT;
}

static void uart3_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	UCSR3B = 0;
}

static void uart3_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt
	 * sets hw->sending to false when it runs with
	 * an empty fifo.  The order of the statements
	 * in the if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART3_BUS_TXBEGIN;
	}
}

static void uart3_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	uint16_t period = uart_period(rate);
	UBRR3H = period >> 8;
	UBRR3L = period;
}

static void uart3_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	UCSR3C = (UCSR3C & ~(BV(UPM31) | BV(UPM30))) | ((parity) << UPM30);
}

#endif // AVR_HAS_UART3


static void spi_init(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(struct Serial *, ser))
{
	/*
	 * Set MOSI and SCK ports out, MISO in.
	 *
	 * The ATmega64/128 datasheet explicitly states that the input/output
	 * state of the SPI pins is not significant, as when the SPI is
	 * active the I/O port are overrided.
	 * This is *blatantly FALSE*.
	 *
	 * Moreover, the MISO pin on the board_kc *must* be in high impedance
	 * state even when the SPI is off, because the line is wired together
	 * with the KBus serial RX, and the transmitter of the slave boards
	 * would be unable to drive the line.
	 */
	ATOMIC(SPI_DDR |= (BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT)));

	/*
	 * If the SPI master mode is activated and the SS pin is in input and tied low,
	 * the SPI hardware will automatically switch to slave mode!
	 * For proper communication this pins should therefore be:
	 * - as output
	 * - as input but tied high forever!
	 * This driver set the pin as output.
	 */

	// Squelching this warning
	// #warning FIXME:SPI SS pin set as output for proper operation, check schematics for possible conflicts.
	ATOMIC(SPI_DDR |= BV(SPI_SS_BIT));

	ATOMIC(SPI_DDR &= ~BV(SPI_MISO_BIT));
	/* Enable SPI, IRQ on, Master */
	SPCR = BV(SPE) | BV(SPIE) | BV(MSTR);

	/* Set data order */
	#if CONFIG_SPI_DATA_ORDER == SER_LSB_FIRST
		SPCR |= BV(DORD);
	#endif

	/* Set SPI clock rate */
	#if CONFIG_SPI_CLOCK_DIV == 128
		SPCR |= (BV(SPR1) | BV(SPR0));
	#elif (CONFIG_SPI_CLOCK_DIV == 64 || CONFIG_SPI_CLOCK_DIV == 32)
		SPCR |= BV(SPR1);
	#elif (CONFIG_SPI_CLOCK_DIV == 16 || CONFIG_SPI_CLOCK_DIV == 8)
		SPCR |= BV(SPR0);
	#elif (CONFIG_SPI_CLOCK_DIV == 4 || CONFIG_SPI_CLOCK_DIV == 2)
		// SPR0 & SDPR1 both at 0
	#else
		#error Unsupported SPI clock division factor.
	#endif

	/* Set SPI2X bit (spi double frequency) */
	#if (CONFIG_SPI_CLOCK_DIV == 128 || CONFIG_SPI_CLOCK_DIV == 64 \
	  || CONFIG_SPI_CLOCK_DIV == 16 || CONFIG_SPI_CLOCK_DIV == 4)
		SPSR &= ~BV(SPI2X);
	#elif (CONFIG_SPI_CLOCK_DIV == 32 || CONFIG_SPI_CLOCK_DIV == 8 || CONFIG_SPI_CLOCK_DIV == 2)
		SPSR |= BV(SPI2X);
	#else
		#error Unsupported SPI clock division factor.
	#endif

	/* Set clock polarity */
	#if CONFIG_SPI_CLOCK_POL == 1
		SPCR |= BV(CPOL);
	#endif

	/* Set clock phase */
	#if CONFIG_SPI_CLOCK_PHASE == 1
		SPCR |= BV(CPHA);
	#endif
	SER_SPI_BUS_TXINIT;

	SER_STROBE_INIT;
}

static void spi_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	SPCR = 0;

	SER_SPI_BUS_TXCLOSE;

	/* Set all pins as inputs */
	ATOMIC(SPI_DDR &= ~(BV(SPI_MISO_BIT) | BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT) | BV(SPI_SS_BIT)));
}

static void spi_starttx(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!hw->sending && !fifo_isempty(&ser_handles[SER_SPI]->txfifo))
	{
		hw->sending = true;
		SPDR = fifo_pop(&ser_handles[SER_SPI]->txfifo);
	}

	IRQ_RESTORE(flags);
}

static void spi_setbaudrate(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(unsigned long, rate))
{
	// nop
}

static void spi_setparity(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(int, parity))
{
	// nop
}

static bool tx_sending(struct SerialHardware* _hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;
	return hw->sending;
}



// FIXME: move into compiler.h?  Ditch?
#if COMPILER_C99
	#define	C99INIT(name,val) .name = val
#elif defined(__GNUC__)
	#define C99INIT(name,val) name: val
#else
	#warning No designated initializers, double check your code
	#define C99INIT(name,val) (val)
#endif

/*
 * High-level interface data structures
 */
static const struct SerialHardwareVT UART0_VT =
{
	C99INIT(init, uart0_init),
	C99INIT(cleanup, uart0_cleanup),
	C99INIT(setBaudrate, uart0_setbaudrate),
	C99INIT(setParity, uart0_setparity),
	C99INIT(txStart, uart0_enabletxirq),
	C99INIT(txSending, tx_sending),
};

#if AVR_HAS_UART1
static const struct SerialHardwareVT UART1_VT =
{
	C99INIT(init, uart1_init),
	C99INIT(cleanup, uart1_cleanup),
	C99INIT(setBaudrate, uart1_setbaudrate),
	C99INIT(setParity, uart1_setparity),
	C99INIT(txStart, uart1_enabletxirq),
	C99INIT(txSending, tx_sending),
};
#endif // AVR_HAS_UART1

#if AVR_HAS_UART2
static const struct SerialHardwareVT UART2_VT =
{
	C99INIT(init, uart2_init),
	C99INIT(cleanup, uart2_cleanup),
	C99INIT(setBaudrate, uart2_setbaudrate),
	C99INIT(setParity, uart2_setparity),
	C99INIT(txStart, uart2_enabletxirq),
	C99INIT(txSending, tx_sending),
};
#endif // AVR_HAS_UART2

#if AVR_HAS_UART3
static const struct SerialHardwareVT UART3_VT =
{
	C99INIT(init, uart3_init),
	C99INIT(cleanup, uart3_cleanup),
	C99INIT(setBaudrate, uart3_setbaudrate),
	C99INIT(setParity, uart3_setparity),
	C99INIT(txStart, uart3_enabletxirq),
	C99INIT(txSending, tx_sending),
};
#endif // AVR_HAS_UART3

static const struct SerialHardwareVT SPI_VT =
{
	C99INIT(init, spi_init),
	C99INIT(cleanup, spi_cleanup),
	C99INIT(setBaudrate, spi_setbaudrate),
	C99INIT(setParity, spi_setparity),
	C99INIT(txStart, spi_starttx),
	C99INIT(txSending, tx_sending),
};

static struct AvrSerial UARTDescs[SER_CNT] =
{
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART0_VT),
			C99INIT(txbuffer, uart0_txbuffer),
			C99INIT(rxbuffer, uart0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart0_rxbuffer)),
		},
		C99INIT(sending, false),
	},
#if AVR_HAS_UART1
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART1_VT),
			C99INIT(txbuffer, uart1_txbuffer),
			C99INIT(rxbuffer, uart1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart1_rxbuffer)),
		},
		C99INIT(sending, false),
	},
#endif
#if AVR_HAS_UART2
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART2_VT),
			C99INIT(txbuffer, uart2_txbuffer),
			C99INIT(rxbuffer, uart2_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart2_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart2_rxbuffer)),
		},
		C99INIT(sending, false),
	},
#endif
#if AVR_HAS_UART3
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART3_VT),
			C99INIT(txbuffer, uart3_txbuffer),
			C99INIT(rxbuffer, uart3_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart3_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart3_rxbuffer)),
		},
		C99INIT(sending, false),
	},
#endif
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &SPI_VT),
			C99INIT(txbuffer, spi_txbuffer),
			C99INIT(rxbuffer, spi_rxbuffer),
			C99INIT(txbuffer_size, sizeof(spi_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(spi_rxbuffer)),
		},
		C99INIT(sending, false),
	}
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}


/*
 * Interrupt handlers
 */

#if CONFIG_SER_HWHANDSHAKE

/// This interrupt is triggered when the CTS line goes high
DECLARE_ISR(SIG_CTS)
{
	// Re-enable UDR empty interrupt and TX, then disable CTS interrupt
	UCSR0B = BV(BIT_RXCIE0) | BV(BIT_UDRIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0);
	EIMSK &= ~EIMSKF_CTS;
}

#endif // CONFIG_SER_HWHANDSHAKE


/**
 * Serial 0 TX interrupt handler
 */
DECLARE_ISR(USART0_UDRE_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART0]->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART0_BUS_TXEND;
#ifndef SER_UART0_BUS_TXOFF
		UARTDescs[SER_UART0].sending = false;
#endif
	}
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA103
	else if (!IS_CTS_ON)
	{
		// Disable rx interrupt and tx, enable CTS interrupt
		// UNTESTED
		UCSR0B = BV(BIT_RXCIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0);
		EIFR |= EIMSKF_CTS;
		EIMSK |= EIMSKF_CTS;
	}
#endif
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART0_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART0_BUS_TXOFF
/**
 * Serial port 0 TX complete interrupt handler.
 *
 * This IRQ is usually disabled.  The UDR-empty interrupt
 * enables it when there's no more data to transmit.
 * We need to wait until the last character has been
 * transmitted before switching the 485 transceiver to
 * receive mode.
 *
 * The txfifo might have been refilled by putchar() while
 * we were waiting for the transmission complete interrupt.
 * In this case, we must restart the UDR empty interrupt,
 * otherwise we'd stop the serial port with some data
 * still pending in the buffer.
 */
DECLARE_ISR(USART0_TX_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART0]->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART0_BUS_TXOFF;
		UARTDescs[SER_UART0].sending = false;
	}
	else
		UCSR0B = BV(BIT_RXCIE0) | BV(BIT_UDRIE0) | BV(BIT_RXEN0) | BV(BIT_TXEN0);

	SER_STROBE_OFF;
}
#endif /* SER_UART0_BUS_TXOFF */


#if AVR_HAS_UART1

/**
 * Serial 1 TX interrupt handler
 */
DECLARE_ISR(USART1_UDRE_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART1]->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART1_BUS_TXEND;
#ifndef SER_UART1_BUS_TXOFF
		UARTDescs[SER_UART1].sending = false;
#endif
	}
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA103
	else if (!IS_CTS_ON)
	{
		// Disable rx interrupt and tx, enable CTS interrupt
		// UNTESTED
		UCSR1B = BV(BIT_RXCIE1) | BV(BIT_RXEN1) | BV(BIT_TXEN1);
		EIFR |= EIMSKF_CTS;
		EIMSK |= EIMSKF_CTS;
	}
#endif
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART1_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART1_BUS_TXOFF
/**
 * Serial port 1 TX complete interrupt handler.
 *
 * \sa port 0 TX complete handler.
 */
DECLARE_ISR(USART1_TX_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART1]->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART1_BUS_TXOFF;
		UARTDescs[SER_UART1].sending = false;
	}
	else
		UCSR1B = BV(BIT_RXCIE1) | BV(BIT_UDRIE1) | BV(BIT_RXEN1) | BV(BIT_TXEN1);

	SER_STROBE_OFF;
}
#endif /* SER_UART1_BUS_TXOFF */

#endif // AVR_HAS_UART1

#if AVR_HAS_UART2

/**
 * Serial 2 TX interrupt handler
 */
DECLARE_ISR(USART2_UDRE_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART2]->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART2_BUS_TXEND;
#ifndef SER_UART2_BUS_TXOFF
		UARTDescs[SER_UART2].sending = false;
#endif
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART2_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART2_BUS_TXOFF
/**
 * Serial port 2 TX complete interrupt handler.
 *
 * \sa port 0 TX complete handler.
 */
DECLARE_ISR(USART2_TX_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART2]->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART2_BUS_TXOFF;
		UARTDescs[SER_UART2].sending = false;
	}
	else
		UCSR2B = BV(BIT_RXCIE2) | BV(BIT_UDRIE2) | BV(BIT_RXEN2) | BV(BIT_TXEN2);

	SER_STROBE_OFF;
}
#endif /* SER_UART2_BUS_TXOFF */

#endif // AVR_HAS_UART2

#if AVR_HAS_UART3

/**
 * Serial 3 TX interrupt handler
 */
DECLARE_ISR(USART3_UDRE_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART3]->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART3_BUS_TXEND;
#ifndef SER_UART3_BUS_TXOFF
		UARTDescs[SER_UART3].sending = false;
#endif
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART3_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART3_BUS_TXOFF
/**
 * Serial port 3 TX complete interrupt handler.
 *
 * \sa port 0 TX complete handler.
 */
DECLARE_ISR(USART3_TX_vect)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART3]->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART3_BUS_TXOFF;
		UARTDescs[SER_UART3].sending = false;
	}
	else
		UCSR3B = BV(BIT_RXCIE3) | BV(BIT_UDRIE3) | BV(BIT_RXEN3) | BV(BIT_TXEN3);

	SER_STROBE_OFF;
}
#endif /* SER_UART3_BUS_TXOFF */

#endif // AVR_HAS_UART3


/**
 * Serial 0 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \note The code that re-enables interrupts is commented out
 *       because in some nasty cases the interrupt is retriggered.
 *       This is probably due to the RXC flag being set before
 *       RXCIE is cleared.  Unfortunately the RXC flag is read-only
 *       and can't be cleared by code.
 */
DECLARE_ISR(USART0_RX_vect)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR0B &= ~BV(RXCIE);
	//IRQ_ENABLE;

	/* Should be read before UDR */
	ser_handles[SER_UART0]->status |= UCSR0A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To clear the RXC flag we must _always_ read the UDR even when we're
	 * not going to accept the incoming data, otherwise a new interrupt
	 * will occur once the handler terminates.
	 */
	char c = UDR0;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART0]->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_handles[SER_UART0]->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}

	/* Reenable receive complete int */
	//IRQ_DISABLE;
	//UCSR0B |= BV(RXCIE);

	SER_STROBE_OFF;
}


#if AVR_HAS_UART1

/**
 * Serial 1 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \see DECLARE_ISR(USART1_RX_vect)
 */
DECLARE_ISR(USART1_RX_vect)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR1B &= ~BV(RXCIE);
	//IRQ_ENABLE;

	/* Should be read before UDR */
	ser_handles[SER_UART1]->status |= UCSR1A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To avoid an IRQ storm, we must _always_ read the UDR even when we're
	 * not going to accept the incoming data
	 */
	char c = UDR1;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART1]->rxfifo;
	//ASSERT_VALID_FIFO(rxfifo);

	if (UNLIKELY(fifo_isfull(rxfifo)))
		ser_handles[SER_UART1]->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}
	/* Re-enable receive complete int */
	//IRQ_DISABLE;
	//UCSR1B |= BV(RXCIE);

	SER_STROBE_OFF;
}

#endif // AVR_HAS_UART1

#if AVR_HAS_UART2

/**
 * Serial 2 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \see DECLARE_ISR(USART2_RX_vect)
 */
DECLARE_ISR(USART2_RX_vect)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR1B &= ~BV(RXCIE);
	//IRQ_ENABLE;

	/* Should be read before UDR */
	ser_handles[SER_UART2]->status |= UCSR2A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To avoid an IRQ storm, we must _always_ read the UDR even when we're
	 * not going to accept the incoming data
	 */
	char c = UDR2;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART2]->rxfifo;
	//ASSERT_VALID_FIFO(rxfifo);

	if (UNLIKELY(fifo_isfull(rxfifo)))
		ser_handles[SER_UART2]->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}
	/* Re-enable receive complete int */
	//IRQ_DISABLE;
	//UCSR1B |= BV(RXCIE);

	SER_STROBE_OFF;
}

#endif // AVR_HAS_UART2

#if AVR_HAS_UART3

/**
 * Serial 3 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \see DECLARE_ISR(USART3_RX_vect)
 */
DECLARE_ISR(USART3_RX_vect)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR1B &= ~BV(RXCIE);
	//IRQ_ENABLE;

	/* Should be read before UDR */
	ser_handles[SER_UART3]->status |= UCSR3A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To avoid an IRQ storm, we must _always_ read the UDR even when we're
	 * not going to accept the incoming data
	 */
	char c = UDR3;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART3]->rxfifo;
	//ASSERT_VALID_FIFO(rxfifo);

	if (UNLIKELY(fifo_isfull(rxfifo)))
		ser_handles[SER_UART3]->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}
	/* Re-enable receive complete int */
	//IRQ_DISABLE;
	//UCSR1B |= BV(RXCIE);

	SER_STROBE_OFF;
}

#endif // AVR_HAS_UART3


/**
 * SPI interrupt handler
 */
DECLARE_ISR(SPI_STC_vect)
{
	SER_STROBE_ON;

	/* Read incoming byte. */
	if (!fifo_isfull(&ser_handles[SER_SPI]->rxfifo))
		fifo_push(&ser_handles[SER_SPI]->rxfifo, SPDR);
	/*
	 * FIXME
	else
		ser_handles[SER_SPI]->status |= SERRF_RXFIFOOVERRUN;
	*/

	/* Send */
	if (!fifo_isempty(&ser_handles[SER_SPI]->txfifo))
		SPDR = fifo_pop(&ser_handles[SER_SPI]->txfifo);
	else
		UARTDescs[SER_SPI].sending = false;

	SER_STROBE_OFF;
}
