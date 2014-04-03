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
 * \brief HX8347 low-level hardware macros for Atmel SAM3X-EK board.
 *
 * The LCD controller is connected to the cpu static memory controller.
 * LCD has 16 data lines and usual RS/WR/RD lines.  The data lines
 * are connected to the SMC data bus (D0-15), while the SCM address bus
 * (A1 only) is used to drive the RS pin.  WR/RD are connected to SMC's
 * NWE and NRD respectively.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_HX8347_H
#define HW_HX8347_H

#warning TODO: This is an example implementation, you must implement it!

/**
 * Send a command to LCD controller.
 */
INLINE void hx8347_cmd(uint8_t cmd)
{
	/* Implement me */
	(void)cmd;
}

/**
 * Send data to LCD controller.
 */
INLINE void hx8347_write(uint16_t data)
{
	/* Implement me */
	(void)data;
}

/**
 * Read data from LCD controller.
 */
INLINE uint16_t hx8347_read(void)
{
	/* Implement me */
	return 0;
}

/**
 * Bus initialization: setup hardware where LCD is connected.
 */
INLINE void hx8347_busInit(void)
{
	/* Implement me */
}

#endif /* HW_HX8347_H */
