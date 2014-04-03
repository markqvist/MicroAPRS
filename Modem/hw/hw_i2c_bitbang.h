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
 * Copyright 2003, 2004, 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \brief Macro for I2C bitbang operation.
 *
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_I2C_BITBANG_H
#define HW_I2C_BITBANG_H

#warning TODO:This is an example implementation, you must implement it!

#define SDA_HI  do { /* Implement me:Set SDA High by setting SDA pin as input */ } while (0)
#define SDA_LO  do { /* Implement me:Set SDA Low by setting SDA pin as open collector output */ } while (0)
#define SCL_HI  do { /* Implement me:Set SCL High by setting SCL pin as input */ } while (0)
#define SCL_LO  do { /* Implement me:Set SCL Low by setting SCL pin as open collector output */ } while (0)


#define SCL_IN       (true) /* Implement me: read SDA pin state */
#define SDA_IN       (true) /* Implement me: read SCL pin state */

/**
 * This macro should set SDA and SCL lines as input.
 */
#define I2C_BITBANG_HW_INIT do { /* Implement me! */ } while (0)

/**
 * Half bit delay routine used to generate the correct timings.
 */
#define I2C_HALFBIT_DELAY() do { /* Implement me! */ } while (0)


/*
 * New api
 */
#include <cfg/compiler.h>

INLINE void i2c_sdaHi(int dev)
{
	(void)(dev);
	/* Implement me:Set SDA High by setting SDA pin as input */
}

INLINE void i2c_sdaLo(int dev)
{
	(void)(dev);
	/* Implement me:Set SDA Low by setting SDA pin as open collector output */
}

INLINE void i2c_sclHi(int dev)
{
	(void)(dev);
	/* Implement me:Set SCL High by setting SCL pin as input */
}

INLINE void i2c_sclLo(int dev)
{
	(void)(dev);
	/* Implement me:Set SCL Low by setting SCL pin as open collector output */
}

INLINE bool i2c_sdaIn(int dev)
{
	(void)(dev);
	/* Implement me: read SDA pin state */
	return true;
}

INLINE bool i2c_sclIn(int dev)
{
	(void)(dev);
	/* Implement me: read SCL pin state */
	return true;
}

/**
 * Half bit delay routine used to generate the correct timings.
 */
INLINE void i2c_halfbitDelay(int dev)
{
	(void)(dev);
	/* Implement me! */
}

/**
 * This macro should set SDA and SCL lines as input.
 */
INLINE void i2c_bitbangInit(int dev)
{
	(void)(dev);
	/* Implement me! */
}

#endif /* HW_I2C_BITBANG_H */
