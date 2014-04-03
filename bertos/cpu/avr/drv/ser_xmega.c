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
 * Copyright 2011 Onno <developer@gorgoz.org>
 *
 * -->
 *
 * \brief AVR XMEGA USART driver (Implementation)
 *
 * This file is heavily inspired by the AVR implementation for BeRTOS,
 * but uses a different approach for implementing the different debug
 * ports, by using the USART_t structs.
 *
 * \author Onno <developer@gorgoz.org>
 * notest:all
 */

#include "hw/hw_ser.h"     /* Required for bus macros overrides */
#include <hw/hw_cpufreq.h> /* CPU_FREQ */

#include "cfg/cfg_ser.h"   /* Serialport configuration settings */

#include <cfg/macros.h>    /* DIV_ROUND */
#include <cfg/debug.h>     /* debug configuration */

#include <drv/ser.h>
#include <drv/ser_p.h>
#include <drv/timer.h>

#include <struct/fifobuf.h>

#include <avr/io.h>        /* AVR IO ports and structures */
#include <avr/interrupt.h> /* AVR Interrupt methods */

/*
 * Scalefactor to use for computing the baudrate
 * this scalefactor should be an integer value between -7
 * and 7
 */
#ifndef USART_SCALE_FACTOR
	#define USART_SCALE_FACTOR (-7)
#else
	#if USART_SCALE_FACTOR > 7 || USART_SCALE_FACTOR < -7
		#error USART_SCALE_FACTOR should be an integer between -7 and 7
	#endif
#endif

/* Helper macros, mostly taken from the Atmel Examples
 * Slightly alterd to match the BeRTOS naming convention
 */

/* \brief Set USART baud rate.
 *
 *  Sets the USART's baud rate register.
 *
 *  UBRR_Value   : Value written to UBRR
 *  ScaleFactor  : Time Base Generator Scale Factor
 *
 *  Equation for calculation of BSEL value in asynchronous normal speed mode:
 *  	If ScaleFactor >= 0
 *  		BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
 *  	If ScaleFactor < 0
 *  		BSEL = (1/(2^(ScaleFactor)*16))*(((I/O clock frequency)/Baudrate)-1)
 *
 *	\note See XMEGA manual for equations for calculation of BSEL value in other
 *        modes.
 *
 *  \param _usart          Pointer to the USART module.
 *  \param _bselValue      Value to write to BSEL part of Baud control register.
 *                         Use uint16_t type.
 *  \param _bScaleFactor   USART baud rate scale factor.
 *                         Use uint8_t type
 */
#define USART_SET_BAUDRATE(_usart, _bselValue, _bScaleFactor)                  \
	(_usart)->BAUDCTRLA =(uint8_t)_bselValue;                                           \
	(_usart)->BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)

/* \brief Enable USART receiver.
 *
 *  \param _usart    Pointer to the USART module
 */
#define USART_RX_ENABLE(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)

/* \brief Disable USART receiver.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_RX_DISABLE(_usart) ((_usart)->CTRLB &= ~USART_RXEN_bm)

/* \brief Enable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_TX_ENABLE(_usart)	((_usart)->CTRLB |= USART_TXEN_bm)

/* \brief Disable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_TX_DISABLE(_usart) ((_usart)->CTRLB &= ~USART_TXEN_bm)

/* \brief Set USART RXD interrupt level.
 *
 *  Sets the interrupt level on RX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _rxdIntLevel  Interrupt level of the RXD interrupt.
 *                       Use USART_RXCINTLVL_t type.
 */
#define USART_SET_RX_INTERRUPT_LEVEL(_usart, _rxdIntLevel)                      \
	((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)

/* \brief Set USART TXD interrupt level.
 *
 *  Sets the interrupt level on TX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _txdIntLevel  Interrupt level of the TXD interrupt.
 *                       Use USART_TXCINTLVL_t type.
 */
#define USART_SET_TX_INTERRUPT_LEVEL(_usart, _txdIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel

/* \brief Set USART DRE interrupt level.
 *
 *  Sets the interrupt level on Data Register interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _dreIntLevel  Interrupt level of the DRE interrupt.
 *                       Use USART_DREINTLVL_t type.
 */
#define USART_SET_DRE_INTERRUPT_LEVEL(_usart, _dreIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_DREINTLVL_gm) | _dreIntLevel

/* \brief Set the mode the USART run in.
 *
 * Set the mode the USART run in. The default mode is asynchronous mode.
 *
 *  \param  _usart       Pointer to the USART module register section.
 *  \param  _usartMode   Selects the USART mode. Use  USART_CMODE_t type.
 *
 *  USART modes:
 *  - 0x0        : Asynchronous mode.
 *  - 0x1        : Synchronous mode.
 *  - 0x2        : IrDA mode.
 *  - 0x3        : Master SPI mode.
 */
#define USART_SET_MODE(_usart, _usartMode)                                      \
	((_usart)->CTRLC = ((_usart)->CTRLC & (~USART_CMODE_gm)) | _usartMode)

/* \brief Check if data register empty flag is set.
 *
 *  \param _usart      The USART module.
 */
#define USART_IS_TX_DATA_REGISTER_EMPTY(_usart) (((_usart)->STATUS & USART_DREIF_bm) != 0)

/* \brief Put data (5-8 bit character).
 *
 *  Use the macro USART_IsTXDataRegisterEmpty before using this function to
 *  put data to the TX register.
 *
 *  \param _usart      The USART module.
 *  \param _data       The data to send.
 */
#define USART_PUT_CHAR(_usart, _data) ((_usart)->DATA = _data)

/* \brief Checks if the RX complete interrupt flag is set.
 *
 *   Checks if the RX complete interrupt flag is set.
 *
 *  \param _usart     The USART module.
 */
#define USART_IS_RX_COMPLETE(_usart) (((_usart)->STATUS & USART_RXCIF_bm) != 0)

/* \brief Get received data (5-8 bit character).
 *
 *  This macro reads out the RX register.
 *  Use the macro USART_RX_Complete to check if anything is received.
 *
 *  \param _usart     The USART module.
 *
 *  \retval           Received data.
 */
#define USART_GET_CHAR(_usart)  ((_usart)->DATA)

/* configurable macros */

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

/*
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

#ifndef SER_UART_BUS_TXINIT
	/*
	 * Default TXINIT macro - invoked in uart_init()
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	#define SER_UART_BUS_TXINIT(_usart) do { \
		USART_RX_ENABLE(_usart); \
		USART_TX_ENABLE(_usart); \
		USART_SET_RX_INTERRUPT_LEVEL(_usart, USART_RXCINTLVL_MED_gc); \
	} while (0)
#endif

#ifndef SER_UART_BUS_TXBEGIN
	/*
	 * Invoked before starting a transmission
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable both the RX complete and UDR empty interrupts
	 */
	#define SER_UART_BUS_TXBEGIN(_usart) do { \
		USART_SET_RX_INTERRUPT_LEVEL(_usart, USART_RXCINTLVL_MED_gc); \
		USART_SET_DRE_INTERRUPT_LEVEL(_usart, USART_DREINTLVL_MED_gc);\
	} while (0)
#endif

#ifndef SER_UART_BUS_TXCHAR
	/*
	 * Invoked to send one character.
	 */
	#define SER_UART_BUS_TXCHAR(_usart, c) do { \
		USART_PUT_CHAR(_usart, c); \
	} while (0)
#endif

#ifndef SER_UART_BUS_TXEND
	/*
	 * Invoked as soon as the txfifo becomes empty
	 *
	 * - Keep both the receiver and the transmitter enabled
	 * - Keep the RX complete interrupt enabled
	 * - Disable the UDR empty interrupt
	 */
	#define SER_UART_BUS_TXEND(_usart) do { \
		USART_SET_DRE_INTERRUPT_LEVEL(_usart, USART_DREINTLVL_OFF_gc); \
	} while (0)
#endif

#ifndef SER_UART_BUS_TXOFF
	/*
	 * \def SER_UART_BUS_TXOFF
	 *
	 * Invoked after the last character has been transmitted
	 *
	 * The default is no action.
	 */
	#ifdef __doxygen__
	#define SER_UART_BUS_TXOFF(_usart)
	#endif
#endif

/*\}*/

/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];
#ifdef CPU_AVR_XMEGA_A
static unsigned char uart2_txbuffer[CONFIG_UART2_TXBUFSIZE];
static unsigned char uart2_rxbuffer[CONFIG_UART2_RXBUFSIZE];
static unsigned char uart3_txbuffer[CONFIG_UART3_TXBUFSIZE];
static unsigned char uart3_rxbuffer[CONFIG_UART3_RXBUFSIZE];
static unsigned char uart4_txbuffer[CONFIG_UART4_TXBUFSIZE];
static unsigned char uart4_rxbuffer[CONFIG_UART4_RXBUFSIZE];
#endif

/*
 * Internal hardware state structure
 *
 * The \a sending variable is true while the transmission
 * interrupt is retriggering itself.
 *
 * the \a usart variable will point to the USART_t structure
 * that should be used.
 *
 * the \a port variable will point to the PORT_t structure
 * that should be modified to set the tx pin as an output and the
 * rx pin as an input
 *
 * the \a txpin variable will hold the pinnumber of the pin to use
 * as the tx output
 *
 * the \a rxpin variable will hold the pinnumber of the pin to use
 * as the rx input
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
struct AvrxmegaSerial
{
	struct SerialHardware hw;
	volatile bool sending;
	volatile USART_t* usart;
	volatile PORT_t* port;
	uint8_t	txpin;
	uint8_t rxpin;
};

/*
 * Callbacks
 * The same callbacks are used for all USARTS.
 * By casting the SerialHardware structure to the AvrxmegaSerial
 * structure a pointer to the USART_t structure can be obtained,
 * to perform the callback for the specific USART.
 * This methode might cost some more cpu time, but saves on
 * code duplication and code size.
 */


/*
 * \brief Initializes the uart
 *
 * The TX pin of the uart will be set as an outputpin
 * The RX pin of the uart will be set as an inputpin
 * The usart will be initialized
 * \see SER_UART_BUS_TXINIT
 *
 * \param _hw struct AvrxmegaSerial
 * \param ser Unused
 */
static void uart_init(struct SerialHardware * _hw, UNUSED_ARG(struct Serial *, ser))
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;
	//set transmit pin as output
	hw->port->DIRSET = BV(hw->txpin);
	hw->port->OUTCLR = BV(hw->txpin);
	//set receive pin as input
	hw->port->DIRCLR = BV(hw->rxpin);
	//initialize the USART
	SER_UART_BUS_TXINIT(hw->usart);
	RTS_ON;
	SER_STROBE_INIT;
}

/*
 * \brief Cleans up / Disables the uart
 *
 * \param _hw struct AvrxmegaSerial
 */
static void uart_cleanup(struct SerialHardware * _hw)
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;
	hw->usart->CTRLA = 0;
	hw->usart->CTRLB = 0;
}

/*
 * \brief Enableds the TX interrupt
 *
 * \param _hw struct AvrxmegaSerial
 */
static void uart_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt sets hw->sending to false
	 * when it runs with an empty fifo.  The order of statements in the
	 * if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART_BUS_TXBEGIN(hw->usart);
	}
}

/*
 * \brief  sets the uart to the provided baudrate
 *
 * For setting the baudrate an scale factor (bscale) and a period
 * setting (BSEL) is required.
 *
 * The scale factor should be privided by defining USART_SCALE_FACTOR
 *
 * Atmel specifies BSEL for normal speed mode and bscale >= 0 as:
 * BSEL = (cpu_freq / ((2^bscale) * 16 * rate)) - 1
 * To allow BSEL to be calculated with an power function this can be
 * rewriten to:
 * BSEL = BSEL = (cpu_freq / ((1 << bscale) * 16 * rate)) - 1
 *
 * Atmel specifies BSEL for normal speed mode and bscale < 0 as:
 * BSEL = (1 / (2^bscale)) * ( (cpu_freq / (16 * rate)) - 1)
 * To calculte this float atheritmic is required as the second product will be smaller
 * than zero in a lot of cases.
 * To allow BSEL to be calculated with interger devision and no power function
 * this can be rewriten by folowing simple math rules to:
 * BSEL = ((1 << -bscale) * (cpu_freq - (16 * rate)) / (16 * rate)
 *
 * \param _hw struct AvrxmegaSerial
 * \param _rate the required baudrate
 *
 */
static void uart_setbaudrate(struct SerialHardware * _hw, unsigned long _rate)
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;
	/* Compute baud-rate period, this requires a valid USART_SCALE_FACTOR */
	#if USART_SCALE_FACTOR < 0
		uint16_t bsel = DIV_ROUND((1 << (-(USART_SCALE_FACTOR))) * (CPU_FREQ - (16 * _rate)), 16 * _rate);
	#else
		uint16_t bsel = DIV_ROUND(CPU_FREQ, (1 << (USART_SCALE_FACTOR)) * 16 * _rate) - 1;
	#endif
	USART_SET_BAUDRATE(hw->usart, bsel, USART_SCALE_FACTOR);
}

/*
 * \brief Sets the parity of the uart
 *
 * \param _hw struct AvrxmegaSerial
 * \param _parity the parity to set
 */
static void uart_setparity(struct SerialHardware * _hw, int _parity)
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;
	USART_SET_MODE(hw->usart, _parity);
}

/*
 * \brief Returns true if Transmitter is sending
 *
 * \param _hw struct AvrxmegaSerial
 * \return true if transmitter is sending
 */
static bool tx_sending(struct SerialHardware* _hw)
{
	struct AvrxmegaSerial *hw = (struct AvrxmegaSerial *)_hw;
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
static const struct SerialHardwareVT UART_VT =
{
	C99INIT(init, uart_init),
	C99INIT(cleanup, uart_cleanup),
	C99INIT(setBaudrate, uart_setbaudrate),
	C99INIT(setParity, uart_setparity),
	C99INIT(txStart, uart_enabletxirq),
	C99INIT(txSending, tx_sending)
};

static struct AvrxmegaSerial UARTDescs[SER_CNT] =
{
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART_VT),
			C99INIT(txbuffer, uart0_txbuffer),
			C99INIT(rxbuffer, uart0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart0_rxbuffer)),
		},
		C99INIT(sending, false),
		C99INIT(usart, &USARTC0),
		C99INIT(port, &PORTC),
		C99INIT(txpin, PIN3_bp),
		C99INIT(rxpin, PIN2_bp),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART_VT),
			C99INIT(txbuffer, uart1_txbuffer),
			C99INIT(rxbuffer, uart1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart1_rxbuffer)),
		},
		C99INIT(sending, false),
		C99INIT(usart, &USARTD0),
		C99INIT(port, &PORTD),
		C99INIT(txpin, PIN3_bp),
		C99INIT(rxpin, PIN2_bp),
	},
#ifdef CPU_AVR_XMEGA_A
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART_VT),
			C99INIT(txbuffer, uart2_txbuffer),
			C99INIT(rxbuffer, uart2_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart2_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart2_rxbuffer)),
		},
		C99INIT(sending, false),
		C99INIT(usart, &USARTC1),
		C99INIT(port, &PORTC),
		C99INIT(txpin, PIN7_bp),
		C99INIT(rxpin, PIN6_bp),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART_VT),
			C99INIT(txbuffer, uart3_txbuffer),
			C99INIT(rxbuffer, uart3_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart3_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart3_rxbuffer)),
		},
		C99INIT(sending, false),
		C99INIT(usart, &USARTD1),
		C99INIT(port, &PORTD),
		C99INIT(txpin, PIN7_bp),
		C99INIT(rxpin, PIN6_bp),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART_VT),
			C99INIT(txbuffer, uart4_txbuffer),
			C99INIT(rxbuffer, uart4_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart4_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart4_rxbuffer)),
		},
		C99INIT(sending, false),
		C99INIT(usart, &USARTE0),
		C99INIT(port, &PORTE),
		C99INIT(txpin, PIN3_bp),
		C99INIT(rxpin, PIN2_bp),
	},
#endif //CPU_AVR_XMEGA_A
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}


/*
 * Interrupt handlers
 */
static inline void usart_handleDreInterrupt(uint8_t usartNumber)
{
	SER_STROBE_ON;
	struct FIFOBuffer * const txfifo = &ser_handles[usartNumber]->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART_BUS_TXEND(UARTDescs[usartNumber].usart);
		#ifndef SER_UART_BUS_TXOFF
			UARTDescs[usartNumber].sending = false;
		#endif
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART_BUS_TXCHAR(UARTDescs[usartNumber].usart, c);
	}
	SER_STROBE_OFF;
}

#define USART_DRE_INTERRUPT_VECTOR(_vector, _usart)		\
DECLARE_ISR(_vector)										\
{															\
	usart_handleDreInterrupt( _usart );	\
}

USART_DRE_INTERRUPT_VECTOR(USARTC0_DRE_vect, SER_UART0)
USART_DRE_INTERRUPT_VECTOR(USARTD0_DRE_vect, SER_UART1)
#ifdef CPU_AVR_XMEGA_A
	USART_DRE_INTERRUPT_VECTOR(USARTC1_DRE_vect, SER_UART2)
	USART_DRE_INTERRUPT_VECTOR(USARTD1_DRE_VECT, SER_UART3)
	USART_DRE_INTERRUPT_VECTOR(USARTE0_DRE_vect, SER_UART4)
#endif

#ifdef SER_UART_BUS_TXOFF
	static inline void USART_handleTXCInterrupt(uint8_t usartNumber)
	{
		SER_STROBE_ON;
		struct FIFOBuffer * const txfifo = &ser_handles[usartNumber]->txfifo;
		if (fifo_isempty(txfifo))
		{
			SER_UART_BUS_TXOFF(UARTDescs[usartNumber].usart);
			UARTDescs[usartNumber].sending = false;
		}
		else
		{
			SER_UART_BUS_TXBEGIN(UARTDescs[usartNumber].usart);
		}
		SER_STROBE_OFF;
	}

	/*
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
	#define USART_TXC_INTERRUPT_VECTOR(_vector, _usart)	\
	DECLARE_ISR(_vector)								\
	{													\
		USART_handleTXCInterrupt( _usart );				\
	}

	USART_TXC_INTERRUPT_VECTOR(USARTC0_TXC_vect, SER_UART0)
	USART_TXC_INTERRUPT_VECTOR(USARTD0_TXC_vect, SER_UART1)
	#ifdef CPU_AVR_XMEGA_A
		USART_TXC_INTERRUPT_VECTOR(USARTC1_TXC_vect, SER_UART2)
		USART_TXC_INTERRUPT_VECTOR(USARTD1_TXC_vect, SER_UART3)
		USART_TXC_INTERRUPT_VECTOR(USARTE0_TXC_vect, SER_UART4)
	#endif /* CPU_AVR_XMEGA_A */
#endif /* SER_UART_BUS_TXOFF */

/*
 * Serial RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 */
static inline void USART_handleRXCInterrupt(uint8_t usartNumber)
{
	SER_STROBE_ON;
	/* read status */
	ser_handles[usartNumber]->status |=	(UARTDescs[usartNumber].usart)->STATUS & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);
	/* To clear the RXC flag we must _always_ read the UDR even when we're
	 * not going to accept the incoming data, otherwise a new interrupt
	 * will occur once the handler terminates.
	 */
	char c = (UARTDescs[usartNumber].usart)->DATA;
	struct FIFOBuffer * const rxfifo = &ser_handles[usartNumber]->rxfifo;
	if (fifo_isfull(rxfifo))
	{
		ser_handles[usartNumber]->status |= SERRF_RXFIFOOVERRUN;
	}
	else
	{
		fifo_push(rxfifo, c);
		#if CONFIG_SER_HWHANDSHAKE
			if (fifo_isfull(rxfifo))
			{
				RTS_OFF(UARTDescs[usartNumber].usart);
			}
		#endif
	}
	SER_STROBE_OFF;
}

#define USART_RXC_INTERRUPT_VECTOR(_vector, _usart)	\
DECLARE_ISR(_vector)								\
{													\
	USART_handleRXCInterrupt( _usart );				\
}
USART_RXC_INTERRUPT_VECTOR(USARTC0_RXC_vect, SER_UART0)
USART_RXC_INTERRUPT_VECTOR(USARTD0_RXC_vect, SER_UART1)
#ifdef CPU_AVR_XMEGA_A
	USART_RXC_INTERRUPT_VECTOR(USARTC1_RXC_vect, SER_UART2)
	USART_RXC_INTERRUPT_VECTOR(USARTD1_RXC_vect, SER_UART3)
	USART_RXC_INTERRUPT_VECTOR(USARTE0_RXC_vect, SER_UART4)
#endif
