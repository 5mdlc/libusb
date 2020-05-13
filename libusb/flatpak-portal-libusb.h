/*
 * Copyright © 2020 5mdlc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: 5mdlc
 */

#ifndef FLATPAK_PORTAL_ENVIROMENT_H
#define FLATPAK_PORTAL_ENVIROMENT_H

#include "libusb.h"


#define PORTAL_BUS_NAME "org.freedesktop.portal.Desktop"
#define PORTAL_OBJECT_PATH "/org/freedesktop/portal/desktop"
#define PORTAL_REQUEST_INTERFACE "org.freedesktop.portal.Request"
#define PORTAL_SESSION_INTERFACE "org.freedesktop.portal.Session"
#define PORTAL_FILECHOOSER_INTERFACE "org.freedesktop.portal.FileChooser"
#define PORTAL_LIBUSB_INIT_INTERFACE "org.freedesktop.portal.LibUsb"
#define PORTAL_PRINT_INTERFACE "org.freedesktop.portal.Print"
#define PORTAL_SCREENSHOT_INTERFACE "org.freedesktop.portal.Screenshot"
#define PORTAL_INHIBIT_INTERFACE "org.freedesktop.portal.Inhibit"


int fp_check_enviroment(void);
int fp_libusb_init(libusb_context **context);
ssize_t fp_libusb_get_device_list(libusb_context *ctx, libusb_device ***list);
void fp_libusb_exit(struct libusb_context *ctx);
void fp_libusb_free_device_list(libusb_device **list, int unref_devices);
int fp_libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc);
int fp_libusb_open(libusb_device *dev, libusb_device_handle **dev_handle);

uint8_t fp_libusb_get_bus_number(libusb_device *dev);
uint8_t fp_libusb_get_device_address(libusb_device *dev);

#endif//FLATPAK_PORTAL_ENVIROMENT_H
