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
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 * notest:all
 */

#include "tftp.h"
#include "cfg/cfg_tftp.h"
#define LOG_LEVEL   TFTP_LOG_LEVEL
#define LOG_FORMAT  TFTP_LOG_FORMAT
#include <cfg/log.h>

//#include <lwip/in.h>
#include <lwip/inet.h>
#include <lwip/sockets.h>
#include <string.h> //memset

#define TFTP_PACKET_SIZE 516

#define DECLARE_TIMEOUT(name, timeout) \
	struct timeval name; \
	name.tv_sec = timeout / 1000; \
	name.tv_usec = (timeout % 1000) * 1000;

#define KFT_TFTPSESSION MAKE_ID('T', 'F', 'T', 'P')
INLINE TftpSession *TFTP_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_TFTPSESSION);
	return (TftpSession *)containerof(fd, TftpSession, kfile_request);
}

/*
 * Check if received data is correct and send ACK if ok.
 */
static int checkPacket(TftpSession *ctx, const Tftpframe *frame)
{
	LOG_INFO("Checking block %hd\n", ctx->block);
	if (ntohs(frame->hdr.opcode) != TFTP_DATA)
	{
		LOG_INFO("Opcode != TFTP_DATA (%hd != %d)\n", ntohs(frame->hdr.opcode), TFTP_DATA);
		return -1;
	}
	if (ntohs(frame->hdr.th_u.block) != ctx->block + 1)
		return -1;

	ctx->block++;
	// if everything was ok, send ACK
	// ACK is already in network order
	struct ackframe ack;
	ack.opcode = TFTP_ACK;
	ack.block_num = htons(ctx->block);
	ssize_t rc = lwip_sendto(ctx->sock, &ack, 4, 0, (struct sockaddr *)&ctx->addr, ctx->addr_len);
	if (rc == 4)
		return rc;
	else
		return -1;
}

/*
 * Return >0 if there's something to read in ctx, 0 on timeout, -1 on errors
 */
static int tftp_waitEvent(TftpSession *ctx, struct timeval *timeout)
{
	fd_set inset;
	FD_ZERO(&inset);
	FD_SET(ctx->sock, &inset);
	struct timeval tmp = *timeout;
	return lwip_select(ctx->sock + 1, &inset, NULL, NULL, &tmp);
}

/*
 * Read a block from TFTP.
 * \param size Must be exactly 516 bytes
 * \param timeout Time to wait the network connection, may be NULL to wait forever
 * \return Number of bytes read if success, TFTP_ERR_TIMEOUT on timeout, TFTP_ERR otherwise
 */
static ssize_t tftp_readPacket(TftpSession *ctx, Tftpframe *frame, mtime_t timeout)
{
	DECLARE_TIMEOUT(wait_tm, timeout);

	int res = tftp_waitEvent(ctx, &wait_tm);
	if (res == 0)
		return TFTP_ERR_TIMEOUT;
	if (res == -1)
		return TFTP_ERR;

	ssize_t rlen = lwip_recvfrom(ctx->sock, frame, sizeof(Tftpframe), 0, NULL, NULL);
	LOG_INFO("Received %zd bytes\n", rlen);
	if (rlen > 0 && (checkPacket(ctx, frame) > 0))
		return rlen;
	else
		return TFTP_ERR;
}

static size_t tftp_read(struct KFile *fd, void *buf, size_t size)
{
	TftpSession *fds = TFTP_CAST(fd);
	uint8_t *_buf = (uint8_t *) buf;
	size_t read_bytes = 0;
	size_t offset = fds->valid_data - fds->bytes_available;

	if (fds->pending_ack)
	{
		ASSERT(fds->block == 0);
		struct ackframe ack;
		ack.opcode = TFTP_ACK;
		ack.block_num = fds->block;
		lwip_sendto(fds->sock, &ack, 4, 0, (struct sockaddr *)&fds->addr, fds->addr_len);
		fds->pending_ack = false;
	}

	if (fds->bytes_available < size)
	{
		/* check if we were called again after an error */
		if (fds->bytes_available > 0)
		{
			memcpy(_buf, fds->frame.data + offset, fds->bytes_available);
			LOG_INFO("ba < size. Copied %zd bytes from offset %zd\n", fds->bytes_available, offset);
			/* adjust buf and size */
			_buf += fds->bytes_available;
			size -= fds->bytes_available;
			read_bytes += fds->bytes_available;
		}

		if (!fds->is_xfer_end)
		{
			LOG_INFO("Waiting for new TFTP packet\n");
			/* get more data, we can wait since the function is blocking */
			ssize_t rd = tftp_readPacket(fds, &fds->frame, fds->timeout);
			if (rd < 0)
			{
				fds->bytes_available = 0;
				fds->error = rd;
				return 0;
			}
			else
			{
				if (rd < TFTP_PACKET_SIZE)
				{
					fds->is_xfer_end = true;
					LOG_INFO("Received the last packet\n");
				}
				fds->bytes_available = (size_t)rd - sizeof(struct TftpHeader);
				fds->valid_data = fds->bytes_available;
				offset = 0;
			}
		}
		else
		{
			LOG_INFO("Transfer finished\n");
			fds->bytes_available -= fds->bytes_available;
			fds->valid_data = 0;
			return read_bytes;
		}
	}

	/* check how many bytes we need to copy */
	size_t res = MIN(fds->bytes_available, size);
	LOG_INFO("Copying %zd bytes from offset %zd\n", res, offset);
	memcpy(_buf, fds->frame.data + offset, res);
	fds->bytes_available -= res;
	read_bytes += res;
	return read_bytes;
}

static int tftp_error(struct KFile *fd)
{
	TftpSession *fds = TFTP_CAST(fd);
	return fds->error;
}

static void tftp_clearerr(struct KFile *fd)
{
	TftpSession *fds = TFTP_CAST(fd);
	fds->error = 0;
}

static int tftp_close(struct KFile *fd)
{
	TftpSession *fds = TFTP_CAST(fd);
	struct errframe err;
	if (fds->pending_ack)
	{
		err.opcode = TFTP_PROTOERR;
		err.errcode = TFTP_PROTOERR_ACCESS_VIOLATION;
		err.str = '\0';
		lwip_sendto(fds->sock, &err, 5, 0, (struct sockaddr *)&fds->addr, fds->addr_len);
		LOG_INFO("Closed connection upon user request\n");
	}
	return 0;
}

static void resetTftpState(TftpSession *ctx)
{
	ctx->block = 0;
	ctx->error = 0;
	ctx->bytes_available = 0;
	ctx->valid_data = 0;
	ctx->is_xfer_end = false;
	ctx->pending_ack = false;
}

/**
 * Listen for incoming tftp sessions.
 *
 * \note Only write requests are accepted.
 *
 * \param ctx Initialized TftpChannel
 * \param filename String to be filled with file name to be written
 * \param len Length of the filename
 * \param mode Open mode for the returned KFile
 * \return KFile pointer to read from
 */
KFile *tftp_listen(TftpSession *ctx, char *filename, size_t len, TftpOpenMode *mode)
{
	DECLARE_TIMEOUT(wait_tm, ctx->timeout);
	resetTftpState(ctx);

	int res = tftp_waitEvent(ctx, &wait_tm);
	if (res == 0)
	{
		ctx->error = TFTP_ERR_TIMEOUT;
		return NULL;
	}
	if (res == -1)
	{
		ctx->error = TFTP_ERR;
		return NULL;
	}

	// listen onto TFTP port
	ctx->addr_len = sizeof(ctx->addr);
	ssize_t rd = 0;
	if ((rd = lwip_recvfrom(ctx->sock, &ctx->frame, sizeof(Tftpframe), 0, (struct sockaddr *)&ctx->addr, &ctx->addr_len)) > 0)
	{
		// check if the packet is WRQ, otherwise discard the packet
		if (ctx->frame.hdr.opcode == TFTP_WRQ)
		{
			*mode = TFTP_WRITE;
			ctx->pending_ack = true;
			strncpy(filename, (char *)&ctx->frame.hdr.th_u, len);
			filename[len - 1] = '\0';
			ctx->error = 0;
			return &ctx->kfile_request;
		}
		else
			*mode = TFTP_READ;
	}
	ctx->error = TFTP_ERR;
	return NULL;
}

/**
 * Init a server session
 *
 * Create a IPv4 session on all addresses and port \a port.
 *
 * \param ctx Context to be initialized as server
 * \param port Port to listen incoming connections
 * \param timeout Timeout to be used for tftp connections
 * \return 0 if successful, -1 otherwise
 */
int tftp_init(TftpSession *ctx, unsigned short port, mtime_t timeout)
{
	DB(ctx->kfile_request._type = KFT_TFTPSESSION);
	ctx->kfile_request.read = tftp_read;
	ctx->kfile_request.error = tftp_error;
	ctx->kfile_request.clearerr = tftp_clearerr;
	ctx->kfile_request.close = tftp_close;
	resetTftpState(ctx);

	/* Unused kfile methods */
	ctx->kfile_request.seek = NULL;
	ctx->kfile_request.write = NULL;
	ctx->kfile_request.flush = NULL;
	ctx->kfile_request.reopen = NULL;

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        sa.sin_port = htons(port);
	ctx->timeout = timeout;

	ctx->sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (ctx->sock == -1)
	{
		LOG_INFO("TFTP socket error\n");
		return -1;
	}

	if(lwip_bind(ctx->sock, (struct sockaddr *)&sa, sizeof(sa)))
	{
		LOG_INFO("Error binding socket\n");
		return -1;
	}
	return 0;
}

