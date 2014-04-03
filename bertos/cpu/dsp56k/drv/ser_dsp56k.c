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
 *
 * -->
 *
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief DSP5680x CPU specific serial I/O driver
 */


#include <drv/ser.h>
#include <drv/ser_p.h>
#include <drv/irq.h>
#include <cfg/debug.h>
#include <hw.h>
#include <DSP56F807.h>

// GPIO E is shared with SPI (in DSP56807). Pins 0&1 are TXD0 and RXD0. To use
//  the serial, we need to disable the GPIO functions on them.
#define REG_GPIO_SERIAL_0       REG_GPIO_E
#define REG_GPIO_SERIAL_MASK_0  0x03

#define REG_GPIO_SERIAL_1       REG_GPIO_D
#define REG_GPIO_SERIAL_MASK_1  0xC0


// Check flag consistency
#if (SERRF_PARITYERROR != REG_SCI_SR_PF) || \
	(SERRF_RXSROVERRUN != REG_SCI_SR_OR) || \
	(SERRF_FRAMEERROR  != REG_SCI_SR_FE) || \
	(SERRF_NOISEERROR  != REG_SCI_SR_NF)
	#error error flags do not match with register bits
#endif

static unsigned char ser0_fifo_rx[CONFIG_SER0_FIFOSIZE_RX];
static unsigned char ser0_fifo_tx[CONFIG_SER0_FIFOSIZE_TX];
static unsigned char ser1_fifo_rx[CONFIG_SER1_FIFOSIZE_RX];
static unsigned char ser1_fifo_tx[CONFIG_SER1_FIFOSIZE_TX];

#if CONFIG_SER_MULTI
	#include <kern/sem.h>

	#define MAX_MULTI_GROUPS     1

	struct Semaphore multi_sems[MAX_MULTI_GROUPS];
#endif


struct SCI
{
	struct SerialHardware hw;
	struct Serial* serial;
	volatile struct REG_SCI_STRUCT* regs;
	IRQ_VECTOR irq_tx;
	IRQ_VECTOR irq_rx;
	int num_group;
	int id;
};

static inline void enable_tx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR |= REG_SCI_CR_TEIE | REG_SCI_CR_TIIE;
}

static inline void enable_rx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR |= REG_SCI_CR_RIE;
}

static inline void disable_tx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR &= ~(REG_SCI_CR_TEIE | REG_SCI_CR_TIIE);
}

static inline void disable_rx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR &= ~(REG_SCI_CR_RIE | REG_SCI_CR_REIE);
}

static inline void disable_tx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	disable_tx_irq_bare(hw->regs);
}

static inline void disable_rx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	disable_rx_irq_bare(hw->regs);
}

static inline void enable_tx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	enable_tx_irq_bare(hw->regs);
}

static inline void enable_rx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	enable_rx_irq_bare(hw->regs);
}

static inline bool tx_irq_enabled(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	return (hw->regs->CR & REG_SCI_CR_TEIE);
}

static void tx_isr(const struct SCI *hw)
{
#pragma interrupt warn
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	if (fifo_isempty(&hw->serial->txfifo))
		disable_tx_irq_bare(regs);
	else
	{
		// Clear transmitter flags before sending data
		(void)regs->SR;
		regs->DR = fifo_pop(&hw->serial->txfifo);
	}
}

static void rx_isr(const struct SCI *hw)
{
#pragma interrupt warn
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	// Propagate errors
	hw->serial->status |= regs->SR & (SERRF_PARITYERROR |
	                                  SERRF_RXSROVERRUN |
	                                  SERRF_FRAMEERROR |
	                                  SERRF_NOISEERROR);

	/*
	 * Serial IRQ can happen for two reason: data ready (RDRF) or overrun (OR)
	 * If the data is ready, we need to fetch it from the data register or
	 * the interrupt will retrigger immediatly. In case of overrun, instead,
	 * the value of the data register is meaningless.
	 */
	if (regs->SR & REG_SCI_SR_RDRF)
	{
		unsigned char data = regs->DR;

		if (fifo_isfull(&hw->serial->rxfifo))
			hw->serial->status |= SERRF_RXFIFOOVERRUN;
		else
			fifo_push(&hw->serial->rxfifo, data);
	}

	// Writing anything to the status register clear the error bits.
	regs->SR = 0;
}

static void init(struct SerialHardware* _hw, struct Serial* ser)
{
	struct SCI* hw = (struct SCI*)_hw;
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	// Clear status register (IRQ/status flags)
	(void)regs->SR;
	regs->SR = 0;

	// Clear data register
	(void)regs->DR;

	// Install the handlers and set priorities for both IRQs
	irq_install(hw->irq_tx, (isr_t)tx_isr, hw);
	irq_install(hw->irq_rx, (isr_t)rx_isr, hw);
	irq_setpriority(hw->irq_tx, IRQ_PRIORITY_SCI_TX);
	irq_setpriority(hw->irq_rx, IRQ_PRIORITY_SCI_RX);

	// Activate the RX error interrupts, and RX/TX transmissions
	regs->CR = REG_SCI_CR_TE | REG_SCI_CR_RE;
	enable_rx_irq_bare(regs);

	// Disable GPIO pins for TX and RX lines
	// \todo this should be divided into serial 0 and 1
	REG_GPIO_SERIAL_0->PER |= REG_GPIO_SERIAL_MASK_0;
	REG_GPIO_SERIAL_1->PER |= REG_GPIO_SERIAL_MASK_1;

	hw->serial = ser;
}

static void cleanup(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	// Uninstall the ISRs
	disable_rx_irq(_hw);
	disable_tx_irq(_hw);
	irq_uninstall(hw->irq_tx);
	irq_uninstall(hw->irq_rx);
}

static void setbaudrate(struct SerialHardware* _hw, unsigned long rate)
{
	struct SCI* hw = (struct SCI*)_hw;

	// SCI has an internal 16x divider on the input clock, which comes
	//  from the IPbus (see the scheme in user manual, 12.7.3). We apply
	//  it to calculate the period to store in the register.
	hw->regs->BR = (IPBUS_FREQ + rate * 8ul) / (rate * 16ul);
}

static void setparity(struct SerialHardware* _hw, int parity)
{
	// ???
	ASSERT(0);
}


#if CONFIG_SER_MULTI

static void multi_init(void)
{
	static bool flag = false;
	int i;

	if (flag)
		return;

	for (i = 0; i < MAX_MULTI_GROUPS; ++i)
		sem_init(&multi_sems[i]);
	flag = true;
}

static void init_lock(struct SerialHardware* _hw, struct Serial *ser)
{
	struct SCI* hw = (struct SCI*)_hw;

	// Initialize the multi engine (if needed)
	multi_init();

	// Acquire the lock of the semaphore for this group
	ASSERT(hw->num_group >= 0);
	ASSERT(hw->num_group < MAX_MULTI_GROUPS);
	sem_obtain(&multi_sems[hw->num_group]);

	// Do a hardware switch to the given serial
	ser_hw_switch(hw->num_group, hw->id);

	init(_hw, ser);
}

static void cleanup_unlock(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;

	cleanup(_hw);

	sem_release(&multi_sems[hw->num_group]);
}

#endif /* CONFIG_SER_MULTI */


static const struct SerialHardwareVT SCI_VT =
{
	.init = init,
	.cleanup = cleanup,
	.setBaudrate = setbaudrate,
	.setParity = setparity,
	.txStart = enable_tx_irq,
	.txSending = tx_irq_enabled,
};

#if CONFIG_SER_MULTI
static const struct SerialHardwareVT SCI_MULTI_VT =
{
	.init = init_lock,
	.cleanup = cleanup_unlock,
	.setBaudrate = setbaudrate,
	.setParity = setparity,
	.txStart = enable_tx_irq,
	.txSending = tx_irq_enabled,
};
#endif /* CONFIG_SER_MULTI */

#define SCI_DESC_NORMAL(hwch) \
	{ \
		.hw = \
		{ \
			.table = &SCI_VT, \
			.rxbuffer = ser ## hwch ## _fifo_rx, \
			.txbuffer = ser ## hwch ## _fifo_tx, \
			.rxbuffer_size = countof(ser ## hwch ## _fifo_rx), \
			.txbuffer_size = countof(ser ## hwch ## _fifo_tx), \
		}, \
		.regs = &REG_SCI[hwch], \
		.irq_rx = IRQ_SCI ## hwch ## _RECEIVER_FULL, \
		.irq_tx = IRQ_SCI ## hwch ## _TRANSMITTER_READY, \
		.num_group = -1, \
		.id = -1, \
	} \
	/**/

#if CONFIG_SER_MULTI
#define SCI_DESC_MULTI(hwch, group_, id_) \
	{ \
		.hw = \
		{ \
			.table = &SCI_MULTI_VT, \
			.rxbuffer = ser ## hwch ## _fifo_rx, \
			.txbuffer = ser ## hwch ## _fifo_tx, \
			.rxbuffer_size = countof(ser ## hwch ## _fifo_rx), \
			.txbuffer_size = countof(ser ## hwch ## _fifo_tx), \
		}, \
		.regs = &REG_SCI[hwch], \
		.irq_rx = IRQ_SCI ## hwch ## _RECEIVER_FULL, \
		.irq_tx = IRQ_SCI ## hwch ## _TRANSMITTER_READY, \
		.num_group = group_, \
		.id = id_, \
	} \
	/**/
#endif /* CONFIG_SER_MULTI */

// \todo Move this into hw.h, with a little preprocessor magic
static struct SCI SCIDescs[] =
{
	SCI_DESC_NORMAL(0),
	SCI_DESC_MULTI(1, 0, 0),
	SCI_DESC_MULTI(1, 0, 1),
};

struct SerialHardware* ser_hw_getdesc(int unit)
{
	ASSERT(unit < countof(SCIDescs));
	return &SCIDescs[unit].hw;
}
