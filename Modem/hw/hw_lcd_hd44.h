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
 * Copyright 2003, 2004, 2005, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief LCD low-level hardware macros
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */

#ifndef HW_LCD_HD44_H
#define HW_LCD_HD44_H

#include "cfg/cfg_lcd_hd44.h"  /* CONFIG_LCD_4BIT */

#include <cpu/types.h>
#include <cpu/irq.h>

#warning TODO:This is an example implementation, you must implement it!

/**
 * \name LCD I/O pins/ports
 * @{
 */
#define LCD_RS    /* Implement me! */
#define LCD_RW    /* Implement me! */
#define LCD_E     /* Implement me! */
#define LCD_DB0   /* Implement me! */
#define LCD_DB1   /* Implement me! */
#define LCD_DB2   /* Implement me! */
#define LCD_DB3   /* Implement me! */
#define LCD_DB4   /* Implement me! */
#define LCD_DB5   /* Implement me! */
#define LCD_DB6   /* Implement me! */
#define LCD_DB7   /* Implement me! */
/*@}*/

/**
 * \name DB high nibble (DB[4-7])
 * @{
 */

#if CONFIG_LCD_4BIT
	#define LCD_MASK    (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4)
	#define LCD_SHIFT   4
#else
	#define LCD_MASK (uint8_t)0xff
	#define LCD_SHIFT 0
#endif
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
#define LCD_CLR_RS      /* Implement me! */
#define LCD_SET_RS      /* Implement me! */
#define LCD_CLR_RD      /* Implement me! */
#define LCD_SET_RD      /* Implement me! */
#define LCD_CLR_E       /* Implement me! */
#define LCD_SET_E       /* Implement me! */

/* Enter command mode */
#define LCD_SET_COMMAND() /* Implement me! */

/* Enter data mode */
#define LCD_SET_DATA() /* Implement me! */

#if CONFIG_LCD_4BIT
	#define LCD_WRITE_H(x)  ((void)x)/* Implement me! */
	#define LCD_WRITE_L(x)  ((void)x)/* Implement me! */
	#define LCD_READ_H      ( 0 /* Implement me! */ )
	#define LCD_READ_L      ( 0 /* Implement me! */ )
#else
	#define LCD_WRITE(x)    ((void)x)/* Implement me! */
	#define LCD_READ        (0 /* Implement me! */ )
#endif
/*@}*/

/** Set data bus direction to output (write to display) */
#define LCD_DB_OUT          /* Implement me! */

/** Set data bus direction to input (read from display) */
#define LCD_DB_IN           /* Implement me! */
/** Delay for write (Enable pulse width, 220ns) */
#define LCD_DELAY_WRITE \
	do { \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
	} while (0)

/** Delay for read (Data ouput delay time, 120ns) */
#define LCD_DELAY_READ \
	do { \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
	} while (0)



INLINE void lcd_hd44_hw_bus_init(void)
{
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/*
	 * Here set bus pin!
	 * to init a lcd device.
	 *
	 */

	/*
	 * Data bus is in output state most of the time:
	 * LCD r/w functions assume it is left in output state
	 */
	LCD_DB_OUT;


	IRQ_RESTORE(flags);
}

#endif /* HW_LCD_HD44_H */
