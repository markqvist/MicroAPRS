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
 *
 * -->
 *
 * \brief DataFlash test.
 *
 * This module test the dataflash memory among the supported memory (see drv/dataflash.h
 * for more detail).
 * To test memory we fill one buffer with casual char, and write it in different
 * part of memory. After every write we read the data that we have been write
 * and compare this with test buffer, checking if write and read command work
 * correclty. We also check if driver work properly when we make a write out the
 * limit of memory size.
 *
 * Note: dataflash driver use a kfile interface, so for write/read test
 * we use a kfile_test module that perform some generic test.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "hw/hw_dataflash.h"
#include "cfg/cfg_dataflash.h"
#include "cfg/cfg_proc.h"

#include <cfg/test.h>
#include <cfg/debug.h>
#include <cfg/module.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL      DATAFLASH_LOG_LEVEL
#define LOG_FORMAT     DATAFLASH_LOG_FORMAT
#include <cfg/log.h>   // for logging system

#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/dataflash.h>

#include <kern/proc.h>
#include <io/kfile.h>

#include <string.h>

/*
 * Settings for dataflash test
 *
 * \{
 */
// Datafalsh type memory to test (see drv/dataflash.h for supported memory types)
#define DATAFLASH_MEM_MODEL            DFT_AT45DB642D

// Function to set CS, this is typically implement in hw/hw_dataflash.{c, h}
#define DATAFLASH_FUNC_CS_SET      dataflash_hw_setCS

// Function to reset memery, this is typically implement in hw/hw_dataflash.{c, h}
#define DATAFLASH_FUNC_RESET                     NULL

// Buffer len to test dataflash
#define DATAFLASH_TEST_STR_LEN                  12307

// If you want use a rand function of standard library set to 1.
#define DATAFLASH_USE_RAND_FUNC                     0
/* \} */

/*
 * Kfile structure to test a dataflash.
 */
static Serial spi_fd;
static DataFlash dflash_fd;

/*
 * Define tmp buffer to stora data for
 * write and read flash memory test.
 */
static uint8_t test_buf[DATAFLASH_TEST_STR_LEN];
static uint8_t save_buf[DATAFLASH_TEST_STR_LEN];

/**
 * Setup all needed to test dataflash memory
 *
 */
int dataflash_testSetup(void)
{
        kfile_testSetup();
        LOG_INFO("KFILE setup..ok\n");

        LOG_INFO("Check if kernel is enable (if enable you should see the assert message.)\n");
        SILENT_ASSERT("bertos/drv/dataflash_test.c:119: Assertion failed: !CONFIG_KERN");
        ASSERT(!CONFIG_KERN);

        /*
         * This test use a kfile_test module,
         * so should include source in your makefile.
         */
        MOD_CHECK(kfile_test);

        timer_init();
        LOG_INFO("Timer init..ok\n");

        /*
         * Init SPI module and dataflash driver.
         */
        // Open SPI comunication channel
        spimaster_init(&spi_fd, 0);
        LOG_INFO("SPI0 init..ok\n");

        ser_setbaudrate(&spi_fd, 5000000UL);
        LOG_INFO("SPI0 set baudrate..ok\n");

        //Init dataflash memory
        dataflash_hw_init();
        LOG_INFO("DATAFLASH HW..ok\n");

        if (dataflash_init(&dflash_fd, &spi_fd.fd, DATAFLASH_MEM_MODEL, DATAFLASH_FUNC_CS_SET, DATAFLASH_FUNC_RESET))
                LOG_INFO("DATAFLASH init..ok\n");
        else
                LOG_ERR("DATAFLASH init..fail\n");


        //Fill tmp buffer with rand chars.
        for (int i = 0; i < DATAFLASH_TEST_STR_LEN; i++)
        {
                #if DATAFLASH_USE_RAND_FUNC
                        #include <stdlib.h> //Rand()

                        test_buf[i] = (uint8_t)rand();
                #else
                        test_buf[i] = (i & 0xff);
                #endif
        }

        LOG_INFO("Fill tmp buff..ok\n");

	return 0;
}


/**
 * Run dataflash test memory
 *
 */
int dataflash_testRun(void)
{
        LOG_INFO("Run KFILE test.\n");

        SILENT_ASSERT("bertos/drv/dataflash.c:405: Assertion failed: fd->fd.seek_pos + size <= fd->fd.size");
        if (kfile_testRunGeneric(&dflash_fd.fd, test_buf, save_buf, sizeof(test_buf)) != EOF)
        {
                LOG_INFO("KFILE test..ok\n");
        }
        else
        {
                LOG_ERR("KFILE test..fail!\n");
                return EOF;
        }

        return 0;
}

/**
 * End a dataflash Test.
 * (Unused)
 */
int dataflash_testTearDown(void)
{
	/*    */
	return 0;
}

/*
 * Empty main.
 *
 * Look it as exmple, or use it if
 * you want test a data flash driver stand alone.
 */
#if 0
int main(void)
{
	IRQ_ENABLE;
	kdbg_init();

	#if CONFIG_KERN
	proc_init();
    #endif

	if (!dataflash_testSetup())
	{
			LOG_INFO("DATAFLASH setup..ok\n");
	}
	else
	{
			LOG_ERR("DATAFLASH setup..fail!\n");
			return EOF;
	}

	dataflash_testRun();

	for(;;)
	{
	}
}
#endif
