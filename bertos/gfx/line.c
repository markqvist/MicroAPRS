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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Line drawing graphics routines
 * \sa gfx.h
 */

#include "gfx.h"
#include "gfx_p.h"

#include "cfg/cfg_gfx.h"   /* CONFIG_GFX_CLIPPING */
#include <cfg/debug.h>   /* ASSERT() */
#include <cfg/macros.h>  /* SWAP() */

/* Configuration sanity checks */
#if !defined(CONFIG_GFX_CLIPPING) || (CONFIG_GFX_CLIPPING != 0 && CONFIG_GFX_CLIPPING != 1)
	#error CONFIG_GFX_CLIPPING must be defined to either 0 or 1
#endif
#if !defined(CONFIG_GFX_VCOORDS) || (CONFIG_GFX_VCOORDS != 0 && CONFIG_GFX_VCOORDS != 1)
	#error CONFIG_GFX_VCOORDS must be defined to either 0 or 1
#endif

/**
 * Draw a sloped line without performing clipping.
 *
 * Parameters are the same of gfx_line().
 * This routine is based on the Bresenham Line-Drawing Algorithm.
 *
 * \note Passing coordinates outside the bitmap boundaries will
 *       result in memory trashing.
 *
 * \todo Optimize for vertical and horiziontal lines.
 *
 * \sa gfx_line()
 */
static void gfx_lineUnclipped(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
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
			ASSERT((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height));
			BM_PLOT(bm, x, y);
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
			ASSERT ((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height));
			BM_PLOT(bm, x, y);
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

#if CONFIG_GFX_CLIPPING

/// Helper routine for gfx_line().
static int gfx_findRegion(int x, int y, Rect *cr)
{
	int code = 0;

	if (y >= cr->ymax)
		code |= 1; /* below */
	else if (y < cr->ymin)
		code |= 2; /* above */

	if (x >= cr->xmax)
		code |= 4; /* right */
	else if (x < cr->xmin)
		code |= 8; /* left */

	return code;
}

#endif /* CONFIG_CLIPPING */

/**
 * Draw a sloped line segment.
 *
 * Draw a sloped line segment identified by the provided
 * start and end coordinates on the bitmap \a bm.
 *
 * The line endpoints are clipped inside the current bitmap
 * clipping rectangle using the Cohen-Sutherland algorithm,
 * which is very fast.
 *
 * \note The point at coordinates \a x2 \a y2 is not drawn.
 *
 * \note This function does \b not update the current pen position.
 *
 * \todo Compute updated Bresenham error term.
 */
void gfx_line(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
#if CONFIG_GFX_CLIPPING
	int clip1 = gfx_findRegion(x1, y1, &bm->cr);
	int clip2 = gfx_findRegion(x2, y2, &bm->cr);

	/* Loop while there is at least one point outside */
	while (clip1 | clip2)
	{
		/* Check for line totally outside */
		if (clip1 & clip2)
			return;

		int c = clip1 ? clip1 : clip2;
		int x, y;

		if (c & 1) /* Below */
		{
			x = x1 + (x2 - x1) * (bm->cr.ymax - y1) / (y2 - y1);
			y = bm->cr.ymax - 1;
		}
		else if (c & 2) /* Above */
		{
			x = x1 + (x2 - x1) * (bm->cr.ymin - y1) / (y2 - y1);
			y = bm->cr.ymin;
		}
		else if (c & 4) /* Right */
		{
			y = y1 + (y2 - y1) * (bm->cr.xmax - x1) / (x2 - x1);
			x = bm->cr.xmax - 1;
		}
		else /* Left */
		{
			y = y1 + (y2 - y1) * (bm->cr.xmin - x1) / (x2 - x1);
			x = bm->cr.xmin;
		}

		if (c == clip1) /* First endpoint was clipped */
		{
			// TODO: adjust Bresenham error term
			//coord_t clipdx = ABS(x - x1);
			//coord_t clipdy = ABS(y - y1);
			//e += (clipdy * e2) + ((clipdx - clipdy) * e1);

			x1 = x;
			y1 = y;
			clip1 = gfx_findRegion(x1, y1, &bm->cr);
		}
		else /* Second endpoint was clipped */
		{
			x2 = x;
			y2 = y;
			clip2 = gfx_findRegion(x2, y2, &bm->cr);
		}
	}
#endif /* CONFIG_GFX_CLIPPING */

	gfx_lineUnclipped(bm, x1, y1, x2, y2);
}

/**
 * Move the current pen position to the specified coordinates.
 *
 * The pen position is used for drawing operations such as
 * gfx_lineTo(), which can be used to draw polygons.
 */
void gfx_moveTo(Bitmap *bm, coord_t x, coord_t y)
{
	bm->penX = x;
	bm->penY = y;
}

/**
 * Draw a line from the current pen position to the new coordinates.
 *
 * \note This function moves the current pen position to the
 *       new coordinates.
 *
 * \sa gfx_line()
 */
void gfx_lineTo(Bitmap *bm, coord_t x, coord_t y)
{
	gfx_line(bm, bm->penX, bm->penY, x, y);
	gfx_moveTo(bm, x, y);
}


/**
 * Draw the perimeter of an hollow rectangle.
 *
 * \note The bottom-right corner of the rectangle is drawn at (x2-1;y2-1).
 * \note This function does \b not update the current pen position.
 */
void gfx_rectDraw(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	/* Sort coords (needed for correct bottom-right semantics) */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

	/* Draw rectangle */
	gfx_line(bm, x1,   y1,   x2-1, y1);
	gfx_line(bm, x2-1, y1,   x2-1, y2-1);
	gfx_line(bm, x2-1, y2-1, x1,   y2-1);
	gfx_line(bm, x1,   y2-1, x1,   y1);
}


/**
 * Fill a rectangular area with \a color.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_rectFillC(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color)
{
	coord_t x, y;

	/* Sort coords */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

#if CONFIG_GFX_CLIPPING
	/* Clip rect to bitmap clip region */
	if (x1 < bm->cr.xmin)   x1 = bm->cr.xmin;
	if (x2 < bm->cr.xmin)   x2 = bm->cr.xmin;
	if (x1 > bm->cr.xmax)   x1 = bm->cr.xmax;
	if (x2 > bm->cr.xmax)   x2 = bm->cr.xmax;
	if (y1 < bm->cr.ymin)   y1 = bm->cr.ymin;
	if (y2 < bm->cr.ymin)   y2 = bm->cr.ymin;
	if (y1 > bm->cr.ymax)   y1 = bm->cr.ymax;
	if (y2 > bm->cr.ymax)   y2 = bm->cr.ymax;
#endif /* CONFIG_GFX_CLIPPING */

	/* NOTE: Code paths are duplicated for efficiency */
	if (color) /* fill */
	{
		for (x = x1; x < x2; x++)
			for (y = y1; y < y2; y++)
				BM_PLOT(bm, x, y);
	}
	else /* clear */
	{
		for (x = x1; x < x2; x++)
			for (y = y1; y < y2; y++)
				BM_CLEAR(bm, x, y);
	}
}


/**
 * Draw a filled rectangle.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_rectFill(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0xFF);
}


/**
 * Clear a rectangular area.
 *
 * \note The bottom-right border of the rectangle is not cleared.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_rectClear(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0x00);
}


#if CONFIG_GFX_VCOORDS
/**
 * Imposta gli estremi del sistema di coordinate cartesiane rispetto
 * al rettangolo di clipping della bitmap.
 */
void gfx_setViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
{
	ASSERT(x1 != x2);
	ASSERT(y1 != y2);

	bm->orgX    = x1;
	bm->orgY    = y1;
	bm->scaleX  = (vcoord_t)(bm->cr.xmax - bm->cr.xmin - 1) / (vcoord_t)(x2 - x1);
	bm->scaleY  = (vcoord_t)(bm->cr.ymax - bm->cr.ymin - 1) / (vcoord_t)(y2 - y1);

/*	DB(kprintf("orgX = %f, orgY = %f, scaleX = %f, scaleY = %f\n",
		bm->orgX, bm->orgY, bm->scaleX, bm->scaleY);)
*/
}


/**
 * Transform a coordinate from the current reference system to a
 * pixel offset within the bitmap.
 */
coord_t gfx_transformX(Bitmap *bm, vcoord_t x)
{
	return bm->cr.xmin + (coord_t)((x - bm->orgX) * bm->scaleX);
}

/**
 * Transform a coordinate from the current reference system to a
 * pixel offset within the bitmap.
 */
coord_t gfx_transformY(Bitmap *bm, vcoord_t y)
{
	return bm->cr.ymin + (coord_t)((y - bm->orgY) * bm->scaleY);
}


/**
 * Draw a line from (x1;y1) to (x2;y2).
 */
void gfx_vline(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
{
	gfx_line(bm,
		gfx_transformX(bm, x1), gfx_transformY(bm, y1),
		gfx_transformY(bm, x2), gfx_transformY(bm, y2));
}
#endif /* CONFIG_GFX_VCOORDS */
