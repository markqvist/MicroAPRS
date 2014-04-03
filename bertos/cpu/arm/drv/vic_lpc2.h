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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Vectored Interrupt Controller VIC driver.
 */
 
#ifndef DRV_VIC_LPC2_H
#define DRV_VIC_LPC2_H

#include <cfg/compiler.h>
#include <cpu/irq.h>

#if CPU_ARM_LPC2378
	#include <io/lpc23xx.h>
	#define vic_vector(i)   (*(&VICVectAddr0 + i))
	#define vic_priority(i) (*(&VICVectCntl0 + i))
	#define VIC_SRC_CNT 32
	#define vic_enable(i)  do { ASSERT(i < VIC_SRC_CNT); VICIntEnable = BV(i); } while (0)
	#define vic_disable(i) do { ASSERT(i < VIC_SRC_CNT); VICIntEnClr  = BV(i); } while (0)

	typedef void vic_handler_t(void);
	void vic_defaultHandler(void);

	INLINE void vic_init(void)
	{
		IRQ_DISABLE;
		/* Assign all sources to IRQ (not to FIQ) */
		VICIntSelect = 0;
		/* Disable all sw interrupts */
		VICSoftIntClr = 0xFFFFFFFF;
		/* Disable all interrupts */
		VICIntEnClr = 0xFFFFFFFF;

		for (int i = 0; i < VIC_SRC_CNT; i++)
			vic_vector(i) = (reg32_t)vic_defaultHandler;
	}

	INLINE void vic_setVector(int id, vic_handler_t *handler)
	{
		ASSERT(id < VIC_SRC_CNT);
		vic_vector(id) = (reg32_t)handler;
	}
#else
	#error Unknown CPU
#endif

#endif /* DRV_VIC_LPC2_H */
