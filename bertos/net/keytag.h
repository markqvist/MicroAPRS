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
 *
 * -->
 *
 * \author Andrea Grandi <andrea@develer.com>
 *
 * \brief Tag protocol. (interface).
 *
 * $WIZ$ module_name = "keytag"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_keytag.h"
 * $WIZ$ module_depends = "kfile"
 * $WIZ$ module_hw = ""
 */

#ifndef NET_KEYTAG_H
#define NET_KEYTAG_H

#include "cfg/cfg_keytag.h"
#include <cfg/compiler.h>

#include <io/kfile.h>


/**
 * Structure of a Tag packet
 */
typedef struct TagPacket
{
	KFile *tag;                        ///< Tag communication channel
	KFile *host;                       ///< Host communication channel
	bool sync;                         ///< Status flag: true if we find an STX
	size_t len;                        ///< Packet lenght
	uint8_t buf[CONFIG_TAG_MAX_LEN];   ///< Reception buffer
} TagPacket;

void keytag_init(struct TagPacket *pkt, struct KFile *comm, struct KFile *tag);
int keytag_recv(struct TagPacket *pkt, uint8_t *tag, size_t len);

DEPRECATED void keytag_poll(struct TagPacket *pkt);

#endif /* NET_TAG_H */
