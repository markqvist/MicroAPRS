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
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Displaytech 32122A LCD driver
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 */


#include "lcd_32122a.h"

#include "hw/hw_lcd_32122a.h"

#include "cfg/cfg_lcd_32122a.h"

#include <cfg/macros.h> /* BV() */
#include <cfg/debug.h>
#include <cfg/module.h>

#include <gfx/gfx.h>

#include <drv/timer.h>
#include <drv/pwm.h>

#include <cpu/irq.h>
#include <cpu/types.h>


/** Number of LCD pages */
#define LCD_PAGES 4

/** Width of an LCD page */
#define LCD_PAGESIZE (LCD_WIDTH / 2)

/**
 * \name 32122A Commands
 * @{
 */
#define LCD_CMD_DISPLAY_ON  0xAF
#define LCD_CMD_DISPLAY_OFF 0xAE
#define LCD_CMD_STARTLINE   0xC0
#define LCD_CMD_PAGEADDR    0xB8
#define LCD_CMD_COLADDR     0x00
#define LCD_CMD_ADC_LEFT    0xA1
#define LCD_CMD_ADC_RIGHT   0xA0
#define LCD_CMD_STATIC_OFF  0xA4
#define LCD_CMD_STATIC_ON   0xA5
#define LCD_CMD_DUTY_32     0xA9
#define LCD_CMD_DUTY_16     0xA8
#define LCD_CMD_RMW_ON      0xE0
#define LCD_CMD_RMW_OFF     0xEE
#define LCD_CMD_RESET       0xE2
/*@}*/


/* Status flags */
#define LCDF_BUSY BV(7)

#if CONFIG_LCD_WAIT
	/**
	 * \code
	 *      __              __
	 * RS   __\____________/__
	 *         ____________
	 * R/W  __/            \__
	 *            _______
	 * E1   _____/       \____
	 *        ______      ____
	 * DATA X/      \====/
	 *
	 * \endcode
	 */
	#define WAIT_LCD \
		do { \
			uint8_t status; \
			LCD_DB_IN; \
			do { \
				LCD_SET_RD; \
				LCD_CLR_A0; \
				LCD_SET_E1; \
				LCD_DELAY_READ; \
				status = LCD_READ; \
				LCD_CLR_E1; \
				LCD_SET_A0; \
				LCD_CLR_RD; \
			} while (status & LCDF_BUSY); \
			LCD_DB_OUT; \
		} while (0)
#else /* CONFIG_LCD_WAIT */
	#define WAIT_LCD do {} while(0)
#endif /* CONFIG_LCD_WAIT */


/**
 * Raster buffer to draw into.
 *
 * Bits in the bitmap bytes have vertical orientation,
 * as required by the LCD driver.
 */
static uint8_t lcd_raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];

/** Default LCD bitmap */
struct Bitmap lcd_bitmap;


#if CONFIG_LCD_SOFTINT_REFRESH

/** Timer for regular LCD refresh */
static Timer lcd_refresh_timer;

#endif /* CONFIG_LCD_SOFTINT_REFRESH */


INLINE void lcd_32122_cmd(uint8_t cmd, uint8_t chip)
{
	WAIT_LCD;

	/*      __              __
	 * A0   __\____________/__
	 *
	 * R/W  __________________
	 *            ______
	 * E1   _____/      \_____
	 *
	 * DATA --<============>--
	 */
	LCD_WRITE(cmd);
	//LCD_DB_OUT;
	LCD_CLR_A0;
	LCD_SET_E(chip);
	LCD_DELAY_WRITE;
	LCD_CLR_E(chip);
	LCD_SET_A0;
	//LCD_DB_IN;

}


INLINE uint8_t lcd_32122_read(uint8_t chip)
{
	uint8_t data;

	WAIT_LCD;

	/**
	 * \code
	 *      __________________
	 * A0   __/            \__
	 *         ____________
	 * R/W  __/            \__
	 *            _______
	 * E1   _____/       \____
	 *
	 * DATA -------<=====>----
	 *
	 * \endcode
	 */
	LCD_DB_IN;
	//LCD_SET_A0;
	LCD_SET_RD;
	LCD_SET_E(chip);
	LCD_DELAY_READ;
	data = LCD_READ;
	LCD_CLR_E(chip);
	LCD_CLR_RD;
	//LCD_CLR_A0;
	LCD_DB_OUT;

	return data;
}

INLINE void lcd_32122_write(uint8_t c, uint8_t chip)
{
	WAIT_LCD;

	/**
	 * \code
	 *      __________________
	 * A0   ___/          \___
	 *
	 * R/W  __________________
	 *            ______
	 * E1   _____/      \_____
	 *
	 * DATA -<==============>-
	 *
	 * \endcode
	 */
	LCD_WRITE(c);
	//LCD_DB_OUT;
	//LCD_SET_A0;
	LCD_SET_E(chip);
	LCD_DELAY_WRITE;
	LCD_CLR_E(chip);
	//LCD_CLR_A0;
	//LCD_DB_IN;
}

static void lcd_32122_clear(void)
{
	uint8_t page, j;

	for (page = 0; page < LCD_PAGES; ++page)
	{
		lcd_32122_cmd(LCD_CMD_COLADDR, LCDF_E1 | LCDF_E2);
		lcd_32122_cmd(LCD_CMD_PAGEADDR | page, LCDF_E1 | LCDF_E2);
		for (j = 0; j < LCD_PAGESIZE; j++)
			lcd_32122_write(0, LCDF_E1 | LCDF_E2);
	}
}


static void lcd_32122_writeRaster(const uint8_t *raster)
{
	uint8_t page, rows;
	const uint8_t *right_raster;

	for (page = 0; page < LCD_PAGES; ++page)
	{
		lcd_32122_cmd(LCD_CMD_PAGEADDR | page, LCDF_E1 | LCDF_E2);
		lcd_32122_cmd(LCD_CMD_COLADDR | 0, LCDF_E1 | LCDF_E2);

		/* Super optimized lamer loop */
		right_raster = raster + LCD_PAGESIZE;
		rows = LCD_PAGESIZE;
		do
		{
			lcd_32122_write(*raster++, LCDF_E1);
			lcd_32122_write(*right_raster++, LCDF_E2);
		}
		while (--rows);
		raster = right_raster;
	}
}

#if CONFIG_LCD_SOFTINT_REFRESH

static void lcd_32122_refreshSoftint(void)
{
	lcd_32122_blitBitmap(&lcd_bitmap);
	timer_setDelay(&lcd_refresh_timer, ms_to_ticks(CONFIG_LCD_REFRESH));
	timer_add(&lcd_refresh_timer);
}

#endif /* CONFIG_LCD_SOFTINT_REFRESH */

/**
 * Set LCD contrast PWM.
 */
void lcd_32122_setPwm(int duty)
{
	ASSERT(duty >= LCD_MIN_PWM);
	ASSERT(duty <= LCD_MAX_PWM);

	pwm_setDuty(LCD_PWM_CH, duty);
	pwm_enable(LCD_PWM_CH, true);
}

/**
 * Update the LCD display with data from the provided bitmap.
 */
void lcd_32122_blitBitmap(const Bitmap *bm)
{
	lcd_32122_writeRaster(bm->raster);
}


/**
 * Initialize LCD subsystem.
 *
 * \note The PWM used for LCD contrast is initialized in drv/pwm.c
 *       because it is the same PWM used for output attenuation.
 */
void lcd_32122_init(void)
{
	MOD_CHECK(timer);

	pwm_init();

	lcd_32122a_hw_bus_init();
	LCD_32122_RESET();

	lcd_32122_cmd(LCD_CMD_RESET, LCDF_E1 | LCDF_E2);
	lcd_32122_cmd(LCD_CMD_DISPLAY_ON, LCDF_E1 | LCDF_E2);
	lcd_32122_cmd(LCD_CMD_STARTLINE | 0, LCDF_E1 | LCDF_E2);


	lcd_32122_clear();
	lcd_32122_setPwm(LCD_DEF_PWM);

	gfx_bitmapInit(&lcd_bitmap, lcd_raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_bitmapClear(&lcd_bitmap);

#if CONFIG_LCD_SOFTINT_REFRESH
	/* Init IRQ driven LCD refresh */
	timer_setSoftint(&lcd_refresh_timer, (Hook)lcd_32122_refreshSoftint, 0);
	timer_setDelay(&lcd_refresh_timer, ms_to_ticks(CONFIG_LCD_REFRESH));
	timer_add(&lcd_refresh_timer);
#endif /* CONFIG_LCD_SOFTINT_REFRESH */

}

