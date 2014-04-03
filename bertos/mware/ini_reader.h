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
 *
 * -->
 *
 * \defgroup ini_reader Ini file reader
 * \ingroup mware
 * \{
 *
 * \brief Ini file reader module.
 *
 * The format accepted is:
 * - Sections must begin at beginning of line. [ Long name ] will be found only if " Long name " is specified as section name.
 * - key can contain any spaces at the beginning and before '=' but not in the middle. Eg. "long key name" is not valid.
 * - values will be stripped of spaces at the beginning and will run until end-of-line. Eg. "=    long value" will be treated as "long value".
 * - no nested sections are allowed.
 * - no comments are allowed inside a line with key=value pair.
 * - every line that doesn't contain a '=' or doesn't start with '[' will be ignored.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * $WIZ$ module_name = "ini_reader"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_ini_reader.h"
 * $WIZ$ module_depends = "kfile"
 */

#ifndef INI_READER_H
#define INI_READER_H

#include <io/kfile.h>

/**
 * \brief Returns the value for the given string in char* format.
 * Reads the whole input file looking for section and key and fills the provided buffer with
 * the corresponding value.
 * On errors, the function fills the provided buffer with the default value and returns EOF.
 * \param fd An initialized KFile structure.
 * \param section The section to be looked for.
 * \param key The key to search for.
 * \param default_value The default value.
 * \param buf The buffer to be filled.
 * \param size The size of the provided buffer.
 * \return 0 if section and key were found, EOF on errors.
 */
int ini_getString(KFile *fd, const char *section, const char *key, const char *default_value, char *buf, size_t size);

int ini_reader_testSetup(void);
int ini_reader_testRun(void);
int ini_reader_testTearDown(void);

/** \} */ // defgroup ini_reader
#endif /* INI_READER_H */
