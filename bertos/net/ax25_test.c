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
 * \brief AX25 test.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "ax25.h"

#include <struct/kfile_mem.h>

#include <cfg/debug.h>
#include <cfg/kfile_debug.h>
#include <cfg/test.h>

#include <string.h> //strncmp

static AX25Ctx ax25;
static KFileMem mem;
static KFileDebug dbg;

#define APRS_MSG \
	0x3D, 0x34, 0x36, 0x30, 0x33, 0x2E, 0x36, 0x33, \
	0x4E, 0x2F, 0x30, 0x31, 0x34, 0x33, 0x31, 0x2E, \
	0x32, 0x36, 0x45, 0x2D, 0x4F, 0x70, 0x2E, 0x20, \
	0x41, 0x6E, 0x64, 0x72, 0x65, 0x6A

uint8_t aprs_packet[] =
{
	HDLC_FLAG,
	0x82, 0xA0, 0xA4, 0xA6, 0x40, 0x40, 0xE0, /* dst */
	0xA6, 0x6A, 0x6E, 0x98, 0x9C, 0x40, 0x61, /* src */
	0x03, /* ctrl */
	0xF0, /* pid */
	APRS_MSG, /* payload */
	0x40, 0x65, /* CRC */
	HDLC_FLAG,
};

uint8_t buf[] = { APRS_MSG };
KFileMem mem1;
uint8_t aprs_packet_check[256];


static void msg_callback(AX25Msg *msg)
{
	ax25_print(&dbg.fd, msg);
	ASSERT(strncmp(msg->dst.call, "APRS\x0\x0", 6) == 0);
	ASSERT(strncmp(msg->src.call, "S57LN\x0", 6) == 0);
	ASSERT(msg->src.ssid == 0);
	ASSERT(msg->dst.ssid == 0);
	ASSERT(msg->ctrl == AX25_CTRL_UI);
	ASSERT(msg->pid == AX25_PID_NOLAYER3);
	ASSERT(msg->len == 30);
	ASSERT(strncmp((const char *)msg->info, "=4603.63N/01431.26E-Op. Andrej", 30) == 0);
}

int ax25_testSetup(void)
{
	kdbg_init();
	kfiledebug_init(&dbg);
	kfilemem_init(&mem, aprs_packet, sizeof(aprs_packet));
	kfilemem_init(&mem1, aprs_packet_check, sizeof(aprs_packet_check));
	ax25_init(&ax25, &mem.fd, msg_callback);
	return 0;
}

int ax25_testTearDown(void)
{
	return 0;
}

int ax25_testRun(void)
{
	ax25_poll(&ax25);
	ax25_init(&ax25, &mem1.fd, NULL);
	ax25_send(&ax25, AX25_CALL("aprs", 0x70), AX25_CALL("s57ln", 0x30), buf, sizeof(buf));
	ASSERT(memcmp(aprs_packet, aprs_packet_check, sizeof(aprs_packet)) == 0);
	return  0;
}

TEST_MAIN(ax25);
