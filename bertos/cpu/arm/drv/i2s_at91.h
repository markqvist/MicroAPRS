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
 * \brief I2S driver functions.
 *
 * This driver uses a double buffering technique to keep i2s bus busy. First fill in the two buffers
 * using i2s_getBuffer(), then start audio playing with i2s_start(). Then call i2s_getFreeBuffer()
 * until you have finished your samples. The reproduction will automatically stop if you don't
 * call i2s_getFreeBuffer() frequently enough.
 *
 * Example:
 * \code
 * // fill in the buffers before start
 * buf = i2s_getBuffer(I2S_FIRST_BUF);
 * // ...
 * buf = i2s_getBuffer(I2S_SECOND_BUF);
 * // ...
 * // here the driver will play only the first two buffers...
 * i2s_start();
 * // ...call getFreeBuffer() to continue playing.
 * while (!(buf = i2s_getFreeBuffer()))
 *     ;
 * // now fill the buffer again
 * \endcode
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * $WIZ$ module_name = "i2s"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_i2s.h"
 * $WIZ$ module_supports = "at91"
 */

#ifndef DRV_I2S_AT91_H
#define DRV_I2S_AT91_H


#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <io/arm.h>

/**
 * First buffer.
 */
#define I2S_FIRST_BUF  0
/**
 * Second buffer.
 */
#define I2S_SECOND_BUF 1

/**
 * Initializes the module and sets current buffer to I2S_FIRST_BUF.
 */
void i2s_init(void);

/**
 * Returns one of the two buffers or NULL if none is available.
 *
 * You can't call this function if you have already started the player.
 * \param buf_num The number of the buffer, ie I2S_FIRST_BUF or I2S_SECOND_BUF.
 * \return A pointer to the buffer if the buffer is available (not full), 0 on errors
 */
uint8_t *i2s_getBuffer(unsigned buf_num);

/**
 * Returns a buffer that will be played after the current one.
 *
 * You should fill it faster than your reproduction time. You can't call this function
 * if the player is not running
 * \return The next buffer to be played, 0 if both are busy.
 */
uint8_t *i2s_getFreeBuffer(void);

/**
 * Starts playing from I2S_FIRST_BUFFER.
 *
 * You must have filled both buffers before calling this function. Does nothing if already playing.
 * \return false on errors, true otherwise.
 */
bool i2s_start(void);

INLINE bool i2s_isPlaying(void)
{
	return !(SSC_SR & BV(SSC_TXEMPTY));
}

#endif /* DRV_I2S_AT91_H */
