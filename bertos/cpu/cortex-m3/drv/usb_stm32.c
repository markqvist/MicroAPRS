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
 * \brief STM32 USB driver
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "cfg/cfg_usb.h"

#define LOG_LEVEL  USB_LOG_LEVEL
#define LOG_FORMAT USB_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cfg/module.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <drv/irq_cm3.h>
#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h>
#include <drv/timer.h>
#include <drv/usb.h>

#include <mware/event.h>

#include <string.h> /* memcpy() */

#include "usb_stm32.h"

/* XXX: consider to move this to cfg/macros.h */

/* XXX: redefine this to make it usable within C expression */
#define _MIN(a,b)	(((a) < (b)) ? (a) : (b))

/* STM32 USB registers */
struct stm32_usb
{
	reg32_t EP0R;
	reg32_t EP1R;
	reg32_t EP2R;
	reg32_t EP3R;
	reg32_t EP4R;
	reg32_t EP5R;
	reg32_t EP6R;
	reg32_t EP7R;
	reg32_t __reserved[8];
	reg32_t CNTR;
	reg32_t ISTR;
	reg32_t FNR;
	reg32_t DADDR;
	reg32_t BTABLE;
};

/* Hardware registers */
static struct stm32_usb *usb = (struct stm32_usb *)USB_BASE_ADDR;

/* Endpoint descriptors: used for handling requests to use with endpoints */
static stm32_UsbEp ep_cnfg[EP_MAX_NUM];
STATIC_ASSERT(EP_MAX_NUM <= EP_MAX_HW_NUM);

/* USB EP0 control descriptor */
static const UsbEndpointDesc USB_CtrlEpDescr0 =
{
	.bLength = sizeof(USB_CtrlEpDescr0),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_OUT | 0,
	.bmAttributes = USB_ENDPOINT_XFER_CONTROL,
	.wMaxPacketSize = USB_EP0_MAX_SIZE,
	.bInterval = 0,
};

/* USB EP1 control descriptor */
static const UsbEndpointDesc USB_CtrlEpDescr1 =
{
	.bLength = sizeof(USB_CtrlEpDescr1),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 0,
	.bmAttributes = USB_ENDPOINT_XFER_CONTROL,
	.wMaxPacketSize = USB_EP0_MAX_SIZE,
	.bInterval = 0,
};

/* USB setup packet */
static UsbCtrlRequest setup_packet;

/* USB device controller: max supported interfaces */
#define USB_MAX_INTERFACE	CONFIG_USB_INTERFACE_MAX

/* USB device controller features */
#define STM32_UDC_FEATURE_SELFPOWERED	BV(0)
#define STM32_UDC_FEATURE_REMOTE_WAKEUP	BV(1)

/* Hardware-specific USB device controller structure */
typedef struct stm32_udc
{
	uint8_t state;
	uint32_t cfg_id;
	const UsbConfigDesc *cfg;
	uint32_t interfaces;
	uint32_t alt[USB_MAX_INTERFACE];
	uint32_t address;
	uint8_t feature;
} PACKED stm32_udc_t;

/* Hardware-specific USB Device Controller */
static stm32_udc_t udc;

/* Generic USB Device Controller structure */
static UsbDevice *usb_dev;

/* USB packet memory management: list of allocated chunks */
static stm32_UsbMemSlot *mem_use;

/* USB packet memory management: memory buffer metadata */
static stm32_UsbMemSlot memory_buffer[EP_MAX_NUM];

/* Endpoint TX and RX buffers */
static size_t rx_size, tx_size;

#define EP_BUFFER_SIZE _MIN(CONFIG_USB_BUFSIZE, USB_XFER_MAX_SIZE)
STATIC_ASSERT(!(EP_BUFFER_SIZE & 0x03));

static uint8_t ep_buffer[EP_MAX_NUM][EP_BUFFER_SIZE] ALIGNED(4);

static Event usb_event_done[EP_MAX_SLOTS];

/* Check if we're running in atomic (non-sleepable) context or not */
static volatile bool in_atomic = false;

/* Allocate a free block of the packet memory */
static stm32_UsbMemSlot *usb_malloc(void)
{
	unsigned int i;

	for (i = 0; i < countof(memory_buffer); i++)
		if (memory_buffer[i].Size == 0)
			return &memory_buffer[i];
	return NULL;
}

/* Release a block of the packet memory */
static void usb_free(stm32_UsbMemSlot *pPntr)
{
	pPntr->Size = 0;
}

/* Allocate a free chunk of the packet memory (inside a block) */
static bool usb_alloc_buffer(uint16_t *pOffset, uint32_t *size,
                            int EndPoint)
{
	stm32_UsbMemSlot *mem = mem_use,
			*memNext, *mem_useNew;
	uint32_t max_size = *size;

	/*
	 * Packet size alignment:
	 *  - fine-granularity allocation: size alignment by 2;
	 *  - coarse-granularity allocation: size alignment by 32.
	 */
	if (max_size < 62)
		max_size = ALIGN_UP(max_size, 2);
	else
		max_size = ALIGN_UP(max_size, 32);
	/*
	 * Finding free memory chunks from the allocated blocks of the USB
	 * packet memory.
	 */
	*pOffset = 0;
	while (mem != NULL)
	{
		/* Offset alignment by 4 */
		*pOffset = ALIGN_UP(mem->Start + mem->Size, 4);
		memNext = mem->next;
		if ((mem->next == NULL) ||
				(memNext->Start >=
					*pOffset + max_size))
			break;
		mem = mem->next;
	}
	/* Check for out-of-memory condition */
	if (UNLIKELY((*pOffset + max_size) >= USB_BDT_OFFSET))
		return false;
	/*
	 * Allocate a new memory block, next to the last allocated block.
	 */
	mem_useNew = usb_malloc();
	if (UNLIKELY(mem_useNew == NULL))
		return false;
	/* Insert the block to the list of allocated blocks */
	if (mem_use == NULL)
	{
		mem_use = mem_useNew;
		mem_use->next = NULL;
	}
	else
	{
		mem_useNew->next = mem->next;
		mem->next = mem_useNew;
	}
	/* Update block's metadata */
	mem_useNew->ep_addr = EndPoint;
	mem_useNew->Start = *pOffset;
	mem_useNew->Size = max_size;

	*size = max_size;

	return true;
}

/* Release a chunk of the packet memory (inside a block) */
static void usb_free_buffer(int EndPoint)
{
	stm32_UsbMemSlot *mem, *memPrev = NULL;
	mem = mem_use;

	while (mem != NULL)
	{
		if (mem->ep_addr == EndPoint)
		{
			if (UNLIKELY(memPrev == NULL))
			{
				/* Free the first element of the list */
				mem_use = mem_use->next;
				usb_free(mem);
				mem = mem_use;
				continue;
			}
			memPrev->next = mem->next;
			usb_free(mem);
		}
		else
			memPrev = mem;
		mem = memPrev->next;
	}
}

/*-------------------------------------------------------------------------*/

/* Connect USB controller */
static void usb_connect(void)
{
	stm32_gpioPinWrite((struct stm32_gpio *)GPIOC_BASE, 1 << 11, 0);
}

/* Set USB device address */
static void usb_set_address(uint32_t addr)
{
	usb->DADDR = addr | 0x80;
}

/* Suspend USB controller */
static void usb_suspend(void)
{
	usb->CNTR |= bmFSUSP | bmLPMODE;
}

/* Resume USB controller */
static void usb_resume(void)
{
	uint32_t line_status;

	line_status = usb->FNR & 0xc000;
	if (!line_status)
		return;
	/* check for noise and eventually return to sleep */
	if (line_status == 0xc000)
		usb_suspend();
	else
		usb->CNTR &= ~(bmFSUSP | bmLPMODE);
}

/* Convert logical EP address to physical EP address */
static int usb_ep_logical_to_hw(uint8_t ep_addr)
{
	int addr = (ep_addr & 0x0f) << 1;
	return (ep_addr & 0x80) ? addr + 1 : addr;
}

/* Set EP address */
static void ep_ctrl_set_ea(reg32_t *reg, uint32_t val)
{
	val &= 0x0f;
	val |= *reg & 0x0700;
	val |= USB_CTRL_CLEAR_ONLY_MASK;
	*reg = val;
}

/* Get EP IN status */
static uint32_t ep_ctrl_get_stat_tx(reg32_t *reg)
{
	return (*reg & (0x3UL << 4)) >> 4;
}

/* Set EP IN state */
static void ep_ctrl_set_stat_tx(reg32_t *reg, stm32_UsbEpState val)
{
	uint32_t state;
	int i;

	/*
	 * The EP can change state between read and write operations from VALID
	 * to NAK and result of set operation will be invalid.
	 */
	for (i = 0; i < 2; i++)
	{
		if (ep_ctrl_get_stat_tx(reg) == val)
			return;
		state = val;
		state <<= 4;
		state ^= *reg;
		state |= USB_CTRL_CLEAR_ONLY_MASK;
		/* Clear the toggle bits without STAT_TX (4,5) */
		state &= ~0x7040;
		*reg = state;
	}
}

/* Set EP DTOG_TX bit (IN) */
static void ep_ctrl_set_dtog_tx(reg32_t *reg, uint32_t val)
{
	val = val ? (*reg ^ (1UL << 6)) : *reg;
	/* Clear the toggle bits without DTOG_TX (6) */
	val &= ~0x7030;
	val |= USB_CTRL_CLEAR_ONLY_MASK;
	*reg = val;
}

/* Clear EP CTR_TX bit (IN) */
static void ep_ctrl_clr_ctr_tx(reg32_t *reg)
{
	uint32_t val = *reg;

	val &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 7);
	/* Set RX_CTR */
	val |= 1UL << 15;
	*reg = val;
}

/* Clear EP CTR_RX bit (OUT) */
static void ep_ctrl_clr_ctr_rx(reg32_t *reg)
{
	uint32_t val = *reg;
	val &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 15);
	/* Set TX_CTR */
	val |= 1UL << 7;
	*reg = val;
}

/* Set EP KIND bit */
static void ep_ctrl_set_ep_kind(reg32_t *reg, uint32_t val)
{
	val = val ? (1UL << 8) : 0;
	val |= *reg & ~(USB_CTRL_TOGGLE_MASK | (1UL << 8));
	val |= USB_CTRL_CLEAR_ONLY_MASK;
	*reg = val;
}

/* Set EP type */
static int ep_ctrl_set_ep_type(reg32_t *reg, uint8_t val)
{
	uint32_t type;

	if (UNLIKELY(val >= EP_TYPE_MAX))
	{
		ASSERT(0);
		return USB_INVAL_ERROR;
	}
	type = val;
	type <<= 9;
	type |= *reg & ~(USB_CTRL_TOGGLE_MASK | (0x3UL << 9));
	type |= USB_CTRL_CLEAR_ONLY_MASK;
	*reg = type;

	return USB_OK;
}

/* Get EP STAT_RX (OUT) */
static uint32_t ep_ctrl_get_stat_rx(reg32_t *reg)
{
	uint32_t val = *reg & (0x3UL << 12);
	return val >> 12;
}

/* Set EP STAT_RX (OUT) */
static void ep_ctrl_set_stat_rx(reg32_t *reg, stm32_UsbEpState val)
{
	uint32_t state;
	int i;

	/*
	 * The EP can change state between read and write operations from VALID
	 * to NAK and result of set operation will be invalid.
	 */
	for (i = 0; i < 2; i++)
	{
		if (ep_ctrl_get_stat_rx(reg) == val)
			return;
		state = val;
		state <<= 12;
		state ^= *reg;
		state |= USB_CTRL_CLEAR_ONLY_MASK;
		/* Clear the toggle bits without STAT_RX (12,13) */
		state &= ~0x4070;
		*reg = state;
	}
}

/* Set DTOG_RX bit */
static void ep_ctrl_set_dtog_rx(reg32_t *reg, uint32_t val)
{
	val = val ? (*reg ^ (1UL << 14)) : *reg;
	/* Clear the toggle bits without DTOG_RX (14) */
	val &= ~0x3070;
	val |= USB_CTRL_CLEAR_ONLY_MASK;
	*reg = val;
}

/* Get EP SETUP bit */
static uint32_t ep_ctrl_get_setup(reg32_t *reg)
{
	uint32_t val = *reg & (1UL << 11);
	return val ? 1 : 0;
}

/* Core endpoint I/O function */
static void __usb_ep_io(int EP)
{
	ssize_t Count, CountHold, Offset;
	uint32_t *pDst, *pSrc, Data;
	stm32_UsbEp *epd = &ep_cnfg[EP];

	if (UNLIKELY(epd->hw == NULL))
	{
		LOG_ERR("%s: invalid endpoint (EP%d-%s)\n",
				__func__,
				EP >> 1,
				(EP & 0x01) ? "IN" : "OUT");
		ASSERT(0);
		return;
	}
	if (epd->status != BEGIN_SERVICED && epd->status != NO_SERVICED)
		return;

	if (EP & 0x01)
	{
		/* EP IN */
		Count = epd->size - epd->offset;
		while (epd->avail_data)
		{
			if (!Count && !(epd->flags & STM32_USB_EP_ZERO_PACKET))
				break;

			/* Set Status */
			epd->status = BEGIN_SERVICED;
			/* Get data size */
			if ((epd->flags & STM32_USB_EP_ZERO_PACKET) &&
					(Count == epd->max_size))
				epd->flags |= STM32_USB_EP_ZERO_PACKET |
						STM32_USB_EP_ZERO_POSSIBLE;

			CountHold = Count = MIN(Count, epd->max_size);
			if (!Count)
				epd->flags |= STM32_USB_EP_ZERO_PACKET;
			Offset = epd->offset;
			epd->offset += Count;
			switch (epd->type)
			{
			case USB_ENDPOINT_XFER_CONTROL:
			case USB_ENDPOINT_XFER_INT:
				pDst = (uint32_t *)USB_MEM_ADDR(EP_DTB_READ(EP >> 1, ADDR_TX_OFFSET));
				break;
			case USB_ENDPOINT_XFER_BULK:
				pDst = (uint32_t *)USB_MEM_ADDR(EP_DTB_READ(EP >> 1, ADDR_TX_OFFSET));
				break;
			case USB_ENDPOINT_XFER_ISOC:
				LOG_ERR("%s: isochronous transfer not supported\n",
					__func__);
				/* Fallback to default */
			default:
				ASSERT(0);
				return;
			}

			/* Write data to packet memory buffer */
			while (Count)
			{
				Data = *(epd->write_buffer + Offset++);
				if (--Count)
				{
					Data |= (uint32_t)(*(epd->write_buffer + Offset++)) << 8;
					--Count;
				}
				*pDst++ = Data;
			}

			EP_DTB_WRITE(EP >> 1, COUNT_TX_OFFSET, CountHold);
			ep_ctrl_set_stat_tx(epd->hw, EP_VALID);

			--ep_cnfg[EP].avail_data;
			Count = epd->size - epd->offset;
		}
		if (!Count && !(epd->flags & STM32_USB_EP_ZERO_PACKET))
		{
			epd->status = COMPLETE;
			/* call callback function */
			if (epd->complete)
				epd->complete(EP);
		}
	}
	else
	{
		/* EP OUT */
		while (epd->avail_data)
		{
			/* Get data size and buffer pointer */
			switch (epd->type)
			{
			case USB_ENDPOINT_XFER_CONTROL:
			case USB_ENDPOINT_XFER_INT:
				/* Get received bytes number */
				Count = EP_DTB_READ(EP >> 1, COUNT_RX_OFFSET) & 0x3FF;
				/* Get address of the USB packet buffer for corresponding EP */
				pSrc = (uint32_t *)USB_MEM_ADDR(EP_DTB_READ(EP >> 1, ADDR_RX_OFFSET));
				break;
			case USB_ENDPOINT_XFER_BULK:
				/* Get received bytes number */
				Count = EP_DTB_READ(EP >> 1, COUNT_RX_OFFSET) & 0x3FF;
				/* Get address of the USB packet buffer for corresponding EP */
				pSrc = (uint32_t *)USB_MEM_ADDR(EP_DTB_READ(EP >> 1, ADDR_RX_OFFSET));
				break;
			case USB_ENDPOINT_XFER_ISOC:
				LOG_ERR("%s: isochronous transfer not supported\n",
					__func__);
				/* Fallback to default */
			default:
				ASSERT(0);
				return;
			}

			if (Count > (epd->size - epd->offset))
			{
				epd->status = BUFFER_OVERRUN;
				epd->size = ep_cnfg[EP].offset;
				break;
			}
			else if (Count < ep_cnfg[EP].max_size)
			{
				epd->status = BUFFER_UNDERRUN;
				epd->size = ep_cnfg[EP].offset + Count;
			}
			else
				epd->status = BEGIN_SERVICED;

			Offset = epd->offset;
			epd->offset += Count;

			/* Read data from packet memory buffer */
			while (Count)
			{
				Data = *pSrc++;
				*(epd->read_buffer + Offset++) = Data;
				if (--Count)
				{
					Data >>= 8;
					*(epd->read_buffer + Offset++) = Data;
					--Count;
				}
			}

			ep_ctrl_set_stat_rx(epd->hw, EP_VALID);

			--ep_cnfg[EP].avail_data;

			if (*epd->hw & (1UL << 11))
			{
				ep_cnfg[EP].status = SETUP_OVERWRITE;
				return;
			}
			if (!(Count = (epd->size - epd->offset)))
			{
				epd->status = COMPLETE;
				break;
			}
		}
		if (epd->status != BEGIN_SERVICED && epd->status != NO_SERVICED)
		{
			/* call callback function */
			if (epd->complete)
				epd->complete(EP);
		}
	}
}

/*
 * Return the lower value from Host expected size and size and set a flag
 * STM32_USB_EP_ZERO_POSSIBLE when size is lower that host expected size.
 */
static size_t usb_size(size_t size, size_t host_size)
{
	if (size < host_size)
	{
		ep_cnfg[CTRL_ENP_IN].flags |= STM32_USB_EP_ZERO_POSSIBLE;
		return size;
	}
	return host_size;
}

/* Configure an EP descriptor before performing a I/O operation */
#define USB_EP_IO(__EP, __op, __buf, __size, __complete)		\
({									\
	cpu_flags_t flags;						\
	stm32_UsbIoStatus ret;					\
									\
	/* Fill EP descriptor */					\
	IRQ_SAVE_DISABLE(flags);					\
	if (__size < 0)							\
	{								\
		ep_cnfg[__EP].status = NOT_READY;			\
		ep_cnfg[__EP].complete = NULL;				\
		ret = NOT_READY;					\
		goto out;						\
	}								\
	if (ep_cnfg[__EP].status == BEGIN_SERVICED)			\
	{								\
		ret = NOT_READY;					\
		goto out;						\
	}								\
	/*								\
	 * NOTE: the write_buffer and read_buffer are actually the	\
	 * same	location in memory (it's a union).			\
	 *								\
	 * We have to do this trick to silent a build warning by	\
	 * casting the I/O buffer to (void *) or (const void *).	\
	 */								\
	ep_cnfg[__EP].__op ## _buffer = __buf;				\
	ep_cnfg[__EP].offset = 0;					\
	ep_cnfg[__EP].size = __size;					\
	ep_cnfg[__EP].complete = __complete;				\
	if (!size)							\
		ep_cnfg[__EP].flags = STM32_USB_EP_ZERO_PACKET;		\
	else								\
		ep_cnfg[__EP].flags = 0;				\
	ep_cnfg[__EP].status = NO_SERVICED;				\
									\
	/* Perform the I/O operation */					\
	__usb_ep_io(__EP);						\
									\
	ret = ep_cnfg[__EP].status;					\
out:									\
	IRQ_RESTORE(flags);						\
	ret;								\
})

/* Configure and endponint and perform a read operation */
static stm32_UsbIoStatus
__usb_ep_read(int ep, void *buffer, ssize_t size, void (*complete)(int))
{
	if (UNLIKELY((ep >= EP_MAX_NUM) || (ep & 0x01)))
	{
		LOG_ERR("%s: invalid EP number %d\n", __func__, ep);
		ASSERT(0);
		return STALLED;
	}
	if (UNLIKELY((size_t)buffer & 0x03))
	{
		LOG_ERR("%s: unaligned buffer @ %p\n", __func__, buffer);
		ASSERT(0);
		return STALLED;
	}
	return USB_EP_IO(ep, read, buffer, size, complete);
}

/* Configure and endponint and perform a write operation */
static stm32_UsbIoStatus
__usb_ep_write(int ep, const void *buffer, ssize_t size, void (*complete)(int))
{
	if (UNLIKELY((ep >= EP_MAX_NUM) || !(ep & 0x01)))
	{
		LOG_ERR("%s: invalid EP number %d\n", __func__, ep);
		ASSERT(0);
		return STALLED;
	}
	if (UNLIKELY((size_t)buffer & 0x03))
	{
		LOG_ERR("%s: unaligned buffer @ %p\n", __func__, buffer);
		ASSERT(0);
		return STALLED;
	}
	return USB_EP_IO(ep, write, buffer, size, complete);
}

static void usb_ep_low_level_config(int ep, uint16_t offset, uint16_t size)
{
	stm32_UsbEp *epc = &ep_cnfg[ep];

	/* IN EP */
	if (ep & 0x01)
	{
		/* Disable EP */
		ep_ctrl_set_stat_tx(epc->hw, EP_DISABLED);
		/* Clear Tx toggle */
		ep_ctrl_set_dtog_tx(epc->hw, 0);
		/* Clear Correct Transfer for transmission flag */
		ep_ctrl_clr_ctr_tx(epc->hw);

		/* Update EP description table */
		EP_DTB_WRITE(ep >> 1, ADDR_TX_OFFSET, offset);
		EP_DTB_WRITE(ep >> 1, COUNT_TX_OFFSET, 0);
	}
	/* OUT EP */
	else
	{
		uint16_t rx_count = 0;

		/* Disable EP */
		ep_ctrl_set_stat_rx(epc->hw, EP_DISABLED);
		/* Clear Rx toggle */
		ep_ctrl_set_dtog_rx(epc->hw, 0);
		/* Clear Correct Transfer for reception flag */
		ep_ctrl_clr_ctr_rx(epc->hw);
		/* Descriptor block size field */
		rx_count |= (size > 62) << 15;
		/* Descriptor number of blocks field */
		rx_count |= (((size > 62) ?  (size >> 5) - 1 : size >> 1) &
				0x1f) << 10;
		/* Update EP description table */
		EP_DTB_WRITE(ep >> 1, ADDR_RX_OFFSET, offset);
		EP_DTB_WRITE(ep >> 1, COUNT_RX_OFFSET, rx_count);
	}
}

/* Enable/Disable an endpoint */
static int usb_ep_configure(const UsbEndpointDesc *epd, bool enable)
{
	int EP;
	stm32_UsbEp *ep_hw;
	reg32_t *hw;
	uint16_t Offset;
	uint32_t size;

	EP = usb_ep_logical_to_hw(epd->bEndpointAddress);
	ep_hw = &ep_cnfg[EP];

	if (enable)
	{
		/*
		 * Allocate packet memory for EP buffer/s calculate actual size
		 * only for the OUT EPs.
		 */
		size = epd->wMaxPacketSize;
		if (!usb_alloc_buffer(&Offset, &size, EP))
			return -USB_MEMORY_FULL;

		/* Set EP status */
		ep_hw->status  = NOT_READY;
		/* Init EP flags */
		ep_hw->flags = 0;

		/* Set endpoint type */
		ep_hw->type = usb_endpointType(epd);
		/* Init EP max packet size */
		ep_hw->max_size = epd->wMaxPacketSize;

		if (EP & 0x01)
			ep_hw->avail_data = 1;
		else
			ep_hw->avail_data = 0;
		hw = (reg32_t *)&usb->EP0R;
		hw += EP >> 1;

		/* Set Ep Address */
		ep_ctrl_set_ea(hw, EP >> 1);
		ep_hw->hw = hw;
		LOG_INFO("%s: EP%d-%s configured\n",
				__func__, EP >> 1, EP & 1 ? "IN" : "OUT");

		/* Low-level endpoint configuration */
		usb_ep_low_level_config(EP, Offset, size);

		/* Set EP Kind & enable */
		switch (ep_hw->type)
		{
		case USB_ENDPOINT_XFER_CONTROL:
			LOG_INFO("EP%d: CONTROL %s\n", EP >> 1,
					EP & 1 ? "IN" : "OUT");
			ep_ctrl_set_ep_type(hw, EP_CTRL);
			ep_ctrl_set_ep_kind(hw, 0);
			break;
		case USB_ENDPOINT_XFER_INT:
			LOG_INFO("EP%d: INTERRUPT %s\n", EP >> 1,
					EP & 1 ? "IN" : "OUT");
			ep_ctrl_set_ep_type(hw, EP_INTERRUPT);
			ep_ctrl_set_ep_kind(hw, 0);
			break;
		case USB_ENDPOINT_XFER_BULK:
			LOG_INFO("EP%d: BULK %s\n", EP >> 1,
					EP & 1 ? "IN" : "OUT");
			ep_ctrl_set_ep_type(hw, EP_BULK);
			ep_ctrl_set_ep_kind(hw, 0);
			break;
		case USB_ENDPOINT_XFER_ISOC:
			LOG_ERR("EP%d: ISOCHRONOUS %s: not supported\n",
					EP >> 1,
					EP & 1 ? "IN" : "OUT");
			/* Fallback to default */
		default:
			ASSERT(0);
			return -USB_NODEV_ERROR;
		}
		if (EP & 0x01)
		{
			/* Enable EP */
			ep_ctrl_set_stat_tx(hw, EP_NAK);
			/* Clear Correct Transfer for transmission flag */
			ep_ctrl_clr_ctr_tx(hw);
		}
		else
		{
			/* Enable EP */
			ep_ctrl_set_stat_rx(hw, EP_VALID);
		}
	}
	else if (ep_cnfg[EP].hw)
	{
		hw = (reg32_t *)&usb->EP0R;
		hw += EP >> 1;

		/* IN EP */
		if (EP & 0x01)
		{
			/* Disable IN EP */
			ep_ctrl_set_stat_tx(hw, EP_DISABLED);
			/* Clear Correct Transfer for reception flag */
			ep_ctrl_clr_ctr_tx(hw);
		}
		/* OUT EP */
		else
		{
			/* Disable OUT EP */
			ep_ctrl_set_stat_rx(hw, EP_DISABLED);
			/* Clear Correct Transfer for reception flag */
			ep_ctrl_clr_ctr_rx(hw);
		}
		/* Release buffer */
		usb_free_buffer(EP);
		ep_cnfg[EP].hw = NULL;
	}
	return 0;
}

/* Get EP stall/unstall */
static int usb_ep_get_stall(int EP, bool *pStall)
{
	if (ep_cnfg[EP].hw == NULL)
		return -USB_NODEV_ERROR;

	*pStall = (EP & 0x01) ?
		(ep_ctrl_get_stat_tx(ep_cnfg[EP].hw) == EP_STALL):  /* IN EP  */
		(ep_ctrl_get_stat_rx(ep_cnfg[EP].hw) == EP_STALL);  /* OUT EP */

	return USB_OK;
}

/* Set EP stall/unstall */
static int usb_ep_set_stall(int EP, bool Stall)
{
	if (ep_cnfg[EP].hw == NULL)
		return -USB_NODEV_ERROR;

	if (Stall)
	{
		ep_cnfg[EP].status = STALLED;
		if (EP & 0x01)
		{
			/* IN EP */
			ep_ctrl_set_stat_tx(ep_cnfg[EP].hw, EP_STALL);
			ep_cnfg[EP].avail_data = 1;
		}
		else
		{
			/* OUT EP */
			ep_ctrl_set_stat_rx(ep_cnfg[EP].hw, EP_STALL);
			ep_cnfg[EP].avail_data = 0;
		}
	}
	else
	{
		ep_cnfg[EP].status = NOT_READY;
		if(EP & 0x01)
		{
			/* IN EP */
			ep_cnfg[EP].avail_data = 1;
			/* reset Data Toggle bit */
			ep_ctrl_set_dtog_tx(ep_cnfg[EP].hw, 0);
			ep_ctrl_set_stat_tx(ep_cnfg[EP].hw, EP_NAK);
		}
		else
		{
			/* OUT EP */
			ep_cnfg[EP].avail_data = 0;
			/* reset Data Toggle bit */
			ep_ctrl_set_dtog_rx(ep_cnfg[EP].hw, 0);
			ep_ctrl_set_stat_rx(ep_cnfg[EP].hw, EP_VALID);
		}
	}
	return USB_OK;
}

/* Stall both directions of the control EP */
static void usb_ep_set_stall_ctrl(void)
{
	ep_cnfg[CTRL_ENP_IN].avail_data = 1;
	ep_cnfg[CTRL_ENP_IN].status = STALLED;
	ep_cnfg[CTRL_ENP_OUT].avail_data = 0;
	ep_cnfg[CTRL_ENP_OUT].status = STALLED;

	usb_ep_set_stall(CTRL_ENP_IN, true);
	usb_ep_set_stall(CTRL_ENP_OUT, true);
}

/*
 * Find the position of an interface descriptor inside the configuration
 * descriptor.
 */
static int usb_find_interface(uint32_t num, uint32_t alt)
{
	const UsbInterfaceDesc *id;
	int i;

	for (i = 0; ; i++)
	{
		/* TODO: support more than one configuration per device */
		id = (const UsbInterfaceDesc *)usb_dev->config[i];
		if (id == NULL)
			break;
		if (id->bDescriptorType != USB_DT_INTERFACE)
			continue;
		if ((id->bInterfaceNumber == num) &&
				(id->bAlternateSetting == alt))
			return i;
	}
	return -USB_NODEV_ERROR;
}

/*
 * Configure/deconfigure EPs of a certain interface.
 */
static void
usb_configure_ep_interface(unsigned int num, unsigned int alt, bool enable)
{
	const UsbEndpointDesc *epd;
	int i, start;

	/*
	 * Find the position of the interface descriptor (inside the
	 * configuration descriptor).
	 */
	start = usb_find_interface(num, alt);
	if (start < 0)
	{
		LOG_ERR("%s: interface (%u,%u) not found\n",
			__func__, num, alt);
		return;
	}
	/*
	 * Cycle over endpoint descriptors.
	 *
	 * NOTE: the first endpoint descriptor is placed next to the interface
	 * descriptor, so we need to add +1 to the position of the interface
	 * descriptor to find it.
	 */
	for (i = start + 1; ; i++)
	{
		epd = (const UsbEndpointDesc *)usb_dev->config[i];
		if ((epd == NULL) || (epd->bDescriptorType == USB_DT_INTERFACE))
			break;
		if (epd->bDescriptorType != USB_DT_ENDPOINT)
			continue;
		if (UNLIKELY(usb_ep_configure(epd, enable) < 0))
		{
			LOG_ERR("%s: out of memory, can't initialize EP\n",
				__func__);
			return;
		}
	}
}

/* Set device state */
static void usb_set_device_state(int state)
{
	unsigned int i;

	LOG_INFO("%s: new state %d\n", __func__, state);

	if (udc.state == USB_STATE_CONFIGURED)
	{
		/* Deconfigure device */
		for (i = 0; i < udc.interfaces; ++i)
			usb_configure_ep_interface(i,
				udc.alt[i], false);
	}
	switch (state)
	{
	case USB_STATE_ATTACHED:
	case USB_STATE_POWERED:
	case USB_STATE_DEFAULT:
		usb_set_address(0);
		usb_dev->configured = false;
		udc.address = udc.cfg_id = 0;
		break;
	case USB_STATE_ADDRESS:
		udc.cfg_id = 0;
		break;
	case USB_STATE_CONFIGURED:
		/* Configure device */
		for (i = 0; i < udc.interfaces; ++i)
			usb_configure_ep_interface(i,
				udc.alt[i], true);
		break;
	default:
		/* Unknown state: disconnected or connection in progress */
		usb_dev->configured = false;
		udc.address = 0;
		udc.cfg_id = 0;
		break;
	}
	udc.state = state;
}

/* Setup packet: set address status phase end handler */
static void usb_add_status_handler_end(UNUSED_ARG(int, EP))
{
	uint16_t w_value;

	w_value = usb_le16_to_cpu(setup_packet.wValue);
	udc.address = w_value & 0xff;
	usb_set_address(udc.address);

	if (udc.address)
		usb_set_device_state(USB_STATE_ADDRESS);
	else
		usb_set_device_state(USB_STATE_DEFAULT);

	__usb_ep_write(CTRL_ENP_IN, NULL, -1, NULL);
	__usb_ep_read(CTRL_ENP_OUT, NULL, -1, NULL);
}

/* Prepare status phase */
static void usb_status_phase(bool in)
{
	if (in)
		__usb_ep_write(CTRL_ENP_IN, NULL, 0, NULL);
}

/* Setup packet: status phase end handler */
static void usb_status_handler_end(UNUSED_ARG(int, EP))
{
	__usb_ep_write(CTRL_ENP_IN, NULL, -1, NULL);
	__usb_ep_read(CTRL_ENP_OUT, NULL, -1, NULL);
}

/* Address status handler */
static void usb_status_handler(UNUSED_ARG(int, EP))
{
	if (setup_packet.mRequestType & USB_DIR_IN)
	{
		usb_status_phase(false);
		ep_cnfg[CTRL_ENP_OUT].complete = usb_status_handler_end;
	}
	else
	{
		usb_status_phase(true);
		ep_cnfg[CTRL_ENP_IN].complete =
			(setup_packet.bRequest == USB_REQ_SET_ADDRESS) ?
				usb_add_status_handler_end :
				usb_status_handler_end;
	}
}

static void usb_endpointRead_complete(int ep)
{
	if (UNLIKELY(ep >= EP_MAX_NUM))
	{
		ASSERT(0);
		return;
	}
	ASSERT(!(ep & 0x01));

	event_do(&usb_event_done[ep >> 1]);
	rx_size = ep_cnfg[ep].size;
}

ssize_t usb_endpointReadTimeout(int ep, void *buffer, ssize_t size,
				ticks_t timeout)
{
	int ep_num = usb_ep_logical_to_hw(ep);
	ssize_t max_size = sizeof(ep_buffer[ep_num]);

	/* Non-blocking read for EP0 */
	if (in_atomic && (ep_num == CTRL_ENP_OUT))
	{
		size = usb_size(size, usb_le16_to_cpu(setup_packet.wLength));
		if (UNLIKELY(size > max_size))
		{
			LOG_ERR("%s: ep_buffer exceeded, try to enlarge CONFIG_USB_BUFSIZE\n",
					__func__);
			ASSERT(0);
			return -USB_BUF_OVERFLOW;
		}
		if (!size)
			usb_status_handler(ep_num);
		else
		{
			__usb_ep_read(ep_num, ep_buffer[ep_num], size,
					usb_status_handler);
			memcpy(buffer, ep_buffer[ep_num], size);
		}
		return size;
	}
	if (UNLIKELY(!size))
		return 0;
	size = MIN(size, max_size);
	event_initGeneric(&usb_event_done[ep_num >> 1]);
	rx_size = 0;

	/* Blocking read */
	__usb_ep_read(ep_num, ep_buffer[ep_num], size,
				usb_endpointRead_complete);
	if (timeout < 0)
		event_wait(&usb_event_done[ep_num >> 1]);
	else
		if (!event_waitTimeout(&usb_event_done[ep_num >> 1], timeout))
			return 0;
	memcpy(buffer, ep_buffer[ep_num], rx_size);

	return rx_size;
}

static void usb_endpointWrite_complete(int ep)
{
	if (UNLIKELY(ep >= EP_MAX_NUM))
	{
		ASSERT(0);
		return;
	}
	ASSERT(ep & 0x01);

	event_do(&usb_event_done[ep >> 1]);
	tx_size = ep_cnfg[ep].size;
}

ssize_t usb_endpointWriteTimeout(int ep, const void *buffer, ssize_t size,
				ticks_t timeout)
{
	int ep_num = usb_ep_logical_to_hw(ep);
	ssize_t max_size = sizeof(ep_buffer[ep_num]);

	/* Non-blocking write for EP0 */
	if (in_atomic && (ep_num == CTRL_ENP_IN))
	{
		size = usb_size(size, usb_le16_to_cpu(setup_packet.wLength));
		if (UNLIKELY(size > max_size))
		{
			LOG_ERR("%s: ep_buffer exceeded, try to enlarge CONFIG_USB_BUFSIZE\n",
					__func__);
			ASSERT(0);
			return -USB_BUF_OVERFLOW;
		}
		if (!size)
			usb_status_handler(ep_num);
		else
		{
			memcpy(ep_buffer[ep_num], buffer, size);
			__usb_ep_write(ep_num, ep_buffer[ep_num], size,
					usb_status_handler);
		}
		return size;
	}
	if (UNLIKELY(!size))
		return 0;
	size = MIN(size, max_size);
	event_initGeneric(&usb_event_done[ep_num >> 1]);
	tx_size = 0;

	/* Blocking write */
	memcpy(ep_buffer[ep_num], buffer, size);
	__usb_ep_write(ep_num, ep_buffer[ep_num], size,
				usb_endpointWrite_complete);
	if (timeout < 0)
		event_wait(&usb_event_done[ep_num >> 1]);
	else
		if (!event_waitTimeout(&usb_event_done[ep_num >> 1], timeout))
			return 0;

	return tx_size;
}

/* Global variable to handle the following non-blocking I/O operations */
static uint32_t InData;

/* Get device status */
static int usb_send_device_status(uint16_t index)
{
	if (index)
		return -USB_NODEV_ERROR;

	InData = ((uint32_t)udc.feature) & 0xff;
	__usb_ep_write(CTRL_ENP_IN,
			(uint8_t *)&InData, sizeof(uint16_t),
			usb_status_handler);
	return 0;
}

/* Get interface status */
static int usb_send_interface_status(UNUSED_ARG(uint16_t, index))
{
	InData = 0;
	__usb_ep_write(CTRL_ENP_IN,
			(uint8_t *)&InData, sizeof(uint16_t),
			usb_status_handler);
	return 0;
}

/* Get endpoint status */
static int usb_send_ep_status(uint16_t index)
{
	if ((index & 0x7F) > 16)
		return -USB_NODEV_ERROR;

	InData = 0;
	usb_ep_get_stall(usb_ep_logical_to_hw(index), (bool *)&InData);
	__usb_ep_write(CTRL_ENP_IN,
			(uint8_t *)&InData, sizeof(uint16_t),
			usb_status_handler);
	return 0;
}

/* USB setup packet: GET_STATUS request handler */
static void usb_get_status_handler(void)
{
	uint16_t w_value = usb_le16_to_cpu(setup_packet.wValue);
	uint16_t w_index = usb_le16_to_cpu(setup_packet.wIndex);
	uint16_t w_length = usb_le16_to_cpu(setup_packet.wLength);

	/* GET_STATUS sanity checks */
	if (udc.state < USB_STATE_ADDRESS)
	{
		LOG_WARN("%s: bad GET_STATUS request (State=%02x)\n",
				__func__, udc.state);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		return;
	}
	if (w_length != 2)
	{
		LOG_WARN("%s: bad GET_STATUS request (wLength.Word=%02x)\n",
				__func__, w_length);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		return;
	}
	if (!(setup_packet.mRequestType & USB_DIR_IN))
	{
		LOG_WARN("%s: bad GET_STATUS request (mRequestType=%02x)\n",
				__func__, setup_packet.mRequestType);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		return;
	}
	if (w_value)
	{
		LOG_WARN("%s: bad GET_STATUS request (wValue=%02x)\n",
				__func__, w_value);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		return;
	}

	/* Process GET_STATUS request */
	switch (setup_packet.mRequestType & USB_RECIP_MASK)
	{
	case USB_RECIP_DEVICE:
		if (usb_send_device_status(w_index) < 0)
		{
			LOG_WARN("%s: GET_STATUS: invalid UsbRecipientDevice\n",
					__func__);
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
			return;
		}
		LOG_INFO("%s: GET_STATUS: mRequestType=%02x (UsbRecipientDevice)\n",
				__func__, setup_packet.mRequestType);
		break;
	case USB_RECIP_INTERFACE:
		if (usb_send_interface_status(w_index) < 0)
		{
			LOG_WARN("%s: GET_STATUS: invalid UsbRecipientInterface\n",
					__func__);
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
			return;
		}
		LOG_INFO("%s: GET_STATUS: mRequestType=%02x (UsbRecipientInterface)\n",
				__func__, setup_packet.mRequestType);
		break;
	case USB_RECIP_ENDPOINT:
		if (usb_send_ep_status(w_index) < 0)
		{
			LOG_WARN("%s: GET_STATUS: invalid UsbRecipientEndpoint\n",
					__func__);
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
			return;
		}
		LOG_INFO("%s: GET_STATUS: mRequestType=%02x (UsbRecipientEndpoint)\n",
				__func__, setup_packet.mRequestType);
		break;
	default:
		LOG_WARN("%s: GET_STATUS: invalid UsbRecipientEndpoint\n",
				__func__);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	}
}

static int usb_get_device_descriptor(int id)
{
	if (id)
		return -USB_NODEV_ERROR;

	usb_dev->device->bMaxPacketSize0 = USB_EP0_MAX_SIZE;
	__usb_ep_write(CTRL_ENP_IN, (const uint8_t *)usb_dev->device,
			usb_size(usb_dev->device->bLength,
				usb_le16_to_cpu(setup_packet.wLength)),
			usb_status_handler);
	return 0;
}

/*
 * TODO: refactor this part to remove this temporary buffer.
 *
 * It would be better to define all the USB descriptors in the right order and
 * send them as a contiguous buffer directly from the flash / rodata memory.
 */
#define USB_BUFSIZE (128)
static uint8_t usb_cfg_buffer[USB_BUFSIZE];
STATIC_ASSERT(USB_BUFSIZE < (1 << (sizeof(uint16_t) * 8)));

static int usb_get_configuration_descriptor(int id)
{
	const UsbConfigDesc **config =
			(const UsbConfigDesc **)usb_dev->config;
	uint8_t *p = usb_cfg_buffer;
	int i;

	/* TODO: support more than one configuration per device */
	if (UNLIKELY(id > 0))
		return -USB_NODEV_ERROR;

	for (i = 0; config[i]; i++)
	{
		memcpy(p, config[i], config[i]->bLength);
		p += config[i]->bLength;

		if (UNLIKELY((p - usb_cfg_buffer) > USB_BUFSIZE))
		{
			ASSERT(0);
			return -USB_BUF_OVERFLOW;
		}
	}
	((UsbConfigDesc *)usb_cfg_buffer)->wTotalLength =
			usb_cpu_to_le16((uint16_t)(p - usb_cfg_buffer));
	__usb_ep_write(CTRL_ENP_IN,
			usb_cfg_buffer,
			usb_size(p - usb_cfg_buffer,
				usb_le16_to_cpu(setup_packet.wLength)),
			usb_status_handler);
	return 0;
}

static int usb_get_string_descriptor(unsigned int id)
{
	const UsbStringDesc *lang_str;
	unsigned int lang_id, str_id;
	uint16_t w_index_lo = usb_le16_to_cpu(setup_packet.wIndex) & 0x00ff;
	uint16_t w_index_hi = (usb_le16_to_cpu(setup_packet.wIndex) &
						0xff00) >> 8;

	ASSERT(usb_dev->strings != NULL);
	ASSERT(usb_dev->strings[0] != NULL);

	lang_str = usb_dev->strings[0];
	if (id)
	{
		/* Find Language index */
		for (lang_id = 0; ; lang_id++)
		{
			const UsbStringDesc *str =
						usb_dev->strings[lang_id];
			if (UNLIKELY(str == NULL))
				return -USB_NODEV_ERROR;
			if ((str->data[0] == w_index_lo) &&
					(str->data[1] == w_index_hi))
				break;
		}
		/* Check buffer overflow to find string index */
		for (str_id = 0; str_id < id; str_id++)
		{
			lang_str = usb_dev->strings[lang_id + 1 + str_id];
			if (lang_str == NULL)
				return -USB_NODEV_ERROR;
		}
	}
	__usb_ep_write(CTRL_ENP_IN,
			lang_str,
			usb_size(lang_str->bLength,
				usb_le16_to_cpu(setup_packet.wLength)),
			usb_status_handler);
	return 0;
}

static void usb_get_descriptor(void)
{
	uint16_t w_value_lo = usb_le16_to_cpu(setup_packet.wValue) & 0x00ff;
	uint16_t w_value_hi = (usb_le16_to_cpu(setup_packet.wValue) & 0xff00) >> 8;

	if (udc.state < USB_STATE_DEFAULT)
	{
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		return;
	}
	switch (w_value_hi)
	{
	case USB_DT_DEVICE:
		LOG_INFO("%s: GET_DEVICE_DESCRIPTOR: id=%d, state=%d\n",
				__func__,
				w_value_lo,
				udc.state);
		if (usb_get_device_descriptor(w_value_lo) < 0)
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	case USB_DT_CONFIG:
		LOG_INFO("%s: GET_CONFIG_DESCRIPTOR: id=%d, state=%d\n",
				__func__, w_value_lo, udc.state);
		if (usb_get_configuration_descriptor(w_value_lo) < 0)
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	case USB_DT_STRING:
		LOG_INFO("%s: GET_STRING_DESCRIPTOR: id=%d, state=%d\n",
				__func__, w_value_lo, udc.state);
		if (usb_get_string_descriptor(w_value_lo) < 0)
			ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	default:
		LOG_WARN("%s: GET_UNKNOWN_DESCRIPTOR: id=%d, state=%d\n",
				__func__, w_value_lo, udc.state);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	}
}

/* USB setup packet: class/vendor request handler */
static void usb_event_handler(UsbDevice *dev)
{
	/*
	 * TODO: get the appropriate usb_dev in function of the endpoint
	 * address.
	 */
	if (dev->event_cb)
		dev->event_cb(&setup_packet);
}

/* USB setup packet: GET_DESCRIPTOR handler */
static void usb_get_descriptor_handler(void)
{
	LOG_INFO("%s: GET_DESCRIPTOR: RECIP = %d\n",
			__func__,
			setup_packet.mRequestType & USB_RECIP_MASK);
	if ((setup_packet.mRequestType & USB_RECIP_MASK) ==
			USB_RECIP_DEVICE)
		usb_get_descriptor();
	else
		usb_event_handler(usb_dev);
}

/* USB setup packet: SET_ADDRESS handler */
static void usb_set_address_handler(void)
{
	uint16_t w_value = usb_le16_to_cpu(setup_packet.wValue);
	uint16_t w_index = usb_le16_to_cpu(setup_packet.wIndex);
	uint16_t w_length = usb_le16_to_cpu(setup_packet.wLength);

	LOG_INFO("%s: SET_ADDRESS: %d\n",
			__func__, usb_le16_to_cpu(setup_packet.wValue));
	if ((udc.state >= USB_STATE_DEFAULT) &&
			((setup_packet.mRequestType & USB_RECIP_MASK) ==
					USB_RECIP_DEVICE) &&
			(w_index == 0) && (w_length == 0) && (w_value < 128))
		usb_status_handler(CTRL_ENP_IN);
	else
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
}

/* USB setup packet: GET_CONFIGURATION handler */
static void usb_get_config_handler(void)
{
	uint16_t w_value = usb_le16_to_cpu(setup_packet.wValue);
	uint16_t w_index = usb_le16_to_cpu(setup_packet.wIndex);

	LOG_INFO("%s: GET_CONFIGURATION\n", __func__);
	if ((udc.state >= USB_STATE_ADDRESS) &&
			(w_value == 0) && (w_index == 0) && (w_value == 1))
	{
		InData = udc.cfg_id;
		__usb_ep_write(CTRL_ENP_IN, (uint8_t *)&InData, 1, usb_status_handler);
	}
	else
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
}

static const UsbConfigDesc *usb_find_configuration(int num)
{
	const UsbConfigDesc *cfg;
	int i;

	for (i = 0; ; i++)
	{
		cfg = (const UsbConfigDesc *)usb_dev->config[i];
		if (cfg == NULL)
			break;
		if (cfg->bDescriptorType != USB_DT_CONFIG)
			continue;
		if (cfg->bConfigurationValue == num)
			return cfg;
	}
	return NULL;
}

static int usb_set_config_state(uint32_t conf)
{
	const UsbConfigDesc *pCnfg;
	unsigned int i;

	if (conf)
	{
		/* Find configuration descriptor */
		pCnfg = usb_find_configuration(conf);
		if (pCnfg == NULL)
			return -USB_NODEV_ERROR;

		/* Reset current configuration */
		usb_set_device_state(USB_STATE_ADDRESS);
		usb_dev->configured = false;
		udc.cfg = pCnfg;

		/* Set Interface and Alternative Setting */
		udc.cfg_id = conf;
		/* Set self-powered state */
		if (pCnfg->bmAttributes & USB_CONFIG_ATT_SELFPOWER)
			udc.feature |= STM32_UDC_FEATURE_SELFPOWERED;

		/* Configure all existing interfaces to alternative setting 0 */
		ASSERT(pCnfg->bNumInterfaces <= USB_MAX_INTERFACE);
		udc.interfaces = pCnfg->bNumInterfaces;
		for (i = 0; i < udc.interfaces; i++)
			udc.alt[i] = 0;
		usb_set_device_state(USB_STATE_CONFIGURED);
		usb_dev->configured = true;
		event_do(&usb_event_done[0]);
		LOG_INFO("%s: device configured\n", __func__);
	}
	else
	{
		usb_dev->configured = false;
		usb_set_device_state(USB_STATE_ADDRESS);
	}
	return 0;
}

/* USB setup packet: SET_CONFIGURATION handler */
static void usb_set_config_handler(void)
{
	uint16_t w_value = usb_le16_to_cpu(setup_packet.wValue);
	uint16_t w_index = usb_le16_to_cpu(setup_packet.wIndex);
	uint16_t w_length = usb_le16_to_cpu(setup_packet.wLength);

	LOG_INFO("%s: SET_CONFIGURATION: %d\n",
			__func__, w_value);
	if ((udc.state >= USB_STATE_ADDRESS) &&
			(w_index == 0) && (w_length == 0) &&
			(usb_set_config_state(w_value & 0xff) == 0))
		usb_status_handler(CTRL_ENP_OUT);
	else
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
}

/* USB setup packet: standard request handler */
static void usb_standard_request_handler(void)
{
	switch (setup_packet.bRequest)
	{
	case USB_REQ_GET_STATUS:
		usb_get_status_handler();
		break;
	case USB_REQ_CLEAR_FEATURE:
		LOG_INFO("%s: bRequest=%d (CLEAR_FEATURE)\n",
				__func__, setup_packet.bRequest);
		break;
	case USB_REQ_SET_FEATURE:
		LOG_INFO("%s: bRequest=%d (SET_FEATURE)\n",
				__func__, setup_packet.bRequest);
		break;
	case USB_REQ_SET_ADDRESS:
		usb_set_address_handler();
		break;
	case USB_REQ_GET_DESCRIPTOR:
		usb_get_descriptor_handler();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		LOG_INFO("%s: bRequest=%d (SET_DESCRIPTOR)\n",
				__func__, setup_packet.bRequest);
		break;
	case USB_REQ_GET_CONFIGURATION:
		usb_get_config_handler();
		break;
	case USB_REQ_SET_CONFIGURATION:
		usb_set_config_handler();
		break;
	case USB_REQ_GET_INTERFACE:
		LOG_INFO("%s: bRequest=%d (GET_INTERFACE)\n",
				__func__, setup_packet.bRequest);
		break;
	case USB_REQ_SET_INTERFACE:
		LOG_INFO("%s: bRequest=%d (SET_INTERFACE)\n",
				__func__, setup_packet.bRequest);
		break;
	case USB_REQ_SYNCH_FRAME:
		LOG_INFO("%s: bRequest=%d (SYNCH_FRAME)\n",
				__func__, setup_packet.bRequest);
		break;
	default:
		LOG_WARN("%s: bRequest=%d (Unknown)\n",
				__func__, setup_packet.bRequest);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	}
}

/* USB setup packet handler */
static void usb_setup_handler(void)
{
	switch (setup_packet.mRequestType & USB_TYPE_MASK)
	{
	/* Standard */
	case USB_TYPE_STANDARD:
		LOG_INFO("%s: bmRequestType=%02x (Standard)\n",
				__func__, setup_packet.mRequestType);
		usb_standard_request_handler();
		break;
	/* Class */
	case USB_TYPE_CLASS:
		LOG_INFO("%s: bmRequestType=%02x (Class)\n",
				__func__, setup_packet.mRequestType);
		usb_event_handler(usb_dev);
		break;
	/* Vendor */
	case USB_TYPE_VENDOR:
		LOG_INFO("%s: bmRequestType=%02x (Vendor)\n",
				__func__, setup_packet.mRequestType);
		usb_event_handler(usb_dev);
		break;
	case USB_TYPE_RESERVED:
		LOG_INFO("%s: bmRequestType=%02x (Reserved)\n",
				__func__, setup_packet.mRequestType);
		break;
	/* Other */
	default:
		LOG_WARN("%s: bmRequestType=%02x (Unknown)\n",
				__func__, setup_packet.mRequestType);
		ep_cnfg[CTRL_ENP_OUT].status = STALLED;
		break;
	}
}

/* USB: low-level hardware initialization */
static void usb_hw_reset(void)
{
	unsigned int i;
	int ret;

	/* Initialize endpoint descriptors */
	for (i = 0; i < countof(ep_cnfg); i++)
		ep_cnfg[i].hw = NULL;

	/* Initialize USB memory */
	for (i = 0; i < countof(memory_buffer); i++)
		memory_buffer[i].Size = 0;
	usb->BTABLE = USB_BDT_OFFSET;
	mem_use = NULL;

	/* Endpoint initialization */
	ret = usb_ep_configure(&USB_CtrlEpDescr0, true);
	if (UNLIKELY(ret < 0))
	{
		LOG_WARN("%s: out of memory, cannot initialize EP0\n",
				__func__);
		return;
	}
	ret = usb_ep_configure(&USB_CtrlEpDescr1, true);
	if (UNLIKELY(ret < 0))
	{
		LOG_WARN("%s: out of memory, cannot initialize EP1\n",
				__func__);
		return;
	}

	/* Set default address */
	usb_set_address(0);

	/* Enable all the device interrupts */
	usb->CNTR = bmCTRM | bmRESETM | bmSOFM | bmERRM | bmPMAOVRM |
			bmSUSPM | bmWKUPM;
}

/* Handle a correct transfer under ISR */
static void usb_isr_correct_transfer(stm32_usb_irq_status_t interrupt)
{
	int EP;
	reg32_t *pReg = (reg32_t *)&usb->EP0R;

	/* Find corresponding EP */
	pReg += interrupt.EP_ID;
	EP = (int)(((*pReg & 0x0f) << 1) + (interrupt.DIR ? 0 : 1));
	ep_cnfg[EP].avail_data = 1;

	ASSERT(ep_cnfg[EP].hw);
	/* IN EP */
	if (EP & 0x01)
		ep_ctrl_clr_ctr_tx(ep_cnfg[EP].hw);
	else
		ep_ctrl_clr_ctr_rx(ep_cnfg[EP].hw);
	if (EP == CTRL_ENP_OUT)
	{
		/* Determinate type of packet (only for control EP) */
		bool SetupPacket = ep_ctrl_get_setup(ep_cnfg[CTRL_ENP_OUT].hw);

		if (SetupPacket)
		{
			ep_cnfg[CTRL_ENP_IN].avail_data = 1;
			/* init IO to receive Setup packet */
			__usb_ep_write(CTRL_ENP_IN, NULL, -1, NULL);
			__usb_ep_read(CTRL_ENP_OUT, &setup_packet,
					sizeof(setup_packet), NULL);

			/* reset EP IO ctrl */
			if (setup_packet.mRequestType & USB_DIR_IN)
				usb_status_handler(CTRL_ENP_OUT);
			usb_setup_handler();
			if (ep_cnfg[CTRL_ENP_OUT].status == STALLED)
				usb_ep_set_stall_ctrl();
		}
		else
		{
			if (ep_cnfg[CTRL_ENP_OUT].complete &&
					setup_packet.mRequestType & USB_DIR_IN)
				ep_cnfg[CTRL_ENP_OUT].complete(CTRL_ENP_OUT);
			else
				__usb_ep_io(EP);
		}
	}
	else if (EP == CTRL_ENP_IN)
	{
		if (ep_cnfg[CTRL_ENP_IN].complete &&
				!(setup_packet.mRequestType & USB_DIR_IN))
			ep_cnfg[CTRL_ENP_IN].complete(CTRL_ENP_IN);
		else
			__usb_ep_io(EP);

	}
	else
		__usb_ep_io(EP);
}

/* USB: interrupt service routine */
static void usb_isr(void)
{
	stm32_usb_irq_status_t interrupt;

	/* Get masked interrupt flags */
	interrupt.status = usb->ISTR;
	interrupt.status &= usb->CNTR | 0x1f;

	/* Set the context as atomic */
	in_atomic = true;

	if (interrupt.PMAOVR)
	{
		LOG_WARN("%s: DMA overrun / underrun\n", __func__);
		usb->ISTR = ~bmPMAOVRM;
	}
	if (interrupt.ERR)
	{
		LOG_WARN("%s: engine error\n", __func__);
		usb->ISTR = ~bmERRM;
	}
	if (interrupt.RESET)
	{
		LOG_INFO("%s: device reset\n", __func__);
		usb->ISTR = ~bmRESETM;
		usb_hw_reset();
		usb_set_device_state(USB_STATE_DEFAULT);
	}
	if (interrupt.SOF)
	{
#if 0
		/*
		 * XXX: disable logging of frame interrupts (too much noise!)
		 */
		uint16_t frame_nr = usb->FNR & 0x0fff;
		LOG_INFO("%s: frame %#x\n", __func__, frame_nr);
#endif
		usb->ISTR = ~bmSOFM;
	}
	if (interrupt.WKUP)
	{
		LOG_INFO("%s: wake-up\n", __func__);
		usb->ISTR = ~(bmSUSPM | bmWKUPM);
		usb_resume();
	}
	if (interrupt.SUSP)
	{
		LOG_INFO("%s: suspend\n", __func__);
		usb_suspend();
		usb->ISTR = ~(bmSUSPM | bmWKUPM);
	}
	if (interrupt.ESOF)
	{
		LOG_INFO("%s: expected frame\n", __func__);
		usb->ISTR = ~bmESOFM;
	}
	if (interrupt.CTR)
	{
		usb_isr_correct_transfer(interrupt);
	}
	in_atomic = false;
}

/* USB: hardware initialization */
static void usb_hw_init(void)
{
	/* Enable clocking on the required GPIO pins */
	RCC->APB2ENR |= RCC_APB2_GPIOA | RCC_APB2_GPIOC;

	/* Make sure that the CAN controller is disabled and held in reset */
	RCC->APB1ENR &= ~RCC_APB1_CAN;

	/* Configure USB_DM and USB_DP to work as USB lines */
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOA_BASE,
			USB_DM_PIN | USB_DP_PIN,
			GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
	/* Configure USB_DISC to work as USB disconnect */
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOC_BASE,
			USB_DISC_PIN,
			GPIO_MODE_OUT_PP, GPIO_SPEED_50MHZ);
	stm32_gpioPinWrite((struct stm32_gpio *)GPIOC_BASE,
				USB_DISC_PIN, 1);

	/* Ensure the USB clock is disabled before setting the prescaler */
	RCC->APB1ENR &= ~RCC_APB1_USB;

	/* Configure USB clock (48MHz) */
	*CFGR_USBPRE_BB &= ~RCC_USBCLK_PLLCLK_1DIV5;

	/* Activate USB clock */
	RCC->APB1ENR |= RCC_APB1_USB;

	/* Force USB reset and disable USB interrupts */
	usb->CNTR = bmFRES;
	timer_delayHp(1);

	/* Issue a USB reset */
	usb_hw_reset();

	/* Clear spurious pending interrupt */
	usb->ISTR = 0;

	/* Register interrupt handler */
	sysirq_setHandler(USB_LP_CAN_RX0_IRQHANDLER, usb_isr);

	/* Software connection enable */
	usb_connect();
}

/* Initialize the USB controller */
static void usb_init(void)
{
	udc.state = USB_STATE_NOTATTACHED;
	udc.feature = 0;

	usb_hw_init();
}

/* Register an upper layer USB device into the driver */
int usb_deviceRegister(UsbDevice *dev)
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	usb_dev = dev;
	usb_dev->configured = false;

	event_initGeneric(&usb_event_done[0]);
	usb_init();
	event_wait(&usb_event_done[0]);

	return 0;
}
