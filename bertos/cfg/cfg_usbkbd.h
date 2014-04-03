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
 * \brief Configuration file for the usbkbd driver module
 */

#ifndef CFG_USBKBD_H
#define CFG_USBKBD_H

/**
 * Enable the usbkbd module.
 *
 * $WIZ$ type = "autoenabled"
 */
#define CONFIG_USBKBD 0

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define USB_KEYBOARD_LOG_LEVEL      LOG_LVL_INFO

/**
 * module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define USB_KEYBOARD_LOG_FORMAT     LOG_FMT_TERSE

/**
 * USB vendor ID (please change this in your project, using a valid ID number!).
 *
 * $WIZ$ type = "hex"
 */
#define USB_KEYBOARD_VENDOR_ID      0x046d

/**
 * USB product ID (please change this in your project, using a valid ID number!).
 *
 * $WIZ$ type = "hex"
 */
#define USB_KEYBOARD_PRODUCT_ID     0xffff

#endif /* CFG_USB_KEYBOARD_H */
