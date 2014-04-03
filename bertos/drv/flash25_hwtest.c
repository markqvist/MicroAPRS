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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *  \brief Test function for serial Flash memory.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

/**
 * Test function for flash25.
 *
 * \note: This implentation use a SPI channel.
 */


#include <io/kfile.h>

#include <drv/flash25.h>

#include <cfg/debug.h>

#warning FIXME:Review and refactor this test..

bool flash25_test(KFile *channel)
{
	Flash25 fd;
	uint8_t test_buf[256];

	/*
	 * Init a spi kfile interface and
	 * flash driver.
	 */
	flash25_init(&fd, channel);

	kprintf("Init serial flash\n");

	flash25_chipErase(&fd);

	flash25_sectorErase(&fd, FLASH25_SECT1);
	flash25_sectorErase(&fd, FLASH25_SECT2);
	flash25_sectorErase(&fd, FLASH25_SECT3);
	flash25_sectorErase(&fd, FLASH25_SECT4);

	/*
	 * Launche a kfile test interface.
	 */
	kprintf("Kfile test start..\n");
	if (!kfile_testRunGeneric(&fd.fd, test_buf, NULL, sizeof(test_buf)))
		return false;

	return true;
}


