/**
 * \file
 * Copyright 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 *
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Graphics level bar widget
 */

#ifndef GUI_LEVELBAR_H
#define GUI_LEVELBAR_H

#include <gfx/gfx.h>


/** Type of levelbar */
#define LBAR_HORIZONTAL 1
#define	LBAR_VERTICAL   2

typedef struct LevelBar
{
	struct Bitmap *bitmap;
	int type;
	int pos;                 ///< Current level
	int min;                 ///< Minimum level
	int max;                 ///< Maximum level
	coord_t x1, y1, x2, y2;  ///< Position of widget in the bitmap
} LevelBar;

void lbar_init(struct LevelBar *lb, struct Bitmap *bmp, int type, int min, int max, int pos,
		coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void lbar_setLevel(struct LevelBar *lb, int level);
int  lbar_getLevel(struct LevelBar *lb);
void lbar_changeLevel(struct LevelBar *lb, int delta);
void lbar_setMax(struct LevelBar *lb, int max);
void lbar_draw(struct LevelBar *lb);

#endif /* GUI_LEVELBAR_H */
