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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief ARM UART and SPI I/O driver
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "hw/hw_ser.h"  /* Required for bus macros overrides */
#include <hw/hw_cpufreq.h>  /* CPU_FREQ */

#include "cfg/cfg_ser.h"
#include <cfg/debug.h>


//#include <io/arm.h>
#include <io/cm3.h>
#include <drv/irq_cm3.h>

#include <cpu/attr.h>

#include <drv/ser.h>
#include <drv/ser_p.h>

#include <struct/fifobuf.h>


#define SERIRQ_PRIORITY 4 ///< default priority for serial irqs.

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
	 * - Disable GPIO on USART0 tx/rx pins
	 */
	#if CPU_ARM_AT91 && !CPU_ARM_SAM7S_LARGE && !CPU_ARM_SAM7X
		#warning Check USART0 pins!
	#endif
	#define SER_UART0_BUS_TXINIT do { \
		PIOA_PDR = BV(RXD0) | BV(TXD0); \
		PIO_PERIPH_SEL(PIOA_BASE, BV(RXD0) | BV(TXD0), USART0_PERIPH); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXBEGIN
	/**
	 * Invoked before starting a transmission
	 */
	#define SER_UART0_BUS_TXBEGIN
#endif

#ifndef SER_UART0_BUS_TXCHAR
	/**
	 * Invoked to send one character.
	 */
	#define SER_UART0_BUS_TXCHAR(c) do { \
		US0_THR = (c); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXEND
	/**
	 * Invoked as soon as the txfifo becomes empty
	 */
	#define SER_UART0_BUS_TXEND
#endif

/* End USART0 macros */

#if USART_PORTS > 1

	#ifndef SER_UART1_BUS_TXINIT
		/**
		 * Default TXINIT macro - invoked in uart1_init()
		 *
		 * - Disable GPIO on USART1 tx/rx pins
		 */
		#if CPU_ARM_AT91 && !CPU_ARM_SAM7S_LARGE && !CPU_ARM_SAM7X
			#warning Check USART1 pins!
		#endif
		#define SER_UART1_BUS_TXINIT do { \
			PIOA_PDR = BV(RXD1) | BV(TXD1); \
			PIO_PERIPH_SEL(PIOA_BASE, BV(RXD1) | BV(TXD1), USART1_PERIPH); \
		} while (0)
	#endif

	#ifndef SER_UART1_BUS_TXBEGIN
		/**
		 * Invoked before starting a transmission
		 */
		#define SER_UART1_BUS_TXBEGIN
	#endif

	#ifndef SER_UART1_BUS_TXCHAR
		/**
		 * Invoked to send one character.
		 */
		#define SER_UART1_BUS_TXCHAR(c) do { \
			US1_THR = (c); \
		} while (0)
	#endif

	#ifndef SER_UART1_BUS_TXEND
		/**
		 * Invoked as soon as the txfifo becomes empty
		 */
		#define SER_UART1_BUS_TXEND
	#endif

#endif

/**
* \name Overridable SPI hooks
*
* These can be redefined in hw.h to implement
* special bus policies such as slave select pin handling, etc.
*
* \{
*/

#ifndef SER_SPI0_BUS_TXINIT
	/**
	* Default TXINIT macro - invoked in spi_init()
	* The default is no action.
	*/
	#if CPU_CM3_SAM3
		#define SER_SPI0_BUS_TXINIT do { \
			/* Disable PIO on SPI pins */ \
			PIOA_PDR = BV(SPI0_SPCK) | BV(SPI0_MOSI) | BV(SPI0_MISO); \
			/* SPI is peripheral A on SAM3X,A,N,S,U */ \
			PIO_PERIPH_SEL(PIOA_BASE, BV(SPI0_SPCK) | BV(SPI0_MOSI) | BV(SPI0_MISO), PIO_PERIPH_A); \
		} while (0)
	#else
		#define SER_SPI0_BUS_TXINIT do { \
			/* Disable PIO on SPI pins */ \
			PIOA_PDR = BV(SPI0_SPCK) | BV(SPI0_MOSI) | BV(SPI0_MISO); \
		} while (0)
	#endif
#endif

#ifndef SER_SPI0_BUS_TXCLOSE
	/**
	* Invoked after the last character has been transmitted.
	* The default is no action.
	*/
	#define SER_SPI0_BUS_TXCLOSE do { \
		/* Enable PIO on SPI pins */ \
		PIOA_PER = BV(SPI0_SPCK) | BV(SPI0_MOSI) | BV(SPI0_MISO); \
	} while (0)
#endif

#if CPU_ARM_SAM7X

	#ifndef SER_SPI1_BUS_TXINIT
		/**
		* Default TXINIT macro - invoked in spi_init()
		* The default is no action.
		*/
		#define SER_SPI1_BUS_TXINIT do { \
			/* Disable PIO on SPI pins */ \
			PIOA_PDR = BV(SPI1_SPCK) | BV(SPI1_MOSI) | BV(SPI1_MISO); \
			/* SPI1 pins are on B peripheral function! */ \
			PIOA_BSR = BV(SPI1_SPCK) | BV(SPI1_MOSI) | BV(SPI1_MISO); \
		} while (0)
	#endif

	#ifndef SER_SPI1_BUS_TXCLOSE
		/**
		* Invoked after the last character has been transmitted.
		* The default is no action.
		*/
		#define SER_SPI1_BUS_TXCLOSE do { \
			/* Enable PIO on SPI pins */ \
			PIOA_PER = BV(SPI1_SPCK) | BV(SPI1_MOSI) | BV(SPI1_MISO); \
		} while (0)
	#endif
#endif
/*\}*/


/**
 * \name Core dependent interrupt handling macros
 *
 * Atmel serial hardware is used on different CPU cores,
 * i.e. SAM3 and SAM7.  The user interface of the serial
 * subsystem is identical but core interrupt controllers
 * are different.
 *
 * \{
 */
#if CPU_ARM_AT91

INLINE void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler)
{
	/* Set the vector. */
	AIC_SVR(irq) = uart0_irq_dispatcher;

	/* Initialize to level/edge sensitive with defined priority. */
#if CPU_ARM_SAM7X
	if (irq == SPI0_ID || irq == SPI1_ID)
#else
	if (irq == SPI0_ID)
#endif
		AIC_SMR(irq) = (AIC_SMR(irq) & ~AIC_SRCTYPE_MASK) | AIC_SRCTYPE_INT_EDGE_TRIGGERED;
	else // USART/UART
		AIC_SMR(irq) = (AIC_SMR(irq) & ~AIC_SRCTYPE_MASK) | AIC_SRCTYPE_INT_LEVEL_SENSITIVE;

	/* Enable IRQ */
	AIC_IECR = BV(irq);
}

INLINE void sysirq_setPriority(sysirq_t irq, int prio)
{
	AIC_SMR(irq) = (AIC_SMR(irq) & ~AIC_PRIOR_MASK) | SERIRQ_PRIORITY;
}

/** Inform hw that we have served the IRQ */
#define SER_INT_ACK do { \
	AIC_EOICR = 0; \
} while (0)

#elif CPU_CM3_SAM3

/** Inform hw that we have served the IRQ */
#define SER_INT_ACK do { /* nop */ } while (0)

#else
	#error No interrupt handling macros defined for current architecture
#endif

/*\}*/

/* From the high-level serial driver */
extern struct Serial *ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
#if USART_PORTS > 1
static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];
#endif
static unsigned char spi0_txbuffer[CONFIG_SPI0_TXBUFSIZE];
static unsigned char spi0_rxbuffer[CONFIG_SPI0_RXBUFSIZE];
#if CPU_ARM_SAM7X
static unsigned char spi1_txbuffer[CONFIG_SPI1_TXBUFSIZE];
static unsigned char spi1_rxbuffer[CONFIG_SPI1_RXBUFSIZE];
#endif

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
struct ArmSerial
{
	struct SerialHardware hw;
	volatile bool sending;
};

static ISR_PROTO(uart0_irq_dispatcher);
#if USART_PORTS > 1
static ISR_PROTO(uart1_irq_dispatcher);
#endif
static ISR_PROTO(spi0_irq_handler);
#if CPU_ARM_SAM7X
static ISR_PROTO(spi1_irq_handler);
#endif
/*
 * Callbacks for USART0
 */
static void uart0_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	US0_IDR = 0xFFFFFFFF;
	pmc_periphEnable(US0_ID);

	/*
	 * - Reset USART0
	 * - Set serial param: mode Normal, 8bit data, 1bit stop, parity none
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	US0_CR = BV(US_RSTRX) | BV(US_RSTTX);
	US0_MR = US_CHMODE_NORMAL | US_CHRL_8 | US_NBSTOP_1 | US_PAR_NO;
	US0_CR = BV(US_RXEN) | BV(US_TXEN);
	US0_IER = BV(US_RXRDY);

	SER_UART0_BUS_TXINIT;

	sysirq_setPriority(INT_US0, SERIRQ_PRIORITY);
	sysirq_setHandler(INT_US0, uart0_irq_dispatcher);

	SER_STROBE_INIT;
}

static void uart0_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	US0_CR = BV(US_RSTRX) | BV(US_RSTTX) | BV(US_RXDIS) | BV(US_TXDIS) | BV(US_RSTSTA);
}

static void uart0_enabletxirq(struct SerialHardware *_hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt sets hw->sending to false
	 * when it runs with an empty fifo.  The order of statements in the
	 * if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		/*
		 * - Enable the transmitter
		 * - Enable TX empty interrupt
		 */
		SER_UART0_BUS_TXBEGIN;
		US0_IER = BV(US_TXEMPTY);
	}
}

static void uart0_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	US0_BRGR = CPU_FREQ / (16 * rate);
	//DB(kprintf("uart0_setbaudrate(rate=%lu): period=%d\n", rate, period);)
}

static void uart0_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	US0_MR &= ~US_PAR_MASK;
	/* Set UART parity */
	switch(parity)
	{
		case SER_PARITY_NONE:
		{
			/* Parity none. */
			US0_MR |= US_PAR_NO;
			break;
		}
		case SER_PARITY_EVEN:
		{
			/* Even parity. */
			US0_MR |= US_PAR_EVEN;
			break;
		}
		case SER_PARITY_ODD:
		{
			/* Odd parity. */
			US0_MR |= US_PAR_ODD;
			break;
		}
		default:
			ASSERT(0);
	}
}

#if USART_PORTS > 1

/*
 * Callbacks for USART1
 */
static void uart1_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	US1_IDR = 0xFFFFFFFF;
	pmc_periphEnable(US1_ID);

	/*
	 * - Reset USART1
	 * - Set serial param: mode Normal, 8bit data, 1bit stop, parity none
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	US1_CR = BV(US_RSTRX) | BV(US_RSTTX);
	US1_MR = US_CHMODE_NORMAL | US_CHRL_8 | US_NBSTOP_1 | US_PAR_NO;
	US1_CR = BV(US_RXEN) | BV(US_TXEN);
	US1_IER = BV(US_RXRDY);

	SER_UART1_BUS_TXINIT;

	sysirq_setPriority(INT_US1, SERIRQ_PRIORITY);
	sysirq_setHandler(INT_US1, uart1_irq_dispatcher);

	SER_STROBE_INIT;
}

static void uart1_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	US1_CR = BV(US_RSTRX) | BV(US_RSTTX) | BV(US_RXDIS) | BV(US_TXDIS) | BV(US_RSTSTA);
}

static void uart1_enabletxirq(struct SerialHardware *_hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt sets hw->sending to false
	 * when it runs with an empty fifo.  The order of statements in the
	 * if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		/*
		 * - Enable the transmitter
		 * - Enable TX empty interrupt
		 */
		SER_UART1_BUS_TXBEGIN;
		US1_IER = BV(US_TXEMPTY);
	}
}

static void uart1_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	US1_BRGR = CPU_FREQ / (16 * rate);
	//DB(kprintf("uart0_setbaudrate(rate=%lu): period=%d\n", rate, period);)
}

static void uart1_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	US1_MR &= ~US_PAR_MASK;
	/* Set UART parity */
	switch(parity)
	{
		case SER_PARITY_NONE:
		{
			/* Parity none. */
			US1_MR |= US_PAR_NO;
			break;
		}
		case SER_PARITY_EVEN:
		{
			/* Even parity. */
			US1_MR |= US_PAR_EVEN;
			break;
		}
		case SER_PARITY_ODD:
		{
			/* Odd parity. */
			US1_MR |= US_PAR_ODD;
			break;
		}
		default:
			ASSERT(0);
	}
}

#endif /* USART_PORTS > 1 */

/* SPI driver */
static void spi0_init(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(struct Serial *, ser))
{
	SER_SPI0_BUS_TXINIT;

	/* Reset device */
	SPI0_CR = BV(SPI_SWRST);

	/*
	 * Set SPI to master mode, fixed peripheral select, chip select directly connected to a peripheral device,
	 * SPI clock set to MCK, mode fault detection disabled, loopback disable, NPCS0 active, Delay between CS = 0
	 */
	SPI0_MR = BV(SPI_MSTR) | BV(SPI_MODFDIS);

	/*
	 * Set SPI mode.
	 * At reset clock division factor is set to 0, that is
	 * *forbidden*. Set SPI clock to minimum to keep it valid.
	 * Set all possible chip select registers in case user manually
	 * change CPS field in SPI_MR.
	 */
	SPI0_CSR0 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI0_CSR1 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI0_CSR2 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI0_CSR3 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);

	/* Disable all irqs */
	SPI0_IDR = 0xFFFFFFFF;

	//sysirq_setPriority(INT_SPI0, SERIRQ_PRIORITY);
	sysirq_setHandler(INT_SPI0, spi0_irq_handler);
	pmc_periphEnable(SPI0_ID);

	/* Enable SPI */
	SPI0_CR = BV(SPI_SPIEN);

	SER_STROBE_INIT;
}

static void spi0_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	/* Disable SPI */
	SPI0_CR = BV(SPI_SPIDIS);

	/* Disable all irqs */
	SPI0_IDR = 0xFFFFFFFF;

	SER_SPI0_BUS_TXCLOSE;
}

static void spi0_starttx(struct SerialHardware *_hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;

	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!hw->sending && !fifo_isempty(&ser_handles[SER_SPI0]->txfifo))
	{
		hw->sending = true;
		SPI0_TDR = fifo_pop(&ser_handles[SER_SPI0]->txfifo);
		/* Enable interrupt on tx buffer empty */
		SPI0_IER = BV(SPI_TXEMPTY);
	}

	IRQ_RESTORE(flags);
}

static void spi0_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	SPI0_CSR0 &= ~SPI_SCBR;

	ASSERT((uint8_t)DIV_ROUND(CPU_FREQ, rate));
	SPI0_CSR0 |= DIV_ROUND(CPU_FREQ, rate) << SPI_SCBR_SHIFT;
}

#if CPU_ARM_SAM7X
/* SPI driver */
static void spi1_init(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(struct Serial *, ser))
{
	SER_SPI1_BUS_TXINIT;

	/* Reset device */
	SPI1_CR = BV(SPI_SWRST);

	/*
	 * Set SPI to master mode, fixed peripheral select, chip select directly connected to a peripheral device,
	 * SPI clock set to MCK, mode fault detection disabled, loopback disable, NPCS0 active, Delay between CS = 0
	 */
	SPI1_MR = BV(SPI_MSTR) | BV(SPI_MODFDIS);

	/*
	 * Set SPI mode.
	 * At reset clock division factor is set to 0, that is
	 * *forbidden*. Set SPI clock to minimum to keep it valid.
	 * Set all possible chip select registers in case user manually
	 * change chip select.
	 */
	SPI1_CSR0 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI1_CSR1 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI1_CSR2 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);
	SPI1_CSR3 = BV(SPI_NCPHA) | (255 << SPI_SCBR_SHIFT);

	/* Disable all SPI irqs */
	SPI1_IDR = 0xFFFFFFFF;

	sysirq_setPriority(INT_SPI1, SERIRQ_PRIORITY);
	sysirq_setHandler(INT_SPI1, spi1_irq_dispatcher);
	pmc_periphEnable(SPI1_ID);

	/* Enable SPI */
	SPI1_CR = BV(SPI_SPIEN);

	SER_STROBE_INIT;
}

static void spi1_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	/* Disable SPI */
	SPI1_CR = BV(SPI_SPIDIS);

	/* Disable all irqs */
	SPI1_IDR = 0xFFFFFFFF;

	SER_SPI1_BUS_TXCLOSE;
}

static void spi1_starttx(struct SerialHardware *_hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;

	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!hw->sending && !fifo_isempty(&ser_handles[SER_SPI1]->txfifo))
	{
		hw->sending = true;
		SPI1_TDR = fifo_pop(&ser_handles[SER_SPI1]->txfifo);
		/* Enable interrupt on tx buffer empty */
		SPI1_IER = BV(SPI_TXEMPTY);
	}

	IRQ_RESTORE(flags);
}

static void spi1_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	SPI1_CSR0 &= ~SPI_SCBR;

	ASSERT((uint8_t)DIV_ROUND(CPU_FREQ, rate));
	SPI1_CSR0 |= DIV_ROUND(CPU_FREQ, rate) << SPI_SCBR_SHIFT;
}
#endif

static void spi_setparity(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(int, parity))
{
	// nop
}


static bool tx_sending(struct SerialHardware* _hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;
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

#if USART_PORTS > 1

static const struct SerialHardwareVT UART1_VT =
{
	C99INIT(init, uart1_init),
	C99INIT(cleanup, uart1_cleanup),
	C99INIT(setBaudrate, uart1_setbaudrate),
	C99INIT(setParity, uart1_setparity),
	C99INIT(txStart, uart1_enabletxirq),
	C99INIT(txSending, tx_sending),
};

#endif /* USART_PORTS > 1 */

static const struct SerialHardwareVT SPI0_VT =
{
	C99INIT(init, spi0_init),
	C99INIT(cleanup, spi0_cleanup),
	C99INIT(setBaudrate, spi0_setbaudrate),
	C99INIT(setParity, spi_setparity),
	C99INIT(txStart, spi0_starttx),
	C99INIT(txSending, tx_sending),
};
#if CPU_ARM_SAM7X
static const struct SerialHardwareVT SPI1_VT =
{
	C99INIT(init, spi1_init),
	C99INIT(cleanup, spi1_cleanup),
	C99INIT(setBaudrate, spi1_setbaudrate),
	C99INIT(setParity, spi_setparity),
	C99INIT(txStart, spi1_starttx),
	C99INIT(txSending, tx_sending),
};
#endif

static struct ArmSerial UARTDescs[SER_CNT] =
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
#if USART_PORTS > 1
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

	{
		C99INIT(hw, /**/) {
			C99INIT(table, &SPI0_VT),
			C99INIT(txbuffer, spi0_txbuffer),
			C99INIT(rxbuffer, spi0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(spi0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(spi0_rxbuffer)),
		},
		C99INIT(sending, false),
	},
	#if CPU_ARM_SAM7X
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &SPI1_VT),
			C99INIT(txbuffer, spi1_txbuffer),
			C99INIT(rxbuffer, spi1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(spi1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(spi1_rxbuffer)),
		},
		C99INIT(sending, false),
	}

	#endif
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}

/**
 * Serial 0 TX interrupt handler
 */
INLINE void uart0_irq_tx(void)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART0]->txfifo;

	if (fifo_isempty(txfifo))
	{
		/*
		 * - Disable the TX empty interrupts
		 */
		US0_IDR = BV(US_TXEMPTY);
		SER_UART0_BUS_TXEND;
		UARTDescs[SER_UART0].sending = false;
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART0_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

/**
 * Serial 0 RX complete interrupt handler.
 */
INLINE void uart0_irq_rx(void)
{
	SER_STROBE_ON;

	/* Should be read before US_CRS */
	ser_handles[SER_UART0]->status |= US0_CSR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);
	US0_CR = BV(US_RSTSTA);

	char c = US0_RHR;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART0]->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_handles[SER_UART0]->status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(rxfifo, c);

	SER_STROBE_OFF;
}

/**
 * Serial IRQ dispatcher for USART0.
 */
static DECLARE_ISR(uart0_irq_dispatcher)
{
	if (US0_CSR & BV(US_RXRDY))
		uart0_irq_rx();

	if (US0_CSR & BV(US_TXEMPTY))
		uart0_irq_tx();

	SER_INT_ACK;
}

#if USART_PORTS > 1

/**
 * Serial 1 TX interrupt handler
 */
INLINE void uart1_irq_tx(void)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_handles[SER_UART1]->txfifo;

	if (fifo_isempty(txfifo))
	{
		/*
		 * - Disable the TX empty interrupts
		 */
		US1_IDR = BV(US_TXEMPTY);
		SER_UART1_BUS_TXEND;
		UARTDescs[SER_UART1].sending = false;
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART1_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

/**
 * Serial 1 RX complete interrupt handler.
 */
INLINE void uart1_irq_rx(void)
{
	SER_STROBE_ON;

	/* Should be read before US_CRS */
	ser_handles[SER_UART1]->status |= US1_CSR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);
	US1_CR = BV(US_RSTSTA);

	char c = US1_RHR;
	struct FIFOBuffer * const rxfifo = &ser_handles[SER_UART1]->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_handles[SER_UART1]->status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(rxfifo, c);

	SER_STROBE_OFF;
}

/**
 * Serial IRQ dispatcher for USART1.
 */
static DECLARE_ISR(uart1_irq_dispatcher)
{
	if (US1_CSR & BV(US_RXRDY))
		uart1_irq_rx();

	if (US1_CSR & BV(US_TXEMPTY))
		uart1_irq_tx();

	SER_INT_ACK;
}

#endif /* USART_PORTS > 1 */

/**
 * SPI0 interrupt handler
 */
static DECLARE_ISR(spi0_irq_handler)
{
	SER_STROBE_ON;

	char c = SPI0_RDR;
	/* Read incoming byte. */
	if (!fifo_isfull(&ser_handles[SER_SPI0]->rxfifo))
		fifo_push(&ser_handles[SER_SPI0]->rxfifo, c);
	/*
	 * FIXME
	else
		ser_handles[SER_SPI0]->status |= SERRF_RXFIFOOVERRUN;
	*/

	/* Send */
	if (!fifo_isempty(&ser_handles[SER_SPI0]->txfifo))
		SPI0_TDR = fifo_pop(&ser_handles[SER_SPI0]->txfifo);
	else
	{
		UARTDescs[SER_SPI0].sending = false;
		/* Disable interrupt on tx buffer empty */
		SPI0_IDR = BV(SPI_TXEMPTY);
	}

	SER_INT_ACK;

	SER_STROBE_OFF;
}


#if CPU_ARM_SAM7X
/**
 * SPI1 interrupt handler
 */
static DECLARE_ISR(spi1_irq_handler)
{
	SER_STROBE_ON;

	char c = SPI1_RDR;
	/* Read incoming byte. */
	if (!fifo_isfull(&ser_handles[SER_SPI1]->rxfifo))
		fifo_push(&ser_handles[SER_SPI1]->rxfifo, c);
	/*
	 * FIXME
	else
		ser_handles[SER_SPI1]->status |= SERRF_RXFIFOOVERRUN;
	*/

	/* Send */
	if (!fifo_isempty(&ser_handles[SER_SPI1]->txfifo))
		SPI1_TDR = fifo_pop(&ser_handles[SER_SPI1]->txfifo);
	else
	{
		UARTDescs[SER_SPI1].sending = false;
		/* Disable interrupt on tx buffer empty */
		SPI1_IDR = BV(SPI_TXEMPTY);
	}

	SER_INT_ACK;

	SER_STROBE_OFF;
}
#endif
