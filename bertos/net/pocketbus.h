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
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Basical functions to use pocketBus protocol.
 *
 * $WIZ$ module_name = "pocketbus"
 * $WIZ$ module_depends = "rotating_hash", "kfile"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_pocketbus.h"
 */

#ifndef NET_POCKETBUS_H
#define NET_POCKETBUS_H

#include "cfg/cfg_pocketbus.h"        /* for CONFIG_POCKETBUS_BUFLEN */
#include <cfg/compiler.h>

#include <algo/rotating_hash.h>

#include <io/kfile.h>

/**
 * pocketBus special characters definitions.
 * \{
 */
#define POCKETBUS_STX 0x02 //ASCII STX
#define POCKETBUS_ETX 0x03 //ASCII ETX
#define POCKETBUS_ESC 0x1B //ASCII ESC
#define POCKETBUS_ACK 0x06 //ASCII ACK
#define POCKETBUS_NAK 0x15 //ASCII NAK
/*\}*/

#define POCKETBUS_BROADCAST_ADDR 0xFFFF ///< pocketBus broadcast address

/**
 * Type for pocketBus length.
 */
typedef uint16_t pocketbus_len_t;

/**
 * Type for pocketBus addresses.
 */
typedef uint16_t pocketbus_addr_t;

/**
 * Header of pocketBus messages.
 */
typedef struct PocketBusHdr
{
	#define POCKETBUS_VER 1
	uint8_t ver;   ///< packet version
	pocketbus_addr_t addr; ///< slave address
} PocketBusHdr;

/**
 * pocketBus context structure.
 */
typedef struct PocketBusCtx
{
	uint8_t buf[CONFIG_POCKETBUS_BUFLEN]; ///< receiving Buffer
	struct KFile *fd;   ///< File descriptor
	bool sync;           ///< Status flag: true if we have received an STX, false otherwise
	bool escape;         ///< Status flag: true if we are in escape mode, false otherwise
	rotating_t in_cks;   ///< Checksum computation for received data.
	rotating_t out_cks;  ///< Checksum computation for transmitted data.
	pocketbus_len_t len; ///< Received length
} PocketBusCtx;

STATIC_ASSERT(offsetof(PocketBusCtx, buf) == 0);
/**
 * Structure holding pocketBus message parameters.
 */
typedef struct PocketMsg
{
	struct PocketBusCtx *ctx; ///< pocketBus message context
	pocketbus_addr_t addr;    ///< address for received packet
	pocketbus_len_t len;      ///< payload length
	const uint8_t *payload;   ///< payload data
} PocketMsg;

/**
 * This ensure that endianess convertion functions work on
 * the right data size.
 * \{
 */
STATIC_ASSERT(sizeof(pocketbus_addr_t) == sizeof(uint16_t));
STATIC_ASSERT(sizeof(rotating_t) == sizeof(uint16_t));
/*\}*/

void pocketbus_putchar(struct PocketBusCtx *ctx, uint8_t c);
void pocketbus_begin(struct PocketBusCtx *ctx, pocketbus_addr_t addr);
void pocketbus_write(struct PocketBusCtx *ctx, const void *_data, size_t len);
void pocketbus_end(struct PocketBusCtx *ctx);

void pocketbus_send(struct PocketBusCtx *ctx, pocketbus_addr_t addr, const void *data, size_t len);
bool pocketbus_recv(struct PocketBusCtx *ctx, struct PocketMsg *msg);
void pocketbus_init(struct PocketBusCtx *ctx, struct KFile *fd);

#endif /* NET_POCKETBUS_H */
