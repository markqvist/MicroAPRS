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
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief C++-like structured exception handling for C programs
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef MWARE_EXCEPT_H
#define MWARE_EXCEPT_H

#include <cfg/debug.h>

#include <setjmp.h>

#define EXCEPT_CONTEXTS 8

/**
 * A stack of jump buffers used to record try sites
 * so they can be reached from throw sites.
 *
 * The stack contains return points for each nested
 * context. jmp_buf's are pushed into the stack at
 * try points and popped out when the try block ends
 * normally or when an exception is thrown.
 */
extern jmp_buf except_stack[EXCEPT_CONTEXTS];
extern int except_top;

#define PUSH_EXCEPT	(ASSERT(except_top < EXCEPT_CONTEXTS), setjmp(except_stack[except_top++]))
#define POP_EXCEPT	(ASSERT(except_top > 0), --except_top)
#define DO_EXCEPT	(ASSERT(except_top > 0), longjmp(except_stack[--except_top], true))

/**
 * Jump buffer to use when throwing an exception or aborting an operation
 *
 * User code can throw exceptions like this:
 *
 * \code
 *   void a_function_throwing_exceptions(void)
 *   {
 *       if (some_error_condition)
 *          THROW;
 *   }
 * \endcode
 *
 * Catching exceptions (brackets are optional):
 *
 * \code
 *    EXCEPT_DEFINE;
 *
 *    void a_function_catching_an_exception(void)
 *    {
 *        TRY
 *        {
 *            printf("Entered try block\n");
 *            a_function_throwing_exceptions();
 *            printf("Survived execution of critical code\n");
 *        }
 *        CATCH
 *        {
 *            printf("Exception caught!\n");
 *        }
 *        CATCH_END
 *    }
 * \endcode
 *
 * Simple syntax when you don't need to do anything when catching an excaption:
 *
 * \code
 *    TRY
 *        printf("Entered try block\n");
 *        a_function_throwing_exceptions();
 *        printf("Survived execution of critical code\n");
 *    TRY_END
 * \endcode
 *
 * You also need to declare the exception stack once in
 * your global declarations:
 * \code
 *    EXCEPT_DEFINE;
 * \endcode
 */
#define TRY          if (PUSH_EXCEPT) { {
#define TRY_END      } POP_EXCEPT; }
#define CATCH        } POP_EXCEPT; } else {
#define CATCH_END    }
#define THROW        DO_EXCEPT


#define EXCEPT_DEFINE \
	jmp_buf except_stack[EXCEPT_CONTEXTS]; \
	int except_top;

#endif /* MWARE_EXCEPT_H */
