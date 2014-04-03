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
 *
 * -->
 *
 * \brief Simple notifier for the subject/observer pattern (interface)
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#ifndef MWARE_OBSERVER_H
#define MWARE_OBSERVER_H

#include <struct/list.h>

/**
 *
 * Here's a simple example:
 *
 * \code
 * Subject kbd_driver;
 *
 * Observer kbd_observer;
 *
 * void key_pressed(int event, void *_param)
 * {
 *     char *param = (char *)_param;
 *
 *     if (event == EVENT_KBD_PRESSED)
 *         printf("You pressed %c\n", *param);
 * }
 *
 * void register_kbd_listener(void)
 * {
 *     observer_SetEvent(&kbd_observer, key_pressed);
 *     observer_Subscribe(&kbd_driver, &kbd_observer);
 * }
 * \endcode
 */
typedef struct Observer
{
	Node link;
	void (*event)(int event_id, void *param);
} Observer;

typedef struct Subject
{
	/// Subscribed observers.
	List observers;

} Subject;

void observer_SetEvent(Observer *observer, void (*event)(int event_id, void *param));

#define OBSERVER_INITIALIZER(callback) { { NULL, NULL }, callback }

void observer_InitSubject(Subject *subject);

/// Aggiunge un Observer all'insieme
void observer_Subscribe(Subject *subject, Observer *observer);

/// Rimuove un Observer dall'insieme
void observer_Unsubscribe(Subject *subject, Observer *observer);

/// per tutti gli elementi nel set notifica l'evento, chiamando la relativa
/// funzione event
void observer_notify(Subject *subject, int event_id, void *param);

#endif /* MWARE_OBSERVER_H */
