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
 * \author Daniele Basile <asterix@develer.com>
 *
 * AT91 Ethernet MAC 10/100 controller.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef AT91_EMAC_H
#define AT91_EMAC_H


/**
 *
 * Network Control Register
 *
 */
/* \{ */
#define EMAC_NCR_OFF                0x00000000  ///< Network control register offset.
#define EMAC_NCR    (*((reg32_t *)(EMAC_BASE +  EMAC_NCR_OFF)))  ///< Network Control register address.
#define EMAC_LB                              0  ///< PHY loopback.
#define EMAC_LLB                             1   ///< EMAC loopback.
#define EMAC_RE                              2  ///< Receive enable.
#define EMAC_TE                              3  ///< Transmit enable.
#define EMAC_MPE                             4  ///< Management port enable.
#define EMAC_CLRSTAT                         5  ///< Clear statistics registers.
#define EMAC_INCSTAT                         6  ///< Increment statistics registers.
#define EMAC_WESTAT                          7  ///< Write enable for statistics registers.
#define EMAC_BP                              8  ///< Back pressure.
#define EMAC_TSTART                          9  ///< Start Transmission.
#define EMAC_THALT                          10  ///< Transmission halt.
// Not in sam7x
//#define EMAC_TPFR                           11  ///< Transmit pause frame.
//#define EMAC_TZQ                            12  ///< Transmit zero quantum pause frame.
/* \} */

/**
 * Network Configuration Register
 *
 */
/* \{ */
#define EMAC_NCFGR_OFF              0x00000004  ///< Network configuration register offset.
#define EMAC_NCFGR   (*((reg32_t *)(EMAC_BASE +  EMAC_NCFGR_OFF))) ///< Network configuration register address.
#define EMAC_SPD                             0  ///< Speed, set for 100Mb.
#define EMAC_FD                              1  ///< Full duplex.
#define EMAC_JFRAME                          3  ///< Jumbo Frames.
#define EMAC_CAF                             4  ///< Copy all frames.
#define EMAC_NBC                             5  ///< No broadcast.
#define EMAC_MTI                             6  ///< Multicast hash event enable.
#define EMAC_UNI                             7  ///< Unicast hash enable.
#define EMAC_BIG                             8  ///< Receive 1522 bytes.
// Not in sam7x
//#define EMAC_EAE                           9  ///< External address match enable.
#define EMAC_CLK                    0x00000C00  ///< Clock divider mask.
#define EMAC_CLK_HCLK_8             0x00000000  ///< HCLK divided by 8.
#define EMAC_CLK_HCLK_16            0x00000400  ///< HCLK divided by 16.
#define EMAC_CLK_HCLK_32            0x00000800  ///< HCLK divided by 32.
#define EMAC_CLK_HCLK_64            0x00000C00  ///< HCLK divided by 64.
#define EMAC_RTY                            12  ///< Retry test.
#define EMAC_PAE                            13  ///< Pause enable.
#define EMAC_RBOF                   0x0000C000  ///< Receive buffer offset.
#define EMAC_RBOF_OFFSET_0          0x00000000  ///< No offset from start of receive buffer.
#define EMAC_RBOF_OFFSET_1          0x00004000  ///< One byte offset from start of receive buffer.
#define EMAC_RBOF_OFFSET_2          0x00008000  ///< Two bytes offset from start of receive buffer.
#define EMAC_RBOF_OFFSET_3          0x0000C000  ///< Three bytes offset from start of receive buffer.
#define EMAC_RLCE                           16  ///< Receive length field checking enable.
#define EMAC_DRFCS                          17  ///< Discard receive FCS.
#define EMAC_EFRHD                          18  ///< Allow receive during transmit in half duplex.
#define EMAC_IRXFCS                         19  ///< Ignore received FCS.
/* \} */

/**
 * Network Status Register
 *
 */
/* \{ */
#define EMAC_NSR_OFF                0x00000008  ///< Network Status register offset.
#define EMAC_NSR    (*((reg32_t *)(EMAC_BASE +  EMAC_NSR_OFF)))  ///< Network Status register address.
// Not in sam7x
//#define EMAC_LINKR                         0  ///< .
#define EMAC_MDIO                            1  ///< Status of MDIO input pin.
#define EMAC_IDLE                            2  ///< Set when PHY is running.
/* \} */

/**
 * Transmit Status Register
 */
/* \{ */
#define EMAC_TSR_OFF                0x00000014  ///< Transmit Status register offset.
#define EMAC_TSR    (*((reg32_t *)(EMAC_BASE +  EMAC_TSR_OFF)))  ///< Transmit Status register address.

#define EMAC_UBR                             0  ///< Used bit read.
#define EMAC_COL                             1  ///< Collision occurred.
#define EMAC_RLES                            2  ///< Retry limit exceeded.
#define EMAC_TGO                             3  ///< Transmit active.
#define EMAC_BEX                             4  ///< Buffers exhausted mid frame.
#define EMAC_COMP                            5  ///< Transmit complete.
#define EMAC_UND                             6  ///< Transmit underrun.
/* \} */

/**
 * Buffer Queue Pointer Register
 *
 */
/* \{ */
#define EMAC_RBQP_OFF               0x00000018  ///< Receive buffer queue pointer.
#define EMAC_RBQP   (*((reg32_t *)(EMAC_BASE +  EMAC_RBQP_OFF))) ///< Receive buffer queue pointer.
#define EMAC_TBQP_OFF               0x0000001C  ///< Transmit buffer queue pointer.
#define EMAC_TBQP   (*((reg32_t *)(EMAC_BASE +  EMAC_TBQP_OFF))) ///< Transmit buffer queue pointer.
/* \} */

/**
 * Receive Status Register
 */
/* \{ */
#define EMAC_RSR_OFF                0x00000020  ///< Receive status register offset.
#define EMAC_RSR    (*((reg32_t *)(EMAC_BASE +  EMAC_RSR_OFF)))  ///< Receive status register address.
#define EMAC_BNA                             0  ///< Buffer not available.
#define EMAC_REC                             1  ///< Frame received.
#define EMAC_OVR                             2  ///< Receive overrun.
/* \} */

/** Interrupt Registers */
/* \{ */
#define EMAC_ISR_OFF                0x00000024  ///< Status register offset.
#define EMAC_ISR    (*((reg32_t *)(EMAC_BASE +  EMAC_ISR_OFF)))  ///< Status register address.
#define EMAC_IER_OFF                0x00000028  ///< Enable register offset.
#define EMAC_IER    (*((reg32_t *)(EMAC_BASE +  EMAC_IER_OFF)))  ///< Enable register address.
#define EMAC_IDR_OFF                0x0000002C  ///< Disable register offset.
#define EMAC_IDR    (*((reg32_t *)(EMAC_BASE +  EMAC_IDR_OFF)))  ///< Disable register address.
#define EMAC_IMR_OFF                0x00000030  ///< Mask register offset.
#define EMAC_IMR    (*((reg32_t *)(EMAC_BASE +  EMAC_IMR_OFF)))  ///< Mask register address.

#define EMAC_MFD                             0  ///< Management frame done.
#define EMAC_RCOMP                           1  ///< Receive complete.
#define EMAC_RXUBR                           2  ///< Receive used bit read.
#define EMAC_TXUBR                           3  ///< Transmit used bit read.
#define EMAC_TUND                            4  ///< Ethernet transmit buffer underrun.
#define EMAC_RLEX                            5  ///< Retry limit exceeded.
#define EMAC_TXERR                           6  ///< Transmit error.
#define EMAC_TCOMP                           7  ///< Transmit complete.
//Not in sam7x
//#define EMAC_LINK                          8  ///< .
#define EMAC_ROVR                           10  ///< Receive overrun.
#define EMAC_HRESP                          11  ///< DMA bus error.
#define EMAC_PFR                            12  ///< Pause frame received.
#define EMAC_PTZ                            13  ///< Pause time zero.
/* \} */

/**
 * PHY Maintenance Register
 *
 */
/* \{ */
#define EMAC_MAN_OFF                0x00000034  ///< PHY maintenance register offset.
#define EMAC_MAN    (*((reg32_t *)(EMAC_BASE +  EMAC_MAN_OFF)))  ///< PHY maintenance register address.
#define EMAC_DATA                   0x0000FFFF  ///< PHY data mask.
#define EMAC_CODE                   0x00020000  ///< Fixed value.
#define EMAC_REGA                   0x007C0000  ///< PHY register address mask.
#define EMAC_REGA_SHIFT                     18  ///< PHY register address mask.
#define EMAC_PHYA                   0x0F800000  ///< PHY address mask.
#define EMAC_PHYA_SHIFT                     23  ///< PHY address mask.
#define EMAC_RW                     0x30000000  ///< PHY read/write command mask.
#define EMAC_RW_READ                0x20000000  ///< PHY read command.
#define EMAC_RW_WRITE               0x10000000  ///< PHY write command.
#define EMAC_SOF                    0x40000000  ///< Fixed value.
/* \} */

/**
 * Pause Time Register
 *
 */
/* \{ */
#define EMAC_PTR_OFF                0x00000038  ///< Pause time register offset.
#define EMAC_PTR    (*((reg32_t *)(EMAC_BASE +  EMAC_PTR_OFF)))  ///< Pause time register address.
#define EMAC_PTIME                  0x0000FFFF  ///< Pause time mask.
/* \} */

/**
 * Statistics Registers
 */
/* \{ */
#define EMAC_PFRR_OFF               0x0000003C  ///< Pause frames received register offset.
#define EMAC_PFRR  (*((reg32_t *)(EMAC_BASE +  EMAC_PFRR_OFF)))  ///< Pause frames received register address.
#define EMAC_FTO_OFF                0x00000040  ///< Frames transmitted OK register offset.
#define EMAC_FTO    (*((reg32_t *)(EMAC_BASE +  EMAC_FTO_OFF)))  ///< Frames transmitted OK register address.
#define EMAC_SCF_OFF                0x00000044  ///< Single collision frame register offset.
#define EMAC_SCF    (*((reg32_t *)(EMAC_BASE +  EMAC_SCF_OFF)))  ///< Single collision frame register address.
#define EMAC_MCF_OFF                0x00000048  ///< Multiple collision frame register offset.
#define EMAC_MCF    (*((reg32_t *)(EMAC_BASE +  EMAC_MCF_OFF)))  ///< Multiple collision frame register address.
#define EMAC_FRO_OFF                0x0000004C  ///< Frames received OK register offset.
#define EMAC_FRO    (*((reg32_t *)(EMAC_BASE +  EMAC_FRO_OFF)))  ///< Frames received OK register address.
#define EMAC_FCSE_OFF               0x00000050  ///< Frame check sequence error register offset.
#define EMAC_FCSE   (*((reg32_t *)(EMAC_BASE +  EMAC_FCSE_OFF))) ///< Frame check sequence error register address.
#define EMAC_ALE_OFF                0x00000054  ///< Alignment error register offset.
#define EMAC_ALE    (*((reg32_t *)(EMAC_BASE +  EMAC_ALE_OFF)))  ///< Alignment error register address.
#define EMAC_DTF_OFF                0x00000058  ///< Deferred transmission frame register offset.
#define EMAC_DTF    (*((reg32_t *)(EMAC_BASE +  EMAC_DTF_OFF)))  ///< Deferred transmission frame register address.
#define EMAC_LCOL_OFF               0x0000005C  ///< Late collision register offset.
#define EMAC_LCOL   (*((reg32_t *)(EMAC_BASE +  EMAC_LCOL_OFF))) ///< Late collision register address.
#define EMAC_ECOL_OFF               0x00000060  ///< Excessive collision register offset.
#define EMAC_ECOL   (*((reg32_t *)(EMAC_BASE +  EMAC_ECOL_OFF))) ///< Excessive collision register address.
#define EMAC_TUNDR_OFF              0x00000064  ///< Transmit underrun error register offset.
#define EMAC_TUNDR (*((reg32_t *)(EMAC_BASE +  EMAC_TUNDR_OFF))) ///< Transmit underrun error register address.
#define EMAC_CSE_OFF                0x00000068  ///< Carrier sense error register offset.
#define EMAC_CSE    (*((reg32_t *)(EMAC_BASE +  EMAC_CSE_OFF)))  ///< Carrier sense error register address.
#define EMAC_RRE_OFF                0x0000006C  ///< Receive resource error register offset.
#define EMAC_RRE    (*((reg32_t *)(EMAC_BASE +  EMAC_RRE_OFF)))  ///< Receive resource error register address.
#define EMAC_ROV_OFF                0x00000070  ///< Receive overrun errors register offset.
#define EMAC_ROV    (*((reg32_t *)(EMAC_BASE +  EMAC_ROV_OFF)))  ///< Receive overrun errors register address.
#define EMAC_RSE_OFF                0x00000074  ///< Receive symbol errors register offset.
#define EMAC_RSE    (*((reg32_t *)(EMAC_BASE +  EMAC_RSE_OFF)))  ///< Receive symbol errors register address.
#define EMAC_ELE_OFF                0x00000078  ///< Excessive length errors register offset.
#define EMAC_ELE    (*((reg32_t *)(EMAC_BASE +  EMAC_ELE_OFF)))  ///< Excessive length errors register address.
#define EMAC_RJA_OFF                0x0000007C  ///< Receive jabbers register offset.
#define EMAC_RJA    (*((reg32_t *)(EMAC_BASE +  EMAC_RJA_OFF)))  ///< Receive jabbers register address.
#define EMAC_USF_OFF                0x00000080  ///< Undersize frames register offset.
#define EMAC_USF    (*((reg32_t *)(EMAC_BASE +  EMAC_USF_OFF)))  ///< Undersize frames register address.
#define EMAC_STE_OFF                0x00000084  ///< SQE test error register offset.
#define EMAC_STE    (*((reg32_t *)(EMAC_BASE +  EMAC_STE_OFF)))  ///< SQE test error register address.
#define EMAC_RLE_OFF                0x00000088  ///< Receive length field mismatch register offset.
#define EMAC_RLE    (*((reg32_t *)(EMAC_BASE +  EMAC_RLE_OFF)))  ///< Receive length field mismatch register address.
// Not in sam7x
//#define EMAC_TPF_OFF                0x0000008C  ///< Transmitted pause frames register offset.
//#define EMAC_TPF    (*((reg32_t *)(EMAC_BASE +  EMAC_TPF_OFF)  ///< Transmitted pause frames register address.
/* \} */

/**
 * MAC Adressing Registers
 *
 */
/* \{ */
#define EMAC_HRB_OFF                0x00000090  ///< Hash address bottom[31:0].
#define EMAC_HRB    (*((reg32_t *)(EMAC_BASE +  EMAC_HRB_OFF)))  ///< Hash address bottom[31:0].
#define EMAC_HRT_OFF                0x00000094  ///< Hash address top[63:32].
#define EMAC_HRT    (*((reg32_t *)(EMAC_BASE +  EMAC_HRT_OFF)))  ///< Hash address top[63:32].
#define EMAC_SA1L_OFF               0x00000098  ///< Specific address 1 bottom, first 4 bytes.
#define EMAC_SA1L   (*((reg32_t *)(EMAC_BASE +  EMAC_SA1L_OFF))) ///< Specific address 1 bottom, first 4 bytes.
#define EMAC_SA1H_OFF               0x0000009C  ///< Specific address 1 top, last 2 bytes.
#define EMAC_SA1H   (*((reg32_t *)(EMAC_BASE +  EMAC_SA1H_OFF))) ///< Specific address 1 top, last 2 bytes.
#define EMAC_SA2L_OFF               0x000000A0  ///< Specific address 2 bottom, first 4 bytes.
#define EMAC_SA2L   (*((reg32_t *)(EMAC_BASE +  EMAC_SA2L_OFF))) ///< Specific address 2 bottom, first 4 bytes.
#define EMAC_SA2H_OFF               0x000000A4  ///< Specific address 2 top, last 2 bytes.
#define EMAC_SA2H   (*((reg32_t *)(EMAC_BASE +  EMAC_SA2H_OFF))) ///< Specific address 2 top, last 2 bytes.
#define EMAC_SA3L_OFF               0x000000A8  ///< Specific address 3 bottom, first 4 bytes.
#define EMAC_SA3L   (*((reg32_t *)(EMAC_BASE +  EMAC_SA3L_OFF))) ///< Specific address 3 bottom, first 4 bytes.
#define EMAC_SA3H_OFF               0x000000AC  ///< Specific address 3 top, last 2 bytes.
#define EMAC_SA3H   (*((reg32_t *)(EMAC_BASE +  EMAC_SA3H_OFF))) ///< Specific address 3 top, last 2 bytes.
#define EMAC_SA4L_OFF               0x000000B0  ///< Specific address 4 bottom, first 4 bytes.
#define EMAC_SA4L   (*((reg32_t *)(EMAC_BASE +  EMAC_SA4L_OFF))) ///< Specific address 4 bottom, first 4 bytes.
#define EMAC_SA4H_OFF               0x000000B4  ///< Specific address 4 top, last 2 bytes.
#define EMAC_SA4H   (*((reg32_t *)(EMAC_BASE +  EMAC_SA4H_OFF))) ///< Specific address 4 top, last 2 bytes.
/* \} */

/**
 * Type ID Register
 *
 */
/* \{ */
#define EMAC_TID_OFF                0x000000B8  ///< Type ID checking register offset.
#define EMAC_TID    (*((reg32_t *)(EMAC_BASE +  EMAC_TID_OFF))) ///< Type ID checking register address.
// Not in sam7x
//#define EMAC_TPQ_OFF                0x000000BC  ///< Transmit pause quantum register offset.
//#define EMAC_TPQ    (*((reg32_t *)(EMAC_BASE +  EMAC_TPQ_OFF)))  ///< Transmit pause quantum register address.
/* \} */

/**
 * User Input/Output Register
 *
 */
/* \{ */
#define EMAC_USRIO_OFF              0x000000C0  ///< User input/output register offset.
#define EMAC_USRIO (*((reg32_t *)(EMAC_BASE +  EMAC_USRIO_OFF))) ///< User input/output register address.

#define EMAC_RMII                            0  ///< Enable reduced MII.
#define EMAC_CLKEN                           1  ///< Enable tranceiver input clock.
/* \} */

// Not in sam7x
/*
 * Wake On LAN Register
 *
 *
* \{ *
#define EMAC_WOL_OFF                0x000000C4  ///< Wake On LAN register offset.
#define EMAC_WOL    (*((reg32_t *)(EMAC_BASE +  EMAC_WOL_OFF)  ///< Wake On LAN register address.
#define EMAC_IP                     0x0000FFFF  ///< ARP request IP address mask.
#define EMAC_MAG                    0x00010000  ///< Magic packet event enable.
#define EMAC_ARP                    0x00020000  ///< ARP request event enable.
#define EMAC_SA1                    0x00040000  ///< Specific address register 1 event enable.
* \} *

** Revision Register *
* \{ *
#define EMAC_REV_OFF                0x000000FC  ///< Revision register offset.
#define EMAC_REV    (*((reg32_t *)(EMAC_BASE +  EMAC_REV_OFF)  ///< Revision register address.
#define EMAC_REVREF                 0x0000FFFF  ///< Revision.
#define EMAC_PARTREF                0xFFFF0000  ///< Part.
* \} *
*/

#endif /* AT91_EMAC_H */
