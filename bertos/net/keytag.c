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
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief KeyTAG parser.
 *
 * This module parse TAG message that come from comunication channel,
 * and convert the tag value into string.
 *
 * TAG protocol is decribed in this way:
 * <pre>
 * ---------------------------------------------------
 * |STX (0x02)|data...(10 HEX chars)|CR|LF|ETX (0x03)|
 * ---------------------------------------------------
 * </pre>
 */

#include "keytag.h"

#include <cfg/debug.h>
// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_KEYTAG_LOG_LEVEL
#define LOG_FORMAT   CONFIG_KEYTAG_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/macros.h>

#include <io/kfile.h>

#include <string.h>
/**
 * Starting communication char (STX).
 */
#define TAG_STX 0x02

/**
 * Ending communication char (ETX).
 */
#define TAG_ETX 0x03

static void keytag_clearPkt(struct TagPacket *pkt)
{
	pkt->sync = false;
	pkt->len = 0;
}

/**
 * DEPRECATED FUCNTIONS
 * To read the tag string from device you shoul use the keytag_recv
 * fuction, that return the string if we had received it.
 */
void keytag_poll(struct TagPacket *pkt)
{
	uint8_t buf[CONFIG_TAG_MAX_LEN];
	int len;
	if ((len = keytag_recv(pkt, buf, sizeof(buf))) != EOF)
		kfile_write(pkt->host, buf, len);
}

/**
 * Receive the tag message from channel, and if
 * the tag is good put the converted string into given buffer.
 * The fuction return the len of found tag string, otherwise EOF.
 */
int keytag_recv(struct TagPacket *pkt, uint8_t *tag, size_t len)
{
	int c;

	/* Get all chars from buffer */
	while ((c = kfile_getc(pkt->tag)) != EOF)
	{
		/* Search for STX char in received chars */
		if (c == TAG_STX)
		{
			/* When STX is found a new packet begins */
			if (pkt->sync)
				LOG_WARN("TAG double sync!\n");
			keytag_clearPkt(pkt);
			pkt->sync = true;
		}
		else if (pkt->sync)
		{
			/* Check for end of packet */
			if (c == TAG_ETX)
			{
				/* Terminate the tag string */
				size_t tag_len = MIN(len, pkt->len);

				/* Save read tag */
				memcpy(tag, pkt->buf, tag_len);
				pkt->sync = false;
				return tag_len;
			}
			else
			{
				/* Check for buffer overflow */
				if (pkt->len >= CONFIG_TAG_MAX_LEN)
				{
					LOG_ERR("TAG buffer overflow\n");
					pkt->sync = false;
				}
				else
				{
					/* Add every char after STX to tag reading buffer */
					if (pkt->sync)
					{
						pkt->buf[pkt->len] = c;
						pkt->len++;
					}
				}
			}
		}
	}
	if (kfile_error(pkt->tag) != 0)
	{
		LOG_ERR("Error %04x\n", kfile_error(pkt->tag));
		kfile_clearerr(pkt->tag);
	}

	return EOF;
}

/**
 * Init the keytag module.
 */
void keytag_init(struct TagPacket *pkt, struct KFile *comm, struct KFile *tag)
{
	keytag_clearPkt(pkt);
	pkt->tag = tag;
	pkt->host = comm;
}

