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
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief Ethernet standard descriptors
 *
 * $WIZ$ module_name = "eth"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_eth.h"
 * $WIZ$ module_supports = "at91sam7x or sam3x"
 * $WIZ$ module_hw = "bertos/hw/hw_eth.h", "bertos/hw/hw_eth.c"
 */

#ifndef DRV_ETH_H
#define DRV_ETH_H

#include "hw/hw_eth.h"
#include <cpu/types.h>

#define ETH_ADDR_LEN	6
#define ETH_HEAD_LEN	14
#define ETH_DATA_LEN	1500
#define ETH_FRAME_LEN	(ETH_HEAD_LEN + ETH_DATA_LEN)

#define ETH_TYPE_IP	0x0800

typedef union Ethernet
{
	struct
	{
		uint8_t  dst[ETH_ADDR_LEN];
		uint8_t  src[ETH_ADDR_LEN];
		uint16_t type;
		uint8_t data[0];
	};
	uint8_t raw[ETH_FRAME_LEN];
} PACKED Ethernet;

/**
 * Determine if ethernet address \a addr is a all zero.
 */
INLINE int eth_addrIsZero(const uint8_t *addr)
{
	return !(addr[0] | addr[1] | addr[2] |
		 addr[3] | addr[4] | addr[5]);
}

/**
 * Determine if ethernet address \a addr is a multicast address.
 */
INLINE int eth_addrIsMcast(const uint8_t *addr)
{
	return (0x01 & addr[0]);
}

/**
 * Determine if ethernet address \a addr is locally-assigned (IEEE 802).
 */
INLINE int eth_addrIsLocal(const uint8_t *addr)
{
	return (0x02 & addr[0]);
}

/**
 * Determine if ethernet address \a addr is broadcast.
 */
INLINE bool eth_addrIsBcast(const uint8_t *addr)
{
	return (addr[0] & addr[1] & addr[2] &
		addr[3] & addr[4] & addr[5]) == 0xff;
}

/**
 * Check if the ethernet address \a addr is not all zero, is not a multicast
 * address, and is not broadcast.
 */
INLINE bool eth_addrIsValid(const uint8_t *addr)
{
	return !eth_addrIsMcast(addr) && !eth_addrIsZero(addr);
}

/**
 * Compare two ethernet addresses: \a addr1 and \a addr2, returns 0 if equal.
 */
INLINE bool eth_addrCmp(const uint8_t *addr1, const uint8_t *addr2)
{
	return !!((addr1[0] ^ addr2[0]) |
			(addr1[1] ^ addr2[1]) |
			(addr1[2] ^ addr2[2]) |
			(addr1[3] ^ addr2[3]) |
			(addr1[4] ^ addr2[4]) |
			(addr1[5] ^ addr2[5]));
}

ssize_t eth_putFrame(const uint8_t *buf, size_t len);
void eth_sendFrame(void);

size_t eth_getFrameLen(void);
ssize_t eth_getFrame(uint8_t *buf, size_t len);

ssize_t eth_send(const uint8_t *buf, size_t len);
ssize_t eth_recv(uint8_t *buf, size_t len);

int eth_init(void);

#endif /* DRV_ETH_H */
