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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Utility for the test suite.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * When you want to test a module that is emulable on hosted
 * platforms, these macros come in handy.
 * Your module_test should supply three basic functions:
 *
 * \code
 * int module_testSetup(void)
 * int module_testRun(void)
 * int module_testTearDown(void)
 * \endcode
 *
 * All of these should return 0 if ok or a value != 0 on errors.
 *
 * Then, at the end of your module_test you can write:
 * \code
 * #if UNIT_TEST
 *	#include <whatuneed.h>
 *	#include <whatuneed.c>
 *	#include <...>
 *
 *	TEST_MAIN(module);
 * #endif
 * \endcode
 *
 * Including directly into your module the file.c you need to
 * run the test allows you to build and run the test compiling
 * only one file.
 *
 * To achieve this you also need a main() that is supplied by
 * the TEST_MAIN macro.
 * This will expand to a full main that calls, in sequence:
 * Setup, Run and TearDown of your module.
 */

#ifndef CFG_TEST_H
#define CFG_TEST_H

#include "cfg/cfg_arch.h"

#if defined(ARCH_UNITTEST) && (ARCH & ARCH_UNITTEST)
	#define UNIT_TEST 1

	/**
	 * Macro used to generate a main() for a test to be compiled
	 * on hosted platform.
	 */
	#define TEST_MAIN(module) \
	int main(void) \
	{ \
		if (module##_testSetup() != 0) \
			return 1; \
		if (module##_testRun() != 0) \
			return 2; \
		if (module##_testTearDown() != 0) \
			return 3; \
		return 0; \
	}
#else /* !TEST */
	#define UNIT_TEST 0

	#define TEST_MAIN(module) /* nothing */
#endif /* TEST */


/**
 * Silent an assert in a test.
 *
 * This is useful when we run a test and we want to test
 * an error condition. We know that an assert will fail but
 * this is not really an error. To ignore that we mark it
 * with this macro, where str is the message string of the assert
 * that we want to drop.
 * To use this macro copy the assert log message and paste as argument
 * of this macro. In assert message log is reported also the line number
 * of the code that have generated the assert.
 * In this way you can trap only the selected assert message.
 */
#define SILENT_ASSERT(str) kputs("SILENT_ASSERT:$"str"$\n")

#endif /* CFG_TEST_H */
