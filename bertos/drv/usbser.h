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
 * This driver exports a USB-serial converter. It provides a KFile interface
 * to access the data.
 * \attention The API is work in progress and may change in future versions.
 *
 * $WIZ$ module_name = "usbser"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_usbser.h"
 * $WIZ$ module_depends = "usb"
 */

#ifndef USBSER_H
#define USBSER_H

#include <io/kfile.h>

typedef uint32_t usbser_status_t;

typedef struct USBSerial
{
        /** KFile structure implementation **/
        KFile fd;
        /** Logical port number */
        unsigned int unit;
#ifdef _DEBUG
	/** Used for debugging only */
        bool is_open;
#endif
        /** Holds the status flags. Set to 0 when no errors have occurred. */
        usbser_status_t status;
} USBSerial;

/**
 * ID for usb-serial.
 */
#define KFT_USB_SERIAL MAKE_ID('U', 'S', 'B', 'S')

INLINE USBSerial *USB_SERIAL_CAST(KFile *fd)
{
        ASSERT(fd->_type == KFT_USB_SERIAL);
        return (USBSerial *)fd;
}

int usbser_init(struct USBSerial *fds, int unit);

#endif /* USBSER_H */
