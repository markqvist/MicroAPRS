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
 * Copyright 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Generic editor for (volume/gain/contrast/...) setting.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "leveledit.h"

#include "cfg/cfg_menu.h"
#include <cfg/macros.h> /* MAX() */

#include <drv/kbd.h>
#include <drv/timer.h>

#include <gui/levelbar.h>

#include <cpu/pgm.h>

#include <gfx/text.h>
#include <gfx/font.h>

#if CONFIG_MENU_MENUBAR
#include <gui/menubar.h>
#endif

#warning FIXME: Revise me!

#define LBAR_HEIGHT 16

/**
 * Allow user to change level.
 */
void level_edit(struct LevelEdit *lev)
{
#if CONFIG_MENU_MENUBAR
	/* Labels for menubars */
	enum LabelId ch_labels[] = { LABEL_C1PLUS2, LABEL_CH_1, LABEL_CH_2 };
	const_iptr_t labels[] =
	{
		(const_iptr_t)LABEL_BACK,
		(const_iptr_t)LABEL_MINUS,
		(const_iptr_t)LABEL_PLUS,
		(const_iptr_t)LABEL_EMPTY
	};
	struct MenuBar mb;
#endif /* CONFIG_MENU_MENUBAR */

	struct LevelBar bar1, bar2;
	keymask_t keys, old_rpt_mask;
	int step, rep_step;

	rep_step = MAX(lev->step, ((lev->max - lev->min) / 200));
	step = lev->step;

	// Allow keys repetition.
	old_rpt_mask = kbd_setRepeatMask(K_UP | K_DOWN);

	text_clear(lev->bitmap);
	//text_style(STYLEF_UNDERLINE, STYLEF_UNDERLINE);
	text_puts(lev->title, lev->bitmap);
	//text_style(0, STYLEF_UNDERLINE);

	if (lev->type & LEVELEDIT_DOUBLE)
	{
		int chn = 0;  /* edit both channels */

		/* Levelbars init */
		lbar_init(&bar1, lev->bitmap, LBAR_HORIZONTAL,
				lev->min, lev->max, *lev->ch1_val, 0, 16, lev->bitmap->width / 2 - 1, 23);
		lbar_init(&bar2, lev->bitmap, LBAR_HORIZONTAL,
				lev->min, lev->max, *lev->ch2_val, lev->bitmap->width / 2 + 1, 16, lev->bitmap->width, 23);

		#if CONFIG_MENU_MENUBAR
			labels[3] = (const_iptr_t)ch_labels[chn];
			mbar_init(&mb, lev->bitmap, labels, countof(labels));
			mbar_draw(&mb);
		#endif /* CONFIG_MENU_MENUBAR */

		/* Input loop for double level setting */
		for (;;)
		{
			#if CONFIG_LEVELEDIT_TIMEOUT != 0
				ticks_t idle_timeout = timer_clock();
			#endif
			do
			{
				if (lev->display_hook)
					lev->display_hook(lev);
				else
				{
					text_xprintf(lev->bitmap, 1, 0, TEXT_CENTER | TEXT_FILL, lev->unit);
					PGM_FUNC(text_xprintf)(lev->bitmap, 1, 3, 0, PGM_STR("%d"), *lev->ch1_val);
					PGM_FUNC(text_xprintf)(lev->bitmap, 1, 14, 0, PGM_STR("%d"), *lev->ch2_val);

					lbar_setLevel(&bar1, *lev->ch1_val);
					lbar_setLevel(&bar2, *lev->ch2_val);
					lbar_draw(&bar1);
					lbar_draw(&bar2);
				}

				#if CONFIG_LEVELEDIT_TIMEOUT != 0
					if (timer_clock() - idle_timeout > ms_to_ticks(CONFIG_LEVELEDIT_TIMEOUT))
					{
						/* Accept input implicitly */
						keys = K_OK;
						break;
					}
				#endif
			}
			while (!(keys = kbd_peek()));

			if (keys & K_CANCEL)
				break;

			if (keys & K_OK)
			{
				chn = (chn + 1) % 3;

				#if CONFIG_MENU_MENUBAR
					labels[3] = (const_iptr_t)ch_labels[chn];
					mbar_draw(&mb);
				#endif /* CONFIG_MENU_MENUBAR */
			}

			/* Increment step to achieve greater accelerations on larger values */
			if (keys & K_REPEAT)
				step = MIN(rep_step, step + 1);
			else
				step = lev->step;

			if (keys & (K_UP | K_DOWN))
			{
				if (keys & K_UP)
				{
					/* If changing both channels (chn == 0), don't change
					 * level if one of two is at min or max */
					if (chn != 0 ||
							(*lev->ch1_val + step <= lev->max
							 && *lev->ch2_val + step <= lev->max))
					{
						/* If chn == 0 change both channels */
						if (chn != 2)
						{
							*lev->ch1_val += step;
							if (*lev->ch1_val > lev->max)
								*lev->ch1_val = lev->max;
						}
						if (chn != 1)
						{
							*lev->ch2_val += step;
							if (*lev->ch2_val > lev->max)
								*lev->ch2_val = lev->max;
						}
					}
				}
				else
				{
					if (chn != 0 ||
							(*lev->ch1_val - step >= lev->min
							 && *lev->ch2_val - step >= lev->min))
					{
						if (chn != 2)
						{
							*lev->ch1_val -= step;
							if (*lev->ch1_val < lev->min)
								*lev->ch1_val = lev->min;
						}
						if (chn != 1)
						{
							*lev->ch2_val -= step;
							if (*lev->ch2_val < lev->min)
								*lev->ch2_val = lev->min;
						}
					}
				}

				if (lev->set_hook)
					lev->set_hook();
			}
		} // end for(;;)
	}
	else
	{
		const PGM_ATTR char *fmt = lev->unit ? PGM_STR("%d %s") : PGM_STR("%d");

/*
		const int textw = MAX(PGM_FUNC(text_widthf)(lev->bitmap, fmt, lev->max, lev->unit),
		                PGM_FUNC(text_widthf)(lev->bitmap, fmt, lev->min, lev->unit));

		const coord_t barlen = lev->bitmap->width - 6 - textw;
*/
		const coord_t barlen = lev->bitmap->width;
		const coord_t barvtop = lev->bitmap->height / 2 - LBAR_HEIGHT/2 + lev->bitmap->font->height;
		lbar_init(&bar1, lev->bitmap, LBAR_HORIZONTAL,
			  lev->min, lev->max, *lev->ch1_val,
			  0, barvtop, barlen,  barvtop + LBAR_HEIGHT);

		#if CONFIG_MENU_MENUBAR
			mbar_init(&mb, lev->bitmap, labels, countof(labels));
			mbar_draw(&mb);
		#endif /* CONFIG_MENU_MENUBAR */

		/* Input loop for single level setting */
		for (;;)
		{
			#if CONFIG_LEVELEDIT_TIMEOUT != 0
				ticks_t idle_timeout = timer_clock();
			#endif
			do
			{
				if (lev->display_hook)
					lev->display_hook(lev);
				else
				{
					if (lev->type != LEVELEDIT_NOBAR)
					{
						lbar_setLevel(&bar1, *lev->ch1_val);
						lbar_draw(&bar1);
					}
					PGM_FUNC(text_xyprintf)(lev->bitmap, 0, bar1.y1 - lev->bitmap->font->height,
						TEXT_CENTER | TEXT_FILL, fmt, *lev->ch1_val, lev->unit);
				}

				#if CONFIG_LEVELEDIT_TIMEOUT != 0
					if (timer_clock() - idle_timeout > CONFIG_LEVELEDIT_TIMEOUT)
					{
						/* Accept input implicitly */
						keys = K_CANCEL;
						break;
					}
				#endif

			}
			while (!(keys = kbd_peek()));

			if (keys & K_CANCEL)
				break;

			/* Increment step to achieve greater accelerations on larger values */
			if (keys & K_REPEAT)
				step = MIN(rep_step, step + 1);
			else
				step = lev->step;

			if (keys & K_UP)
			{
				*lev->ch1_val += step;
				if (*lev->ch1_val > lev->max)
					*lev->ch1_val = lev->max;
			}

			if (keys & K_DOWN)
			{
				*lev->ch1_val -= step;
				if (*lev->ch1_val < lev->min)
					*lev->ch1_val = lev->min;
			}

			if (lev->set_hook)
				lev->set_hook();
		}
	}

	kbd_setRepeatMask(old_rpt_mask);
}

/**
 * LevelEdit structure initialization.
 * Init data structure and init LevelEdit widgets.
 */
void level_init(struct LevelEdit *lev,
		int type,
		struct Bitmap *bmp, const char *title, const char *unit,
		int min, int max, int step,
		int *ch1_val, int *ch2_val,
		level_set_callback *set_hook, display_callback *display_hook)
{
	lev->type   = type;
	lev->bitmap = bmp;
	lev->title  = title;
	lev->unit   = unit;
	lev->min    = min;
	lev->max    = max;
	lev->step   = step;

	lev->ch1_val = ch1_val;
	lev->ch2_val = ch2_val;
	lev->set_hook     = set_hook;
	lev->display_hook = display_hook;
}
