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
 * \brief Main Qt window for embedded applications emulator (implementation)
 */

#include "emulwin.h"

#include <drv/lcd_gfx_qt.h>
#include <emul/emul.h>
#include <emul/emulkbd.h>

#include <cassert>

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QCheckBox>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
using namespace Qt;

EmulWin::EmulWin(Emulator *e)
{
	setWindowTitle(tr("BeRTOS Emul Demo"));
	setAttribute(Qt::WA_DeleteOnClose);

	// Create the menu bar
	QMenu *file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(tr("&Quit"),
		e->emulApp, SLOT(closeAllWindows()), CTRL+Key_Q);

	menuBar()->addSeparator();

	QMenu *help_menu = menuBar()->addMenu(tr("&Help"));
	help_menu->addAction(tr("&About"),
		this, SLOT(about()), Key_F1);

	// Make a central widget to contain the other widgets
	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	// Create a layout to position the widgets
	QHBoxLayout *box_main = new QHBoxLayout(central);

	// Main layout
	QVBoxLayout *box_right = new QVBoxLayout();
	box_main->addLayout(box_right);

		// LCD
		QHBoxLayout *lay_lcd = new QHBoxLayout();
		box_right->addLayout(lay_lcd);
			lay_lcd->addStretch(1);
			lay_lcd->addWidget(e->emulLCD = new EmulLCD(central), 8);
			lay_lcd->addStretch(1);

		// Keyboard
		box_right->addWidget(e->emulKbd = new EmulKbd(central));

	// Setup keyboard: Label   Keycode     Row Col MRow MCol
	e->emulKbd->addKey("^",    Key_Up,     0,  0,  0,   0);
	e->emulKbd->addKey("v",    Key_Down,   1,  0,  0,   1);
	e->emulKbd->addKey("OK",   Key_Return, 0,  1,  0,   2);
	e->emulKbd->addKey("ESC",  Key_Escape, 1,  1,  0,   3);
}


EmulWin::~EmulWin()
{
	emul->quit();
}


void EmulWin::closeEvent(QCloseEvent *ce)
{
	emul->quit();
	ce->accept();
}


void EmulWin::about()
{
    QMessageBox::about(this,
		"BeRTOS Embedded Application Emulator",
		"Version 0.1\n"
		"Copyright 2006, 2008 Develer S.r.l. (http://www.develer.com/)\n"
		"Copyright 2001, 2002, 2003, 2005 Bernie Innocenti <bernie@codewiz.org>\n"
		"All rights reserved."
	);
}

#include "emulwin_moc.cpp"
