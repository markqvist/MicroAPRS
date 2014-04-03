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
 * \brief USB 2.0 standard descriptors
 *
 * This file holds USB constants and structures that are needed for USB device
 * APIs, as defined in the USB 2.0 specification.
 *
 * \attention The API is work in progress and may change in future versions.
 *
 * $WIZ$ module_name = "usb"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_usb.h"
 * $WIZ$ module_supports = "stm32"
 */

#ifndef USB_H
#define USB_H

#include <cpu/byteorder.h>

#define usb_cpu_to_le16(x)	cpu_to_le16(x)
#define usb_le16_to_cpu(x)	le16_to_cpu(x)
#define usb_cpu_to_le32(x)	cpu_to_le32(x)
#define usb_le32_to_cpu(x)	le32_to_cpu(x)

/* State of a USB device */
enum usb_device_state {
        USB_STATE_NOTATTACHED = 0,

        /* chapter 9 device states */
        USB_STATE_ATTACHED,
        USB_STATE_POWERED,                      /* wired */
        USB_STATE_DEFAULT,                      /* limited function */
        USB_STATE_ADDRESS,
        USB_STATE_CONFIGURED,                   /* most functions */
};

/*
 * USB directions
 *
 * This bit flag is used in endpoint descriptors' bEndpointAddress field.
 * It's also one of three fields in control requests bRequestType.
 */
#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */
#define USB_DIR_MASK			0x80

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03

/*
 * USB standard requests, for the bRequest field of a SETUP packet.
 */
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07
#define USB_DT_INTERFACE_POWER		0x08

/*
 * Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE		(USB_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(USB_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING		(USB_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(USB_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(USB_TYPE_CLASS | USB_DT_ENDPOINT)

/**
 *
 * USB Control Request descriptor
 *
 * This structure is used to send control requests to a USB device.
 *
 * It matches the different fields of the USB 2.0 specification (section 9.3,
 * table 9-2).
 */
typedef struct UsbCtrlRequest
{
	uint8_t mRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} PACKED UsbCtrlRequest;

/**
 * USB common descriptor header.
 *
 * \note All the USB standard descriptors have these 2 fields at the beginning.
 */
typedef struct UsbDescHeader
{
	uint8_t bLength;
	uint8_t bDescriptorType;
} PACKED UsbDescHeader;

/**
 * USB Device descriptor
 *
 * \note See USB 2.0 specification.
 */
typedef struct UsbDeviceDesc
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} PACKED UsbDeviceDesc;

/**
 * USB string descriptor.
 *
 * \note See USB 2.0 specification.
 */
typedef struct UsbStringDesc
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t data[0];
} PACKED UsbStringDesc;

#define USB_STRING_1(__a, ...) __a "\x00"
#define USB_STRING_2(__a, ...) __a "\x00" USB_STRING_1(__VA_ARGS__)
#define USB_STRING_3(__a, ...) __a "\x00" USB_STRING_2(__VA_ARGS__)
#define USB_STRING_4(__a, ...) __a "\x00" USB_STRING_3(__VA_ARGS__)
#define USB_STRING_5(__a, ...) __a "\x00" USB_STRING_4(__VA_ARGS__)
#define USB_STRING_6(__a, ...) __a "\x00" USB_STRING_5(__VA_ARGS__)
#define USB_STRING_7(__a, ...) __a "\x00" USB_STRING_6(__VA_ARGS__)
#define USB_STRING_8(__a, ...) __a "\x00" USB_STRING_7(__VA_ARGS__)
#define USB_STRING_9(__a, ...) __a "\x00" USB_STRING_8(__VA_ARGS__)
#define USB_STRING_10(__a, ...) __a "\x00" USB_STRING_9(__VA_ARGS__)
#define USB_STRING_11(__a, ...) __a "\x00" USB_STRING_10(__VA_ARGS__)
#define USB_STRING_12(__a, ...) __a "\x00" USB_STRING_11(__VA_ARGS__)
#define USB_STRING_13(__a, ...) __a "\x00" USB_STRING_12(__VA_ARGS__)
#define USB_STRING_14(__a, ...) __a "\x00" USB_STRING_13(__VA_ARGS__)
#define USB_STRING_15(__a, ...) __a "\x00" USB_STRING_14(__VA_ARGS__)
#define USB_STRING_16(__a, ...) __a "\x00" USB_STRING_15(__VA_ARGS__)

/**
 * Pack a list with a variable number of elements into a UTF-16LE USB string.
 *
 * \note The macro is recursively defined according the number of elements
 * passed as argument. At the moment we support strings with up to 16
 * characters.
 */
#define USB_STRING(...) PP_CAT(USB_STRING_, PP_COUNT(__VA_ARGS__))(__VA_ARGS__)

/**
 * Define and initialize an USB string descriptor.
 *
 * This macro is reuquired to properly declare and initialize a constant USB
 * string in UTF-16LE format.
 *
 * The structure must contain the standard common USB header (UsbDescHeader)
 * and the UTF-16LE string all packed in a contiguous memory region.
 */
#define DEFINE_USB_STRING(__name, __text)				\
	struct {							\
		UsbDescHeader __header;					\
		uint8_t __body[sizeof(__text)];				\
	} PACKED __name = {						\
		.__header = {						\
			.bLength =					\
				cpu_to_le16((uint16_t)sizeof(__name)),	\
			.bDescriptorType = USB_DT_STRING,		\
		},							\
		.__body = {__text},					\
	}

/*
 * Device and/or Interface Class codes as found in bDeviceClass or
 * bInterfaceClass and defined by www.usb.org documents.
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO			1
#define USB_CLASS_COMM			2
#define USB_CLASS_HID			3
#define USB_CLASS_PHYSICAL		5
#define USB_CLASS_STILL_IMAGE		6
#define USB_CLASS_PRINTER		7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB			9
#define USB_CLASS_CDC_DATA		0x0a
#define USB_CLASS_CSCID			0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		0x0d	/* content security */
#define USB_CLASS_VIDEO			0x0e
#define USB_CLASS_WIRELESS_CONTROLLER	0xe0
#define USB_CLASS_MISC			0xef
#define USB_CLASS_APP_SPEC		0xfe
#define USB_CLASS_VENDOR_SPEC		0xff

/* USB Device subclasses */
#define USB_CDC_SUBCLASS_ACM                    0x02
#define USB_CDC_SUBCLASS_ETHERNET               0x06
#define USB_CDC_SUBCLASS_WHCM                   0x08
#define USB_CDC_SUBCLASS_DMM                    0x09
#define USB_CDC_SUBCLASS_MDLM                   0x0a
#define USB_CDC_SUBCLASS_OBEX                   0x0b
#define USB_CDC_SUBCLASS_EEM                    0x0c
#define USB_CDC_SUBCLASS_NCM                    0x0d

/**
 * Device configuration descriptor
 *
 * \note See USB 2.0 specification.
 */
typedef struct UsbConfigDesc
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} PACKED UsbConfigDesc;

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE		(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define USB_CONFIG_ATT_BATTERY		(1 << 4)	/* battery powered */

/**
 * Device interface descriptor
 *
 * \note See USB 2.0 specification.
 */
typedef struct UsbInterfaceDesc
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} PACKED UsbInterfaceDesc;

/**
 * Endpoint descriptor
 *
 * \note See USB 2.0 specification.
 */
typedef struct UsbEndpointDesc
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} PACKED UsbEndpointDesc;

/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		USB_DIR_MASK

#define USB_ENDPOINT_SYNCTYPE		0x0c
#define USB_ENDPOINT_SYNC_NONE		(0 << 2)
#define USB_ENDPOINT_SYNC_ASYNC		(1 << 2)
#define USB_ENDPOINT_SYNC_ADAPTIVE	(2 << 2)
#define USB_ENDPOINT_SYNC_SYNC		(3 << 2)

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3
#define USB_ENDPOINT_MAX_ADJUSTABLE	0x80

/**
 * USB: generic device descriptor
 */
typedef struct UsbDevice
{
	UsbDeviceDesc *device;              ///< USB 2.0 device descriptor
	const UsbDescHeader **config;       ///< USB 2.0 configuration descriptors
	const UsbStringDesc **strings;      ///< USB strings

	/* Callbacks */
	void (*event_cb)(UsbCtrlRequest *); ///< Called to handle control requests.

	/* Private data */
	bool configured;                    ///< True when the device has been correctly initialized.
} UsbDevice;

/**
 * Get the endpoint's address number of \a epd.
 */
INLINE int usb_endpointNum(const UsbEndpointDesc *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

/**
 * Get the transfer type of the endpoint \a epd.
 */
INLINE int usb_endpointType(const struct UsbEndpointDesc *epd)
{
	return epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
}

/**
 * Check if the endpoint \a epd has IN direction.
 */
INLINE int usb_endpointDirIn(const struct UsbEndpointDesc *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}

/**
 * Check if the endpoint \a epd has OUT direction.
 */
INLINE int usb_endpointDirOut(const struct UsbEndpointDesc *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT);
}

/**
 * Check if the endpoint \a epd has bulk transfer type.
 */
INLINE int usb_endpointXferBulk(const struct UsbEndpointDesc *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK);
}

/**
 * Check if the endpoint \a epd has control transfer type.
 */
INLINE int usb_endpointXferControl(const struct UsbEndpointDesc *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_CONTROL);
}

/**
 * Check if the endpoint \a epd has interrupt transfer type.
 */
INLINE int usb_endpointXferInt(const struct UsbEndpointDesc *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_INT);
}

/**
 * Check if the endpoint \a epd has isochronous transfer type.
 */
INLINE int usb_endpointXferIsoc(const struct UsbEndpointDesc *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_ISOC);
}

/**
 * Check if the endpoint \a epd is bulk IN.
 */
INLINE int usb_endpointIsBulkIn(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferBulk(epd) && usb_endpointDirIn(epd);
}

/**
 * Check if the endpoint \a epd is bulk OUT.
 */
INLINE int usb_endpointIsBulkOut(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferBulk(epd) && usb_endpointDirOut(epd);
}

/**
 * Check if the endpoint \a epd is interrupt IN.
 */
INLINE int usb_endpointIsIntIn(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferInt(epd) && usb_endpointDirIn(epd);
}

/**
 * Check if the endpoint \a epd is interrupt OUT.
 */
INLINE int usb_endpointIsIntOut(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferInt(epd) && usb_endpointDirOut(epd);
}

/**
 * Check if the endpoint \a epd is isochronous IN.
 */
INLINE int usb_endpointIsIsocIn(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferIsoc(epd) && usb_endpointDirIn(epd);
}

/**
 * Check if the endpoint \a epd is isochronous OUT.
 */
INLINE int usb_endpointIsIsocOut(const struct UsbEndpointDesc *epd)
{
	return usb_endpointXferIsoc(epd) && usb_endpointDirOut(epd);
}

/**
 * Read up to \a size bytes from the USB endpoint identified by the address
 * \a ep and store them in \a buffer.
 *
 * The \a timeout is an upper bound on the amount of time (in ticks) elapsed
 * before returns. If \a timeout is zero, the the function returns immediatly
 * and it basically works in non-blocking fashion. A negative value for \a
 * timeout means that the function can block indefinitely.
 *
 * \return number of bytes actually read, or a negative value in case of
 * errors.
 */
ssize_t usb_endpointReadTimeout(int ep, void *buffer, ssize_t size,
				ticks_t timeout);

INLINE ssize_t usb_endpointRead(int ep, void *buffer, ssize_t size)
{
	return usb_endpointReadTimeout(ep, buffer, size, -1);
}

/**
 * Write up to \a size bytes from the buffer pointed \a buffer to the USB
 * endpoint identified by the address \a ep.
 *
 * The \a timeout is an upper bound on the amount of time (in ticks) elapsed
 * before returns. If \a timeout is zero, the the function returns immediatly
 * and it basically works in non-blocking fashion. A negative value for \a
 * timeout means that the function can block indefinitely.
 *
 * \return number of bytes actually wrote, or a negative value in case of
 * errors.
 */
ssize_t usb_endpointWriteTimeout(int ep, const void *buffer, ssize_t size,
				ticks_t timeout);

INLINE ssize_t usb_endpointWrite(int ep, const void *buffer, ssize_t size)
{
	return usb_endpointWriteTimeout(ep, buffer, size, -1);
}

/**
 * Register a generic USB device driver \a dev in the USB controller.
 */
int usb_deviceRegister(UsbDevice *dev);

#endif /* USB_H */
