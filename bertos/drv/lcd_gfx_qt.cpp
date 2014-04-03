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
 *
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Custom control for graphics LCD emulation (implementation)
 */

#include "lcd_gfx_qt.h"
#include <emul/emul.h>
#include <cfg/debug.h>
#include <gfx/gfx.h> // CONFIG_BITMAP_FMT

#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QSizePolicy>
#include <QtCore/QSize>

// Display colors
#define LCD_FG_COLOR 0x0, 0x0, 0x0
#define LCD_BG_COLOR 0xBB, 0xCC, 0xBB


EmulLCD::EmulLCD(QWidget *parent) :
	QFrame(parent),
	fg_color(LCD_FG_COLOR),
	bg_brush(QColor(LCD_BG_COLOR))
{
	// Optimized rendering: we repaint everything anyway
	setAttribute(Qt::WA_NoSystemBackground);

	// initialize bitmap
	memset(raster, 0xAA, sizeof(raster));

	// set widget frame
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	frame_width = frameWidth();

	setMinimumSize(WIDTH + frame_width * 2, HEIGHT + frame_width * 2);

	#if CONFIG_EMULLCD_SCALE
		QSizePolicy pol = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred, QSizePolicy::Frame);
		pol.setHeightForWidth(true);
	#else
		QSizePolicy pol = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::Frame);
	#endif
	setSizePolicy(pol);
}


EmulLCD::~EmulLCD()
{
	// nop
}

#if CONFIG_EMULLCD_SCALE
int EmulLCD::heightForWidth(int w) const
{
		int h;

		w -= frame_width * 2;
		h = (w * HEIGHT + WIDTH/2) / WIDTH;
		h += frame_width * 2;

		return h;
}
#endif // CONFIG_EMULLCD_SCALE

void EmulLCD::paintEvent(QPaintEvent * /*event*/)
{
	QPainter p(this);
	QImage img(raster, WIDTH, HEIGHT, QImage::Format_Mono);

	#if CONFIG_EMULLCD_SCALE
		int w = width() - frame_width * 2;
		int h = height() - frame_width * 2;
		if ((w != WIDTH) || (h != HEIGHT))
		{
			p.scale((qreal)w / WIDTH, (qreal)h / HEIGHT);
			//p.setRenderHint(QPainter::SmoothPixmapTransform);
		}
	#endif // CONFIG_EMULLCD_SCALE

	p.setBackgroundMode(Qt::OpaqueMode);
	p.setBackground(bg_brush);
	p.setPen(fg_color);

	p.drawImage(QPoint(frame_width, frame_width), img);
}

void EmulLCD::writeRaster(uint8_t *new_raster)
{
#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB

	// Straight copy
	//memcpy(raster, new_raster, sizeof(raster));

	// Inverting copy
	for (int i = 0; i < (int)sizeof(raster); ++i)
		raster[i] = ~new_raster[i];

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB

	// Rotation + inversion
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int xbyte = 0; xbyte < WIDTH/8; ++xbyte)
		{
			uint8_t v = 0;
			for (int xbit = 0; xbit < 8; ++xbit)
				v |= (new_raster[(xbyte * 8 + xbit) + (y / 8) * WIDTH] & (1 << (y%8)) )
					? (1 << (7 - xbit)) : 0;

			raster[y * ((WIDTH + 7) / 8) + xbyte] = v;
		}
	}
#else
	#error Unsupported bitmap format
#endif

	repaint();
}



#include <gfx/gfx.h>
#include <cfg/debug.h>

DECLARE_WALL(wall_before_raster, WALL_SIZE)
/**
 * Raster buffer to draw into.
 *
 * Bits in the bitmap bytes have vertical orientation,
 * as required by the LCD driver.
 */
static uint8_t lcd_raster[RAST_SIZE(EmulLCD::WIDTH, EmulLCD::HEIGHT)];
DECLARE_WALL(wall_after_raster, WALL_SIZE)




/*extern "C"*/ void lcd_gfx_qt_init(Bitmap *lcd_bitmap)
{
	//FIXME INIT_WALL(wall_before_raster);
	//FIXME INIT_WALL(wall_after_raster);
	gfx_bitmapInit(lcd_bitmap, lcd_raster, EmulLCD::WIDTH, EmulLCD::HEIGHT);
	gfx_bitmapClear(lcd_bitmap);
}

/*extern "C"*/ void lcd_gfx_qt_blitBitmap(const	Bitmap *bm)
{
	//FIXME CHECK_WALL(wall_before_raster);
	//FIXME CHECK_WALL(wall_after_raster);
	emul->emulLCD->writeRaster(bm->raster);
}

#include "lcd_gfx_qt_moc.cpp"

