/**
 * \file
 * Copyright 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 *
 *
 * \brief Graphics user interface element to display a level bar.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "levelbar.h"


/**
 * Initialize the LevelBar widget with the bitmap associated,
 * the value range and the coordinates in the bitmap.
 * \note The levelbar should be at least 5 pixels wide and high
 *       for correct borders drawing. No check is done on this.
 */
void lbar_init(struct LevelBar *lb, struct Bitmap *bmp, int type, int min, int max, int pos,
		coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	lb->bitmap = bmp;
	lb->type = type;
	lb->min = min;
	lb->max = max;
	lb->pos = pos;
	lb->x1 = x1;
	lb->y1 = y1;
	lb->x2 = x2;
	lb->y2 = y2;
}


/**
 * Set the level.
 */
void lbar_setLevel(struct LevelBar *lb, int level)
{
	if (level < lb->min)
		level = lb->min;
	if (level > lb->max)
		level = lb->max;

	lb->pos = level;
}


/**
 * Get current level.
 */
int lbar_getLevel(struct LevelBar *lb)
{
	return lb->pos;
}


/**
 * Change level with respect to previous value
 * (delta can be negative).
 */
void lbar_changeLevel(struct LevelBar *lb, int delta)
{
	lbar_setLevel(lb, lb->pos + delta);
}


/**
 * Change the top limit.
 */
void lbar_setMax(struct LevelBar *lb, int max)
{
	lb->max = max;
}


/**
 * Render the LevelBar on the bitmap.
 */
void lbar_draw(struct LevelBar *lb)
{
#define BORDERW 1
#define BORDERH 1

	/* Compute filled bar length in pixels */
	int totlen = (lb->type & LBAR_HORIZONTAL) ? lb->x2 - lb->x1 - BORDERW*4 : lb->y2 - lb->y1 - BORDERH*4;
	int range  = lb->max - lb->min;
	int barlen = ((long)(lb->pos - lb->min) * (long)totlen + range - 1) / range;

	// Draw border
	gfx_rectDraw(lb->bitmap, lb->x1, lb->y1, lb->x2, lb->y2);

	// Clear inside
	gfx_rectClear(lb->bitmap, lb->x1 + BORDERW, lb->y1 + BORDERH, lb->x2 - BORDERW, lb->y2 - BORDERH);

	// Draw bar
	if (lb->type & LBAR_HORIZONTAL)
		gfx_rectFill(lb->bitmap,
				lb->x1 + BORDERW*2, lb->y1 + BORDERH*2,
				lb->x1 + BORDERW*2 + barlen, lb->y2 - BORDERH*2);
	else
		gfx_rectFill(lb->bitmap,
				lb->x1 + BORDERW*2, lb->y2 - BORDERH*2 - barlen,
				lb->x2 - BORDERW*2, lb->y2 - BORDERH*2);
}
