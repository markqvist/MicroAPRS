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
 * Copyright 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Hardware-specific definitions
 *
 *
 * \author Francesco Sacchi <bernie@codewiz.org>
 */

#ifndef HW_CPUFREQ_H
#define HW_CPUFREQ_H

#ifndef CPU_FREQ
	#warning CPU_FREQ is not defined, you should upgrade to the newer clock frequency defining method.

	/*
	 * This file is DEPRECATED, it will be removed in the next major release.
	 * We have set up a new cpu frequency definition method.
	 * The new macro CPU_FREQ should be defined as a compiler flag in the
	 * makefile instead of the old CLOCK_FREQ macro. With new projects you should only
	 * use the CPU_FREQ macro.
	 *
	 * With gcc you should add something like this:
	 *
	 * -D'CPU_FREQ=(12288000UL)'
	 *
	 * For backward compatibility the old method is still supported.
	 */
	#include "hw/hw_cpu.h"

	#define CPU_FREQ (CLOCK_FREQ)
#endif /* CPU_FREQ */

#endif /*  HW_CPUFREQ_H */
