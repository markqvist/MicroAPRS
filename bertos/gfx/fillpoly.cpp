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
 * \author Massimiliano Corsini <chad@develer.com>
 *
 *
 * \brief Low-level drawing routines.
 *
 * This file contains the implementation of the low-level drawing routines
 * to draw fill rectangle, fill triangle and so on.
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/07/19 13:00:01  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.10  2005/10/15 15:03:43  rasky
 *#* Remove per-pixel clipping from line().
 *#* Use clipLine() also for a-scope.
 *#*
 *#* Revision 1.9  2005/10/14 15:21:32  eldes
 *#* Implement the cohen-sutherland clipping on the buffer
 *#*
 *#* Revision 1.8  2005/09/27 13:28:10  rasky
 *#* Add clipping capabilities to line()
 *#* Fix off-by-one computation of rectangles of drawing.
 *#*
 *#* Revision 1.7  2005/09/27 10:41:35  rasky
 *#* Import line-drawing routine from Devlib
 *#*
 *#* Revision 1.6  2005/09/19 16:36:05  chad
 *#* Fix doxygen autobrief
 *#*
 *#* Revision 1.5  2005/07/06 12:51:47  chad
 *#* Make the fillRectangle() independent of the order of the points of the rectangle
 *#*
 *#* Revision 1.4  2005/06/17 15:06:36  chad
 *#* Remove conversion warning
 *#*
 *#* Revision 1.3  2005/06/17 15:04:47  chad
 *#* Add line clipping capability
 *#*
 *#* Revision 1.2  2005/06/15 14:04:43  chad
 *#* Add line routine
 *#*
 *#* Revision 1.1  2005/06/15 13:34:34  chad
 *#* Low-level drawing routines
 *#*
 *#*/

// Qt-specific headers
#include <qpoint.h>


/**
 * Low-level routine to draw a line.
 *
 * This routine is based on the Bresenham Line-Drawing Algorithm.
 *
 * The \a stride represents the width of the image buffer.
 * (\a x1, \a y1) are the coordinates of the starting point.
 * (\a x2, \a y2) are the coordinates of the ending point.
 *
 * The line has no anti-alias, and clipping is not performed. The line
 * must be fully contained in the buffer (use clipLine() if you need
 * to clip it).
 */
void line(unsigned char *buf,
		  unsigned long bufw, unsigned long bufh, unsigned long stride,
		  int x1, int y1, int x2, int y2, unsigned char color)
{
	int x, y, e, len, adx, ady, signx, signy;

	if (x2 > x1)
	{
		/* left to right */
		signx = +1;
		adx = x2 - x1;
	}
	else
	{
		/* right to left */
		signx = -1;
		adx = x1 - x2;
	}

	if (y2 > y1)
	{
		/* top to bottom */
		signy = +1;
		ady = y2 - y1;
	}
	else
	{
		/* bottom to top */
		signy = -1;
		ady = y1 - y2;
	}

	x = x1;
	y = y1;

	if (adx > ady)
	{
		/* X-major line (octants 1/4/5/8) */
		len = adx;
		e = -adx;
		while (len--)
		{
			/* Sanity check */
			assert(y >= 0 && y < static_cast<int>(bufh) &&
				   x >= 0 && x < static_cast<int>(bufw));
			buf[y * stride + x] = color;
			x += signx;
			e += ady;
			if (e >= 0)
			{
				y += signy;
				e -= adx;
			}
		}
	}
	else
	{
		/* Y-major line (octants 2/3/6/7) */
		len = ady;
		e = -ady;
		while (len--)
		{
			/* Sanity check */
			assert(y >= 0 && y < static_cast<int>(bufh) &&
				   x >= 0 && x < static_cast<int>(bufw));
			buf[y * stride + x] = color;
			y += signy;
			e += adx;
			if (e >= 0)
			{
				x += signx;
				e -= ady;
			}
		}
	}
}

/// Helper routine for clipLine().
static int region(int x, int y, int w, int h)
{
	int code = 0;

	if (y >= h)
		code |= 1; // top
	else if (y < 0)
		code |= 2; // bottom

	if (x >= w)
		code |= 4; // right
	else if (x < 0)
		code |= 8; // left

	return code;
}

/**
 * Low-level routine to draw a line, clipped to the buffer extents.
 *
 * This routine executes the clipping, and then invokes line().
 * Parameters are the same of line(). The clipping is performed
 * using the Cohen-Sutherland algorithm, which is very fast.
 */
void clipLine(unsigned char *buf,
		  unsigned long w, unsigned long h, unsigned long stride,
		  int x1, int y1, int x2, int y2, unsigned char color)
{
	int code1 = region(x1, y1, w, h);
	int code2 = region(x2, y2, w, h);

	// Loop while there is at least one point outside
	while (code1 | code2)
	{
		// Check for line totally outside
		if (code1 & code2)
			return;

		int c = code1 ? code1 : code2;
		int x, y;

		if (c & 1) // top
		{
			x = x1 + (x2 - x1) * (h - y1) / (y2 - y1);
			y = h - 1;
		}
		else if (c & 2) //bottom
		{
			x = x1 + (x2 - x1) * -y1 / (y2 - y1);
			y = 0;
		}
		else if (c & 4) //right
		{
			y = y1 + (y2 - y1) * (w - x1) / (x2 - x1);
			x = w - 1;
		}
		else //left
		{
			y = y1 + (y2 - y1) * -x1 / (x2 - x1);
			x = 0;
		}

		if (c == code1) // first endpoint was clipped
		{
			x1 = x; y1 = y;
			code1 = region(x1, y1, w, h);
		}
		else //second endpoint was clipped
		{
			x2 = x; y2 = y;
			code2 = region(x2, y2, w, h);
		}
	}

	line(buf, w, h, stride, x1, y1, x2, y2, color);
}


/**
 * Low-level routine to draw a filled rectangle.
 *
 * The triangle is filled with the given color.
 *
 * The \a stride represents the width of the image buffer.
 * The points \a p1 and \a p2 are two opposite corners of the
 * rectangle.
 */
void fillRectangle(unsigned char *buf, unsigned long stride,
				   QPoint p1, QPoint p2, unsigned char color)
{
	QPoint ul;       // upper-left corner
	QPoint lr;       // lower-right corner

	if (p2.x() > p1.x())
	{
		ul.setX(p1.x());
		lr.setX(p2.x());
	}
	else
	{
		ul.setX(p2.x());
		lr.setX(p1.x());
	}

	if (p2.y() > p1.y())
	{
		ul.setY(p1.y());
		lr.setY(p2.y());
	}
	else
	{
		ul.setY(p2.y());
		lr.setY(p1.y());
	}

	int width = lr.x() - ul.x();
	unsigned long offset = ul.x() + ul.y()*stride;

	for (int h = ul.y(); h < lr.y(); h++)
	{
		memset(buf+offset, color, width);
		offset += stride;
	}
}

/**
 * Low-level routines to draw a filled triangle.
 *
 * The triangle is filled with the given \a color.
 * The \a stride represents the width of the image buffer (\a buf).
 *
 * The routine use fixed-point arithmetic.
 */
void fillTriangle(unsigned char* buf, unsigned long stride,
				  QPoint v1, QPoint v2, QPoint v3, unsigned char color)
{
	int altezza[3];

	// Sort by vertical coordinate
	if (v1.y() > v2.y())
		std::swap(v1, v2);
	if (v1.y() > v3.y())
		std::swap(v1, v3);
	if (v2.y() > v3.y())
		std::swap(v2, v3);

	altezza[0] = v3.y() - v1.y();
	if (!altezza[0])
		return;

	int sezioni = 2;
	int sezione = 1;

	buf += v1.y() * stride;

	altezza[1] = v2.y() - v1.y();
	altezza[2] = v3.y() - v2.y();

	int sinistra = v1.x();
	int destra = sinistra;

	if (v1.y() == v2.y())
	{
		if (v1.x() < v2.x())
			destra = v2.x();
		else
			sinistra = v2.x();
	}

	sinistra <<= 16;
	destra <<= 16;

	int stmp1, stmp2, stmp3;

	stmp1 = (altezza[1] << 16) / altezza[0];
	int lunghezza = stmp1 * (v3.x() - v1.x()) + ((v1.x() - v2.x()) << 16);

	if (!lunghezza )
		return;

	int delta_sinistra[2];
	int delta_destra[2];

	stmp1 = ((v3.x() - v1.x()) << 16) / altezza[0];

	if (altezza[1])
		stmp2 = ((v2.x() - v1.x()) << 16) / altezza[1];
	if (altezza[2])
		stmp3 = ((v3.x() - v2.x()) << 16) / altezza[2];

	if (lunghezza < 0) // Il secondo vertice ~J a destra
	{
		delta_sinistra[0] = stmp1;
		delta_sinistra[1] = stmp1;
		delta_destra[0] = stmp2;
		delta_destra[1] = stmp3;
	}
	else // Il secondo vertice ~J a sinistra
	{
		delta_sinistra[0] = stmp2;
		delta_sinistra[1] = stmp3;
		delta_destra[0] = stmp1;
		delta_destra[1] = stmp1;
	}

	int len2 = lunghezza;

	do
	{
		while (altezza [sezione])
		{
			unsigned char* curpos = buf + ((sinistra )>> 16);
			lunghezza = ((destra ) >> 16) - ((sinistra ) >> 16);
			assert(lunghezza >= 0);
			if (lunghezza)
				memset(curpos, color, lunghezza);
			buf += stride;
			destra += delta_destra[sezione - 1];
			sinistra += delta_sinistra[sezione - 1];
			altezza[sezione]--;
		}
		if (len2 < 0)
			destra = v2.x() << 16;
		else
			sinistra = v2.x() << 16;
		sezione++;
	} while (--sezioni);
}
