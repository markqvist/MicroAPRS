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
 * -->
 *
 * \brief Test function for ini_reader module.
 *
 * $test$: cp bertos/cfg/cfg_kfile.h $cfgdir/
 * $test$: echo "#undef CONFIG_KFILE_GETS" >> $cfgdir/cfg_kfile.h
 * $test$: echo "#define CONFIG_KFILE_GETS 1" >> $cfgdir/cfg_kfile.h
 * 
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include <emul/kfile_posix.h>
#include <cfg/test.h>

#include <string.h> // strcmp

#include "ini_reader.h"

const char ini_file[] = "./test/ini_reader_file.ini";
static KFilePosix kf;

int ini_reader_testSetup(void)
{
	kdbg_init();
	if (!kfile_posix_init(&kf, ini_file, "r"))
	{
		kprintf("No test file found\n");
		return -1;
	}
	return 0;
}

int ini_reader_testRun(void)
{
	char buf[30];
	memset(buf, 0, 30);

	ASSERT(ini_getString(&kf.fd, "First", "String", "default", buf, 30) != EOF);
	ASSERT(strcmp(buf, "noot") == 0);

	ASSERT(ini_getString(&kf.fd, "Second", "Val", "default", buf, 30) != EOF);
	ASSERT(strcmp(buf, "2") == 0);

	ASSERT(ini_getString(&kf.fd, "First", "Empty", "default", buf, 30) != EOF);
	ASSERT(strcmp(buf, "") == 0);

	ASSERT(ini_getString(&kf.fd, "Second", "Bar", "default", buf, 30) == EOF);
	ASSERT(strcmp(buf, "default") == 0);

	ASSERT(ini_getString(&kf.fd, "Foo", "Bar", "default", buf, 30) == EOF);
	ASSERT(strcmp(buf, "default") == 0);

	ASSERT(ini_getString(&kf.fd, "Second", "Long key", "", buf, 30) == EOF);

	ASSERT(ini_getString(&kf.fd, "Second", "comment", "", buf, 30) != EOF);
	ASSERT(strcmp(buf, "line with #comment") == 0);

	ASSERT(ini_getString(&kf.fd, "Long section with spaces", "value", "", buf, 30) != EOF);
	ASSERT(strcmp(buf, "long value") == 0);

	ASSERT(ini_getString(&kf.fd, "Long section with spaces", "no_new_line", "", buf, 30) != EOF);
	ASSERT(strcmp(buf, "value") == 0);
	return 0;
}

int ini_reader_testTearDown(void)
{
	return kfile_close(&kf.fd);
}

TEST_MAIN(ini_reader);
