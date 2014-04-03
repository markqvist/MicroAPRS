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
 * \brief OLED-RIT-128x96 (P14201) graphic display driver
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "lcd_rit128x96.h"

#include "hw/hw_rit128x96.h"

#include <cfg/debug.h>
#include <cfg/macros.h>


/*
 * Hard-coded command initialization sequence.
 *
 * NOTE: the first byte is the size of the command.
 */
static const uint8_t init_cmd[] =
{
	/* Unlock commands */
	3, 0xfd, 0x12, 0xe3,
	/* Display off */
	2, 0xae, 0xe3,
	/* Icon off */
	3, 0x94, 0, 0xe3,
	/* Multiplex ratio */
	3, 0xa8, 95, 0xe3,
	/* Contrast */
	3, 0x81, 0xb7, 0xe3,
	/* Pre-charge current */
	3, 0x82, 0x3f, 0xe3,
	/* Display Re-map */
	3, 0xa0, 0x52, 0xe3,
	/* Display Start Line */
	3, 0xa1, 0, 0xe3,
	/* Display Offset */
	3, 0xa2, 0x00, 0xe3,
	/* Display Mode Normal */
	2, 0xa4, 0xe3,
	/* Phase Length */
	3, 0xb1, 0x11, 0xe3,
	/* Frame frequency */
	3, 0xb2, 0x23, 0xe3,
	/* Front Clock Divider */
	3, 0xb3, 0xe2, 0xe3,
	/* Set gray scale table */
	17, 0xb8, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 19, 22, 26, 30, 0xe3,
	/* Second pre-charge period */
	3, 0xbb, 0x01, 0xe3,
	/* Pre-charge voltage */
	3, 0xbc, 0x3f, 0xe3,
	/* Display ON */
	2, 0xaf, 0xe3,
};

/*
 * Hard-coded command shutdown sequence.
 */
static const uint8_t exit_cmd[] =
{
	/* Display OFF */
	0xae, 0xe3
};

/*
 * Hard-coded horizontal increment command.
 */
static const uint8_t horizontal_inc[] =
{
	0xa0, 0x52
};

/**
 * Write a sequence of data bytes to the LCD controller
 */
static void lcd_dataWrite(const uint8_t *buf, size_t count)
{
	while (count--)
		LCD_WRITE(*buf++);
}

/* Turn on the OLED display */
void rit128x96_on(void)
{
	unsigned int i;

	/* Loop through the SSD1329 controller initialization sequence */
	LCD_SET_COMMAND();
	for (i = 0; i < sizeof(init_cmd); i += init_cmd[i] + 1)
		lcd_dataWrite(init_cmd + i + 1, init_cmd[i] - 1);
}

/* Turn off the OLED display */
void rit128x96_off(void)
{
	LCD_SET_COMMAND();
	lcd_dataWrite(exit_cmd, sizeof(exit_cmd));
}

static void lcd_start_blit(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t buffer[3];

	ASSERT((x + width) <= LCD_WIDTH);
	ASSERT((y + height) <= LCD_HEIGHT);

	/* Enter command mode */
	LCD_SET_COMMAND();

	buffer[0] = 0x15;
	buffer[1] = x / 2;
	buffer[2] = (x + width - 2) / 2;
	lcd_dataWrite(buffer, 3);

	buffer[0] = 0x75;
	buffer[1] = y;
	buffer[2] = y + height - 1;
	lcd_dataWrite(buffer, 3);
	lcd_dataWrite((const uint8_t *)&horizontal_inc, sizeof(horizontal_inc));
}

/* Refresh a raw image on screen */
void rit128x96_blitRaw(const uint8_t *data,
		uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	lcd_start_blit(x, y, width, height);
	/*
	 * Enter data mode and send the encoded image data to the OLED display,
	 * over the SSI bus.
	 */
	LCD_SET_DATA();
	while (height--)
	{
		/* Write an entire row at once */
		lcd_dataWrite(data, width / 2);
		data += width / 2;
	}
}

/* Refresh a bitmap on screen */
void rit128x96_blitBitmap(const Bitmap *bm)
{
	uint8_t lcd_row[bm->width / 2];
	uint8_t mask;
	int i, l;

	lcd_start_blit(0, 0, bm->width, bm->height);
	/*
	 * Enter data mode and send the encoded image data to the OLED display,
	 * over the SSI bus.
	 */
	LCD_SET_DATA();
	for (l = 0; l < bm->height / 8; l++)
	{
		for (mask = 1; mask; mask <<= 1)
		{
			for (i = 0; i < bm->width; i++)
			{
				if (bm->raster[l * bm->width + i] & mask)
					lcd_row[i / 2] |= i & 1 ? 0x0f : 0xf0;
				else
					lcd_row[i / 2] &= i & 1 ? 0xf0 : 0x0f;
			}
			/* Write an entire row at once */
			lcd_dataWrite(lcd_row, sizeof(lcd_row));
		}
	}
}

/* Initialize the OLED display */
void rit128x96_init(void)
{
	/* Initialize the communication bus */
	lcd_rit128x96_hw_bus_init();

	/* Turn on the OLED display */
	rit128x96_on();
}
