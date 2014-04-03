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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Dataflash HW control routines.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "hw/hw_dataflash.h"

#include <cfg/compiler.h>
#include <cfg/module.h>
#include <cfg/macros.h>

#warning TODO:This is an example implementation, you must implement it!

MOD_DEFINE(hw_dataflash);

/**
 * Data flash init function.
 *
 * This function provide to initialize all that
 * needs to drive a dataflash memory.
 * Generaly needs to init pins to drive a CS line
 * and reset line.
 */
void dataflash_hw_init(void)
{

    //Disable CS line (remove if not needed)
	dataflash_hw_setCS(false);

	/*
	 * Put here your code!
	 *
	 * Note:
	 * - if you drive manualy CS line, here init a CS pin
	 * - if you use a dedicated reset line, here init a reset pin
	 */

	MOD_INIT(hw_dataflash);
}

/**
 * Chip Select drive.
 *
 * This function enable or disable a CS line.
 * You must implement this function comply to a dataflash
 * memory datasheet to allow the drive to enable a memory
 * when \p enable flag is true, and disable it when is false.
 */
void dataflash_hw_setCS(bool enable)
{
	if (enable)
	{
		/*
		 * Put here your code to enable
		 * dataflash memory
		 */
	}
	else
	{
		/*
		 * Put here your code to disable
		 * dataflash memory
		 */
	}
}

/**
 * Reset data flash memory.
 *
 * This function provide to send reset signal to
 * dataflash memory. You must impement it comly to a dataflash
 * memory datasheet to allow the drive to set a reset pin
 * when \p enable flag is true, and disable it when is false.
 *
 */
void dataflash_hw_setReset(bool enable)
{
	if (enable)
	{
		/*
		 * Put here your code to set reset of
		 * dataflash memory
		 */
	}
	else
	{
		/*
		 * Put here your code to clear reset of
		 * dataflash memory
		 */
	}
}

