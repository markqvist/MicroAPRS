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
 * Copyright 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Custom control for graphics LCD emulation (interface)
 */

#ifndef DRV_LCD_GFX_QT_H
#define DRV_LCD_GFX_QT_H

// uint8_t
#include <gfx/gfx.h>
#include <cfg/compiler.h>

#include <QtGui/QColor>
#include <QtGui/QFrame>

#define LCD_WIDTH	128

// fwd decls
class QSizePolicy;
class QPaintEvent;
class QResizeEvent;

#define CONFIG_EMULLCD_SCALE 1

class EmulLCD : public QFrame
{
	Q_OBJECT

public:
// Attributes
	enum { WIDTH = 128, HEIGHT = 64 };

// Construction
	EmulLCD(QWidget *parent = 0);
	virtual ~EmulLCD();

// Base class overrides
protected:
	virtual void paintEvent(QPaintEvent *event);

	#if CONFIG_EMULLCD_SCALE
		virtual int heightForWidth(int w) const;
	#endif

// Operations
public:
	void writeRaster(uint8_t *raster);

// Implementation
protected:
	/// Frame thickness
	int frame_width;

	/// Brushes for painting the LCD
	QColor fg_color;
	QBrush bg_brush;

	/// Pixel storage
	unsigned char raster[(WIDTH + 7 / 8) * HEIGHT];
};


void lcd_gfx_qt_init(Bitmap *lcd_bitmap);
void lcd_gfx_qt_blitBitmap(const Bitmap *bm);

#endif // DRV_LCD_GFX_QT_H
