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
 * \brief Generic USB keyboard device driver.
 *
 */

#include "usb_hid.h"
#include "usbkbd.h"

#include "cfg/cfg_usbkbd.h"

#define LOG_LEVEL  USB_KEYBOARD_LOG_LEVEL
#define LOG_FORMAT USB_KEYBOARD_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cfg/compiler.h>
#include <cfg/module.h>

#include <cpu/power.h> // cpu_relax()

#include <drv/usb.h>
#include <drv/usb_endpoint.h>


/*
 * HID device configuration (usb-keyboard)
 */
#define USB_HID_VENDOR_ID	USB_KEYBOARD_VENDOR_ID
#define USB_HID_PRODUCT_ID	USB_KEYBOARD_PRODUCT_ID

#define USB_HID_INTERFACES	1
#define USB_HID_ENDPOINTS	1

#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT	2

#define USB_HID_REPORT_EP	(USB_DIR_IN | USB_KBD_EP_REPORT)

static UsbDeviceDesc usb_hid_device_descriptor =
{
	.bLength = sizeof(usb_hid_device_descriptor),
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x100,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.idVendor = USB_HID_VENDOR_ID,
	.idProduct = USB_HID_PRODUCT_ID,
	.bcdDevice = 0,
	.iManufacturer = USB_STRING_MANUFACTURER,
	.iProduct = USB_STRING_PRODUCT,
	.iSerialNumber = 0,
	.bNumConfigurations = 1,
};

static const UsbConfigDesc usb_hid_config_descriptor =
{
	.bLength = sizeof(usb_hid_config_descriptor),
	.bDescriptorType = USB_DT_CONFIG,
	.bNumInterfaces = USB_HID_INTERFACES,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = USB_CONFIG_ATT_ONE,
	.bMaxPower = 50, /* 100 mA */
};

static const UsbInterfaceDesc usb_hid_interface_descriptor =
{
	.bLength = sizeof(usb_hid_interface_descriptor),
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = USB_HID_ENDPOINTS,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = USB_INTERFACE_SUBCLASS_BOOT,
	.bInterfaceProtocol = USB_INTERFACE_PROTOCOL_KEYBOARD,
	.iInterface = 0,
};

/*
 * Keyboard report descriptor
 *
 * Taken from the USB HID spec:
 *  - E.6 Report Descriptor (Keyboard), HID1_11.pdf, p.69
 */
static const uint8_t hid_report_descriptor[] =
{
	0x05, 0x01, // Usage Page (Generic Desktop)
	0x09, 0x06, // Usage (Keyboard)
	0xA1, 0x01, // Collection (Application)
	0x05, 0x07, // Usage Page (Key Codes)
	0x19, 0xE0, // Usage Minimum (224)
	0x29, 0xE7, // Usage Maximum (231)
	0x15, 0x00, // Logical Minimum (0)
	0x25, 0x01, // Logical Maximum (1)
	0x75, 0x01, // Report Size (1)
	0x95, 0x08, // Report Count (8)
	0x81, 0x02, // Input (Data, Variable, Absolute)
	0x95, 0x01, // Report Count (1)
	0x75, 0x08, // Report Size (8)
	0x81, 0x01, // Input (Constant)
	0x95, 0x05, // Report Count (5)
	0x75, 0x01, // Report Size (1)
	0x05, 0x08, // Usage Page (Page# for LEDs)
	0x19, 0x01, // Usage Minimum (1)
	0x29, 0x05, // Usage Maximum (5)
	0x91, 0x02, // Output (Data, Variable, Absolute)
	0x95, 0x01, // Report Count (1)
	0x75, 0x03, // Report Size (3)
	0x91, 0x01, // Output (Constant)
	0x95, 0x06, // Report Count (6)
	0x75, 0x08, // Report Size (8)
	0x15, 0x00, // Logical Minimum (0)
	0x25, 0x65, // Logical Maximum(101)
	0x05, 0x07, // Usage Page (Key Codes)
	0x19, 0x00, // Usage Minimum (0)
	0x29, 0x65, // Usage Maximum (101)
	0x81, 0x00, // Input (Data, Array)
	0xC0, // End Collection
};

static const usb_HidDesc usb_hid_descriptor =
{
	.bLength = sizeof(usb_hid_descriptor),
	.bDescriptorType = HID_DT_HID,
	.bcdHID = usb_cpu_to_le16((uint16_t)0x0110),
	.bCountryCode = 0,
	.bNumDescriptors = 1,
	.bDescriptorHidType = HID_DT_REPORT,
	.wDescriptorLength =
		usb_cpu_to_le16((uint16_t)sizeof(hid_report_descriptor)),
};

static const UsbEndpointDesc usb_hid_ep_descriptor =
{
	.bLength = sizeof(usb_hid_ep_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_HID_REPORT_EP,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = usb_cpu_to_le16((uint16_t)4),
	.bInterval = 10, /* resolution in ms */
};

static const UsbDescHeader *usb_hid_config[] =
{
	(const UsbDescHeader *)&usb_hid_config_descriptor,
	(const UsbDescHeader *)&usb_hid_interface_descriptor,
	(const UsbDescHeader *)&usb_hid_descriptor,
	(const UsbDescHeader *)&usb_hid_ep_descriptor,
	NULL,
};

static const DEFINE_USB_STRING(language_str, "\x09\x04"); // Language ID: en_US
static const DEFINE_USB_STRING(manufacturer_str,
		USB_STRING("B", "e", "R", "T", "O", "S"));
static const DEFINE_USB_STRING(product_str,
		USB_STRING("U", "S", "B", " ",
				"K", "e", "y", "b", "o", "a", "r", "d"));

static const UsbStringDesc *usb_hid_strings[] =
{
	(const UsbStringDesc *)&language_str,
	(const UsbStringDesc *)&manufacturer_str,
	(const UsbStringDesc *)&product_str,
	NULL,
};

static uint8_t report[8];

static bool hid_keyboard_configured;

static void usb_hid_event_cb(UsbCtrlRequest *ctrl)
{
	uint16_t value = usb_le16_to_cpu(ctrl->wValue);
	uint16_t index = usb_le16_to_cpu(ctrl->wIndex);
	uint16_t length = usb_le16_to_cpu(ctrl->wLength);
	uint8_t type = ctrl->mRequestType;
	uint8_t request = ctrl->bRequest;

	LOG_INFO("%s: s 0x%02x 0x%02x 0x%04x 0x%04x 0x%04x\n",
		__func__, type, request, value, index, length);
	switch (ctrl->bRequest)
	{
	case USB_REQ_GET_DESCRIPTOR:
		switch (value >> 8)
		{
		case HID_DT_HID:
			LOG_INFO("%s: HID_DT_HID\n", __func__);
			usb_endpointWrite(USB_DIR_IN | 0,
					&usb_hid_descriptor,
					sizeof(usb_hid_descriptor));
			break;
		case HID_DT_REPORT:
			LOG_INFO("%s: HID_DT_REPORT\n", __func__);
			usb_endpointWrite(USB_DIR_IN | 0,
					&hid_report_descriptor,
					sizeof(hid_report_descriptor));
			hid_keyboard_configured = true;
			break;
		default:
			LOG_INFO("%s: unknown HID request\n", __func__);
			break;
		}
		break;
	case HID_REQ_GET_REPORT:
		LOG_INFO("%s: HID_REQ_GET_REPORT\n", __func__);
		break;
	case HID_REQ_SET_REPORT:
		LOG_INFO("%s: HID_REQ_SET_REPORT\n", __func__);
		usb_endpointWrite(USB_DIR_IN | 0, NULL, 0);
		break;
	case HID_REQ_GET_IDLE:
		LOG_INFO("%s: HID_REQ_GET_IDLE\n", __func__);
		break;
	case HID_REQ_SET_IDLE:
		LOG_INFO("%s: HID_REQ_SET_IDLE\n", __func__);
		usb_endpointWrite(USB_DIR_IN | 0, NULL, 0);
		break;
	case HID_REQ_GET_PROTOCOL:
		LOG_INFO("%s: HID_REQ_GET_PROTOCOL\n", __func__);
		break;
	case HID_REQ_SET_PROTOCOL:
		LOG_INFO("%s: HID_REQ_SET_PROTOCOL\n", __func__);
		break;
	default:
		LOG_ERR("%s: unknown request: 0x%02x\n",
			__func__, ctrl->bRequest);
		break;
	}
}

/* Global usb-keyboard descriptor that identifies the usb-keyboard device */
static UsbDevice usb_keyboard = {
	.device = &usb_hid_device_descriptor,
	.config = usb_hid_config,
	.strings = usb_hid_strings,
	.event_cb = usb_hid_event_cb,
};

/* Low-level usb-hid device initialization */
static int usb_keyboard_hw_init(void)
{
	if (usb_deviceRegister(&usb_keyboard) < 0)
		return -1;
	LOG_INFO("usb-hid: registered new USB keyboard device\n");
	return 0;
}

/* Send a keyboard event */
void usbkbd_sendEvent(uint8_t mod, uint8_t code)
{
	report[0] = mod;
	report[2] = code;
	usb_endpointWrite(USB_HID_REPORT_EP, &report, sizeof(report));
}

/*
 * Initialize a USB HID keyboard device.
 *
 * TODO: support more than one device at the same time.
 */
int usbkbd_init(UNUSED_ARG(int, unit))
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	usb_keyboard_hw_init();
	while (!hid_keyboard_configured)
		cpu_relax();
	return 0;
}
