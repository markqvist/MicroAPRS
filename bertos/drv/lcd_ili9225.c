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
 * \brief ILI9225B 4 wire interface graphic driver
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * Display initialization sequence is based on Atmel's softpack library
 * implementation, see license below.
 */

/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#include "lcd_ili9225.h"

#include "hw/hw_ili9225.h"

#include <drv/timer.h>
#include <io/kfile.h>
#include <cpu/byteorder.h>


static struct KFile *spi;

/*
 * Display row buffer.  When refreshing display one full row of
 * graphics data is transferred with DMA, to speed up transfer and
 * reduce CPU usage.
 */
static uint16_t lcd_row[LCD_WIDTH];


struct lcd_ili9225_reg
{
	uint8_t  cmd;          // Register index, if 0xFF wait for value ms
	uint16_t data;         // Register value
};

static const struct lcd_ili9225_reg init_seq[] =
{
	{0x01, 0x011c},        // Set SS, SM, GS and NL bits
	{0x02, 0x0100},        // Set 1 line inversion
	{0x03, 0x1030},        // Entry Mode set GRAM write direction and BGR=1
	{0x08, 0x0808},        // Set BP and FP
	{0x0C, 0x0001},        // RGB Input Interface Control: 16-bit RGB interface
	{0x0F, 0x0A01},        // Set frame rate: 83Hz
	{0x20, LCD_WIDTH},     // Set GRAM Address
	{0x21, LCD_HEIGHT},    // Set GRAM Address

	/* power on sequence */
	{0x10, 0x0A00},        // Set asp DSTB,STB
	{0x11, 0x1038},        // SET APON PON AON VCI1EN VC
	{0xFF, 50},            // Wait 50 ms

	{0x12, 0x1121},        // Internal reference voltage = VCI
	{0x13, 0x06CE},        // Set GVDD
	{0x14, 0x676F},        // Set VCOMH/VCOML voltage

	// Set gram area
	{0x30, 0x0000},
	{0x31, 0x00DB},
	{0x32, 0x0000},
	{0x33, 0x0000},
	{0x34, 0x00DB},
	{0x35, 0x0000},
	{0x36, LCD_WIDTH},
	{0x37, 0x0000},
	{0x38, LCD_HEIGHT},
	{0x39, 0x0000},

	// Set gamma curve
	{0x50, 0x0000},
	{0x51, 0x060A},
	{0x52, 0x0D0A},
	{0x53, 0x0303},
	{0x54, 0x0A0D},
	{0x55, 0x0A06},
	{0x56, 0x0000},
	{0x57, 0x0303},
	{0x58, 0x0000},
	{0x59, 0x0000},
};

static void lcd_cmd(uint8_t cmd)
{
	LCD_CS_LOW();
	LCD_RS_LOW();
	kfile_write(spi, &cmd, sizeof(cmd));
}

static void lcd_data(uint16_t *data, size_t count)
{
	kfile_flush(spi);
	LCD_RS_HIGH();
	kfile_write(spi, data, count*2);
	kfile_flush(spi);
	LCD_CS_HIGH();
}

static void lcd_regWrite(uint8_t reg, uint16_t data)
{
	uint16_t word = cpu_to_be16(data);

	lcd_cmd(reg);
	lcd_data(&word, 1);
}

static void lcd_startBlit(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	ASSERT((x + width) <= LCD_WIDTH);
	ASSERT((y + height) <= LCD_HEIGHT);

	lcd_regWrite(0x36, x + width);
	lcd_regWrite(0x37, x);
	lcd_regWrite(0x38, y + height);
	lcd_regWrite(0x39, y);

	lcd_regWrite(0x20, x);
	lcd_regWrite(0x21, y);
}

/*
 * Refresh a raw image on screen
 */
void lcd_ili9225_blitRaw(UNUSED_ARG(const uint8_t *, data),
		uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	lcd_startBlit(x, y, width, height);
	// TODO
}

/*
 * Refresh a bitmap on screen
 */
void lcd_ili9225_blitBitmap(const Bitmap *bm)
{
	uint8_t mask;
	int i, l, r;

	lcd_startBlit(0, 0, bm->width, bm->height);

	for (l = 0; l < bm->height / 8; l++)
	{
		for (mask = 1; mask; mask <<= 1)
		{
			for (i = 0; i < bm->width; i++)
			{
				if (bm->raster[l * bm->width + i] & mask)
					lcd_row[i] = 0x0000;
				else
					lcd_row[i] = 0xFFFF;
			}
			lcd_cmd(0x22);
			lcd_data(lcd_row, bm->width);
		}
	}

	for (r = 0, mask = 1; r < bm->height % 8; r++, mask <<= 1)
	{
		for (i = 0; i < bm->width; i++)
		{
			if (bm->raster[l * bm->width + i] & mask)
				lcd_row[i] = 0x0000;
			else
				lcd_row[i] = 0xFFFF;
		}
		lcd_cmd(0x22);
		lcd_data(lcd_row, bm->width);
	}
}

/*
 * Blit a 24 bit color raw raster directly on screen
 */
void lcd_ili9225_blitBitmap24(int x, int y, int width, int height, const char *bmp)
{
	int l, r;

	lcd_startBlit(x, y, width, height);

	for (l = 0; l < height; l++)
	{
		for (r = 0; r < width; r++)
		{
			lcd_row[r] =
				(((uint16_t)bmp[1] << 11) & 0xE000) |
				(((uint16_t)bmp[2] <<  5) & 0x1F00) |
				(((uint16_t)bmp[0] <<  0) & 0x00F8) |
				(((uint16_t)bmp[1] >>  5) & 0x0007);
			bmp += 3;
		}

		lcd_cmd(0x22);
		lcd_data(lcd_row, width);
	}
}

/**
 * Turn off display.
 */
void lcd_ili9225_off(void)
{
	lcd_regWrite(0x07, 0x0000);
}

/**
 * Turn on display.
 */
void lcd_ili9225_on(void)
{
	lcd_regWrite(0x07, 0x1017);
}

/**
 * Reset display.
 */
static void lcd_reset(void)
{
	LCD_RESET_LOW();
	timer_delay(20);
	LCD_RESET_HIGH();
	timer_delay(50);
}

/**
 * Display initialization.
 */
void lcd_ili9225_init(struct KFile *_spi)
{
	unsigned i;

	ASSERT(_spi);
	spi = _spi;
	lcd_ili9225_hw_bus_init();

	lcd_reset();
	lcd_ili9225_off();

	for (i = 0; i < countof(init_seq); i++)
	{
		if (init_seq[i].cmd != 0xFF)
			lcd_regWrite(init_seq[i].cmd, init_seq[i].data);
		else
			timer_delay(init_seq[i].data);
	}

	lcd_ili9225_on();
}
