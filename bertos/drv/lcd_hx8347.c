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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Himax HX8347 LCD controller driver
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

#include "lcd_hx8347.h"

#include "hw/hw_hx8347.h"
#include <cfg/debug.h>
#include <drv/timer.h>

// Himax HX8347 chip id
#define HX8347_ID_HIMAX  0x47


static uint16_t lcd_row[LCD_WIDTH];


struct lcd_hx8347_reg
{
	uint8_t cmd;    // Register index, if 0xFF wait for value ms
	uint8_t data;   // Register value
};

static const struct lcd_hx8347_reg init_seq[] =
{
	// Start internal OSC
	{ 0x19, 0x49 },   // OSCADJ=10 0000, OSD_EN=1 //60Hz
	{ 0x93, 0x0C },   // RADJ=1100

	// Power on flow
	{ 0x44, 0x4D },   // VCM=100 1101
	{ 0x45, 0x11 },   // VDV=1 0001
	{ 0x20, 0x40 },   // BT=0100
	{ 0x1D, 0x07 },   // VC1=111
	{ 0x1E, 0x00 },   // VC3=000
	{ 0x1F, 0x04 },   // VRH=0100

	{ 0x1C, 0x04 },   // AP=100
	{ 0x1B, 0x10 },   // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
	{ 0xFF, 50   },   // 50 ms delay

	{ 0x43, 0x80 },   // Set VCOMG=1
	{ 0xFF, 50   },   // 50 ms delay

#if 0
	// Gamma for CMO 2.8
	{ 0x46, 0x95 },
	{ 0x47, 0x51 },
	{ 0x48, 0x00 },
	{ 0x49, 0x36 },
	{ 0x4A, 0x11 },
	{ 0x4B, 0x66 },
	{ 0x4C, 0x14 },
	{ 0x4D, 0x77 },
	{ 0x4E, 0x13 },
	{ 0x4F, 0x4C },
	{ 0x50, 0x46 },
	{ 0x51, 0x46 },
#endif

	// 240x320 window setting
	{ 0x02, 0x00 },   // Column address start2
	{ 0x03, 0x00 },   // Column address start1
	{ 0x04, 0x00 },   // Column address end2
	{ 0x05, 0xEF },   // Column address end1
	{ 0x06, 0x00 },   // Row address start2
	{ 0x07, 0x00 },   // Row address start1
	{ 0x08, 0x01 },   // Row address end2
	{ 0x09, 0x3F },   // Row address end1

	// Display Setting
	{ 0x01, 0x06 },   // IDMON=0, INVON=1, NORON=1, PTLON=0
//	{ 0x16, 0xC8 },   // MY=1, MX=1, MV=0, BGR=1
	{ 0x16, 0x68 },   // MY=0, MX=1, MV=1, RGB XY exchange X mirror
	{ 0x23, 0x95 },   // N_DC=1001 0101
	{ 0x24, 0x95 },   // P_DC=1001 0101
	{ 0x25, 0xFF },   // I_DC=1111 1111
	{ 0x27, 0x06 },   // N_BP=0000 0110
	{ 0x28, 0x06 },   // N_FP=0000 0110
	{ 0x29, 0x06 },   // P_BP=0000 0110
	{ 0x2A, 0x06 },   // P_FP=0000 0110
	{ 0x2C, 0x06 },   // I_BP=0000 0110
	{ 0x2D, 0x06 },   // I_FP=0000 0110
	{ 0x3A, 0x01 },   // N_RTN=0000, N_NW=001
	{ 0x3B, 0x01 },   // P_RTN=0000, P_NW=001
	{ 0x3C, 0xF0 },   // I_RTN=1111, I_NW=000
	{ 0x3D, 0x00 },   // DIV=00
	{ 0x3E, 0x38 },   // SON=38h
	{ 0x40, 0x0F },   // GDON=0Fh
	{ 0x41, 0xF0 },   // GDOF=F0h
};

/*
 * Write to an LCD register.
 */
static void regWrite(uint8_t reg, uint16_t val)
{
	hx8347_cmd(reg);
	hx8347_write(val);
}

/*
 * Read data from a LCD register.
 */
static uint16_t regRead(uint8_t reg)
{
	hx8347_cmd(reg);
	return hx8347_read();
}

/*
 * Write data in a buffer to the LCD controller.
 */
static void bufferWrite(const uint16_t *buf, uint16_t size)
{
	uint16_t i;
	for (i = 0 ; i < size; ++i)
		hx8347_write(buf[i]);
}

static void lcd_setCursor(uint16_t x, uint16_t y)
{
	regWrite(0x02, x >> 8);
	regWrite(0x03, x & 0xff);
	regWrite(0x06, y >> 8);
	regWrite(0x07, y & 0xff);
}

static void lcd_setWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	ASSERT(x + width <= LCD_WIDTH);
	ASSERT(y + height <= LCD_HEIGHT);
	ASSERT(width > 0);
	ASSERT(height > 0);

	// Window right and bottom limits are inclusive
	width--;
	height--;

	lcd_setCursor(x, y);

	regWrite(0x04, (x + width) >> 8);
	regWrite(0x05, (x + width) & 0xff);
	regWrite(0x08, (y + height) >> 8);
	regWrite(0x09, (y + height) & 0xff);
}

/*
 * Refresh a bitmap on screen
 */
void lcd_hx8347_blitBitmap(const Bitmap *bm)
{
	uint8_t mask;
	int i, l, r;

	lcd_setWindow(0, 0, bm->width, bm->height);
	hx8347_cmd(0x22);

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
			bufferWrite(lcd_row, bm->width);
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
		bufferWrite(lcd_row, bm->width);
	}
}

/*
 * Blit a 24 bit color raw raster directly on screen
 */
void lcd_hx8347_blitBitmap24(int x, int y, int width, int height, const char *bmp)
{
	int l, r;

	lcd_setWindow(x, y, width, height);
	hx8347_cmd(0x22);

	for (l = 0; l < height; l++)
	{
		for (r = 0; r < width; r++)
		{
			lcd_row[r] =
				(((uint16_t)bmp[0] << 8) & 0xF800) |
				(((uint16_t)bmp[1] << 3) & 0x07E0) |
				(((uint16_t)bmp[2] >> 3) & 0x001F);
			bmp += 3;
		}
		bufferWrite(lcd_row, width);
	}
}

/**
 * Turn off display.
 */
void lcd_hx8347_off(void)
{
	regWrite(0x90, 0);  // SAP=0000 0000
	regWrite(0x26, 0);  // GON=0, DTE=0, D=00
}

/**
 * Turn on display.
 */
void lcd_hx8347_on(void)
{
	regWrite(0x90, 0x7F);  // SAP=0111 1111
	regWrite(0x26, 0x04);  // GON=0, DTE=0, D=01
	timer_delay(100);
	regWrite(0x26, 0x24);  // GON=1, DTE=0, D=01
	regWrite(0x26, 0x2C);  // GON=1, DTE=0, D=11
	timer_delay(100);
	regWrite(0x26, 0x3C);  // GON=1, DTE=1, D=11
}

/**
 * Display initialization.
 */
void lcd_hx8347_init(void)
{
	unsigned i;
	uint16_t chip_id;

	hx8347_busInit();
	lcd_hx8347_off();

	// Check chip id
	if ((chip_id = regRead(0x67)) != HX8347_ID_HIMAX)
	{
		kprintf("HX8347 chip id read error or wrong id (0x%x), skipping initialization.\n", chip_id);
		return;
	}

	for (i = 0; i < countof(init_seq); i++)
	{
		if (init_seq[i].cmd != 0xFF)
			regWrite(init_seq[i].cmd, init_seq[i].data);
		else
			timer_delay(init_seq[i].data);
	}

	lcd_hx8347_on();
}
