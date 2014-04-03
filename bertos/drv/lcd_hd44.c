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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM044L type LCD hardware module (impl.)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "lcd_hd44.h"

#include "hw/hw_lcd_hd44.h"

#include "cfg/cfg_arch.h"

#include <drv/timer.h>

#warning FIXME: Revise and refactor this code.

#if defined(LCD_READ_H) && defined(LCD_READ_L) && defined(LCD_WRITE_H) && defined(LCD_WRITE_L)
	#define CONFIG_LCD_4BIT 1
#elif defined(LCD_READ) && defined(LCD_WRITE)
	#define CONFIG_LCD_4BIT 0
#else
	#error Incomplete or missing LCD_READ/LCD_WRITE macros
#endif

/** Flag di stato del display */
#define LCDF_BUSY  BV(7)

#if CONFIG_LCD_ADDRESS_FAST == 1
#define lcd_address(x) lcd_address[x]
/**
 * Addresses of LCD display character positions, expanded
 * for faster access (DB7 = 1).
 */
static const uint8_t lcd_address[] =
{
	/* row 0 */
	0x80, 0x81, 0x82, 0x83,
	0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8A, 0x8B,
	0x8C, 0x8D, 0x8E, 0x8F,
#if CONFIG_LCD_COLS > 16
	0x90, 0x91, 0x92, 0x93,
#endif

	/* row 1 */
	0xC0, 0xC1, 0xC2, 0xC3,
	0xC4, 0xC5, 0xC6, 0xC7,
	0xC8, 0xC9, 0xCA, 0xCB,
	0xCC, 0xCD, 0xCE, 0xCF,
#if CONFIG_LCD_COLS > 16
	0xD0, 0xD1, 0xD2, 0xD3,
#endif

#if CONFIG_LCD_ROWS > 2
	/* row 2 */
	0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9A, 0x9B,
	0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3,
#if CONFIG_LCD_COLS > 16
	0xA4, 0xA5, 0xA6, 0xA7,
#endif

	/* row 3 */
	0xD4, 0xD5, 0xD6, 0xD7,
	0xD8, 0xD9, 0xDA, 0xDB,
	0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3,
#if CONFIG_LCD_COLS > 16
	0xE4, 0xE5, 0xE6, 0xE7,
#endif

#endif /* CONFIG_LCD_ROWS > 2 */
};

STATIC_ASSERT(countof(lcd_address) == CONFIG_LCD_ROWS * CONFIG_LCD_COLS);
#else  /* CONFIG_LCD_ADDRESS_FAST == 0 */

static const uint8_t col_address[] =
{
	0x80,
	0xC0,
#if CONFIG_LCD_ROWS > 2
	0x94,
	0xD4
#endif
};
STATIC_ASSERT(countof(col_address) == CONFIG_LCD_ROWS);
/**
 * Addresses of LCD display character positions, calculated runtime to save RAM
 */
static uint8_t lcd_address(uint8_t addr)
{
	return col_address[addr / CONFIG_LCD_COLS] + addr % CONFIG_LCD_COLS;
}
#endif /* CONFIG_LCD_ADDRESS_FAST */

/**
 * Current display position. We remember this to optimize
 * LCD output by avoiding to set the address every time.
 */
static lcdpos_t lcd_current_addr;


#if !defined(ARCH_EMUL) || !(ARCH & ARCH_EMUL)
/*      __________________
 * RS
 *
 * R/W  __________________
 *            _______
 * ENA  _____/       \____
 *
 * DATA -<================
 */
INLINE void lcd_dataWrite(uint8_t data)
{
#if CONFIG_LCD_4BIT
	/* Write high nibble */
	LCD_WRITE_H(data);
	LCD_SET_E;
	LCD_DELAY_WRITE;
	LCD_CLR_E;
	LCD_DELAY_WRITE;

	/* Write low nibble */
	LCD_WRITE_L(data);
	LCD_SET_E;
	LCD_DELAY_WRITE;
	LCD_CLR_E;
	LCD_DELAY_WRITE;

#else /* !CONFIG_LCD_4BIT */

	/* Write data */
	LCD_WRITE(data);
	LCD_SET_E;
	LCD_DELAY_WRITE;
	LCD_CLR_E;
	LCD_DELAY_WRITE;

#endif /* !CONFIG_LCD_4BIT */
}

/*      __________________
 * RS
 *         ____________
 * R/W  __/            \__
 *            _______
 * ENA  _____/       \____
 *        ______      ____
 * DATA X/      \====/
 */
INLINE uint8_t lcd_dataRead(void)
{
	uint8_t data;

	LCD_SET_RD;
	LCD_DB_IN;	/* Set bus as input! */
	LCD_DELAY_READ;

#if CONFIG_LCD_4BIT

	/* Read high nibble */
	LCD_SET_E;
	LCD_DELAY_READ;
	data = LCD_READ_H;
	LCD_CLR_E;
	LCD_DELAY_READ;

	/* Read low nibble */
	LCD_SET_E;
	LCD_DELAY_READ;
	data |= LCD_READ_L;
	LCD_CLR_E;
	LCD_DELAY_READ;

#else /* !CONFIG_LCD_4BIT */

	/* Read data */
	LCD_SET_E;
	LCD_DELAY_READ;
	data = LCD_READ;
	LCD_CLR_E;
	LCD_DELAY_READ;

#endif /* !CONFIG_LCD_4BIT */

	LCD_CLR_RD;
	LCD_DB_OUT;	/* Reset bus as output! */

	return data;
}

/*      ___             __
 * RS      \___________/
 *
 * READ __________________
 *            _______
 * ENA  _____/       \____
 *
 * DATA --<===============
 */
INLINE void lcd_regWrite(uint8_t data)
{
	LCD_CLR_RS;
	lcd_dataWrite(data);
	LCD_SET_RS;
}

/*      __               _
 * RS     \_____________/
 *          ___________
 * READ ___/           \__
 *            _______
 * ENA  _____/       \____
 *        ______      ____
 * DATA X/      \====/
 */
INLINE uint8_t lcd_regRead(void)
{
	uint8_t data;

	LCD_CLR_RS;
	data = lcd_dataRead();
	LCD_SET_RS;
	return data;
}

#if CONFIG_LCD_4BIT

INLINE void lcd_mode4Bit(void)
{
	LCD_CLR_RS;

	LCD_WRITE_H(LCD_CMD_SETFUNC);
	LCD_SET_E;
	LCD_DELAY_WRITE;
	LCD_CLR_E;
	LCD_DELAY_WRITE;

	LCD_SET_RS;
}

#endif /* CONFIG_LCD_4BIT */

#else /* ARCH_EMUL */

extern void Emul_LCDWriteReg(uint8_t d);
extern uint8_t Emul_LCDReadReg(void);
extern void Emul_LCDWriteData(uint8_t d);
extern uint8_t Emul_LCDReadData(void);

#define lcd_regWrite(d)   Emul_LCDWriteReg(d)
#define lcd_regRead(d)    Emul_LCDReadReg()
#define lcd_dataWrite(d)  Emul_LCDWriteData(d)
#define lcd_dataRead(d)   Emul_LCDReadData()

#endif /* ARCH_EMUL */


/**
 * Wait until the LCD busy flag clears.
 */
void lcd_waitBusy(void)
{
	for (;;)
	{
		uint8_t val = lcd_regRead();
		if (!(val & LCDF_BUSY))
			break;
	}
}


/**
 * Move the cursor to \a addr, only if not already there.
 */
void lcd_moveTo(uint8_t addr)
{
	if (addr != lcd_current_addr)
	{
		lcd_waitBusy();
		lcd_regWrite(lcd_address(addr));
		lcd_current_addr = addr;
	}
}


/**
 * Write a value in LCD data register, waiting for the busy flag.
 */
void lcd_setReg(uint8_t val)
{
	lcd_waitBusy();
	lcd_regWrite(val);
}

#include <cfg/debug.h>
/**
 * Write the character \a c on display address \a addr.
 *
 * NOTE: argh, the HD44 lcd type is a bad beast: our
 * move/write -> write optimization requires this mess
 * because display lines are interleaved!
 */
void lcd_putc(uint8_t addr, uint8_t c)
{
	if (addr != lcd_current_addr)
		lcd_setReg(lcd_address(addr));

	lcd_waitBusy();
	lcd_dataWrite(c);
	lcd_current_addr = addr + 1;

	/* If we are at end of display wrap the address to 0 */
	if (lcd_current_addr == CONFIG_LCD_COLS * CONFIG_LCD_ROWS)
		lcd_current_addr = 0;

	/* If we are at the end of a row put the cursor at the beginning of the next */
	if (!(lcd_current_addr % CONFIG_LCD_COLS))
		lcd_setReg(lcd_address(lcd_current_addr));
}


/**
 * Remap the glyph of a character.
 *
 * glyph - bitmap of 8x8 bits.
 * code - must be 0-7 for the Hitachi LCD-II controller.
 */
void lcd_remapChar(const char *glyph, char code)
{
	int i;

	/* Set CG RAM address */
	lcd_setReg((uint8_t)((1<<6) | (code << 3)));

	/* Write bitmap data */
	for (i = 0; i < 8; i++)
	{
		lcd_waitBusy();
		lcd_dataWrite(glyph[i]);
	}

	/* Move back to original address */
	lcd_setReg(lcd_address(lcd_current_addr));
}


#if 0 /* unused */
void lcd_remapfont(void)
{
	static const char lcd_glyphs[8] =
	{
		0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00 /* up arrow */
	};
	int i;

	for (i = 0; i < 15; i++)
		lcd_remapChar(i, bernie_char);


	lcd_setAddr(lcd_DefLayer, 0);
	for (i = 0; i < 80; i++)
		lcd_putCharUnlocked(i);
}
#endif /* unused */

void lcd_hw_init(void)
{
	lcd_hd44_hw_bus_init();

	timer_delay(50);

#if CONFIG_LCD_4BIT
	lcd_regWrite(LCD_CMD_SET8BIT);
	lcd_mode4Bit();
	timer_delay(2);
#endif /* CONFIG_LCD_4BIT */

	lcd_regWrite(LCD_CMD_SETFUNC);
	timer_delay(2);

	lcd_regWrite(LCD_CMD_DISPLAY_ON);
	timer_delay(2);

	lcd_regWrite(LCD_CMD_CLEAR);
	timer_delay(2);

#if !CONFIG_LCD_4BIT
	lcd_regWrite(LCD_CMD_RESET_DDRAM); // 4 bit mode doesn't allow char reprogramming
#endif
	lcd_regWrite(LCD_CMD_DISPLAYMODE);
	timer_delay(2);
}


