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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \defgroup chart Chart graphical routines
 * \ingroup graphics
 * \{
 * \brief Simple charts on top of mware/gfx routines (interface).
 *
 * Configuration:
 *  - \c CONFIG_CHART_TYPE_X: type for the input dataset of X-coordinates
 *  - \c CONFIG_CHART_TYPE_Y: type for the input dataset of Y-coordinates
 *
 * Sample usage:
 *
 * \code
 *	bm = chart_init(0, ymax, N_POINTS_CURVE, ymin);
 *
 *	chart_drawCurve(bm, curve_y, curve_points + 1);
 *	gfx_setViewRect(bm, xmin, ymax, xmax, ymin);
 *	chart_drawDots(bm, samples_x, samples_y, samples_cnt);
 *
 *	print_bitmap(bm);
 * \endcode
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef GFX_CHARTS_H
#define GFX_CHARTS_H

#include "cfg/cfg_gfx.h" /* CONFIG_ stuff */

#include <gfx/gfx.h>   /* vcoord_t */

/**
 * \name Width/height of the small ticks drawn over the axes
 * \{
 */
#define TICKS_HEIGHT     2
#define TICKS_WIDTH      2
/*\}*/

/**
 * \name Chart frame dimensions
 * \{
 */
#define CHART_BORDERTOP       0
#define CHART_BORDERBOTTOM    0
#define CHART_BORDERLEFT      0
#define CHART_BORDERRIGHT     0
/*\}*/

#ifndef CONFIG_CHART_TYPE_X
#define CONFIG_CHART_TYPE_X vcoord_t
#endif
#ifndef CONFIG_CHART_TYPE_Y
#define CONFIG_CHART_TYPE_Y vcoord_t
#endif


typedef CONFIG_CHART_TYPE_X chart_x_t;
typedef CONFIG_CHART_TYPE_Y chart_y_t;


/* Public function protos */
void chart_init(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);
void chart_setScale(Bitmap *bm, chart_x_t xmin, chart_y_t ymin, chart_x_t xmax, chart_y_t ymax);
void chart_drawAxis(Bitmap *bm);
void chart_drawCurve(Bitmap *bm, const chart_y_t *curve_y, int curve_cnt);
void chart_drawDots(Bitmap *bm, const chart_x_t *dots_x, const chart_y_t *dots_y, int cnt);

/** \} */ //defgroup charts
#endif /* GFX_CHARTS_H */
