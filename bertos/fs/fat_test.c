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
 * \brief FatFs test.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_fat.h $cfgdir/
 * $test$: echo  "#undef CONFIG_FAT_USE_MKFS" >> $cfgdir/cfg_fat.h
 * $test$: echo "#define CONFIG_FAT_USE_MKFS 1" >> $cfgdir/cfg_fat.h
 *
 */

#include "fat.h"

#include "fatfs/ff.h"
#include "fatfs/diskio.h"

#include <cfg/test.h>

/* avoid compiler warnings... */
int fatfile_testSetup(void);
int fatfile_testTearDown(void);
int fatfile_testRun(void);

static FATFS file_system;

int fatfile_testSetup(void)
{
	FRESULT err;
	err = f_mount(0, &file_system);
	ASSERT(err == FR_OK);

	err = f_mkfs(0, 0, 512);
	ASSERT(err == FR_OK);
	return 0;
}

int fatfile_testTearDown(void)
{
	FRESULT err;
	err = f_mount(0, 0);
	ASSERT(err == FR_OK);
	return 0;
}

int fatfile_testRun(void)
{
	FRESULT fat_err;
	FatFile file_handler;
	const int SIZE = 10;
	int write[SIZE], read[SIZE];

	fat_err = fatfile_open(&file_handler, "foo.txt", FA_WRITE | FA_CREATE_ALWAYS);
	ASSERT(fat_err == FR_OK);

	for (int i = 0; i < SIZE; ++i)
	{
		write[i] = i;
		size_t count = kfile_write(&file_handler.fd, &i, sizeof(int));
		ASSERT(count == sizeof(int));
	}
	/* test error function */
	int tmp;
	if (kfile_read(&file_handler.fd, &tmp, sizeof(int)) < sizeof(int))
		ASSERT(kfile_error(&file_handler.fd) == FR_DENIED);
	kfile_clearerr(&file_handler.fd);
	ASSERT(file_handler.error_code == FR_OK);


	int err = 0;
	err = kfile_close(&file_handler.fd);
	ASSERT(err == 0);

	fat_err = fatfile_open(&file_handler, "foo.txt", FA_READ);
	ASSERT(fat_err == FR_OK);

	for (int i = 0; i < SIZE; ++ i)
	{
		size_t count = kfile_read(&file_handler.fd, &read[i], sizeof(int));
		ASSERT(count == sizeof(int));
		/* check for correctness */
		ASSERT(read[i] == write[i]);
	}
	/* test kfile_seek() */
	ASSERT(kfile_seek(&file_handler.fd, -(sizeof(int) * SIZE * 2), KSM_SEEK_CUR) == 0);
	ASSERT(kfile_seek(&file_handler.fd, sizeof(int), KSM_SEEK_END) == EOF);

	ASSERT(kfile_close(&file_handler.fd) == 0);

	fatfile_open(&file_handler, "foo.txt", FA_READ | FA_WRITE);
	ASSERT((size_t)kfile_seek(&file_handler.fd, sizeof(int), KSM_SEEK_END) == sizeof(int) * (SIZE + 1));
	ASSERT(kfile_seek(&file_handler.fd, -SIZE, KSM_SEEK_SET) == 0);

	return 0;
}


TEST_MAIN(fatfile);
