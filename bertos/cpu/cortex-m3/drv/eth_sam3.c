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
  * Copyright 2010,2011 Develer S.r.l. (http://www.develer.com/)
  * All Rights Reserved.
  * -->
  *
  * \brief EMAC driver for AT91SAM family with Davicom 9161A phy.
  *
  * \author Daniele Basile <asterix@develer.com>
  * \author Andrea Righi <arighi@develer.com>
  * \author Stefano Fedrigo <aleph@develer.com>
  */

#include "cfg/cfg_eth.h"

#define LOG_LEVEL  ETH_LOG_LEVEL
#define LOG_FORMAT ETH_LOG_FORMAT

#include <cfg/log.h>

#include <cfg/debug.h>
#include <cfg/log.h>
#include <cfg/macros.h>
#include <cfg/compiler.h>

// TODO: unify includes
//#include <io/at91sam7.h>
//#include <io/arm.h>
//#include <io/include.h>
#include <io/sam3.h>
#include <drv/irq_cm3.h>

#include <cpu/power.h>
#include <cpu/types.h>
#include <cpu/irq.h>

#include <drv/timer.h>
#include <drv/eth.h>

#include <mware/event.h>

#include <string.h>

#include "eth_sam3.h"

#define EMAC_RX_INTS	(BV(EMAC_RCOMP) | BV(EMAC_ROVR) | BV(EMAC_RXUBR))
#define EMAC_TX_INTS	(BV(EMAC_TCOMP) | BV(EMAC_TXUBR) | BV(EMAC_RLEX))

/* Silent Doxygen bug... */
#ifndef __doxygen__
/*
 * NOTE: this buffer should be declared as 'volatile' because it is read by the
 * hardware. However, this is accessed only via memcpy() that should guarantee
 * coherency when copying from/to buffers.
 */
static uint8_t tx_buf[EMAC_TX_BUFFERS * EMAC_TX_BUFSIZ] ALIGNED(8);
static volatile BufDescriptor tx_buf_tab[EMAC_TX_DESCRIPTORS] ALIGNED(8);

/*
 * NOTE: this buffer should be declared as 'volatile' because it is wrote by
 * the hardware. However, this is accessed only via memcpy() that should
 * guarantee coherency when copying from/to buffers.
 */
static uint8_t rx_buf[EMAC_RX_BUFFERS * EMAC_RX_BUFSIZ] ALIGNED(8);
static volatile BufDescriptor rx_buf_tab[EMAC_RX_DESCRIPTORS] ALIGNED(8);
#endif

static int tx_buf_idx;
static int tx_buf_offset;
static int rx_buf_idx;

static Event recv_wait, send_wait;

static DECLARE_ISR(emac_irqHandler)
{
	/* Read interrupt status and disable interrupts. */
	uint32_t isr = EMAC_ISR;

	/* Receiver interrupt */
	if ((isr & EMAC_RX_INTS))
	{
		if (isr & BV(EMAC_RCOMP))
			event_do(&recv_wait);
		EMAC_RSR = EMAC_RX_INTS;
	}
	/* Transmitter interrupt */
	if (isr & EMAC_TX_INTS)
	{
		if (isr & BV(EMAC_TCOMP))
			event_do(&send_wait);
		EMAC_TSR = EMAC_TX_INTS;
	}
	//AIC_EOICR = 0;
}

/*
 * \brief Read contents of PHY register.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
static uint16_t phy_hw_read(uint8_t phy_addr, reg8_t reg)
{
	// PHY read command.
	EMAC_MAN = EMAC_SOF | EMAC_RW_READ
		| ((phy_addr << EMAC_PHYA_SHIFT) & EMAC_PHYA)
		| ((reg  << EMAC_REGA_SHIFT) & EMAC_REGA)
		| EMAC_CODE;

	// Wait until PHY logic completed.
	while (!(EMAC_NSR & BV(EMAC_IDLE)))
		cpu_relax();

	// Get data from PHY maintenance register.
	return (uint16_t)(EMAC_MAN & EMAC_DATA);
}

#if 0
/*
 * \brief Write value to PHY register.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void phy_hw_write(uint8_t phy_addr, reg8_t reg, uint16_t val)
{
	// PHY write command.
	EMAC_MAN = EMAC_SOF | EMAC_RW_WRITE
		| ((phy_addr << EMAC_PHYA_SHIFT) & EMAC_PHYA)
		| ((reg  << EMAC_REGA_SHIFT) & EMAC_REGA)
		| EMAC_CODE | val;

	// Wait until PHY logic completed.
	while (!(EMAC_NSR & BV(EMAC_IDLE)))
		cpu_relax();
}
#endif

/*
 * Check link speed and duplex as negotiated by the PHY
 * and configure CPU EMAC accordingly.
 * Requires active PHY maintenance mode.
 */
static void emac_autoNegotiation(void)
{
	uint16_t reg;
	time_t start;

	// Wait for auto-negotation to complete
	start = timer_clock();
	do {
		reg = phy_hw_read(NIC_PHY_ADDR, NIC_PHY_BMSR);
		if (timer_clock() - start > 2000)
		{
			kprintf("eth error: auto-negotiation timeout\n");
			return;
		}
	}
	while (!(reg & NIC_PHY_BMSR_ANCOMPL));

	reg = phy_hw_read(NIC_PHY_ADDR, NIC_PHY_ANLPAR);

	if ((reg & NIC_PHY_ANLPAR_TX_FDX) || (reg & NIC_PHY_ANLPAR_TX_HDX))
	{
		LOG_INFO("eth: 100BASE-TX\n");
		EMAC_NCFGR |= BV(EMAC_SPD);
	}
	else
	{
		LOG_INFO("eth: 10BASE-T\n");
		EMAC_NCFGR &= ~BV(EMAC_SPD);
	}

	if ((reg & NIC_PHY_ANLPAR_TX_FDX) || (reg & NIC_PHY_ANLPAR_10_FDX))
	{
		LOG_INFO("eth: full duplex\n");
		EMAC_NCFGR |= BV(EMAC_FD);
	}
	else
	{
		LOG_INFO("eth: half duplex\n");
		EMAC_NCFGR &= ~BV(EMAC_FD);
	}
}


static int emac_reset(void)
{
#if CPU_ARM_AT91
	// Enable devices
	PMC_PCER = BV(PIOA_ID);
	PMC_PCER = BV(PIOB_ID);
	PMC_PCER = BV(EMAC_ID);

	// Disable TESTMODE and RMII
	PIOB_PUDR = BV(PHY_RXDV_TESTMODE_BIT);
	PIOB_PUDR = BV(PHY_COL_RMII_BIT);

	// Disable PHY power down.
	PIOB_PER  = BV(PHY_PWRDN_BIT);
	PIOB_OER  = BV(PHY_PWRDN_BIT);
	PIOB_CODR = BV(PHY_PWRDN_BIT);
#else
	pmc_periphEnable(PIOA_ID);
	pmc_periphEnable(PIOB_ID);
	pmc_periphEnable(PIOC_ID);
	pmc_periphEnable(PIOD_ID);
	pmc_periphEnable(EMAC_ID);

	// Disable TESTMODE
	PIOB_PUDR = BV(PHY_RXDV_TESTMODE_BIT);
#endif

	// Toggle external hardware reset pin.
	RSTC_MR = RSTC_KEY | (1 << RSTC_ERSTL_SHIFT) | BV(RSTC_URSTEN);
	RSTC_CR = RSTC_KEY | BV(RSTC_EXTRST);

	while ((RSTC_SR & BV(RSTC_NRSTL)) == 0)
		cpu_relax();

	// Configure MII ports.
#if CPU_ARM_AT91
	PIOB_ASR = PHY_MII_PINS;
	PIOB_BSR = 0;
	PIOB_PDR = PHY_MII_PINS;

	// Enable receive and transmit clocks.
	EMAC_USRIO = BV(EMAC_CLKEN);
#else
	PIO_PERIPH_SEL(PIOB_BASE, PHY_MII_PINS_PORTB, PIO_PERIPH_A);
	PIOB_PDR = PHY_MII_PINS_PORTB;

	// Enable receive, transmit clocks and RMII mode.
	EMAC_USRIO = BV(EMAC_CLKEN) | BV(EMAC_RMII);
#endif

	// Enable management port.
	EMAC_NCR |= BV(EMAC_MPE);
	EMAC_NCFGR |= EMAC_CLK_HCLK_64;

	// Set local MAC address.
	EMAC_SA1L = (mac_addr[3] << 24) | (mac_addr[2] << 16) |
				(mac_addr[1] << 8) | mac_addr[0];
	EMAC_SA1H = (mac_addr[5] << 8) | mac_addr[4];

	emac_autoNegotiation();

	// Disable management port.
	EMAC_NCR &= ~BV(EMAC_MPE);

	return 0;
}


static int emac_start(void)
{
	uint32_t addr;
	int i;

	for (i = 0; i < EMAC_RX_DESCRIPTORS; i++)
	{
		addr = (uint32_t)(rx_buf + (i * EMAC_RX_BUFSIZ));
		rx_buf_tab[i].addr = addr & BUF_ADDRMASK;
	}
	rx_buf_tab[EMAC_RX_DESCRIPTORS - 1].addr |= RXBUF_WRAP;

	for (i = 0; i < EMAC_TX_DESCRIPTORS; i++)
	{
		addr = (uint32_t)(tx_buf + (i * EMAC_TX_BUFSIZ));
		tx_buf_tab[i].addr = addr & BUF_ADDRMASK;
		tx_buf_tab[i].stat = TXS_USED;
	}
	tx_buf_tab[EMAC_TX_DESCRIPTORS - 1].stat = TXS_USED | TXS_WRAP;

	/* Tell the EMAC where to find the descriptors. */
	EMAC_RBQP = (uint32_t)rx_buf_tab;
	EMAC_TBQP = (uint32_t)tx_buf_tab;

	/* Clear receiver status. */
	EMAC_RSR = BV(EMAC_OVR) | BV(EMAC_REC) | BV(EMAC_BNA);

	/* Copy all frames and discard FCS. */
	EMAC_NCFGR |= BV(EMAC_CAF) | BV(EMAC_DRFCS);

	/* Enable receiver, transmitter and statistics. */
	EMAC_NCR |= BV(EMAC_TE) | BV(EMAC_RE) | BV(EMAC_WESTAT);

	return 0;
}

ssize_t eth_putFrame(const uint8_t *buf, size_t len)
{
	size_t wr_len;

	if (UNLIKELY(!len))
		return -1;
	ASSERT(len <= sizeof(tx_buf));

	/* Check if the transmit buffer is available */
	while (!(tx_buf_tab[tx_buf_idx].stat & TXS_USED))
		event_wait(&send_wait);

	/* Copy the data into the buffer and prepare descriptor */
	wr_len = MIN(len, (size_t)EMAC_TX_BUFSIZ - tx_buf_offset);
	memcpy((uint8_t *)tx_buf_tab[tx_buf_idx].addr + tx_buf_offset,
			buf, wr_len);
	tx_buf_offset += wr_len;

	return wr_len;
}

void eth_sendFrame(void)
{
	tx_buf_tab[tx_buf_idx].stat = (tx_buf_offset & TXS_LENGTH_FRAME) |
		TXS_LAST_BUFF |
		((tx_buf_idx == EMAC_TX_DESCRIPTORS - 1) ?  TXS_WRAP : 0);
	EMAC_NCR |= BV(EMAC_TSTART);

	tx_buf_offset = 0;
	if (++tx_buf_idx >= EMAC_TX_DESCRIPTORS)
		tx_buf_idx = 0;
}

ssize_t eth_send(const uint8_t *buf, size_t len)
 {
	if (UNLIKELY(!len))
		return -1;

	len = eth_putFrame(buf, len);
	eth_sendFrame();

	return len;
}

static void eth_buf_realign(int idx)
{
	/* Empty buffer found. Realign. */
	do {
		rx_buf_tab[rx_buf_idx].addr &= ~RXBUF_OWNERSHIP;
		if (++rx_buf_idx >= EMAC_RX_BUFFERS)
			rx_buf_idx = 0;
	} while (idx != rx_buf_idx);
}

static size_t __eth_getFrameLen(void)
{
	int idx, n = EMAC_RX_BUFFERS;

skip:
	/* Skip empty buffers */
	while ((n > 0) && !(rx_buf_tab[rx_buf_idx].addr & RXBUF_OWNERSHIP))
	{
		if (++rx_buf_idx >= EMAC_RX_BUFFERS)
			rx_buf_idx = 0;
		n--;
	}
	if (UNLIKELY(!n))
	{
		LOG_INFO("no frame found\n");
		return 0;
	}
	/* Search the start of frame and cleanup fragments */
	while ((n > 0) && (rx_buf_tab[rx_buf_idx].addr & RXBUF_OWNERSHIP) &&
			!(rx_buf_tab[rx_buf_idx].stat & RXS_SOF))
	{
		rx_buf_tab[rx_buf_idx].addr &= ~RXBUF_OWNERSHIP;
		if (++rx_buf_idx >= EMAC_RX_BUFFERS)
			rx_buf_idx = 0;
		n--;
	}
	if (UNLIKELY(!n))
	{
		LOG_INFO("no SOF found\n");
		return 0;
	}
	/* Search end of frame to evaluate the total frame size */
	idx = rx_buf_idx;
restart:
	while (n > 0)
	{
		if (UNLIKELY(!(rx_buf_tab[idx].addr & RXBUF_OWNERSHIP)))
		{
			/* Empty buffer found. Realign. */
			eth_buf_realign(idx);
			goto skip;
		}
		if (rx_buf_tab[idx].stat & RXS_EOF)
			return rx_buf_tab[idx].stat & RXS_LENGTH_FRAME;
		if (UNLIKELY((idx != rx_buf_idx) &&
				(rx_buf_tab[idx].stat & RXS_SOF)))
		{
			/* Another start of frame found. Realign. */
			eth_buf_realign(idx);
			goto restart;
		}
		if (++idx >= EMAC_RX_BUFFERS)
			idx = 0;
		n--;
	}
	LOG_INFO("no EOF found\n");
	return 0;
}

size_t eth_getFrameLen(void)
{
	size_t len;

	/* Check if there is at least one available frame in the buffer */
	while (1)
	{
		len = __eth_getFrameLen();
		if (LIKELY(len))
			break;
		/* Wait for RX interrupt */
		event_wait(&recv_wait);
	}
	return len;
}

ssize_t eth_getFrame(uint8_t *buf, size_t len)
{
	uint8_t *addr;
	size_t rd_len = 0;

	if (UNLIKELY(!len))
		return -1;
	ASSERT(len <= sizeof(rx_buf));

	/* Copy data from the RX buffer */
	addr = (uint8_t *)(rx_buf_tab[rx_buf_idx].addr & BUF_ADDRMASK);
	if (addr + len > &rx_buf[countof(rx_buf)])
	{
		size_t count = &rx_buf[countof(rx_buf)] - addr;

		memcpy(buf, addr, count);
		memcpy(buf + count, rx_buf, len - count);
	}
	else
	{
		memcpy(buf, addr, len);
	}
	/* Update descriptors */
	while (rd_len < len)
	{
		if (len - rd_len >= EMAC_RX_BUFSIZ)
			rd_len += EMAC_RX_BUFSIZ;
		else
			rd_len += len - rd_len;
		if (UNLIKELY(!(rx_buf_tab[rx_buf_idx].addr & RXBUF_OWNERSHIP)))
		{
			LOG_INFO("bad frame found\n");
			return 0;
		}
		rx_buf_tab[rx_buf_idx].addr &= ~RXBUF_OWNERSHIP;
		if (++rx_buf_idx >= EMAC_RX_DESCRIPTORS)
			rx_buf_idx = 0;
	}

	return rd_len;
}

ssize_t eth_recv(uint8_t *buf, size_t len)
{
	if (UNLIKELY(!len))
		return -1;
	len = MIN(len, eth_getFrameLen());
	return len ? eth_getFrame(buf, len) : 0;
}

int eth_init()
{
	cpu_flags_t flags;

	emac_reset();
	emac_start();

	event_initGeneric(&recv_wait);
	event_initGeneric(&send_wait);

	// Register interrupt vector
	IRQ_SAVE_DISABLE(flags);

	/* Disable all emac interrupts */
	EMAC_IDR = 0xFFFFFFFF;

#if CPU_ARM_AT91
	// TODO: define sysirq_set...
	/* Set the vector. */
	AIC_SVR(EMAC_ID) = emac_irqHandler;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(EMAC_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED;
	/* Clear pending interrupt */
	AIC_ICCR = BV(EMAC_ID);
	/* Enable the system IRQ */
	AIC_IECR = BV(EMAC_ID);
#else
	sysirq_setHandler(INT_EMAC, emac_irqHandler);
#endif

	/* Enable interrupts */
	EMAC_IER = EMAC_RX_INTS | EMAC_TX_INTS;

	IRQ_RESTORE(flags);

	return 0;
}
