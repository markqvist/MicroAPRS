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
  * All Rights Reserved.
  * -->
  *
  * \brief EMAC driver for AT91SAM7X Family, interface.
  *
  * \author Daniele Basile <asterix@develer.com>
  * \author Andrea Righi <arighi@develer.com>
  */

#ifndef ETH_AT91_H
#define ETH_AT91_H

// Settings and definition for DAVICOM 9161A
// \{
#define NIC_PHY_ADDR            31

//Registry definition
#define NIC_PHY_BMCR            0x00    //  Basic mode control register.
#define NIC_PHY_BMCR_COLTEST    0x0080  //  Collision test.
#define NIC_PHY_BMCR_FDUPLEX    0x0100  //  Full duplex mode.
#define NIC_PHY_BMCR_ANEGSTART  0x0200  //  Restart auto negotiation.
#define NIC_PHY_BMCR_ISOLATE    0x0400  //  Isolate from MII.
#define NIC_PHY_BMCR_PWRDN      0x0800  //  Power-down.
#define NIC_PHY_BMCR_ANEGENA    0x1000  //  Enable auto negotiation.
#define NIC_PHY_BMCR_100MBPS    0x2000  //  Select 100 Mbps.
#define NIC_PHY_BMCR_LOOPBACK   0x4000  //  Enable loopback mode.
#define NIC_PHY_BMCR_RESET      0x8000  //  Software reset.

#define NIC_PHY_BMSR            0x01    //  Basic mode status register.
#define NIC_PHY_BMSR_ANCOMPL    0x0020  //  Auto negotiation complete.
#define NIC_PHY_BMSR_ANEGCAPABLE 0x0008  // Able to do auto-negotiation
#define NIC_PHY_BMSR_LINKSTAT   0x0004  //  Link status.

#define NIC_PHY_ID1             0x02    //  PHY identifier register 1.
#define NIC_PHY_ID2             0x03    //  PHY identifier register 2.
#define NIC_PHY_ANAR            0x04    //  Auto negotiation advertisement register.
#define NIC_PHY_ANLPAR          0x05    //  Auto negotiation link partner availability register.
#define NIC_PHY_ANER            0x06    //  Auto negotiation expansion register.

// Pin definition for DAVICOM 9161A
// See schematic for at91sam7x-ek evalution board
#define PHY_TXCLK_ISOLATE_BIT   0
#define PHY_REFCLK_XT2_BIT      0
#define PHY_TXEN_BIT            1
#define PHY_TXD0_BIT            2
#define PHY_TXD1_BIT            3
#define PHY_CRS_AD4_BIT         4
#define PHY_RXD0_AD0_BIT        5
#define PHY_RXD1_AD1_BIT        6
#define PHY_RXER_RXD4_RPTR_BIT  7
#define PHY_MDC_BIT             8
#define PHY_MDIO_BIT            9
#define PHY_TXD2_BIT            10
#define PHY_TXD3_BIT            11
#define PHY_TXER_TXD4_BIT       12
#define PHY_RXD2_AD2_BIT        13
#define PHY_RXD3_AD3_BIT        14
#define PHY_RXDV_TESTMODE_BIT   15
#define PHY_COL_RMII_BIT        16
#define PHY_RXCLK_10BTSER_BIT   17
#define PHY_PWRDN_BIT           18
#define PHY_MDINTR_BIT          26

#define PHY_MII_PINS   BV(PHY_REFCLK_XT2_BIT) \
	| BV(PHY_TXEN_BIT) \
	| BV(PHY_TXD0_BIT) \
	| BV(PHY_TXD1_BIT) \
	| BV(PHY_CRS_AD4_BIT) \
	| BV(PHY_RXD0_AD0_BIT) \
	| BV(PHY_RXD1_AD1_BIT) \
	| BV(PHY_RXER_RXD4_RPTR_BIT) \
	| BV(PHY_MDC_BIT) \
	| BV(PHY_MDIO_BIT) \
	| BV(PHY_TXD2_BIT) \
	| BV(PHY_TXD3_BIT) \
	| BV(PHY_TXER_TXD4_BIT) \
	| BV(PHY_RXD2_AD2_BIT) \
	| BV(PHY_RXD3_AD3_BIT) \
	| BV(PHY_RXDV_TESTMODE_BIT) \
	| BV(PHY_COL_RMII_BIT) \
	| BV(PHY_RXCLK_10BTSER_BIT)
// \}

#define EMAC_TX_BUFSIZ          1518  //!!! Don't change this
#define EMAC_TX_BUFFERS         1     //!!! Don't change this
#define EMAC_TX_DESCRIPTORS     EMAC_TX_BUFFERS

#define EMAC_RX_BUFFERS         32    //!!! Don't change this
#define EMAC_RX_BUFSIZ          128   //!!! Don't change this
#define EMAC_RX_DESCRIPTORS	EMAC_RX_BUFFERS

// Flag to manage local tx buffer
#define TXS_USED            0x80000000  //Used buffer.
#define TXS_WRAP            0x40000000  //Last descriptor.
#define TXS_ERROR           0x20000000  //Retry limit exceeded.
#define TXS_UNDERRUN        0x10000000  //Transmit underrun.
#define TXS_NO_BUFFER       0x08000000  //Buffer exhausted.
#define TXS_NO_CRC          0x00010000  //CRC not appended.
#define TXS_LAST_BUFF       0x00008000  //Last buffer of frame.
#define TXS_LENGTH_FRAME    0x000007FF  // Length of frame including FCS.

// Flag to manage local rx buffer
#define RXBUF_OWNERSHIP     0x00000001
#define RXBUF_WRAP          0x00000002

#define BUF_ADDRMASK        0xFFFFFFFC

#define RXS_BROADCAST_ADDR  0x80000000  // Broadcast address detected.
#define RXS_MULTICAST_HASH  0x40000000  // Multicast hash match.
#define RXS_UNICAST_HASH    0x20000000  // Unicast hash match.
#define RXS_EXTERNAL_ADDR   0x10000000  // External address match.
#define RXS_SA1_ADDR        0x04000000  // Specific address register 1 match.
#define RXS_SA2_ADDR        0x02000000  // Specific address register 2 match.
#define RXS_SA3_ADDR        0x01000000  // Specific address register 3 match.
#define RXS_SA4_ADDR        0x00800000  // Specific address register 4 match.
#define RXS_TYPE_ID         0x00400000  // Type ID match.
#define RXS_VLAN_TAG        0x00200000  // VLAN tag detected.
#define RXS_PRIORITY_TAG    0x00100000  // Priority tag detected.
#define RXS_VLAN_PRIORITY   0x000E0000  // VLAN priority.
#define RXS_CFI_IND         0x00010000  // Concatenation format indicator.
#define RXS_EOF             0x00008000  // End of frame.
#define RXS_SOF             0x00004000  // Start of frame.
#define RXS_RBF_OFFSET      0x00003000  // Receive buffer offset mask.
#define RXS_LENGTH_FRAME    0x000007FF  // Length of frame including FCS.

#define EMAC_RSR_BITS	(BV(EMAC_BNA) | BV(EMAC_REC) | BV(EMAC_OVR))
#define EMAC_TSR_BITS	(BV(EMAC_UBR) | BV(EMAC_COL) | BV(EMAC_RLES) | \
			BV(EMAC_BEX) | BV(EMAC_COMP) | BV(EMAC_UND))

typedef struct BufDescriptor
{
	volatile uint32_t addr;
	volatile uint32_t stat;
} BufDescriptor;

#endif /* ETH_AT91_H */
