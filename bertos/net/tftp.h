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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TFTP protocol implementation
 *
 * This module implements a TFTP server with a handy KFile interface.
 * The call to tftp_listen() will block until a client tries to connect,
 * then it returns a KFile.
 * If you want to accept the transfer, start reading from the handle and
 * the transfer will continue. Otherwise, don't access it and immediately
 * call kfile_close().
 * Close the KFile when you're done.
 *
 * The usage pattern is as follows:
 * \code
 * // initialize a TFTP session
 * tftp_init();
 * // start listening for connections
 * KFile *f = tftp_listen();
 * // now you can check the returned file name
 * // and change behaviour at runtime
 * if (!strcmp(filename, "firmware.bin"))
 *     // do something
 * else if (!strcmp(filename, "conf.ini"))
 *     // update the configuration
 * kfile_close(f);
 * \endcode
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * $WIZ$ module_name = "tftp"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_tftp.h"
 * $WIZ$ module_depends = "lwip", "kfile"
 */

#ifndef TFTP_H
#define TFTP_H

#include <cfg/compiler.h>
#include <lwip/sockets.h> // sockaddr_in, socklen_t
#include <io/kfile.h>

#define TFTP_RRQ     0x0100     /* TFTP read request packet (already in net endianess). */
#define TFTP_WRQ     0x0200     /* TFTP write request packet (already in net endianess). */
#define TFTP_DATA    03         /* TFTP data packet. */
#define TFTP_ACK     0x0400     /* TFTP acknowledgement packet (already in net endianess). */
#define TFTP_PROTOERR     0x0500     /* TFTP acknowledgement packet (already in net endianess). */

/* TFTP protocol error codes */
#define TFTP_PROTOERR_ACCESS_VIOLATION 0x0200

#define TFTP_SERVER_PORT 69

/* Return error codes */
#define TFTP_ERR_TIMEOUT -2
#define TFTP_ERR         -1

struct PACKED TftpHeader
{
	short opcode;            ///< packet type */
	union PACKED
	{
		short block;         /* block # */
		short code;          /* error code */
		char stuff[1];       /* request packet stuff */
	} th_u;
};

typedef struct PACKED Tftpframe {
	struct TftpHeader hdr;
	char data[512];          /* data or error string */
} Tftpframe;

struct PACKED ackframe
{
	short opcode;
	short block_num;
};

struct PACKED errframe
{
	short opcode;
	short errcode;
	char str;
};

typedef enum
{
	TFTP_READ,
	TFTP_WRITE,
} TftpOpenMode;

typedef struct TftpSession
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int sock;
	unsigned short block;
	mtime_t timeout;
	int error;
	Tftpframe frame;
	size_t bytes_available;
	size_t valid_data;
	bool is_xfer_end;
	bool pending_ack;
	KFile kfile_request;
} TftpSession;

int tftp_init(TftpSession *ctx, unsigned short port, mtime_t timeout);
KFile *tftp_listen(TftpSession *ctx, char *filename, size_t len, TftpOpenMode *mode);

#endif // TFTP_H
