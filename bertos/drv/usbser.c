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
 * \brief Generic USB serial device driver.
 *
 */

#include "usbser.h"

#include "cfg/cfg_usbser.h"

#define LOG_LEVEL  USB_SERIAL_LOG_LEVEL
#define LOG_FORMAT USB_SERIAL_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

#include <cfg/compiler.h>
#include <cfg/module.h>

#include <cpu/irq.h> /* IRQ_DISABLE / IRQ_ENABLE */
#include <cpu/power.h> /* cpu_relax() */

#include <drv/usb.h>
#include <drv/usb_endpoint.h>

#include <string.h> /* memcpy() */


#define USB_SERIAL_INTERFACES	1
#define USB_SERIAL_ENDPOINTS	3

#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT	2
#define USB_STRING_SERIAL	3

static UsbDeviceDesc usb_serial_device_descriptor =
{
	.bLength = sizeof(usb_serial_device_descriptor),
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x110,
	.bDeviceClass = USB_CLASS_COMM,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.idVendor = USB_SERIAL_VENDOR_ID,
	.idProduct = USB_SERIAL_PRODUCT_ID,
	.bcdDevice = 0,
	.iManufacturer = USB_STRING_MANUFACTURER,
	.iProduct = USB_STRING_PRODUCT,
	.iSerialNumber = USB_STRING_SERIAL,
	.bNumConfigurations = 1,
};

static const UsbConfigDesc usb_serial_config_descriptor =
{
	.bLength = sizeof(usb_serial_config_descriptor),
	.bDescriptorType = USB_DT_CONFIG,
	.bNumInterfaces = USB_SERIAL_INTERFACES,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = USB_CONFIG_ATT_ONE,
	.bMaxPower = 50, /* 100 mA */
};

static const UsbInterfaceDesc usb_serial_interface_descriptor =
{
	.bLength = sizeof(usb_serial_interface_descriptor),
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = USB_SERIAL_ENDPOINTS,
	.bInterfaceClass = 0xff,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,
};

static const UsbEndpointDesc usb_serial_ep_report_descriptor =
{
	.bLength = sizeof(usb_serial_ep_report_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | USB_SERIAL_EP_REPORT,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = usb_cpu_to_le16((uint16_t)8),
	.bInterval = 1,
};

static const UsbEndpointDesc usb_serial_ep_in_descriptor =
{
	.bLength = sizeof(usb_serial_ep_in_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | USB_SERIAL_EP_IN,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_cpu_to_le16((uint16_t)64),
	.bInterval = 0,
};

static const UsbEndpointDesc usb_serial_ep_out_descriptor =
{
	.bLength = sizeof(usb_serial_ep_in_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_OUT | USB_SERIAL_EP_OUT,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_cpu_to_le16((uint16_t)64),
	.bInterval = 0,
};

static const UsbDescHeader *usb_serial_config[] =
{
	(const UsbDescHeader *)&usb_serial_config_descriptor,
	(const UsbDescHeader *)&usb_serial_interface_descriptor,
	(const UsbDescHeader *)&usb_serial_ep_report_descriptor,
	(const UsbDescHeader *)&usb_serial_ep_in_descriptor,
	(const UsbDescHeader *)&usb_serial_ep_out_descriptor,
	NULL,
};

static const DEFINE_USB_STRING(language_str, "\x09\x04"); // Language ID: en_US
static const DEFINE_USB_STRING(manufacturer_str,
		USB_STRING("B", "e", "R", "T", "O", "S"));
static const DEFINE_USB_STRING(product_str,
		USB_STRING("U", "S", "B", "-", "s", "e", "r", "i", "a", "l"));
static const DEFINE_USB_STRING(serial_str,
		USB_STRING("0", "0", "1"));

static const UsbStringDesc *usb_serial_strings[] =
{
	(const UsbStringDesc *)&language_str,
	(const UsbStringDesc *)&manufacturer_str,
	(const UsbStringDesc *)&product_str,
	(const UsbStringDesc *)&serial_str,
	NULL,
};

/* Global usb-serial descriptor that identifies the usb-serial device */
static UsbDevice usb_serial = {
	.device = &usb_serial_device_descriptor,
	.config = usb_serial_config,
	.strings = usb_serial_strings,
};

/* Low-level usb-serial device initialization */
static int usb_serial_hw_init(void)
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	if (usb_deviceRegister(&usb_serial) < 0)
		return -1;
	LOG_INFO("usb-serial: registered new USB interface driver\n");
	return 0;
}

/**
 * \brief Write a buffer to a usb-serial port.
 *
 * \return number of bytes actually written.
 */
static size_t usb_serial_write(struct KFile *fd,
			const void *buf, size_t size)
{
	DB(USBSerial *fds = USB_SERIAL_CAST(fd));

	/* Silent compiler warnings if _DEBUG is not enabled */
	(void)fd;
	ASSERT(fds->is_open);
	return usb_endpointWrite(usb_serial_ep_in_descriptor.bEndpointAddress,
				buf, size);
}

/**
 * Read at most \a size bytes from a usb-serial port and put them in \a buf
 *
 * \return number of bytes actually read.
 */
static size_t usb_serial_read(struct KFile *fd, void *buf, size_t size)
{
	DB(USBSerial *fds = USB_SERIAL_CAST(fd));

	/* Silent compiler warnings if _DEBUG is not enabled */
	(void)fd;
	ASSERT(fds->is_open);
	return usb_endpointRead(usb_serial_ep_out_descriptor.bEndpointAddress,
				buf, size);
}

/**
 * Return the status of a usb-serial port.
 *
 * \todo properly implement usb-serial error handling.
 */
static int usb_serial_error(struct KFile *fd)
{
	USBSerial *fds = USB_SERIAL_CAST(fd);
	return fds->status;
}

/**
 * Clear the status of a usb-serial port.
 *
 * \todo properly implement usb-serial error handling.
 */
static void usb_serial_clearerr(struct KFile *fd)
{
	USBSerial *fds = USB_SERIAL_CAST(fd);
	fds->status = 0;
}

/**
 * Close an USB serial port.
 */
static int usb_serial_close(struct KFile *fd)
{
	DB(USBSerial *fds = USB_SERIAL_CAST(fd));

	/* Silent compiler warnings if _DEBUG is not enabled */
	(void)fd;
	ASSERT(fds->is_open);
	DB(fds->is_open = false);
	return 0;
}

/**
 * Initialize an USB serial port.
 *
 * \param fds KFile Serial struct interface.
 * \param unit Serial unit to open.
 */
static int usb_serial_open(struct USBSerial *fds, int unit)
{
	unit = unit;
	ASSERT(!fds->is_open);
	/* TODO: only a single usb-serial unit is supported for now */
	ASSERT(unit == 0);

	/* Initialize usb-serial driver */
	if (usb_serial_hw_init() < 0)
		return -1;
	/* Clear error flags */
	fds->status = 0;
	DB(fds->is_open = true);

	return 0;
}

/**
 * Reopen a usb-serial port.
 */
static struct KFile *usb_serial_reopen(struct KFile *fd)
{
	USBSerial *fds = USB_SERIAL_CAST(fd);

	usb_serial_close(fd);
	usb_serial_open(fds, fds->unit);
	return 0;
}

/**
 * Init serial driver for a usb-serial port \a unit.
 *
 * \return 0 if OK, a negative value in case of error.
 */
int usbser_init(struct USBSerial *fds, int unit)
{
	memset(fds, 0, sizeof(*fds));

	DB(fds->fd._type = KFT_USB_SERIAL);
	fds->fd.reopen = usb_serial_reopen;
	fds->fd.close = usb_serial_close;
	fds->fd.read = usb_serial_read;
	fds->fd.write = usb_serial_write;
	/* TODO: properly implement error handling. */
	fds->fd.error = usb_serial_error;
	fds->fd.clearerr = usb_serial_clearerr;

	return usb_serial_open(fds, unit);
}
