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
 * \brief Configuration file Ramp algorithm module.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_RAMP_H
#define CFG_RAMP_H

/**
 * Define whether the ramp will use floating point calculation within ramp_evaluate().
 * Otherwise, a less precise fixed point version will be used, which is faster on
 * platforms which do no support floating point operations.
 *
 * \note Floating point operations will be always done within ramp_compute() to
 * precalculate values, so there has to be at least a floating point emulation support.
 *
 * $WIZ$ type = "boolean"
 */
#define RAMP_USE_FLOATING_POINT   0


#if !RAMP_USE_FLOATING_POINT

	/**
	 * Number of least-significant bits which are stripped away during ramp evaluation.
	 * This setting allows to specify larger ramps at the price of less precision.
	 *
	 * The maximum ramp size allowed is 2^(24 + RAMP_CLOCK_SHIFT_PRECISION), in clocks.
	 * For instance, using RAMP_CLOCK_SHIFT_PRECISION 1, and a 8x prescaler, the maximum
	 * length of a ramp is about 6.7 secs. Raising RAMP_CLOCK_SHIFT_PRECISION to 2
	 * brings the maximum length to 13.4 secs, at the price of less precision.
	 *
	 * ramp_compute() will check that the length is below the maximum allowed through
	 * a runtime assertion.
	 *
	 * \note This macro is used only for the fixed-point version of the ramp.
	 * $WIZ$ type = "int"
	 * $WIZ$ min = 0
	 * $WIZ$ max = 32
	 */
	#define RAMP_CLOCK_SHIFT_PRECISION 2
#endif


/**
* Negative pulse width for ramp.
* $WIZ$ type = "int"
* $WIZ$ min = 1
*/
#define RAMP_PULSE_WIDTH    50

/**
 * Default ramp time (microsecs).
 * $WIZ$ type = "int"
 * $WIZ$ min = 1000
 */
#define RAMP_DEF_TIME     6000000UL
/**
 * Default ramp maxfreq (Hz).
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define RAMP_DEF_MAXFREQ       5000
/**
 * Default ramp minfreq (Hz).
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define RAMP_DEF_MINFREQ        200
/**
 * Default ramp powerrun (deciampere).
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 */
#define RAMP_DEF_POWERRUN        10
/**
 * Default ramp poweridle (Hz).
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 */
#define RAMP_DEF_POWERIDLE        1

#endif /* CFG_RAMP_H */
