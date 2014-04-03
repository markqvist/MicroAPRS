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
 * \brief Main Qt window for embedded applications emulator (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/05/28 12:17:56  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.3  2006/02/20 02:00:39  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#ifndef EMUL_EMULWIN_H
#define EMUL_EMULWIN_H

#include <QtGui/QMainWindow>

// fwd decls
class Emulator;

class EmulWin : public QMainWindow
{
	Q_OBJECT

// construction
public:
	EmulWin(Emulator *emul);
	~EmulWin();

protected:
	void closeEvent(QCloseEvent *);

private slots:
	void about();
};

#endif // EMUL_EMULWIN_H

