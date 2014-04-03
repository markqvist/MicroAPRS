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
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Graphics private header.
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2006/05/27 17:17:34  bernie
 *#* Optimize away divisions in RAST_ADDR/MASK macros.
 *#*
 *#* Revision 1.5  2006/05/25 23:35:40  bernie
 *#* Cleanup.
 *#*
 *#* Revision 1.4  2006/03/22 09:50:37  bernie
 *#* Use the same format for fonts and rasters.
 *#*
 *#* Revision 1.3  2006/02/15 09:10:15  bernie
 *#* Implement prop fonts; Fix algo styles.
 *#*
 *#* Revision 1.2  2006/02/10 12:28:33  bernie
 *#* Add font support in bitmaps; Make bitmap formats public.
 *#*
 *#* Revision 1.1  2006/01/26 00:32:49  bernie
 *#* Graphics private header.
 *#*
 *#*/

#ifndef GFX_GFX_P_H
#define GFX_GFX_P_H

#include <gfx/gfx.h>

#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB

	/* We use ucoord_t to let the compiler optimize away the division/modulo. */
	#define RAST_ADDR(raster, x, y, stride) \
			((raster) + (ucoord_t)(y) * (ucoord_t)(stride) + (ucoord_t)(x) / 8)
	#define RAST_MASK(raster, x, y) \
			(1 << (7 - (ucoord_t)(x) % 8))

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB

	/* We use ucoord_t to let the compiler optimize away the division/modulo. */
	#define RAST_ADDR(raster, x, y, stride) \
			((raster) + ((ucoord_t)(y) / 8) * (ucoord_t)(stride) + (ucoord_t)(x))
	#define RAST_MASK(raster, x, y) \
			(1 << ((ucoord_t)(y) % 8))

#else
	#error Unknown value of CONFIG_BITMAP_FMT
#endif /* CONFIG_BITMAP_FMT */

#define BM_ADDR(bm, x, y)  RAST_ADDR((bm)->raster, (x), (y), (bm)->stride)
#define BM_MASK(bm, x, y)  RAST_MASK((bm)->raster, (x), (y))

/**
 * Plot a pixel in bitmap \a bm.
 *
 * \note bm is evaluated twice.
 * \see BM_CLEAR BM_DRAWPIXEL
 */
#define BM_PLOT(bm, x, y) \
	( *BM_ADDR(bm, x, y) |= BM_MASK(bm, x, y) )

/**
 * Clear a pixel in bitmap \a bm.
 *
 * \note bm is evaluated twice.
 * \see BM_PLOT BM_DRAWPIXEL
 */
#define BM_CLEAR(bm, x, y) \
	( *BM_ADDR(bm, x, y) &= ~BM_MASK(bm, x, y) )

/**
 * Set a pixel in bitmap \a bm to the specified color.
 *
 * \note bm is evaluated twice.
 * \note This macro is somewhat slower than BM_PLOT and BM_CLEAR.
 * \see BM_PLOT BM_CLEAR
 */
#define BM_DRAWPIXEL(bm, x, y, fg_pen) \
	do { \
		uint8_t *p = BM_ADDR(bm, x, y); \
		uint8_t mask = BM_MASK(bm, x, y); \
		*p = (*p & ~mask) | ((fg_pen) ? mask : 0); \
	} while (0)

/**
 * Get the value of the pixel in bitmap \a bm.
 *
 * \return The returned value is either 0 or 1.
 *
 * \note bm is evaluated twice.
 * \see BM_DRAWPIXEL
 */
#define BM_READPIXEL(bm, x, y) \
	( *BM_ADDR(bm, x, y) & BM_MASK(bm, x, y) ? 1 : 0 )

#define RAST_READPIXEL(raster, x, y, stride) \
		( *RAST_ADDR(raster, x, y, stride) & RAST_MASK(raster, x, y) ? 1 : 0 )

#endif /* GFX_GFX_P_H */
