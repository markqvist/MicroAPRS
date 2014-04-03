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
 * \brief QT-based widget for keyboard emulation (implementation)
 */

#include "emulkbd.h"
#include "emul.h"

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QSizePolicy>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtCore/QEvent>
#include <QtCore/QSize>
#include <QtCore/QRect>


EmulKey::EmulKey(EmulKbd *kbd, const char *label, int _keycode, int _row, int _col) :
	QPushButton(label, kbd),
	row(_row), col(_col),
	keycode(_keycode)
{
	// don't let the widget get focus
	setFocusPolicy(Qt::NoFocus);

	// unused
	connect(this, SIGNAL(pressed()), this, SLOT(keyPressed()));
	connect(this, SIGNAL(released()), this, SLOT(keyReleased()));
}


EmulKey::~EmulKey()
{
	// nop
}


/**
 * Override standad QButton behaviour: we must also emit the signals.
 */
// unused
void EmulKey::setDown(bool enable)
{
	// let our superclass do everything else
	QPushButton::setDown(enable);

	if (enable)
		emit pressed();
	else
		emit released();
}


// unused
void EmulKey::keyPressed(void)
{
	static_cast<EmulKbd *>(parent())->setKey(row, col, true);
}


// unused
void EmulKey::keyReleased(void)
{
	static_cast<EmulKbd *>(parent())->setKey(row, col, false);
}


EmulKbd::EmulKbd(QWidget *parent, Qt::WFlags f) :
	QFrame(parent, f),
	layout(new QGridLayout(this)),
	active_row(0)
{
	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);
	setFocusPolicy(Qt::StrongFocus);
	frame_width = frameWidth();
}


EmulKbd::~EmulKbd()
{
	delete layout;
}


QSizePolicy EmulKbd::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}


void EmulKbd::resizeEvent(QResizeEvent *event)
{
	// Let our superclass process the event first
	QFrame::resizeEvent(event);
}


// handle key presses for all keys in keyboard
bool EmulKbd::event(QEvent *_e)
{
	switch (_e->type())
	{
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		{
			QKeyEvent *e = static_cast<QKeyEvent *>(_e);
			int keycode = e->key();
			EmulKey *key;

			// ignore repeated keys
			if (!e->isAutoRepeat())
			{
				// scan all children
				for (QObjectList::const_iterator it(children().begin()); it != children().end(); ++it)
				{
					// only keys, not other children!
					if ((*it)->metaObject() == &EmulKey::staticMetaObject)
					// if ((key = dynamic_cast<EmulKey *>(*it)))
					{
						key = static_cast<EmulKey *>(*it);

						// same key?
						if (key->keycode == keycode)
						{
							// yes, tell key to go down (or up)
							key->setDown(_e->type() == QEvent::KeyPress);
							break;
						}
					}
				}
			}
			return true;
		}

		default:
			// let superclass process this event
			return QFrame::event(_e);

	} // end switch(_e->type())
}


void EmulKbd::addKey(const char *label, int keycode, int row, int col, int matrix_row, int matrix_col)
{
	if (matrix_row == -1)
		matrix_row = row;
	if (matrix_col == -1)
		matrix_col = col;

	layout->addWidget(new EmulKey(this, label, keycode, matrix_row, matrix_col), row, col);
}


// unused
void EmulKbd::setKey(int /*row*/, int /*col*/, bool /*on*/)
{
}


void EmulKbd::setRow(int r)
{
	active_row = r;
}

int EmulKbd::readCols(void)
{
	QLayoutItem *item;
	EmulKey *key;
	int cols = 0;

	for (int i = 0; (item = layout->itemAt(i)); ++i)
	{
		key = static_cast<EmulKey *>(item->widget());
		if (key->row == active_row)
		{
			if (key->isDown())
				cols |= (1<<key->col);
		}
	}
	return cols;
}

extern "C" void emul_kbdSetRows(int r)
{
	emul->emulKbd->setRow(r);
}


extern "C" int emul_kbdReadCols(void)
{
	return emul->emulKbd->readCols();
}

#include "emulkbd_moc.cpp"
