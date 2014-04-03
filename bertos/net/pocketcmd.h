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
 * \brief PocketBus command abstraction layer.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "pocketcmd"
 * $WIZ$ module_depends = "timer", "pocketbus"
 */

#ifndef NET_POCKETCMD_H
#define NET_POCKETCMD_H

#include "pocketbus.h"
#include <cfg/compiler.h>

#define PKTCMD_NULL 0 ///< pocketBus Null command

typedef uint16_t pocketcmd_t; ///< Type for Command IDs

/**
 * Header for transmitted pocketBus Commands.
 */
typedef struct PocketCmdHdr
{
	pocketcmd_t cmd; ///< command ID
} PocketCmdHdr;

/**
 * This ensure that endianess convertion functions work on
 * the right data size.
 * \{
 */
STATIC_ASSERT(sizeof(pocketcmd_t) == sizeof(uint16_t));
/*\}*/

/* fwd declaration */
struct PocketCmdCtx;

/**
 * pocketBus command message structure.
 */
typedef struct PocketCmdMsg
{
	struct PocketCmdCtx *cmd_ctx; ///< command context
	pocketcmd_t cmd;              ///< command id
	pocketbus_len_t len;          ///< optional arg length
	const uint8_t *buf;           ///< optional arguments
} PocketCmdMsg;

/**
 * Type for command hooks.
 */
typedef void (*pocketcmd_hook_t)(struct PocketCmdMsg *cmd_msg);

/**
 * Type for lookup function hooks.
 */
typedef pocketcmd_hook_t (*pocketcmd_lookup_t)(pocketcmd_t cmd);

/**
 * pocketBus context for command layer communications.
 */
typedef struct PocketCmdCtx
{
	struct PocketBusCtx *bus_ctx; ///< pocketBus context
	pocketbus_addr_t addr;        ///< Our address
	pocketcmd_lookup_t search;    ///< Lookup function used to search for command callbacks
	pocketcmd_t waiting;          ///< The command ID we are waiting for or PKTCMD_NULL.
	ticks_t reply_timer;          ///< For waiting_reply
} PocketCmdCtx;

/**
 * Set slave address \a addr for pocketBus command layer.
 * If we are a slave this is *our* address.
 * If we are the master this is the slave address to send messages to.
 */
INLINE void pocketcmd_setAddr(struct PocketCmdCtx *ctx, pocketbus_addr_t addr)
{
	ctx->addr = addr;
}

void pocketcmd_init(struct PocketCmdCtx *ctx, struct PocketBusCtx *bus_ctx, pocketbus_addr_t addr, pocketcmd_lookup_t search);
void pocketcmd_poll(struct PocketCmdCtx *ctx);
bool pocketcmd_send(struct PocketCmdCtx *ctx, pocketcmd_t cmd, const void *buf, size_t len, bool has_replay);
bool pocketcmd_recv(struct PocketCmdCtx *ctx, PocketCmdMsg *recv_msg);
void pocketcmd_replyNak(struct PocketCmdMsg *msg);
void pocketcmd_replyAck(struct PocketCmdMsg *msg);

/**
 * Helper function used by master to send a command to slave \a addr.
 */
INLINE bool pocketcmd_masterSend(struct PocketCmdCtx *ctx, pocketbus_addr_t addr, pocketcmd_t cmd, const void *buf, size_t len)
{
	pocketcmd_setAddr(ctx, addr);
	return pocketcmd_send(ctx, cmd, buf, len, true);
}

/**
 * Helper function used by slave to reply to a master command.
 */
INLINE bool pocketcmd_slaveReply(struct PocketCmdCtx *ctx, pocketcmd_t cmd, const void *buf, size_t len)
{
	return pocketcmd_send(ctx, cmd, buf, len, false);
}

/**
 * Return true if message contain NAK.
 */
INLINE bool pocketcmd_checkNak(struct PocketCmdMsg *msg)
{
	if (msg->buf[0] == POCKETBUS_NAK)
		return true;

	return false;
}



#endif /* NET_POCKETCMD_H */
