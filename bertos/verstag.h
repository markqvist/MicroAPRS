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
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 by Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Declare application version strings
 */


#ifndef BERTOS_VERSTAG_H
#define BERTOS_VERSTAG_H

#ifndef CFG_ARCH_CONFIG_H
	#include "cfg/cfg_arch.h"
#endif

#define APP_NAME "Appname"
#define APP_DESCRIPTION "Long application name description"
#define APP_AUTHOR "Develer"
#define APP_COPYRIGHT "Copyright 2009 Develer (http://www.develer.com/)"


#if ARCH & ARCH_DEFAULT
	#define VERS_MAJOR 0
	#define VERS_MINOR 1
	#define VERS_REV   0
	#define VERS_LETTER ""
#elif ARCH & ARCH_NIGHTTEST
	#define VERS_MAJOR 0
	#define VERS_MINOR 1
	#define VERS_REV   0
	#define VERS_LETTER ""
#else
	#error unknown architecture
#endif

/**
 * If _SNAPSHOT is defined, \c VERS_TAG contains the build date
 * date instead of a numeric version string.
 */
#define _SNAPSHOT

#ifdef _DEBUG
	#define VERS_DBG "D"
#else
	#define VERS_DBG ""
#endif

#define __STRINGIZE(x) #x
#define _STRINGIZE(x) __STRINGIZE(x)

/** Build application version string (i.e.: "1.7.0") */
#define MAKE_VERS(maj,min,rev) _STRINGIZE(maj) "." _STRINGIZE(min) "." _STRINGIZE(rev) VERS_LETTER VERS_DBG
#ifdef _SNAPSHOT
	#define VERS_TAG "snapshot" " " __DATE__ " " __TIME__ " " VERS_LETTER " " VERS_DBG
#else
	#define VERS_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)
#endif

/** Build application version string suitable for MS windows resource files (i.e.: "1, 7, 0, 1") */
#define MAKE_RCVERS(maj,min,rev,bld) _STRINGIZE(maj) ", " _STRINGIZE(min) ", " _STRINGIZE(rev) ", " _STRINGIZE(bld)
#define RCVERSION_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)

/** The revision string (contains VERS_TAG) */
extern const char vers_tag[];

/** Sequential build number (contains VERS_BUILD) */
extern const int vers_build_nr;
//extern const char vers_build_str[];

/** Hostname of the machine used to build this binary (contains VERS_HOST) */
extern const char vers_host[];

#endif /* BERTOS_VERSTAG_H */
