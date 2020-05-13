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

#ifndef DBUS_LIBUSB_H
#define DBUS_LIBUSB_H

#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

void free_g_list(GList *l);

GList *callMethod(const char *method, GList *listParams);

int dbus_libusb_init(unsigned long **context);

void dbus_libusb_exit(unsigned long context);

int dbus_libusb_get_device_list(unsigned long context, unsigned long ***devs);

void dbus_libusb_free_device_list(unsigned long devs, int unref_devices);

int dbus_libusb_get_device_descriptor(unsigned long *dev, struct libusb_device_descriptor *desc);

int dbus_libusb_open(unsigned long *dev, unsigned long *handle);

int dbus_libusb_get_bus_number(unsigned long *dev);

int dbus_libusb_get_device_address(unsigned long *dev);

#endif//DBUS_LIBUSB_H
