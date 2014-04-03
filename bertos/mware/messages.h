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
 * Copyright 2003, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Definitions of constant string messages.
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef MWARE_MESSAGES_H
#define MWARE_MESSAGES_H

enum
{
	MSG_NULL,

	// TODO: add your labels here.

	MSG_COUNT
};

#warning FIXME:Revise me!

#define MSG_BUFSIZE 6144 /* FIXME: how much? */

/* String tables */
/* extern const char *msg_strings const [MSG_COUNT]; */
/* extern char msg_buf[MSG_BUFSIZE]; */


/* Macros to access translated messages */
#define MSG(x)     msg_strings[x]
#define PTRMSG(x)  ((x) < (const_iptr_t)256 ? msg_strings[(unsigned int)(x)] : (const char *)(x))


#endif /* MWARE_MESSAGES_H */

