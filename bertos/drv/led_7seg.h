/**
 * \file led_7seg.h
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
 * \author Fabio Bizzi <fbizzi@bizzi.org>
 *
 * \defgroup SevenSegDisplay 7 Segments LED Displays Driver
 * \ingroup drivers
 * \{
 *
 * \brief 7 segments LED displays (headers)
 *
 * Here you find the prototypes and the data structures that
 * format and holds the text that has to be displayed by the
 * 7 segments display.
 *
 * The main function is the sevenseg_print()
 * that is called by your software to display the text, also
 * important is the sevenseg_init() that initialize the data
 * structures and set the refresh timer, you need to call
 * this procedure just one time in the init procedure of
 * your software before any use of the sevenseg_print().
 *
 * Usage:
 *
 * To use succesfully the display you have to follow these steps:
 *
 * \li Create the structure and init the display
 * \li Check if the display is ready to accept a new print with
 *    the function sevenseg_isReady()
 * \li Unlock the display with the function sevenseg_unlock()
 *    NOTE: when the display is unlocked the displaying of any
 *    text is stopped but the data (text and attributes like text
 *    position, blinking etc etc) are not erased.
 * \li Set the wanted text attributes with sevenseg_set* functions
 * \li Print the wanted text with sevenseg_print()
 * \li Lock the display with sevenseg_lock()
 *
 * When the display is locked the displaying of the text starts.
 *
 *
 * \code
 * static Seven_Seg display;
 * // ...
 * sevenseg_init(&display);
 * while (!sevenseg_isReady(&display))
 *    ;
 * sevenseg_unlock(&display);
 * sevenseg_setBlink(&display,false,0);
 * sevenseg_setRunonce(&display,false);
 * sevenseg_setRunspeed(&display,10);
 * if ((sevenseg_print(&display, "made with bertos.")) != 0)
 *		return -1;
 * sevenseg_lock(&display);
 * \endcode
 *
 * $WIZ$ module_name = "led_7seg"
 * $WIZ$ module_depends = "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_led_7seg.h"
 * $WIZ$ module_hw = "bertos/hw/hw_led_7seg.h"
 */

#ifndef DRV_LED_7SEG_H
#define DRV_LED_7SEG_H

#include "cfg/cfg_led_7seg.h"
#include <drv/timer.h>
#include <mware/event.h>

/**
 * \name Numbers and Letters Table.
 * \note These tables contain all the printable
 *       characters on a 7 segment digit encoded
 *       for common cathode and common anode
 *       display type.
 *
 * 0, 1, 2, 3, 4, 5, 6, 7, 8,
 * 9, ., -, A, B, C, D, E, F,
 * G, H, I, J, K, L, M, N, O,
 * P, Q, R, S, T, U, V, W, X,
 * Y, Z, SPACE
 *
 * \{
 */
#if CONFIG_LED_7SEG_CCAT
		static const uint8_t segstable[] =
        {
                0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x7, 0x7f,
				0x6f, 0x80, 0x40, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,
				0x3d, 0x74, 0x30, 0x1e, 0x75, 0x38, 0x15, 0x37, 0x3f,
				0x73, 0x67, 0x50, 0x6d, 0x78, 0x3e, 0x2a, 0x6a, 0x76,
				0x6e, 0x5b, 0x0
        };
#else
		static const uint8_t segstable[] =
        {
				0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80,
				0x90, 0x7f, 0xbf, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e,
				0xc2, 0x8b, 0xcf, 0xe1, 0x8a, 0xc7, 0xea, 0xc8, 0xc0,
				0x8c, 0x98, 0xaf, 0x92, 0x87, 0xc1, 0xd5, 0x95, 0x89,
				0x91, 0xa4, 0xff
		};
#endif
/**\} */

/**
 * Context data for Display functions
 *
 */
typedef struct SevenSeg
{
		/** The String to be displayed */
		uint8_t string[CONFIG_LED_7SEG_STRLEN];
		/** The length of the string to be displayed */
		unsigned int string_len;
		/** Has to blink? */
		bool blink;
		/** Has to blink only one digit? */
		uint8_t bdigit;
		/** Has to be displayed only one time? */
		bool runonce;
		/** Scrolling speed */
		unsigned int speed;
		/** Working scrolling speed */
		unsigned int curspeed;
		/** Is it printed at least one time? */
		bool firstrun;
		/** Working current position */
		unsigned int curpos;
		/** Working current digit */
		unsigned int curdigit;
		/** Is the structure in edit? */
		bool busyedit;
} SevenSeg;

/* Functions prototypes */

/*
 * This is the procedure that fills the seven_seg structure with the translated
 * string to display. It swaps also the structures to display the new text when
 * all the data is ready to display.
 */
int sevenseg_print(SevenSeg *SS, const char *sstring);

/*
 * This is the procedure that inits all the structures that rules the 7 segments
 * display and set the timer for the proper print/refresh of the text.
 */
void sevenseg_init(SevenSeg *SS);

/*
 * This is the procedure that does a short print of all segments of all
 * digits of the display.
 */
void sevenseg_test(SevenSeg *SS);

/*
 * This is the procedure that check if the print of the current text is run
 * almost one time and we're ready to print a new text.
 */
bool sevenseg_isReady(SevenSeg *SS);

/*
 * This is the procedure that check if the print of the current text is run
 * almost one time and then set the status of the display to "unlocked".
 */
bool sevenseg_unlock(SevenSeg *SS);

/*
 * This is the procedure that lock the display and permit
 * the print of the text.
 */
bool sevenseg_lock(SevenSeg *SS);

/*
 * This is the procedure that set the blinking of the display.
 * You can choose to blink all the display or only a single
 * digit.
 */
bool sevenseg_setBlink(SevenSeg *SS, bool blink, uint8_t digit);

/*
 * This is the procedure that set if the text has to be displayed
 * just one time
 */
bool sevenseg_setRunonce(SevenSeg *SS, bool runonce);

/*
 * This is the procedure that set the scrolling speed of the text
 * if the text is longer than the display digits or the
 * duration of the display if the text is smaller or equal the
 * length of display digits.
 */
bool sevenseg_setRunspeed(SevenSeg *SS, unsigned int r_speed);

/*
 * This is the procedure that blanks the text to be displayed
 * and so on clear the display.
 */
bool sevenseg_clear(SevenSeg *SS);

#endif /* DRV_LED_7SEG_H */
 /** \} */ //defgroup drivers
