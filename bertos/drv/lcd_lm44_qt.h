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

#ifndef EMULLCD_H
#define EMULLCD_H

#include <qframe.h>
#include <qfont.h>
#include <qcolor.h>

// fwd decls
class QSizePolicy;
class QPaintEvent;
class QResizeEvent;

/**
 * Qt widget to emulate a dot-matrix LCD display.
 */
class EmulLCD : public QFrame
{
	Q_OBJECT

public:
// Attributes
	enum { COLS = 20, ROWS = 4 };

// Construction
	EmulLCD(QWidget *parent = 0, const char *name = 0);
	virtual ~EmulLCD();

// Base class overrides
protected:
	virtual QSizePolicy sizePolicy() const;
	virtual QSize sizeHint() const;
	virtual void drawContents(QPainter *p);

// Operations
public:
	void MoveCursor		(int col, int row);
	void ShowCursor		(bool show = true);
	void PutChar		(unsigned char c);
	char GetChar		();
	void Clear			();
	void SetCGRamAddr	(unsigned char addr);

// Implementation
protected:
	void SetPainter(QPainter & p);
	void RedrawText(QPainter & p);
	void PrintChar(QPainter & p, int row, int col);
	void AdvanceCursor();

	QFont lcd_font;					///< Display character font
	QColor fg_color, bg_color;		///< LCD colors
	int font_width, font_height;	///< Font dimensions
	int frame_width;				///< Frame width (and height)
	int	cr_row, cr_col;				///< Cursor position
	int cgramaddr;					///< CGRAM Address (-1 disabled)
	unsigned char ddram[ROWS][COLS];///< Display data RAM
	unsigned char cgram[8*8];		///< CGRAM
	bool show_cursor;				///< Cursor enabled?
};

#endif // !defined(EMULLCD_H)

