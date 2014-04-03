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
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief QT-based widget for leyboard emulation (interface)
 */

#if !defined(EMULKBD_H)
#define EMULKBD_H

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#include <QtGui/QFrame>

// fwd decl
class QGridLayout;
class EmulKey;

class EmulKbd : public QFrame
{
	Q_OBJECT

// Data members
protected:
	QGridLayout *layout;
	int frame_width;
	int active_row;

// Construction
public:
	EmulKbd(QWidget *parent = 0, Qt::WFlags f = 0);
	virtual ~EmulKbd();

// Public methods
	void addKey(const char *label, int keycode, int row, int col, int matrix_row = -1, int matrix_col = -1);
	void setRow(int row);
	int readCols(void);

// Protected methods
protected:
	void setKey(int row, int col, bool on);

// Base class overrides
protected:
	virtual QSizePolicy sizePolicy() const;
	virtual void resizeEvent(QResizeEvent *e);
	virtual bool event(QEvent *e);

// Friends
	friend class EmulKey;
};


// Private helper class for EmulKbd
// NOTE: with protected inheritance, dynamic_cast<> does not work (gcc 2.96)
#include <QtGui/qpushbutton.h>
class EmulKey : public QPushButton
{
	Q_OBJECT

// Data members
protected:
	int row, col;
	int keycode;

// Construction
public:
	EmulKey(EmulKbd *parent, const char *label, int keycode, int _row, int _col);
	virtual ~EmulKey();

// superclass overrides
	void setDown(bool enable);

protected slots:
	void keyPressed(void);
	void keyReleased(void);

// Friends
public:
	friend class EmulKbd;
};

#endif // !defined(EMULKBD_H)

