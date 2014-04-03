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
 * \brief lwIP TCP/IP stack module
 *
 * \author Andrea Righi	<arighi@develer.com>
 */

/*
 * Required by arm-2010.09-51-arm-none-eabi toolchain
 */
#define __LINUX_ERRNO_EXTENSIONS__

/* XXX: exclude all the lwIP stuff from the BeRTOS documentation for now  */
#ifndef __doxygen__
/* Ensure that the lwIP compile-time options are included first. */
#include "cfg/cfg_lwip.h"

/* Core lwIP TCP/IP stack */
#if LWIP_DHCP
#include "lwip/src/core/dhcp.c"
#endif
#if LWIP_DNS
#include "lwip/src/core/dns.c"
#endif
#include "lwip/src/core/init.c"
#include "lwip/src/core/mem.c"
#include "lwip/src/core/memp.c"
#include "lwip/src/core/netif.c"
#include "lwip/src/core/pbuf.c"
#include "lwip/src/core/raw.c"
#include "lwip/src/core/stats.c"
#include "lwip/src/core/sys.c"
#if LWIP_TCP
#include "lwip/src/core/tcp.c"
#include "lwip/src/core/tcp_in.c"
#include "lwip/src/core/tcp_out.c"
#endif
#if LWIP_UDP
#include "lwip/src/core/udp.c"
#endif

/* lwIP high-level API code */
#include "lwip/src/api/api_lib.c"
#include "lwip/src/api/api_msg.c"
#include "lwip/src/api/err.c"
#include "lwip/src/api/netbuf.c"
#include "lwip/src/api/netdb.c"
#include "lwip/src/api/netifapi.c"
#include "lwip/src/api/sockets.c"
#include "lwip/src/api/tcpip.c"

/* lwIP IPV4 implementation */
#if LWIP_AUTOIP
#include "lwip/src/core/ipv4/autoip.c"
#endif
#if LWIP_ICMP
#include "lwip/src/core/ipv4/icmp.c"
#endif
#if LWIP_IGMP
#include "lwip/src/core/ipv4/igmp.c"
#endif
#include "lwip/src/core/ipv4/inet.c"
#include "lwip/src/core/ipv4/inet_chksum.c"
#include "lwip/src/core/ipv4/ip.c"
#include "lwip/src/core/ipv4/ip_addr.c"
#include "lwip/src/core/ipv4/ip_frag.c"

/* lwIP SNMP implementation */
#if LWIP_SNMP
#include "lwip/src/core/snmp/asn1_dec.c"
#include "lwip/src/core/snmp/asn1_enc.c"
#include "lwip/src/core/snmp/mib2.c"
#include "lwip/src/core/snmp/mib_structs.c"
#include "lwip/src/core/snmp/msg_in.c"
#include "lwip/src/core/snmp/msg_out.c"
#endif

/* lwIP network interface */
#include "lwip/src/netif/etharp.c"
#include "lwip/src/netif/loopif.c"

/* lwIP PPP implementation */
#if PPP_SUPPORT
#include "lwip/src/netif/ppp/auth.c"
#include "lwip/src/netif/ppp/chap.c"
#include "lwip/src/netif/ppp/chpms.c"
#include "lwip/src/netif/ppp/fsm.c"
#include "lwip/src/netif/ppp/ipcp.c"
#include "lwip/src/netif/ppp/lcp.c"
#include "lwip/src/netif/ppp/magic.c"
#include "lwip/src/netif/ppp/md5.c"
#include "lwip/src/netif/ppp/pap.c"
#include "lwip/src/netif/ppp/ppp.c"
#include "lwip/src/netif/ppp/ppp_oe.c"
#include "lwip/src/netif/ppp/randm.c"
#include "lwip/src/netif/ppp/vj.c"
#endif

/* BeRTOS-specific lwIP interface/porting layer */
#include "lwip/src/netif/ethernetif.c"
#include "lwip/src/arch/sys_arch.c"
#endif /* __doxygen__ */
