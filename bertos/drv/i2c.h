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
 * \defgroup i2c_driver I2C driver
 * \ingroup drivers
 * \{
 * \brief I2C generic driver functions.
 *
 * Some hardware requires you to declare the number of transferred
 * bytes and the communication direction before actually reading or writing
 * to the bus.
 * Furthermore, sometimes you need to specify the first transferred byte
 * before any data is sent over the bus.
 *
 * The usage pattern for writing is the following:
 * \code
 * i2c_init(args...);
 * ...
 * i2c_start_w(args...);
 * i2c_write(i2c, buf, len);
 * \endcode
 * The flags in i2c_start_w determine if the stop command is sent after
 * the data. Notice that you don't need to explicitly call a stop function
 * after the write.
 *
 * Reading is a bit more complicated and it largely depends on the specific
 * slave hardware.
 * In general, the hardware may require you to first write something, then
 * read the data without closing the communication. For example, EPROMs
 * require first to write the reading address and then to read the actual
 * data.
 * Here is an example of how you can deal with such hardware:
 *
 * \code
 * // init a session without closing it
 * i2c_start_w(i2c, dev, bytes, I2C_NOSTOP);
 * // write the address to read from
 * i2c_write(i2c, addr, bytes);
 * if (i2c_error(i2c))
 *     // check for errors during setup
 *     //...
 * // now start the real data transfer
 * i2c_start_r(i2c, dev, bytes, I2C_STOP);
 * i2c_read(i2c, buf, bytes);
 * // check for errors
 * if (i2c_error(i2c))
 *     //...
 * \endcode
 *
 * It's not guaranteed that after a single call to i2c_putc, i2c_getc etc.
 * data will pass on the bus (this is hardware dependent).
 * However, it IS guaranteed after you have sent all the data.
 *
 * You can check error conditions by calling the function i2c_error after
 * each function call. (This is similar to libc errno handling).
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "i2c"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_i2c.h"
 * $WIZ$ module_hw = "bertos/hw/hw_i2c_bitbang.h"
 * $WIZ$ module_depends = "i2c_bitbang"
 * $WIZ$ module_supports = "not atmega103 and not atmega168 and not at91"
 */

#ifndef DRV_I2C_H
#define DRV_I2C_H

#include "cfg/cfg_i2c.h"

#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/attr.h>

#define I2C_READBIT BV(0)


/*
 * The following macros are needed to maintain compatibility with older i2c API.
 * They can be safely removed once the old API is removed.
 */

 /**
  * \addtogroup i2c_api
  * \{
  */
#if COMPILER_C99
	#define i2c_init(...)           PP_CAT(i2c_init ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define i2c_start_w(...)        PP_CAT(i2c_start_w ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
	#define i2c_start_r(...)        PP_CAT(i2c_start_r ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
#else
	/**
	 * Initialize I2C module.
	 *
	 * To initialize the module you can write this code:
	 * \code
	 * I2c ctx;
	 * i2c_init(&ctx, 0, CONFIG_I2C_FREQ);
	 * \endcode
	 * This macro expands in two versions, depending on the number of
	 * parameters, to maintain compatibility with old API:
	 * \li i2c_init_3(I2c *i2c, int dev, uint32_t clock)
	 * \li i2c_init_0(void)
	 *
	 * Do NOT use the above functions directly, use i2c_init().
	 * \note Use the version with 3 parameters, the other one is only for
	 * legacy code.
	 */
	#define i2c_init(args...)       PP_CAT(i2c_init ## _, COUNT_PARMS(args)) (args)

	/**
	 * Start a write session.
	 *
	 * To start a write session, use the following code:
	 * \code
	 * i2c_start_w(i2c, dev, bytes, I2C_STOP);
	 * \endcode
	 *
	 * This macro expands in two versions, depending on the number of parameters:
	 * \li i2c_start_w_4(I2c *i2c, uint16_t slave_addr, size_t size, int flags)
	 * \li i2c_builtin_start_w(uint8_t id): Deprecated API, don't use in new projects
	 * \li i2c_bitbang_start_w(uint8_t id): Deprecated API, don't use in new projects
	 *
	 * Do NOT use the above functions directly, use i2c_start_w().
	 * \note Use the version with 4 parameters, the others are only for legacy code
	 */
	#define i2c_start_w(args...)    PP_CAT(i2c_start_w ## _, COUNT_PARMS(args)) (args)

	/**
	 * Start a read session.
	 *
	 * To start a read session, use the following code:
	 * \code
	 * i2c_start_r(i2c, dev, bytes, I2C_STOP);
	 * \endcode
	 *
	 * This macro expands in two versions, depending on the number of parameters:
	 * \li i2c_start_r_4(I2c *i2c, uint16_t slave_addr, size_t size, int flags)
	 * \li i2c_builtin_start_r(uint8_t id): Deprecated API, don't use in new projects
	 * \li i2c_bitbang_start_r(uint8_t id): Deprecated API, don't use in new projects
	 *
	 * Do NOT use the above functions directly, use i2c_start_r().
	 * \note Use the version with 4 parameters, the others are only for legacy code
	 */
	#define i2c_start_r(args...)    PP_CAT(i2c_start_r ## _, COUNT_PARMS(args)) (args)
#endif
/**\}*/


/**
 * \name I2C bitbang devices enum
 */
enum
{
	I2C_BITBANG_OLD = -1,
	I2C_BITBANG0 = 1000, ///< Use bitbang on port 0
	I2C_BITBANG1,        ///< Use bitbang on port 1
	I2C_BITBANG2,
	I2C_BITBANG3,
	I2C_BITBANG4,
	I2C_BITBANG5,
	I2C_BITBANG6,
	I2C_BITBANG7,
	I2C_BITBANG8,
	I2C_BITBANG9,

	I2C_BITBANG_CNT  /**< Number of i2c ports */
};

/** \defgroup i2c_api I2C driver API
 * \ingroup i2c_driver
 * \{
 */

/**
 * \name I2C error flags
 * \ingroup i2c_api
 * @{
 */
#define I2C_OK               0     ///< I2C no errors flag
#define I2C_DATA_NACK     BV(4)    ///< I2C generic error
#define I2C_ERR           BV(3)    ///< I2C generic error
#define I2C_ARB_LOST      BV(2)    ///< I2C arbitration lost error
#define I2C_START_TIMEOUT BV(0)    ///< I2C timeout error on start
#define I2C_NO_ACK        BV(1)    ///< I2C no ack for sla start
/**@}*/

/**
 * \name I2C command flags
 * \ingroup i2c_api
 * @{
 */
#define I2C_NOSTOP           0    ///< Do not program the stop for current transition
#define I2C_STOP          BV(0)   ///< Program the stop for current transition
/** @} */
#define I2C_START_R       BV(1)   // Start read command
#define I2C_START_W          0    // Start write command


#define I2C_TEST_START(flag)  ((flag) & I2C_START_R)
#define I2C_TEST_STOP(flag)   ((flag) & I2C_STOP)

struct I2cHardware;
struct I2c;

typedef void (*i2c_start_t)(struct I2c *i2c, uint16_t slave_addr);
typedef uint8_t (*i2c_getc_t)(struct I2c *i2c);
typedef void (*i2c_putc_t)(struct I2c *i2c, uint8_t data);
typedef void (*i2c_write_t)(struct I2c *i2c, const void *_buf, size_t count);
typedef void (*i2c_read_t)(struct I2c *i2c, void *_buf, size_t count);

typedef struct I2cVT
{
	i2c_start_t start;
	i2c_getc_t   getc;
	i2c_putc_t   putc;
	i2c_write_t  write;
	i2c_read_t   read;
} I2cVT;

typedef struct I2c
{
	int errors;
	int flags;
	size_t xfer_size;
	struct I2cHardware* hw;
	const struct I2cVT *vt;
} I2c;


#include CPU_HEADER(i2c)

/*
 * Low level i2c  init implementation prototype.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock);
void i2c_hw_bitbangInit(I2c *i2c, int dev);

void i2c_genericWrite(I2c *i2c, const void *_buf, size_t count);
void i2c_genericRead(I2c *i2c, void *_buf, size_t count);

/*
 * Start a i2c transfer.
 *
 * \param i2c Context structure.
 * \param slave_addr Address of slave device
 * \param size Size of the transfer
 */
INLINE void i2c_start(I2c *i2c, uint16_t slave_addr, size_t size)
{
	ASSERT(i2c->vt);
	ASSERT(i2c->vt->start);

	if (!i2c->errors)
		ASSERT(i2c->xfer_size == 0);

	i2c->errors = 0;
	i2c->xfer_size = size;

	i2c->vt->start(i2c, slave_addr);
}

/**
 * \name I2C interface functions
 * \ingroup i2c_api
 * @{
 */

/**
 * Start a read session.
 * \param i2c I2C context
 * \param slave_addr Address of the slave device
 * \param size Number of bytes to be read from device
 * \param flags Session flags (I2C command flags)
 */
INLINE void i2c_start_r_4(I2c *i2c, uint16_t slave_addr, size_t size, int flags)
{
	ASSERT(i2c);
	i2c->flags = flags | I2C_START_R;
	i2c_start(i2c, slave_addr, size);
}

/**
 * Start a write session.
 * \param i2c I2C context
 * \param slave_addr Address of the slave device
 * \param size Size to be transferred
 * \param flags Session flags
 */
INLINE void i2c_start_w_4(I2c *i2c, uint16_t slave_addr, size_t size, int flags)
{
	ASSERT(i2c);
	i2c->flags = flags & ~I2C_START_R;
	i2c_start(i2c, slave_addr, size);
}

/**
 * Read a byte from I2C bus.
 * \param i2c I2C context
 * \return Byte read
 */
INLINE uint8_t i2c_getc(I2c *i2c)
{
	ASSERT(i2c);
	ASSERT(i2c->vt);
	ASSERT(i2c->vt->getc);

	ASSERT(i2c->xfer_size);

	ASSERT(I2C_TEST_START(i2c->flags) == I2C_START_R);

	if (!i2c->errors)
	{
		uint8_t data = i2c->vt->getc(i2c);
		i2c->xfer_size--;
		return data;
	}
	else
		return 0xFF;
}

/**
 * Write the byte \a data into I2C port \a i2c.
 * \param i2c I2C context
 * \param data Byte to be written
 */
INLINE void i2c_putc(I2c *i2c, uint8_t data)
{
	ASSERT(i2c);
	ASSERT(i2c->vt);
	ASSERT(i2c->vt->putc);

	ASSERT(i2c->xfer_size);

	ASSERT(I2C_TEST_START(i2c->flags) == I2C_START_W);

	if (!i2c->errors)
	{
		i2c->vt->putc(i2c, data);
		i2c->xfer_size--;
	}
}

/**
 * Write \a count bytes to port \a i2c, reading from \a _buf.
 * \param i2c I2C context
 * \param _buf User buffer to read from
 * \param count Number of bytes to write
 */
INLINE void i2c_write(I2c *i2c, const void *_buf, size_t count)
{
	ASSERT(i2c);
	ASSERT(i2c->vt);
	ASSERT(i2c->vt->write);

	ASSERT(_buf);
	ASSERT(count);
	ASSERT(count <= i2c->xfer_size);

	ASSERT(I2C_TEST_START(i2c->flags) == I2C_START_W);

	if (!i2c->errors)
		i2c->vt->write(i2c, _buf, count);
}

/**
 * Read \a count bytes into buffer \a _buf from device \a i2c.
 * \param i2c Context structure
 * \param _buf Buffer to fill
 * \param count Number of bytes to read
 */
INLINE void i2c_read(I2c *i2c, void *_buf, size_t count)
{
	ASSERT(i2c);
	ASSERT(i2c->vt);
	ASSERT(i2c->vt->read);

	ASSERT(_buf);
	ASSERT(count);
	ASSERT(count <= i2c->xfer_size);

	ASSERT(I2C_TEST_START(i2c->flags) == I2C_START_R);

	if (!i2c->errors)
		i2c->vt->read(i2c, _buf, count);
}

/**
 * Return the error condition of the bus and clear errors.
 */
INLINE int i2c_error(I2c *i2c)
{
	ASSERT(i2c);
	int err = i2c->errors;
	i2c->errors = 0;

	return err;
}

/**
 * Initialize I2C context structure.
 * \param i2c I2C context structure
 * \param dev Number of device to be initialized. You can use I2C_BITBANG0
 *            and similar if you want to activate the bitbang driver.
 * \param clock Peripheral clock
 */
#define i2c_init_3(i2c, dev, clock)   ((((dev) >= I2C_BITBANG0) | ((dev) == I2C_BITBANG_OLD)) ? \
										i2c_hw_bitbangInit((i2c), (dev)) : i2c_hw_init((i2c), (dev), (clock)))
/**@}*/
/**\}*/ // i2c_api

/**
 * \defgroup old_i2c_api Old I2C API
 * \ingroup i2c_driver
 *
 * This is the old and deprecated I2C API. It is maintained for backward
 * compatibility only, don't use it in new projects.
 * @{
 */
#if !CONFIG_I2C_DISABLE_OLD_API

/**
 * \ingroup old_i2c_api
 * \name I2C Backends.
 * Sometimes your cpu does not have a builtin
 * i2c driver or you don't want, for some reason, to
 * use that.
 * With this you can choose, at compile time, which backend to use.
 * Set the CONFIG_I2C_BACKEND configuration variable in cfg_i2c.h
 * @{
 */
#define I2C_BACKEND_BUILTIN 0 ///< Uses cpu builtin i2c driver
#define I2C_BACKEND_BITBANG 1 ///< Uses emulated bitbang driver
/**@}*/

/**
 * \name I2c builtin prototypes.
 * \ingroup old_i2c_api
 * Do NOT use these function directly, instead,
 * you can call the ones named without "_builtin_"
 * and specify in cfg_i2c.h (CONFIG_I2C_BACKEND)
 * that you want the builtin backend.
 * @{
 */
bool i2c_builtin_start_w(uint8_t id);
bool i2c_builtin_start_r(uint8_t id);
void i2c_builtin_stop(void);
bool i2c_builtin_put(uint8_t _data);
int i2c_builtin_get(bool ack);
/**@}*/

/**
 * \name I2c bitbang prototypes.
 * \ingroup old_i2c_api
 * Do NOT use these function directly, instead,
 * you can call the ones named without "_bitbang_"
 * and specify in cfg_i2c.h (CONFIG_I2C_BACKEND)
 * that you want the bitbang backend.
 * @{
 */
bool i2c_bitbang_start_w(uint8_t id);
bool i2c_bitbang_start_r(uint8_t id);
void i2c_bitbang_stop(void);
bool i2c_bitbang_put(uint8_t _data);
int i2c_bitbang_get(bool ack);
/**@}*/

#ifndef CONFIG_I2C_BACKEND
#define  CONFIG_I2C_BACKEND  I2C_BACKEND_BUILTIN
#endif

#if CONFIG_I2C_BACKEND == I2C_BACKEND_BUILTIN
	#define i2c_start_w_1   i2c_builtin_start_w
	#define i2c_start_r_1   i2c_builtin_start_r
	#define i2c_stop        i2c_builtin_stop
	#define i2c_put         i2c_builtin_put
	#define i2c_get         i2c_builtin_get
#elif CONFIG_I2C_BACKEND == I2C_BACKEND_BITBANG
	#define i2c_start_w_1   i2c_bitbang_start_w
	#define i2c_start_r_1   i2c_bitbang_start_r
	#define i2c_stop        i2c_bitbang_stop
	#define i2c_put         i2c_bitbang_put
	#define i2c_get         i2c_bitbang_get
#else
	#error Unsupported i2c backend.
#endif


bool i2c_send(const void *_buf, size_t count);
bool i2c_recv(void *_buf, size_t count);

/**@}*/


extern I2c local_i2c_old_api;

/**
 * Initialize I2C module (old API).
 * \attention This function is deprecated. Use i2c_init(args...) in new code
 */
INLINE void i2c_init_0(void)
{
	#if CONFIG_I2C_BACKEND == I2C_BACKEND_BITBANG
		i2c_init_3(&local_i2c_old_api, I2C_BITBANG_OLD, CONFIG_I2C_FREQ);
	#else
		i2c_init_3(&local_i2c_old_api, 0, CONFIG_I2C_FREQ);
	#endif
}
#endif /* !CONFIG_I2C_DISABLE_OLD_API */

/** \} */ //defgroup i2c_driver

#endif
