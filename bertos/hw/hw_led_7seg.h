/**
 * \file hw_led_7seg.h
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
 * -->
 *
 * \brief led 7 segment display low-level
 *
 * This file has the functions that must be
 * implemented to drive the 7 segments display by your
 * hardware
 *
 * \author Fabio Bizzi <fbizzi@bizzi.org>
 *
 * Example implementation for AtMEGA 1280
 * (Arduino MEGA) with a 4 digit display.
 * We use PORTA to connect the 8 pins of
 * the 7 segments display and 4 bit of
 * PORTC to select which digit of the
 * display we have to write on.
 *
 * \code
 *  7 Seg LED Pin
 *  ----------------
 *  LED SEGMENT
 *  ----------------
 *  DP G F E D C B A
 *  7  6 5 4 3 2 1 0
 *  ----------------
 *  PORT A Pin
 *  ----------------
 *
 *  7 Seg Display Selection
 *  ----------------
 *  Display Nr.
 *  ----------------
 *  N N N N 3 2 1 0
 *  7 6 5 4 3 2 1 0
 *  ----------------
 *  PORT C Pin
 *  ----------------
 * \endcode
 *
 * The implementation of the sseg_on procedure that set the PROPER PIN of PORT C
 * to enable the requested digit of the display, after write the encoded character
 * to PORT A
 *
 * \code
 *
 * INLINE void sseg_on(uint8_t dgt, uint8_t n_dgt)
 * {
 *  switch (n_dgt)
 *  {
 * //Common Cathode
 * #ifdef CONFIG_LED_7SEG_CCAT
 *
 *		case 0:
 *			PORTC &= ~(BV(PORTC1) | BV(PORTC2) | BV(PORTC3));
 *			PORTC |= BV(PORTC0);
 *			break;
 *		case 1:
 *			PORTC &= ~(BV(PORTC0) | BV(PORTC2) | BV(PORTC3));
 *			PORTC |= BV(PORTC1);
 *			break;
 *		case 2:
 *			PORTC &= ~(BV(PORTC0) | BV(PORTC1) | BV(PORTC3));
 *			PORTC |= BV(PORTC2);
 *			break;
 *		case 3:
 *			PORTC &= ~(BV(PORTC0) | BV(PORTC1) | BV(PORTC2));
 *			PORTC |= BV(PORTC3);
 *			break;
 *
 * //Common Anode
 * #else
 *
 *		case 0:
 *			PORTC |= (BV(PORTC1) | BV(PORTC2) | BV(PORTC3));
 *			PORTC &= ~(BV(PORTC0));
 *			break;
 *		case 1:
 *			PORTC |= (BV(PORTC0) | BV(PORTC2) | BV(PORTC3));
 *			PORTC &= ~(BV(PORTC1));
 *			break;
 *		case 2:
 *			PORTC |= (BV(PORTC0) | BV(PORTC1) | BV(PORTC3));
 *			PORTC &= ~(BV(PORTC2));
 *			break;
 *		case 3:
 *			PORTC |= (BV(PORTC0) | BV(PORTC1) | BV(PORTC2));
 *			PORTC &= ~(BV(PORTC3));
 *			break;
 *
 * #endif
 *
 *	}
 *	//Write the charater
 *	PORTA = dgt;
 * }
 *
 * \endcode
 *
 * The implementation of the sseg_init procedure that set the DIRECTION of PORT C
 * and PORT A to output
 *
 * \code
 *
 * INLINE void sseg_init(void)
 * {
 * //Initialize PIN Direction to OUTPUT
 *	DDRA = 0xFF;
 *	DDRC |= (BV(DDC0) | BV(DDC1) | BV(DDC2) | BV(DDC3));
 *	//Set the display OFF
 *	SSEG_OFF();
 * }
 *
 * \endcode
 *
 * The implementation of the sseg_off procedure that set the reset PORT A
 * to clean the display and turn off all the pin of PORT C that drive the
 * display's digits
 *
 * \code
 *
 * INLINE void sseg_off(void)
 * {
 * //Set the display OFF
 * //Common Cathode
 * #ifdef CONFIG_LED_7SEG_CCAT
 *    PORTA = 0x00;
 *    PORTC &= ~(BV(PORTC0) | BV(PORTC1) | BV(PORTC2) | BV(PORTC3));
 * //Common Anode
 * #else
 *    PORTA = 0xFF;
 *    PORTC |= (BV(PORTC0) | BV(PORTC1) | BV(PORTC2) | BV(PORTC3));
 * #endif
 *
 * }
 *
 * \endcode
 *
 */

#ifndef HW_LED_7SEG_H
#define HW_LED_7SEG_H

#include "cfg/cfg_led_7seg.h"

/*
 * INLINE HW Functions
 */

/**
 * \brief Clean the display
 *
 * This is the procedure that clean the display in HW mode.
 * you have to write it according with your hardware and micro.
 */
INLINE void sseg_off(void)
{
/* Set the display OFF */
#ifdef CONFIG_LED_7SEG_CCAT
/* You have to implment it */
/* Common Cathode */
#else
/* You have to implment it */
/* Common Anode */
#endif
}

/**
 * \brief writes the character to the single digit of the display
 *
 * This is the procedure that writes the character to the single digit
 * of the display, you have to write it according with your hardware and micro.
 *
 * 	\param dgt the character that has to be displayed
 * 	\param n_dgt the digit where to disply the character of the display's digits.
 */
INLINE void sseg_on(uint8_t dgt, uint8_t n_dgt)
{
	(void) dgt;
	(void) n_dgt;
#ifdef CONFIG_LED_7SEG_CCAT
/* Common Cathode */
/* You have to implment it */
#else
/* Common Anode */
/* You have to implment it */
#endif
}

/**
 * \brief initalize the HW regsiters
 *
 * This is the procedure that initalize the HW regsiters.
 * you have to write it according with your hardware and micro.
 */
INLINE void sseg_init(void)
{
	/* Initialize PIN Direction to OUTPUT*/
	/* You have to implment it */
	/* Set the display OFF */
	sseg_off();
}

#endif /* HW_LED_7SEG_H */

