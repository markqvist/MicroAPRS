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
 * Copyright 2003, 2004, 2006, 2008, 2009 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \brief Macro for HW_SIPO_H
 *
 *
 *
 * \author Andrea Grandi <andrea@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_SIPO_H
#define HW_SIPO_H

/**
 * Map sipo connection on board.
 */
typedef enum SipoMap
{
	SIPO_CNT
} SipoMap;

/**
 * Define generic macro to set pins logic level
 */
#define SIPO_SET_LEVEL_LOW(dev)    do { /* Implement me! */   } while (0)
#define SIPO_SET_LEVEL_HIGH(dev)   do { /* Implement me! */   } while (0)


/**
 * Generate one low pulse on select line.
 */
#define PULSE_LOW(dev)    do { /* Implement me! */   } while (0)

/**
 * Generate one hight pulse on select line.
 */
#define PULSE_HIGH(dev)     do { /* Implement me! */   } while (0)


/**
 * Define the procedure to drive serial input in sipo device (SI).
 */
#define SIPO_SI_HIGH()   do { /* Implement me! */   } while (0)
#define SIPO_SI_LOW()    do { /* Implement me! */   } while (0)

/**
 * Drive clock to shift SI data into latch.
 */
#define SIPO_SI_CLOCK(clk_pol) \
	do { \
		(void)clk_pol; \
		/* Implement me! */ \
	} while (0)

/**
 * Do everything needed in order to load dato into sipo.
 */
#define SIPO_LOAD(device, load_pol)   do { /* Implement me! */   } while (0)

/**
 * Enable the shift register output.
 */
#define SIPO_ENABLE()             do { /* Implement me! */   } while (0)

/**
 * Set polarity for Load, Clk, SI signals.
 */
#define SIPO_SET_LD_LEVEL(device, load_pol)  do { /* Implement me! */   } while (0)
#define SIPO_SET_CLK_LEVEL(clock_pol)        do { /* Implement me! */   } while (0)
#define SIPO_SET_SI_LEVEL()       do { /* Implement me! */   } while (0)

/**
 * Do anything that needed to init sipo pins.
 */
#define SIPO_INIT_PIN()           do { /* Implement me! */   } while (0)

#endif /* HW_SIPO_H */
