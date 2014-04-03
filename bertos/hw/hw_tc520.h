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
 * Copyright 2005, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 *
 * \brief TC520 hardware-specific definitions
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_TC520_H
#define HW_TC520_H

#include <cfg/compiler.h>

#warning TODO:This is an example implentation, you must implement it!

#define CE_PIN      0
#define DV_PIN      0
#define LOAD_PIN    0
#define READ_PIN    0


#define DV_HIGH()     /* Implement me! */
#define DV_LOW()      /* Implement me! */

#define CE_HIGH()     /* Implement me! */
#define CE_LOW()      /* Implement me! */

#define LOAD_HIGH()   /* Implement me! */
#define LOAD_LOW()    /* Implement me! */

#define READ_HIGH()   /* Implement me! */
#define READ_LOW()    /* Implement me! */

#define TC520_HW_INIT \
do\
{\
 /* Put here your code to init hardware */ \
} while(0)

#endif /* HW_TC520_H */
