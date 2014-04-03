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
 * All Rights Reserved.
 * -->
 *
 * \brief Generic text LCD driver (impl.).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "lcd_text.h"
#include "lcd_hd44.h"

#include <cfg/macros.h> // BV()
#include <cfg/debug.h>

#include <drv/timer.h> // timer_delay()

#include <mware/formatwr.h> // _formatted_write()
#include <struct/list.h> // LIST_EMPTY()

#include <string.h> // strlen()


/** Maximum number of layers. */
#define LCD_LAYERS 6

#if CONFIG_KERN
	#include <kern/sem.h>
	/** Semaphore to arbitrate access to the display. */
	static struct Semaphore lcd_semaphore;
	#define LOCK_LCD	sem_obtain(&lcd_semaphore)
	#define UNLOCK_LCD	sem_release(&lcd_semaphore)
#else /* !CONFIG_KERN */
	#define LOCK_LCD	do {} while (0)
	#define UNLOCK_LCD	do {} while (0)
#endif /* !CONFIG_KERN */

DECLARE_LIST_TYPE(Layer);

Layer *lcd_DefLayer;
static Layer lcd_LayersPool[LCD_LAYERS];
static LIST_TYPE(Layer) lcd_Layers;
static LIST_TYPE(Layer) lcd_FreeLayers;

/**
 * Current cursor status.
 *
 * One of LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLOCK or LCD_CMD_CURSOR_LINE.
 */
static uint8_t lcd_CursorStatus;

/** Current cursor position, encoded as a Cursor position and status. */
static lcdpos_t lcd_CursorAddr;


void lcd_setAddr(Layer *layer, lcdpos_t addr)
{
	/* Sanity check: wrap around to display limits */
	while (addr >= CONFIG_LCD_ROWS * CONFIG_LCD_COLS)
		addr -= CONFIG_LCD_ROWS * CONFIG_LCD_COLS;

	layer->addr = addr;
}

#if CONFIG_KERN

void lcd_lock(void)
{
	LOCK_LCD;
}


void lcd_unlock(void)
{
	UNLOCK_LCD;
}

#endif /* CONFIG_KERN */


/**
 * Write one character to the display at the current
 * cursor prosition, then move the cursor right. The
 * cursor is wrapped to the next line when it moves
 * beyond the end of the current line.
 *
 * \note Does _NOT_ lock the display semaphore.
 */
static void lcd_putCharUnlocked(char c, Layer *layer)
{
	Layer *l2;
	lcdpos_t addr = layer->addr;

	/* Store character in layer buffer */
	layer->buf[addr] = c;

	/* Move to next character */
	if (++layer->addr >= CONFIG_LCD_COLS * CONFIG_LCD_ROWS)
		layer->addr = 0;

	/* Do not write on LCD if layer is hidden. */
	if (layer->pri == LAYER_HIDDEN)
		return;

	/*
	 * Check if this location is obscured by
	 * other layers above us.
	 */
	for (l2 = layer->pred; l2->pred; l2 = l2->pred)
	{
		if (l2->buf[addr])
		{
			/* DB(kprintf("layer %04x obs %04x at %d\n", l2, layer, addr);) */
			return;
		}
	}

	/* Write character */
	if (c)
		lcd_putc(addr, c);
	else
		/* FIXME: should look for layers beneath! */
		lcd_putc(addr, ' ');
}


void lcd_putChar(char c, Layer *layer)
{
	LOCK_LCD;
	lcd_putCharUnlocked(c, layer);
	UNLOCK_LCD;
}

void lcd_layerSet(Layer *layer, char c)
{
	int i;

	LOCK_LCD;
	lcd_setAddr(layer, 0);
	for (i = 0; i < CONFIG_LCD_COLS * CONFIG_LCD_ROWS; i++)
		lcd_putCharUnlocked(c, layer);
	UNLOCK_LCD;
}


void lcd_clear(Layer *layer)
{
	lcd_layerSet(layer, 0);
}


void lcd_clearLine(Layer *layer, int y)
{
	int i;

	LOCK_LCD;
	lcd_setAddr(layer, LCD_POS(0, y));
	for (i = 0; i < CONFIG_LCD_COLS; i++)
		lcd_putCharUnlocked(0, layer);
	UNLOCK_LCD;
}


void lcd_moveCursor(lcdpos_t addr)
{
	LOCK_LCD;
	lcd_moveTo(addr);
	UNLOCK_LCD;
}


char lcd_setCursor(char mode)
{
	static const char cursor_cmd[3] =
	{
		LCD_CMD_CURSOR_OFF, LCD_CMD_CURSOR_BLOCK, LCD_CMD_CURSOR_LINE
	};
	char oldmode = lcd_CursorStatus;

	LOCK_LCD;
	lcd_CursorStatus = mode;
	lcd_setReg(cursor_cmd[(int)mode]);
	if (mode)
		lcd_moveCursor(lcd_CursorAddr);
	UNLOCK_LCD;

	return oldmode;
}


int lcd_vprintf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, va_list ap)
{
	int len;

	LOCK_LCD;

	/*
	 * Se il cursore era acceso, spegnilo durante
	 * l'output per evitare che salti alla posizione
	 * in cui si scrive.
	 */
	if (lcd_CursorStatus)
		lcd_setReg(LCD_CMD_CURSOR_OFF);

	/* Spostamento del cursore */
	lcd_setAddr(layer, addr);

	if (mode & LCD_CENTER)
	{
		int pad;

		/*
		 * NOTE: calculating the string lenght BEFORE it gets
		 * printf()-formatted. Real lenght may differ.
		 */
		pad = (CONFIG_LCD_COLS - strlen(format)) / 2;
		while (pad--)
			lcd_putCharUnlocked(' ', layer);
	}

	len = _formatted_write(format, (void (*)(char, void *))lcd_putCharUnlocked, layer, ap);

	if (mode & (LCD_FILL | LCD_CENTER))
		while (layer->addr % CONFIG_LCD_COLS)
			lcd_putCharUnlocked(' ', layer);

	/*
	 * Riaccendi il cursore e riportalo alla
	 * vecchia posizione
	 */
	if (lcd_CursorStatus)
		lcd_setCursor(lcd_CursorStatus);

	UNLOCK_LCD;

	return len;
}


int lcd_printf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, ...)
{
	int len;
	va_list ap;

	va_start(ap, format);
	len = lcd_vprintf(layer, addr, mode, format, ap);
	va_end(ap);

	return len;
}


/**
 * Internal function to move a layer between two positions.
 *
 * \note The layer must be *already* enqueued in some list.
 * \note The display must be already locked!
 */
static void lcd_enqueueLayer(Layer *layer, char pri)
{
	Layer *l2;

	/* Remove layer from whatever list it was in before */
	REMOVE(layer);

	layer->pri = pri;

	/*
	 * Search for the first layer whose priority
	 * is less or equal to the layer we are adding.
	 */
	FOREACH_NODE(l2, &lcd_Layers)
		if (l2->pri <= pri)
			break;

	/* Enqueue layer */
	INSERT_BEFORE(layer, l2);
}

Layer *lcd_newLayer(char pri)
{
	Layer *layer;

	LOCK_LCD;

	if (LIST_EMPTY(&lcd_FreeLayers))
	{
		UNLOCK_LCD;
		//ASSERT(false);
		return NULL;
	}

	layer = (Layer *)LIST_HEAD(&lcd_FreeLayers);
	layer->addr = 0;
	memset(layer->buf, 0, CONFIG_LCD_ROWS * CONFIG_LCD_COLS);

	lcd_enqueueLayer(layer, pri);

	UNLOCK_LCD;
	return layer;
}

/**
 * Redraw the display (internal).
 *
 * \note The display must be already locked.
 */
static void lcd_refresh(void)
{
	lcdpos_t addr;
	Layer *l;

	for (addr = 0; addr < CONFIG_LCD_ROWS * CONFIG_LCD_COLS; ++addr)
	{
		FOREACH_NODE(l, &lcd_Layers)
		{
			//kprintf("%d %x %p\n", addr, l->buf[0], l);
			if (l->pri == LAYER_HIDDEN)
				break;

			if (l->buf[addr])
			{
				/* Refresh location */
				lcd_putc(addr, l->buf[addr]);
				goto done;
			}
		}

		/* Draw background */
		lcd_putc(addr, ' ');
	done:
		;
	}
}

/**
 * Rearrange layer depth and refresh display accordingly.
 *
 * \note Setting a priority of LAYER_HIDDEN makes the layer invisible.
 */
void lcd_setLayerDepth(Layer *layer, char pri)
{
	if (pri != layer->pri)
	{
		LOCK_LCD;
		lcd_enqueueLayer(layer, pri);
		/* Vile but simple */
		lcd_refresh();
		UNLOCK_LCD;
	}
}

void lcd_deleteLayer(Layer *layer)
{
	LOCK_LCD;

/* We use lcd_refresh() instead.  Much simpler than this mess, but slower. */
#if 0
	Layer *l2;
	lcdpos_t addr;

	/* Repair damage on underlaying layers */
	for (addr = 0; addr < CONFIG_LCD_ROWS * CONFIG_LCD_COLS; ++addr)
	{
		/* If location was covered by us */
		if (layer->buf[addr])
		{
			/* ...and it wasn't covered by others above us... */
			for (l2 = layer->pred; l2->pred; l2 = l2->pred)
				if (l2->buf[addr])
					/* can't just break here! */
					goto not_visible;

			/* ...scan underlaying layers to repair damage */
			for (l2 = layer->succ; l2->succ; l2 = l2->succ)
				if (l2->buf[addr])
				{
					/* Refresh character */
					lcd_putc(addr, l2->buf[addr]);

					/* No need to search on deeper layers */
					break;
				}

			not_visible:
				;
		}
	}
#endif

	// Remove layer from lcd_Layers list.
	REMOVE(layer);

	/* Put layer back into free list */
	ADDHEAD(&lcd_FreeLayers, layer);

	lcd_refresh();

	UNLOCK_LCD;
}


static void lcd_setDefLayer(Layer *layer)
{
	lcd_DefLayer = layer;
}

#include <cfg/debug.h>
void lcd_init(void)
{
	#if CONFIG_KERN
	sem_init(&lcd_semaphore);
	#endif

	int i;

	LIST_INIT(&lcd_Layers);
	LIST_INIT(&lcd_FreeLayers);
	for (i = 0; i < LCD_LAYERS; ++i)
		ADDHEAD(&lcd_FreeLayers, &lcd_LayersPool[i]);

	lcd_setDefLayer(lcd_newLayer(0));

	lcd_hw_init();

	lcd_setCursor(0);
}


