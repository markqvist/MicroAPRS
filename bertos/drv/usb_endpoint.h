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
 * \brief USB endpoint allocations
 *
 * This file defines how the endpoints are allocated among the supported USB
 * device drivers in BeRTOs.
 *
 */

#ifndef USB_ENDPOINT_H
#define USB_ENDPOINT_H

#include "cfg/cfg_usb.h"
#include "cfg/cfg_usbser.h"
#include "cfg/cfg_usbkbd.h"
#include "cfg/cfg_usbmouse.h"

/* Enpoint allocation (according to the compile-time options) */
enum {
	USB_CTRL_ENDPOINT = 0, /* This must be always allocated */
#if (defined(CONFIG_USBSER) && CONFIG_USBSER)
	USB_SERIAL_EP_REPORT,
	USB_SERIAL_EP_OUT,
	USB_SERIAL_EP_IN,
#endif
#if (defined(CONFIG_USBKBD) && CONFIG_USBKBD)
	USB_KBD_EP_REPORT,
#endif
#if (defined(CONFIG_USBMOUSE) && CONFIG_USBMOUSE)
	USB_MOUSE_EP_REPORT,
#endif
 /* Number of allocated endpoints */
#if (CONFIG_USB_EP_MAX == 0)
	USB_EP_MAX,
#else
	USB_EP_MAX = CONFIG_USB_EP_MAX,
#endif
};

/*
 * NOTE: a USB inteface requires at least one endpoint. Moreover, there's the
 * special endpoint 0. In conclusion, the number of endpoints must be always
 * greater than the number of interfaces.
 */
STATIC_ASSERT(USB_EP_MAX >= CONFIG_USB_INTERFACE_MAX);

#endif /* USB_ENDPOINT_H */
