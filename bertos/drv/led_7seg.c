/**
 * \file led_7seg.c
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
 * \brief 7 segments LED displays (impl)
 *
 * \author Fabio Bizzi <fbizzi@bizzi.org>
 *
 * \addtogroup SevenSegDisplay 7 Segments LED Displays Driver
 * \{
 *
 */

#include "drv/led_7seg.h"
#include "hw/hw_led_7seg.h"
#include "cfg/cfg_arch.h"
#include <drv/timer.h>
#include <string.h>

/*
 * Define the timer for the refreshing
 */
static Timer sseg_trefresh;

/*
 * FUNCTION: sseg_refresh
 *
 * This is the procedure that prints the seven_seg structure'string to the display.
 * It prints a single digit at time and does all the checks to a proper display.
 * It is called by the wrapper function fired by the timer set in the init procedure.
 * 
 * param SS The void pointer that holds the pointer to the data structure
 *
 */
static void sseg_refresh(SevenSeg *SS)
{
	/* First Check if the structure is being edited we do a silent exit*/
	if (SS->busyedit == true)
		return;

	/* If we have displayed all the digits */
	if (SS->curdigit == CONFIG_LED_7SEG_DIGIT)
	{
		sseg_off();
		/* And if we have to display again all the digit */
		if (SS->curspeed > 0)
		{
			SS->curspeed--;
			SS->curdigit = 0;
			SS->curpos -= CONFIG_LED_7SEG_DIGIT;
		}
		/* Else is time to scroll the text */
		else
		{
			/* If we aren't at the end of the string */
			if (SS->curpos < SS->string_len)
			{
				SS->curpos -= CONFIG_LED_7SEG_DIGIT;
				SS->curpos++;
				SS->curdigit = 0;
				SS->curspeed = SS->speed;
			}
			/* Else we are at the end of the string */
			else
			{
				/* Set that the string was displayed at least one time and we can accept a new text*/
				SS->firstrun = false;
				/* If the string has to be displayed only one time we set an empty string
				 * to display till next print request*/
				if (SS->runonce == true)
					sevenseg_clear(SS);
				/* Else we reset the text to it's first character ad do again the display */
				else
				{
					SS->curdigit = 0;
					SS->curpos = 0;
					SS->curspeed = SS->speed;
				}
			}
		}
	}
	/* Else We have to do a print*/
	else
	{
		/* If the text doesn't have to blink we write the character to the proper disply's digit */
		if (SS->blink == false)
			sseg_on(SS->string[SS->curpos], SS->curdigit);
		/* Else we do the blinking */
		else
		{
			/* If bdigit == 0 we have to blink all the digits */
			if (SS->bdigit == 0)
			{
				if (SS->curspeed >= (SS->speed/2))
					sseg_on(SS->string[SS->curpos], SS->curdigit);
				else
					sseg_off();
			}
			/* else we have to blink only one digit (bdigit -1) */
			else
				/* is this the digit to blink? */
				if (SS->curdigit == ((unsigned int)SS->bdigit-1))
				{
					if (SS->curspeed >= (SS->speed/2))
						sseg_on(SS->string[SS->curpos], SS->curdigit);
					else
						sseg_off();
				}
				/* no, so let do a normal print */
				else
					sseg_on(SS->string[SS->curpos], SS->curdigit);
		}
		/* Ok, next time next char.... ;) */
		SS->curdigit++;
		SS->curpos++;
	}
}

/*
 * FUNCTION: sseg_refresh_wrapper
 *
 * This is a "wrapper" procedure that is called by the timer_setSoftint()
 * with the unique purpose to call the real sseg_refresh procedure without
 * the cast of the structure from void to SevenSeg.
 *
 * param VSS The void pointer that holds the pointer to the data structure
 *
 */
static void sseg_refresh_wrapper(void *VSS)
{
	/* Here we cast the Structure from void to proper type */
	SevenSeg *SS;
	SS = (SevenSeg *)VSS;
	/* now we call the right refresh routine */
	sseg_refresh(SS);
	/* ReStart the timer */
	timer_add(&sseg_trefresh);
}

/*
 * FUNCTION: sseg_tabcheck
 *
 * This function return the position of the ascii character in the hex
 * segstable.
 *
 * param source The ascii char to be positioned
 */
INLINE uint8_t sseg_tabcheck(char source)
{
	uint8_t hexchar=38;

	/* Numbers */
	if ((((int)source) > 47) && (((int)source) < 58))
		hexchar = source-48;
	else
		/* Capital Letters */
		if ((((int)source) > 64) && (((int)source) < 91))
			hexchar = source-53;
		else
			/* Letters */
			if ((((int)source) > 96) && (((int)source) < 123))
				hexchar = source-85;
			else
				/* Minus */
				if (((int)source) == 45)
					hexchar = 11;
				else
					/* Space */
					if (((int)source) == 32)
						hexchar = 38;
					else
						/* Dot */
						if (((int)source) == 46)
							hexchar = 10;
	return hexchar;
}

/**
 * \brief Print a string on the display
 *
 * This is the procedure that fills the seven_seg structure with the translated
 * string to display. It swaps also the structures to display the new text when
 * all the data is ready to display.
 *
 * \param SS Pointer to the SevenSeg structure
 * \param sstring String to be displayed
 *
 *  \return 0 if all went well, -1 if the display is locked, -2 if the string too long.
 */
int sevenseg_print(SevenSeg *SS, const char *sstring)
{
	size_t string_lenght;
	unsigned int x,y,dotnumber;
	bool dotjump = false;
	uint8_t hexchar;

	/* Check if the display is unlocked */
	if (SS->busyedit == false)
		return -1;

	/* Check if the string is too big */
	if (sizeof(&sstring) > (CONFIG_LED_7SEG_STRLEN-(2*CONFIG_LED_7SEG_DIGIT)))
		return -2;

	/* get the string length and set the number of dots in the string to 0 */
	string_lenght = strlen(sstring);
	dotnumber = 0;

	/* check if there are some dots in the string and report the number in dotnumber */
	for (x=0;x<(unsigned int)string_lenght;x++)
	{
		if (((int)sstring[x]) == 46)
			dotnumber++;
	}

	/* If the *REAL* lenght of the string is less or equal than the number of digits */
	if (((int)string_lenght-dotnumber) <= CONFIG_LED_7SEG_DIGIT)
	{
		/* If the *REAL* lenght of the string is less than number of digits */
		if (((int)string_lenght-dotnumber) < CONFIG_LED_7SEG_DIGIT)
		{
			/* Fill the left side of the string with blanks */
			for (x=0; x<(CONFIG_LED_7SEG_DIGIT-((int)string_lenght-dotnumber)); x++)
				SS->string[x] = segstable[38];
			y = x;
		}
		else
		{
			/* Else we have the exact string length of the display */
			y = 0;
		}
	}
	else
	{
		/* Else we have the string length bigger than the display and we need to fill
		 * the entire left side of the string with blanks to begin the scroll from the
		 * rigthest side of the display */
		for (x=0; x<CONFIG_LED_7SEG_DIGIT; x++)
			SS->string[x] = segstable[38];
		y = CONFIG_LED_7SEG_DIGIT;
	}
	/* Here we start to fill the string with the Hex 7seg characters values */
	hexchar = 0;
	for (x=0; x<(unsigned int)string_lenght; x++)
	{
		hexchar = sseg_tabcheck(sstring[x]);
		/* do we have a dot? */
		if (hexchar == 10)
		{
			/* If we are at the first character of the string it has to be forced
			 * as "lonly" dot ;) */
			if (x > 0)
			{
#if CONFIG_LED_7SEG_CCAT
				SS->string[y-1] = SS->string[y-1] | segstable[(int)hexchar];
#else
				SS->string[y-1] = SS->string[y-1] & segstable[(int)hexchar];
#endif
				dotjump = true;
			}
		}
		/* If the last character was a dot and we aren't at the first character of the string
		 * we have just inserted it */
		if (dotjump)
			dotjump = false;
		/* Let's put the character in the structure's string */
		else
		{
			SS->string[y] = segstable[(int)hexchar];
			y++;
		}
	}
	/* If we have the string length bigger than the display we need to fill
	 * the entire right side of the string with blanks to end the scroll
	 * to the rigthest side of the display */
	if (((int)string_lenght-dotnumber) > CONFIG_LED_7SEG_DIGIT)
	{
		for (x=0; x<CONFIG_LED_7SEG_DIGIT; x++)
		{
			SS->string[y] = segstable[38];
			y++;
		}
	}
	/* Let's put the total string length to the structure */
	SS->string_len = y;

	return 0;
}

/**
 * \brief initialize the structure and the timer for the display
 *
 * This is the procedure that inits all the structures that rules the 7 segments
 * display and set the timer for the proper print/refresh of the text.
 *
 *  \param SS Pointer to the SevenSeg structure
 */
void sevenseg_init(SevenSeg *SS)
{
	/*
	 * Init the 7segments string structure
	 */
	SS->busyedit = true;
	sevenseg_clear(SS);
	SS->busyedit = false;

	/*
	 * Init the I/O ports and set the display OFF
	 */
	sseg_init();

	/*
	 * Define the timer for the refresh of the display
	 * The timer calls the sevenseg_refresh function
	 * every "CONFIG_LED_7SEG_RTIME" milliseconds for
	 * an acceptable persistance of a single 7 segments
	 * display.
	 */
 	// set the callback
	timer_setSoftint(&sseg_trefresh, sseg_refresh_wrapper, (void *)SS);
	// expire time: 1s
	timer_setDelay(&sseg_trefresh, ms_to_ticks(CONFIG_LED_7SEG_RTIME));
	// start the timer
	timer_add(&sseg_trefresh);
}

/**
 * \brief check if is possible to do a new print
 *
 * This is the procedure that check if the print of the current text is run
 * almost one time and we're ready to print a new text.
 *
 *  \param SS Pointer to the SevenSeg structure
 *
 *  \return true if we can print a new text, false if we're still printing the previous text for the first time and we have to wait.
 */
bool sevenseg_isReady(SevenSeg *SS)
{
		return !SS->firstrun;
}

/**
 * \brief unlock the SevenSeg structure and stops the print
 *
 * This is the procedure that check if the print of the current text is run
 * almost one time and then set the status of the display to "unlocked".
 *
 *  \param SS Pointer to the SevenSeg structure
 *
 *  \return true if the display is unlocked, false if the dispaly is still locked.
 */
bool sevenseg_unlock(SevenSeg *SS)
{
	if (SS->firstrun == false)
	{
		SS->busyedit = true;
		SS->firstrun = true;
		SS->curdigit = 0;
		SS->curpos = 0;
	}
	else
		return false;
	return true;
}

/**
 * \brief lock the SeveSeg structure and starts a new print
 *
 * This is the procedure that lock the display and permit
 * the print of the text.
 *
 *  \param SS Pointer to the SevenSeg structure
 *
 *  \return true if the display is now locked, false if the display was already locked.
 */
bool sevenseg_lock(SevenSeg *SS)
{
	if (SS->busyedit == true)
	{
	/* If the string is longer than the number of the digit of the display we
	 * reset the single digit blink to zero to be sure that the display of
	 * the text is clean from previous single digit blinking settings */
		if (SS->string_len > CONFIG_LED_7SEG_DIGIT)
			SS->bdigit = 0;
		SS->busyedit = false;
	}
	else
		return false;
	return true;
}

/**
 * \brief set the blinking of the digits of the display
 *
 * This is the procedure that set the blinking of the display.
 * You can choose to blink all the display or only a single
 * digit.
 *
 *  \param SS Pointer to the SevenSeg structure
 *  \param blink if true the display will blink
 *  \param digit if 0 all the digits have to blink, else the digit that has to blink
 *
 *  \return true if the set was succesfull, false if the set was not succesfull.
 */
bool sevenseg_setBlink(SevenSeg *SS, bool blink, uint8_t digit)
{
	if (SS->busyedit == true)
	{
		if (blink == true)
		{
			if (digit == 0)
				SS->bdigit = digit;
			else
				if ((digit-1) <= CONFIG_LED_7SEG_DIGIT)
					SS->bdigit = digit;
				else
					return false;
		}
		SS->blink = blink;
	}
	else
		return false;
	return true;
}

/**
 * \brief set if the text has to be displayed just one time
 *
 * This is the procedure that set if the text has to be displayed
 * just one time
 *
 *  \param SS Pointer to the SevenSeg structure
 *  \param runonce true if the text has to be displayed only one time, false if the text has to be displayed till next print
 *
 *  \return true if the set was succesfull, false if the set was not succesfull.
 */
bool sevenseg_setRunonce(SevenSeg *SS, bool runonce)
{
	if (SS->busyedit == true)
		SS->runonce = runonce;
	else
		return false;
	return true;
}

/**
 * \brief set the scrolling speed of the text
 *
 * This is the procedure that set the scrolling speed of the text
 * if the text is longer than the display digits or the
 * duration of the display if the text is smaller or equal the
 * length of display digits.
 *
 *  \param SS Pointer to the SevenSeg structure
 *  \param r_speed the Scrolling speed or display time
 *
 *  \return true if the set was succesfull, false if the set was not succesfull.
 */
bool sevenseg_setRunspeed(SevenSeg *SS, unsigned int r_speed)
{
	if (SS->busyedit == true)
	{
		SS->speed = r_speed;
		SS->curspeed = r_speed;
	}
	else
		return false;
	return true;
}

/**
 * \brief clear the display
 *
 * This is the procedure that blanks the text to be displayed
 * and so on clear the display.
 *
 *  \param SS Pointer to the SevenSeg structure
 *
 *  \return true if the clear was succesfull, false if the clear was not succesfull.
 */
bool sevenseg_clear(SevenSeg *SS)
{
	if (SS->busyedit == true)
	{
		memset(((void *)&SS->string),segstable[38],sizeof(SS->string));
		SS->string_len = CONFIG_LED_7SEG_DIGIT;
		SS->blink = false;
		SS->bdigit = 0;
		SS->runonce = false;
		SS->curdigit = 0;
		SS->curpos = 0;
		SS->speed = CONFIG_LED_7SEG_SSPEED;
		SS->curspeed = CONFIG_LED_7SEG_SSPEED;
		SS->firstrun = false;
	}
	else
		return false;
	return true;
}
 /** \} */ //defgroup drivers
