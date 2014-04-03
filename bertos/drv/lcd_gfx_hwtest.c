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
 *
 * -->
 *
 * \brief dot-matrix LCD test.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 */


#warning FIXME: broken test!

#if 0

#include <drv/lcd_gfx.h>
#include <gfx/gfx.h>

#include <emul/emul.h>

static void magic(struct Bitmap *bitmap, coord_t x, coord_t y)
{
	static const coord_t coords[] = { 120, 34, 90, 90, 30, 90, 0, 34, 60, 0, 90, 90, 0, 34, 120, 34, 30, 90, 60, 0 };
	unsigned int i;

	gfx_moveTo(bitmap, coords[countof(coords)-2]/2 + x, coords[countof(coords)-1]/3 + y);
	for (i = 0; i < countof(coords); i += 2)
		gfx_lineTo(bitmap, coords[i]/2 + x, coords[i+1]/3 + y);
}

int main(int argc, char *argv[])
{
	emul_init(&argc, argv);
	lcd_init();

	coord_t x = 0, y = LCD_WIDTH / 2;
	coord_t xdir = +1, ydir = -1;
	Bitmap *bm = &lcd_bitmap;

	for(;;)
	{
		gfx_bitmapClear(bm);
		gfx_setClipRect(bm, 0, 0, bm->width, bm->height);
		gfx_rectDraw(bm, 10, 10, bm->width-10, bm->height-10);
		gfx_setClipRect(bm, 11, 11, bm->width-11, bm->height-11);
		magic(bm, x, y);

		x += xdir;
		y += ydir;
		if (x >= bm->width)  xdir = -1;
		if (x <= -50)        xdir = +1;
		if (y >= bm->height) ydir = -1;
		if (y <= -50)        ydir = +1;

		lcd_blit_bitmap(bm);
		emul_idle();
		usleep(10000);
	}

	emul_cleanup();
	return 0;
}

#endif
