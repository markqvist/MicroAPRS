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
 *
 * \brief Test kernel preemption.
 *
 * This testcase spawns TASKS parallel threads that runs for TIME seconds. They
 * continuously spin updating a global counter (one counter for each thread).
 *
 * At exit each thread checks if the others have been che chance to update
 * their own counter. If not, it means the preemption didn't occur and the
 * testcase returns an error message.
 *
 * Otherwise, if all the threads have been able to update their own counter it
 * means preemption successfully occurs, since there is no active sleep inside
 * each thread's implementation.
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PRI" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PRI 1" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_monitor.h $cfgdir/
 * $test$: sed -i "s/CONFIG_KERN_MONITOR 0/CONFIG_KERN_MONITOR 1/" $cfgdir/cfg_monitor.h
 * $test$: cp bertos/cfg/cfg_signal.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SIGNALS" >> $cfgdir/cfg_signal.h
 * $test$: echo "#define CONFIG_KERN_SIGNALS 1" >> $cfgdir/cfg_signal.h
 *
 * notest: all
 *
 */

#include "../proc_test.c"
