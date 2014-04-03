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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TAS5706A Power DAC i2c driver.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "tas5706a.h"

#include "hw/hw_tas5706a.h"

#include "cfg/cfg_tas5706a.h"
#include "cfg/cfg_i2c.h"

#include <cfg/module.h>

#include <drv/i2c.h>
#include <drv/timer.h>

typedef uint8_t tas_addr_t;

#define TAS_ADDR 0x36

#define TRIM_REG   0x1B
#define SYS_REG2   0x05
#define VOLUME_REG 0x07
#define MUTE_VOL 0xFF

#define DB_TO_REG(db) ((24 - (db)) * 2)

#define CH1_VOL_REG 0x08
#define CH2_VOL_REG 0x09
#define CH3_VOL_REG 0x0A
#define CH4_VOL_REG 0x0B


INLINE bool tas5706a_putc(I2c *i2c, tas_addr_t addr, uint8_t ch)
{
	i2c_start_w(i2c, TAS_ADDR, 2, I2C_STOP);
	i2c_putc(i2c, addr);
	i2c_putc(i2c, ch);

	if (i2c_error(i2c))
		return false;

	return true;
}

INLINE int tas5706a_getc(I2c *i2c, tas_addr_t addr)
{
	int ch;

	i2c_start_w(i2c, TAS_ADDR, 2, I2C_NOSTOP);
	i2c_putc(i2c, addr);
	i2c_start_r(i2c, TAS_ADDR, 1, I2C_STOP);
	ch = (int)(uint8_t)i2c_getc(i2c);

	if (i2c_error(i2c))
		return EOF;

	return ch;
}

void tas5706a_setVolume_3(I2c *i2c, Tas5706aCh ch, tas5706a_vol_t vol)
{
	ASSERT(ch < TAS_CNT);
	ASSERT(vol <= TAS_VOL_MAX);

	tas_addr_t addr1, addr2;

	switch(ch)
	{
		case TAS_CH1:
			addr1 = CH1_VOL_REG;
			addr2 = CH3_VOL_REG;
			break;
		case TAS_CH2:
			addr1 = CH2_VOL_REG;
			addr2 = CH4_VOL_REG;
			break;
		default:
			ASSERT(0);
			return;
	}

	uint8_t vol_att = 0xff - ((vol * 0xff) / TAS_VOL_MAX);

	tas5706a_putc(i2c, addr1, vol_att);
	tas5706a_putc(i2c, addr2, vol_att);
}

void tas5706a_setLowPower_2(I2c *i2c, bool val)
{
	ASSERT(i2c);

	TAS5706A_SETPOWERDOWN(val);
	TAS5706A_SETMUTE(val);
}


void tas5706a_init_1(I2c *i2c)
{
	ASSERT(i2c);
	MOD_CHECK(timer);

	TAS5706A_PIN_INIT();
	timer_delay(200);
	TAS5706A_SETPOWERDOWN(false);
	TAS5706A_SETMUTE(false);
	TAS5706A_MCLK_INIT();
	timer_delay(2);
	TAS5706A_SETRESET(false);
	timer_delay(20);
	tas5706a_putc(i2c, TRIM_REG, 0x00);

	tas5706a_putc(i2c, VOLUME_REG, DB_TO_REG(CONFIG_TAS_MAX_VOL));

	/* Unmute */
	tas5706a_putc(i2c, SYS_REG2, 0);
}
