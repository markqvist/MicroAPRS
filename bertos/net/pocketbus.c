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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief pocketBus protocol implementation.
 *
 * pocketBus protocol is a simple strictly master-slave protocol, usable
 * in embedded systems.
 * pocketBus frame is as follows:
 * <pre>
 * +----------------------------------------+
 * | STX | VER | ADDR | PAYLOAD | CKS | ETX |
 * +----------------------------------------+
 * |     |     |      |         |     |     |
 * + 1B  + 1B  +  2B  + N Byte  + 2B  + 1B  +
 * </pre>
 *
 * - STX, 1 byte (0x02), packet start
 * - VER, 1 byte, packet version
 * - ADDR, 2 byte, slave address
 * - PAYLOAD, N byte, data field
 * - CKS, 2 byte, checksum
 * - ETX, 1 byte, (0x03) packet end
 *
 * Protocol parsing start on STX reception. When the receiving routine
 * finds an STX char, it starts to read characters from the bus
 * until an ETX is received. Once a packet is received,
 * the parser checks packet correctness and checksum. If all is OK
 * the payload is returned.
 *
 * STX (0x02), ETX(0x03) and ESC(0x1B) are special characters and cannot be
 * transmitted inside payload without escaping them.
 * To escape a character you must precede it by the ESC char.
 * E.G. STX -> ESC + STX
 *      ETX -> ESC + ETX
 *      ESC -> ESC + ESC
 *
 * In the ADDR field is always specified the slave address.
 * In the case of master trasmitting, ADDR contains the slave destination
 * address.
 * In case of slave replying, ADDR contains the slave address itself.
 * Thus, the master device does not have an address. Packet must be routed to
 * master by hardware bus design.
 *
 * The checksum algorithm used is rotating hash algortihm, quite simple but more
 * reliable than simple checksum.
 * The checksum in computed on all fields excluding STX, ETX and CHK fields itself.
 * Checksum is computed on the packet *before* escaping.
 * Escape sequence counts for 1 character only (the escaped one).
 */

#include "pocketbus.h"

#include "cfg/cfg_pocketbus.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         POCKETBUS_LOG_LEVEL
#define LOG_VERBOSITY     POCKETBUS_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

#include <io/kfile.h>

#include <cpu/byteorder.h>

#include <string.h>

/**
 * Send a character over pocketBus channel stream, handling escape mode.
 */
void pocketbus_putchar(struct PocketBusCtx *ctx, uint8_t c)
{
	/* Update checksum */
	rotating_update1(c, &ctx->out_cks);

	/* Escape characters with special meaning */
	if (c == POCKETBUS_ESC || c == POCKETBUS_STX || c == POCKETBUS_ETX)
		kfile_putc(POCKETBUS_ESC, ctx->fd);

	kfile_putc(c, ctx->fd);
}

/**
 * Send pocketBus packet header.
 */
void pocketbus_begin(struct PocketBusCtx *ctx, pocketbus_addr_t addr)
{
	PocketBusHdr hdr;

	hdr.ver = POCKETBUS_VER;
	hdr.addr = cpu_to_be16(addr);
	rotating_init(&ctx->out_cks);

	/* Send STX */
	kfile_putc(POCKETBUS_STX, ctx->fd);

	/* Send header */
	pocketbus_write(ctx, &hdr, sizeof(hdr));
}

/**
 * Send buffer \a _data over bus, handling escape.
 */
void pocketbus_write(struct PocketBusCtx *ctx, const void *_data, size_t len)
{
	const uint8_t *data = (const uint8_t *)_data;

	while (len--)
		pocketbus_putchar(ctx, *data++);
}

/**
 * Send pocketBus packet tail.
 */
void pocketbus_end(struct PocketBusCtx *ctx)
{
	/* Send checksum */
	rotating_t cks = cpu_to_be16(ctx->out_cks);
	pocketbus_write(ctx, &cks, sizeof(cks));

	/* Send ETX */
	kfile_putc(POCKETBUS_ETX, ctx->fd);
}

/**
 * Send buffer of \a data to address \a addr with a pocketBus packet over channel stream.
 */
void pocketbus_send(struct PocketBusCtx *ctx, pocketbus_addr_t addr, const void *data, size_t len)
{
	pocketbus_begin(ctx, addr);

	/* Send data */
	pocketbus_write(ctx, data, len);

	pocketbus_end(ctx);
}


/**
 * Try to read a packet from the pocketBus.
 * \return true if a packet is received, false otherwise.
 */
bool pocketbus_recv(struct PocketBusCtx *ctx, struct PocketMsg *msg)
{
	int c;

	/* Process incoming characters until buffer is not empty */
	while ((c = kfile_getc(ctx->fd)) != EOF)
	{
		/* Look for STX char */
		if (c == POCKETBUS_STX && !ctx->escape)
		{
			/* When an STX is found, inconditionally start a new packet */
			if (ctx->sync)
				kprintf("pocketBus double sync!\n");

			ctx->sync = true;
			ctx->len = 0;
			rotating_init(&ctx->in_cks);
			continue;
		}

		if (ctx->sync)
		{
			/* Handle escape mode */
			if (c == POCKETBUS_ESC && !ctx->escape)
			{
				ctx->escape = true;
				continue;
			}

			/* Handle message end */
			if (c == POCKETBUS_ETX && !ctx->escape)
			{
				ctx->sync = false;

				/* Check minimum size */
				if (ctx->len < sizeof(PocketBusHdr) + sizeof(rotating_t))
				{
					kprintf("pocketBus short pkt!\n");
					continue;
				}

				/* Remove checksum bytes from packet len */
				ctx->len -= sizeof(rotating_t);

				/* Compute checksum */
				rotating_update(ctx->buf, ctx->len, &ctx->in_cks);
				uint8_t cks_h = *(ctx->buf + ctx->len);
				uint8_t cks_l = *(ctx->buf + ctx->len + 1);

				rotating_t recv_cks = (cks_h << 8) | cks_l;

				/* Checksum check */
				if (recv_cks == ctx->in_cks)
				{
					PocketBusHdr *hdr = (PocketBusHdr *)ctx;

					/* Check packet version */
					if (hdr->ver == POCKETBUS_VER)
					{
						/* Packet received, set msg fields */
						msg->payload = ctx->buf + sizeof(PocketBusHdr);
						msg->addr = be16_to_cpu(hdr->addr);
						msg->len = ctx->len - sizeof(PocketBusHdr);
						msg->ctx = ctx;
						return true;
					}
					else
					{
						kprintf("pocketBus version mismatch, here[%d], there[%d]\n", POCKETBUS_VER, hdr->ver);
						continue;
					}
				}
				else
				{
					kprintf("pocketBus cks error, here[%04X], there[%04X]\n", ctx->in_cks, recv_cks);
					continue;
				}

			}

			ctx->escape = false;

			/* Check buffer overflow: simply ignore
			   received data and go to unsynced state. */
			if (ctx->len >= CONFIG_POCKETBUS_BUFLEN)
			{
				kprintf("pocketBus buffer overflow\n");
				ctx->sync = false;
				continue;
			}

			/* Put received data in the buffer */
			ctx->buf[ctx->len] = c;
			ctx->len++;
		}
	}

	/*
	 * Check stream status.
	 */
	if (kfile_error(ctx->fd))
	{
		LOG_ERR("fd status[%04X]\n", kfile_error(ctx->fd));
		kfile_clearerr(ctx->fd);
	}

	return false;
}


/**
 * Initialize pocketBus protocol handler.
 */
void pocketbus_init(struct PocketBusCtx *ctx, struct KFile *fd)
{
	ASSERT(ctx);
	ASSERT(fd);

	memset(ctx, 0, sizeof(*ctx));
	ctx->fd = fd;
}
