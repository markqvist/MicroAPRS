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
 * All Rights Reserved.
 * -->
 *
 * \brief Hardware macro definition.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_SPI_H
#define HW_SPI_H

#warning TODO:This is an example implentation, you must implement it!

#include <cfg/macros.h>

/**
 * SPI pin definition.
 *
 * \note CS is assert when level
 * is low.
 *
 * \{
 */
#define CS       /* pin */   ///Connect to CS pin of Flash memory.
#define SCK      /* pin */   ///Connect to SCK pin of Flash memory.
#define MOSI     /* pin */   ///Connect to SI pin of Flash memory.
#define MISO     /* pin */   ///Connect to SO pin of Flash memory.
#define SPI_PORT /* pin */   ///Micro pin PORT register.
#define SPI_PIN  /* pin */   ///Micro pin PIN register.
#define SPI_DDR  /* pin */   ///Micro pin DDR register.
/*\}*/

/**
 * Pin logic level.
 *
 * \{
 */
#define MOSI_LOW()       do { /* Implement me! */ } while(0)
#define MOSI_HIGH()      do { /* Implement me! */ } while(0)
#define MISO_HIGH()      do { /* Implement me! */ } while(0)
#define SCK_LOW()        do { /* Implement me! */ } while(0)
#define SCK_HIGH()       do { /* Implement me! */ } while(0)
#define CS_LOW()         do { /* Implement me! */ } while(0)
#define CS_HIGH()        do { /* Implement me! */ } while(0)
/*\}*/

/**
 * SPI pin commands.
 *
 * \{
 */
#define CS_ENABLE()      CS_LOW()
#define CS_DISABLE()     CS_HIGH()
#define SS_ACTIVE()      CS_LOW()
#define SS_INACTIVE()    CS_HIGH()
#define SCK_INACTIVE()   SCK_LOW()
#define SCK_ACTIVE()     SCK_HIGH()
#define CS_OUT()         do { /* Implement me! */ } while(0)
#define MOSI_IN()        do { /* Implement me! */ } while(0)
#define MOSI_OUT()       do { /* Implement me! */ } while(0)
#define IS_MISO_HIGH()	 (false /* Implement me! */ )
#define MISO_IN()        do { /* Implement me! */ } while(0)
#define MISO_OUT()       do { /* Implement me! */ } while(0)
#define SCK_OUT()        do { /* Implement me! */ } while(0)

#define SCK_PULSE()\
	do {\
			SCK_HIGH();\
			SCK_LOW();\
	} while (0)
/*\}*/


#define SPI_HW_INIT() \
	CS_DISABLE();\
	MOSI_LOW();\
	SCK_LOW();\
	MISO_IN();\
	MOSI_OUT();\
	SCK_OUT();\
	CS_OUT();

#endif /* HW_SPI_H */

