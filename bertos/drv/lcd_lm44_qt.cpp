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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Custom Qt widget for emulating a graphics LCD display (implementation)
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#include <qpainter.h>
#include <qfont.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include "EmulLCD.h"
#include "Emul.h"


// Display colors
#define LCD_FG_COLOR 0x0, 0x0, 0x0
#define LCD_BG_COLOR 0xBB, 0xCC, 0xBB


EmulLCD::EmulLCD(QWidget *parent, const char *name) :
	QFrame(parent, name, WRepaintNoErase | WResizeNoErase),
	lcd_font("courier", 18),
	fg_color(LCD_FG_COLOR),
	bg_color(LCD_BG_COLOR),
	cr_row(0),
	cr_col(0),
	cgramaddr(-1),
	show_cursor(true)
{
	// initialize DDRAM
	memcpy(ddram,
		"01234567890123456789"
		"abcdefghijhlmnopqrst"
		"ABCDEFGHIJKLMNOPQRST"
		"!@#$%^&*()_+|{}':?><",
		sizeof(ddram));

	// setup font
	lcd_font.setFixedPitch(true);
	setFont(lcd_font);

	// get exact font size
	QFontMetrics fm(lcd_font);
	font_width	= fm.width(QChar(' '));
	font_height	= fm.height();

	// set widget frame
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
//	setLineWidth(2);
	frame_width = frameWidth();
}


EmulLCD::~EmulLCD()
{
	// nop
}


QSizePolicy EmulLCD::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, false);
}


QSize EmulLCD::sizeHint() const
{
	return QSize(
		font_width * COLS + frame_width * 2,
		font_height * ROWS + frame_width * 2);
}


void EmulLCD::drawContents(QPainter *p)
{
	RedrawText(*p);
}


void EmulLCD::SetPainter(QPainter & p)
{
	p.setBackgroundMode(OpaqueMode);
	p.setPen(fg_color);
	p.setBackgroundColor(bg_color);
}


void EmulLCD::RedrawText(QPainter & p)
{
	int r, c;

	SetPainter(p);

	for (r = 0; r < ROWS; r++)
		for (c = 0; c < COLS; c++)
			PrintChar(p, r, c);
}


void EmulLCD::PrintChar(QPainter & p, int row, int col)
{
	// Fetch char from DD RAM
	unsigned char c = ddram[row][col];

	// Map some Hitachi characters to ISO Latin1
	switch(c)
	{
		case 0xDF:
			c = 0xBA;	// "degrees" glyph
			break;

		case 0xE4:
			c = 0xB5;	// "micro" glyph
			break;

		default:		// all others
			break;
	}

	// Draw char on display
	int x = col * font_width + frame_width;
	int y = row * font_height + frame_width;
	bool restore_colors = false;

	if (show_cursor && (row == cr_row) && (col == cr_col))
	{
		// Exchange FG/BG colors
		p.setPen(bg_color);
		p.setBackgroundColor(fg_color);
		restore_colors = true;
	}

	p.drawText(x, y, x + font_width, y + font_height, 0 /*tf*/,
		QString(QChar(c)), 1);

	if (restore_colors)
	{
		// Restore FG/BG colors
		p.setPen(fg_color);
		p.setBackgroundColor(bg_color);
	}
}


void EmulLCD::MoveCursor(int r, int c)
{
	// Save old cursor position
	int old_row = cr_row;
	int old_col = cr_col;
	
	// Move the cursor
	cgramaddr = -1;
	cr_row = r;
	cr_col = c;

	if (show_cursor && (old_col != cr_col || old_row != cr_row))
	{
		QPainter p(this);
		SetPainter(p);

		// Draw new cursor
		PrintChar(p, cr_row, cr_col);

		// Erase old cursor
		PrintChar(p, old_row, old_col);
	}
}


void EmulLCD::ShowCursor(bool show)
{
	show_cursor = show;

	// Draw (or erase) cursor
	QPainter p(this);
	SetPainter(p);
	PrintChar(p, cr_row, cr_col);
}


void EmulLCD::AdvanceCursor()
{
	// Move the cursor
	if (cr_col == COLS - 1)
	{
		if (cr_row == ROWS - 1)
			MoveCursor(0, 0);
		else
			MoveCursor(cr_row + 1, 0);
	}
	else
		MoveCursor(cr_row, cr_col + 1);
}


void EmulLCD::PutChar(unsigned char c)
{
	if (cgramaddr != -1)
	{
		// Write data in CGRAM
		cgram[cgramaddr] = c;

		// Auto increment CGRAM address
		cgramaddr = (cgramaddr + 1) & 0x3F;
	}
	else
	{
		// Writing in DDRAM
		ddram[cr_row][cr_col] = c;

		// Update display
		{
			QPainter p(this);
			SetPainter(p);
			PrintChar(p, cr_row, cr_col);
		}
		AdvanceCursor();
	}
}


char EmulLCD::GetChar()
{
	char c = ddram[cr_row][cr_col];
	AdvanceCursor();
	return c;
}


void EmulLCD::Clear()
{
	memset(ddram, ' ', sizeof(ddram));
	cr_row = cr_col = 0;

	QPainter p(this);
	RedrawText(p);
}


void EmulLCD::SetCGRamAddr(unsigned char addr)
{
	cgramaddr = addr & (sizeof(cgram) - 1);
}


// Hitachi LM044L register-level emulation

#define INI_DISPLAY		0x30
#define INI_OP_DISP		0x38	/* 8 bits, 2 lines, 5x7 dots */
#define ON_DISPLAY		0x0F	/* Switch on display */
#define OFF_DISPLAY		0x08	/* Switch off display */
#define CLR_DISPLAY		0x01	/* Clear display */
#define CURSOR_BLOCK	0x0D	/* Show cursor (block) */
#define CURSOR_LINE		0x0F	/* Show cursor (line) */
#define CURSOR_OFF		0x0C	/* Hide cursor */
#define MODE_DISPL		0x06
#define SHIFT_DISPLAY	0x18
#define MOVESHIFT_LEFT	0x00
#define MOVESHIFT_RIGHT	0x04
#define LCD_CGRAMADDR	(1<<6)
#define LCD_DDRAMADDR	(1<<7)


extern "C" void Emul_LCDWriteReg(unsigned char d)
{
	static const unsigned char lcd_rowaddress[EmulLCD::ROWS] = { 0x80, 0xC0, 0x94, 0xD4 };

	switch(d)
	{
		case CLR_DISPLAY:
			emul->emulLCD->Clear();
			break;

		case CURSOR_BLOCK:
		case CURSOR_LINE:
			emul->emulLCD->ShowCursor(true);
			break;

		case CURSOR_OFF:
			emul->emulLCD->ShowCursor(false);
			break;

		default:
			// Set DDRAM address?
			if (d & LCD_DDRAMADDR)
			{
				for (int i = 0; i < EmulLCD::ROWS; i++)
				{
					if ((d >= lcd_rowaddress[i]) && (d < lcd_rowaddress[i] + EmulLCD::COLS))
					{
						emul->emulLCD->MoveCursor(i, d - lcd_rowaddress[i]);
						break;
					}
				}
			}
			else if (d & LCD_CGRAMADDR)
				emul->emulLCD->SetCGRamAddr(d);
			break;
	}
}


extern "C" unsigned char Emul_LCDReadReg(void)
{
	return 0;	/* This LCD model is never busy ;-) */
}


extern "C" void Emul_LCDWriteData(unsigned char d)
{
	emul->emulLCD->PutChar(d);
}


extern "C" unsigned char Emul_LCDReadData(void)
{
	return emul->emulLCD->GetChar();
}

#include "EmulLCD.moc"

