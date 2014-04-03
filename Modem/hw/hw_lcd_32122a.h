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
 * Copyright 2003, 2004, 2005, 2006, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Displaytech 32122A LCD driver
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */

#ifndef HW_LCD_32122A_H
#define HW_LCD_32122A_H

#include <cpu/types.h>
#include <cpu/irq.h>

#warning TODO:This is an example implementation, you must implement it!

/**
 * Predefined LCD PWM contrast values
 */
#define LCD_DEF_PWM 145
#define LCD_MAX_PWM 505
#define LCD_MIN_PWM 130
#define LCD_PWM_CH    0


/**
 * \name LCD I/O pins/ports
 * @{
 */
#define LCD_RS    /* Implement me! */
#define LCD_RW    /* Implement me! */
#define LCD_PE_E1    /* Implement me! */
#define LCD_PE_E2    /* Implement me! */
#define LCD_DB0   /* Implement me! */
#define LCD_DB1   /* Implement me! */
#define LCD_DB2   /* Implement me! */
#define LCD_DB3   /* Implement me! */
#define LCD_DB4   /* Implement me! */
#define LCD_DB5   /* Implement me! */
#define LCD_DB6   /* Implement me! */
#define LCD_DB7   /* Implement me! */
#define LCD_PF_DB0   /* Implment me! */
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
#define LCD_CLR_A0                do { /* Implement me! */ } while (0)
#define LCD_SET_A0                do { /* Implement me! */ } while (0)
#define LCD_CLR_RD                do { /* Implement me! */ } while (0)
#define LCD_SET_RD                do { /* Implement me! */ } while (0)
#define LCD_CLR_E1                do { /* Implement me! */ } while (0)
#define LCD_SET_E1                do { /* Implement me! */ } while (0)
#define LCD_CLR_E2                do { /* Implement me! */ } while (0)
#define LCD_SET_E2                do { /* Implement me! */ } while (0)
#define LCD_SET_E(x)              do { (void)x; /* Implement me! */ } while (0)
#define LCD_CLR_E(x)              do { (void)x; /* Implement me! */ } while (0)
/*@}*/

/**
 * \name Chip select bits for LCD_SET_E()
 * @{
 */
#define LCDF_E1                   ( 0/* Implement me! */)
#define LCDF_E2                   ( 0/* Implement me! */)
/*@}*/
/** Read from the LCD data bus (DB[0-7]) */
#define LCD_WRITE(x)    ((void)x)/* Implement me! */
/** Write to the LCD data bus (DB[0-7]) */
#define LCD_READ        (0 /* Implement me! */ )

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


#define LCD_32122_RESET()         do { /* Implement me! */ } while (0)

INLINE void lcd_32122a_hw_bus_init(void)
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

#endif /* HW_LCD_32122A_H */

