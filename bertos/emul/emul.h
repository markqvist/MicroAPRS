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
 * Copyright 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Qt-based emulator framework for embedded applications (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/02/15 09:11:17  bernie
 *#* Add keyboard emulator.
 *#*
 *#* Revision 1.3  2006/01/23 23:12:08  bernie
 *#* Let Doxygen see through C++ protected section.
 *#*
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#ifndef EMUL_EMUL_H
#define EMUL_EMUL_H

#include <cfg/compiler.h>

#if defined(__cplusplus) || defined(__doxygen__)

// fwd decls
class QApplication;
class EmulWin;
class EmulPRT;
class EmulLCD;
class EmulKbd;
class QCheckBox;
class QSlider;
class QLabel;

class Emulator
{
// data members
public:
	QApplication  *emulApp; ///< QT Application.
	EmulWin       *emulWin; ///< Main window.

	EmulLCD       *emulLCD; ///< Display emulator.
	EmulKbd       *emulKbd; ///< Keyboard emulator.

// construction
	Emulator(int &argc, char **argv);
	~Emulator();

// public methods
	void quit();
};

extern Emulator *emul;

#endif /* __cplusplus */

EXTERN_C void emul_init(int *argc, char *argv[]);
EXTERN_C void emul_cleanup();
EXTERN_C void emul_idle();

#endif /* EMUL_EMUL_H */

