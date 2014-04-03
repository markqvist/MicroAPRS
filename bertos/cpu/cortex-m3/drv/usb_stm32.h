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
 * \brief STM32: USB full-speed device driver
 *
 * Low-level USB device driver for the STM32 architecture.
 */

#ifndef USB_STM32_H
#define USB_STM32_H

#include <cfg/compiler.h>
#include <drv/usb.h>
#include <drv/usb_endpoint.h>

#define USB_BASE_ADDR			0x40005C00

#define USB_DM_PIN	(1 << 11)
#define USB_DP_PIN	(1 << 12)
#define USB_DISC_PIN	(1 << 11)

#define USB_EP0_MAX_SIZE	CONFIG_EP0_MAX_SIZE
#define USB_XFER_MAX_SIZE	64

#define EP_MAX_SLOTS	USB_EP_MAX
#define EP_MAX_NUM	(EP_MAX_SLOTS << 1)

/* USB packet memory organization */
#define USB_PACKET_MEMORY_BASE		0x40006000
#define USB_PACKET_MEMORY_SIZE		512

/* Offset of the buffer descriptor table inside the packet memory */
#define USB_BDT_OFFSET \
	((USB_PACKET_MEMORY_SIZE - (sizeof(stm32_UsbBd) * EP_MAX_NUM)) & ~7)

#define USB_MEM_ADDR(offset) \
	(USB_PACKET_MEMORY_BASE + ((offset << 1) & ~3) + (offset & 1))

#define EP_DTB_READ(slot, offset) \
	(*((uint16_t *)(USB_MEM_ADDR((USB_BDT_OFFSET + \
					(slot) * sizeof(stm32_UsbBd) + \
					(offset))))))

#define EP_DTB_WRITE(slot, offset, data)  (EP_DTB_READ(slot, offset) = data)

#define ADDR_TX_OFFSET	offsetof(stm32_UsbBd, AddrTx)
#define COUNT_TX_OFFSET	offsetof(stm32_UsbBd, CountTx)
#define ADDR_RX_OFFSET	offsetof(stm32_UsbBd, AddrRx)
#define COUNT_RX_OFFSET	offsetof(stm32_UsbBd, CountRx)

#define USB_CTRL_RW_MASK          0x070F
#define USB_CTRL_CLEAR_ONLY_MASK  0x8080
#define USB_CTRL_TOGGLE_MASK      0x7070

/* CNTR register flags */
#define bmCTRM                      0x8000
#define bmPMAOVRM                   0x4000
#define bmERRM                      0x2000
#define bmWKUPM                     0x1000
#define bmSUSPM                     0x0800
#define bmRESETM                    0x0400
#define bmSOFM                      0x0200
#define bmESOFM                     0x0100

#define bmRESUME                    0x0010
#define bmFSUSP                     0x0008
#define bmLPMODE                    0x0004
#define bmPDWN                      0x0002
#define bmFRES                      0x0001

/* USB error codes */
enum stm32_usb_error
{
	USB_OK = 0,
	USB_INTR_ERROR,
	USB_INVAL_ERROR,
	USB_NODEV_ERROR,
	USB_MEMORY_FULL,
	USB_BUF_OVERFLOW,
	USB_EP_STALLED,
	USB_FATAL_ERROR,
};

/* STM32 USB endpoint types */
enum stm32_UsbEpype
{
	EP_BULK = 0,
	EP_CTRL,
	EP_ISO,
	EP_INTERRUPT,

	EP_TYPE_MAX
};

/* STM32 USB interrupt status register bits */
typedef union
{
	uint32_t status;
	struct {
		uint8_t EP_ID  : 4;
		uint8_t DIR    : 1;
		uint8_t        : 2;
		uint8_t SZDPR  : 1;
		uint8_t ESOF   : 1;
		uint8_t SOF    : 1;
		uint8_t RESET  : 1;
		uint8_t SUSP   : 1;
		uint8_t WKUP   : 1;
		uint8_t ERR    : 1;
		uint8_t PMAOVR : 1;
		uint8_t CTR    : 1;
	};
} PACKED stm32_usb_irq_status_t;

/* Endpoint state */
typedef enum
{
	EP_DISABLED = 0,
	EP_STALL,
	EP_NAK,
	EP_VALID
} stm32_UsbEpState;

/* STM32 USB supported endpoints */
typedef enum stm32_UsbEP
{
	CTRL_ENP_OUT = 0, CTRL_ENP_IN,
	ENP1_OUT, ENP1_IN,
	ENP2_OUT, ENP2_IN,
	ENP3_OUT, ENP3_IN,
	ENP4_OUT, ENP4_IN,
	ENP5_OUT, ENP5_IN,
	ENP6_OUT, ENP6_IN,
	ENP7_OUT, ENP7_IN,
	ENP8_OUT, ENP8_IN,
	ENP9_OUT, ENP9_IN,
	ENP10_OUT, ENP10_IN,
	ENP11_OUT, ENP11_IN,
	ENP12_OUT, ENP12_IN,
	ENP13_OUT, ENP13_IN,
	ENP14_OUT, ENP14_IN,
	ENP15_OUT, ENP15_IN,

	EP_MAX_HW_NUM
} stm32_UsbEP;

/* STM32 USB packet memory slot */
typedef struct stm32_UsbMemSlot
{
	stm32_UsbEP ep_addr;
	uint16_t Start;
	uint16_t Size;
	struct stm32_UsbMemSlot *next;
} stm32_UsbMemSlot;

/* STM32 USB buffer descriptor (packet memory) */
typedef struct stm32_UsbBd
{
	uint16_t AddrTx;
	uint16_t CountTx;
	uint16_t AddrRx;
	uint16_t CountRx;
} PACKED stm32_UsbBd;

/* STM32 USB endpoint I/O status */
typedef enum stm32_UsbIoStatus
{
	NOT_READY = 0,
	NO_SERVICED,
	BEGIN_SERVICED,
	COMPLETE,
	BUFFER_UNDERRUN,
	BUFFER_OVERRUN,
	SETUP_OVERWRITE,
	STALLED,
} stm32_UsbIoStatus;

/* STM32 USB hardware endpoint descriptor */
typedef struct stm32_UsbEp
{
	reg32_t *hw;
	uint8_t type;
	void (*complete)(int);
	ssize_t max_size;
	ssize_t offset;
	ssize_t size;
	stm32_UsbIoStatus status;
	union
	{
		uint8_t *read_buffer;
		const uint8_t *write_buffer;
	};
	int32_t avail_data;
	uint8_t flags;
} stm32_UsbEp;

/* STM32 USB hardware endpoint flags */
#define STM32_USB_EP_AVAIL_DATA		BV(0)
#define STM32_USB_EP_ZERO_PACKET	BV(1)
#define STM32_USB_EP_ZERO_POSSIBLE	BV(2)

#endif /* USB_STM32_H */
