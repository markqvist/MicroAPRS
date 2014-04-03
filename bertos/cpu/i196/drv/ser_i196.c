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
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright (C) 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief CPU specific serial I/O driver
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2004/12/13 11:51:08  bernie
 *#* DISABLE_INTS/ENABLE_INTS: Convert to IRQ_DISABLE/IRQ_ENABLE.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/

#include "hw.h"
#include "serhw.h"

#define SER_HW_ENABLE_TX \
	ATOMIC( \
		if (!ser_sending) \
		{ \
			ser_sending = true; \
			(INT_PEND1 |= INT1F_TI) \
		} \
	);

static volatile bool ser_sending;

// Serial TX intr
INTERRUPT(0x30) void TI_interrupt(void)
{
	if (CANT_SEND)
	{
		ser_sending = false;
		return;
	}

	/* Can we send two bytes at the same time? */
	if (SP_STAT & SPSF_TX_EMPTY)
	{
		SBUF = fifo_pop(&ser_txfifo);

		if (CANT_SEND)
		{
			ser_sending = false;
			return;
		}
	}

	SBUF = fifo_pop(&ser_txfifo);
}

INTERRUPT(0x32) void RI_interrupt(void)
{
	ser_status |= SP_STAT &
		(SPSF_OVERRUN_ERROR | SPSF_PARITY_ERROR | SPSF_FRAMING_ERROR);
	if (fifo_isfull(&ser_rxfifo))
		ser_status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(&ser_rxfifo, SBUF);
}

static void ser_setbaudrate(unsigned long rate)
{
	// Calcola il periodo per la generazione del baud rate richiesto
	uint16_t baud = (uint16_t)(((CPU_FREQ / 16) / rate) - 1) | 0x8000;
	BAUD_RATE = (uint8_t)baud;
	BAUD_RATE = (uint8_t)(baud >> 8);
}

static void ser_hw_init(void)
{
	// Inizializza la porta seriale
	SP_CON = SPCF_RECEIVE_ENABLE | SPCF_MODE1;
	ioc1_img |= IOC1F_TXD_SEL | IOC1F_EXTINT_SRC;
	IOC1 = ioc1_img;

	// Svuota il buffer di ricezione
	{
		uint8_t dummy = SBUF;
	}

	// Abilita gli interrupt
	INT_MASK1 |= INT1F_TI | INT1F_RI;
}

