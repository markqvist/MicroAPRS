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
 * \brief LM044L type LCD hardware module (impl.)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "lcd_hd44.h"
#include "hw/hw_lcd_hd44.h"

#include <cfg/test.h>
#include <drv/timer.h>

#warning TODO:Refactor this test to comply whit BeRTOS test policy.

#if UNIT_TEST

void lcd_hw_test(void)
{
	lcd_regWrite(LCD_CMD_SET_DDRAMADDR | 3);
	timer_delay(1);
	kprintf("3 -> %02X\n", lcd_regRead());
	timer_delay(1);

	for (int i = 0; i < 10; i++)
	{
		lcd_dataWrite('c');
		timer_delay(1);
		kprintf("addr = %02X\n", lcd_regRead());
		timer_delay(1);
	}

	lcd_regWrite(LCD_CMD_SET_DDRAMADDR | 0x4a);
	timer_delay(1);
	kprintf("4A -> %02X\n", lcd_regRead());
	timer_delay(1);

	lcd_regWrite(LCD_CMD_SET_DDRAMADDR | 0x52);
	timer_delay(1);
	kprintf("52 -> %02X\n", lcd_regRead());
	timer_delay(1);

	lcd_regWrite(LCD_CMD_SET_DDRAMADDR | 0x1F);
	timer_delay(1);
	kprintf("1F -> %02X\n", lcd_regRead());
	timer_delay(1);
}



#include <drv/timer.c>
#include <drv/lcd_hd44.c>

int main(void)
{
		lcd_hw_test();

		for(;;)
		{
		}

		return 0;
}

#endif
