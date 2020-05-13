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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <gio/gio.h>


#include "libusb.h"
#include "dbus_libusb.h"
#include "flatpak-portal-libusb.h"

int //API_EXPORTED 
fp_libusb_init(libusb_context **context)
{
	fprintf(stderr, "libusb::flatpak-portal-libusb.c::fp_libusb_init() context := 0x%lx\n",
						 (unsigned long)context);
	int rc = dbus_libusb_init((unsigned long **) context);
	fprintf(stderr, "libusb::flatpak-portal-libusb.c::fp_libusb_init() context := 0x%lx, rc := %d.\n",
						 (unsigned long)context, rc );

	return(rc);
}//end fp_libusb_init(libusb_context **context)

void
fp_libusb_exit(struct libusb_context *ctx)
{
	fprintf(stderr, "libusb::flatpak-portal-libusb.c::fp_libusb_exit() ctx := 0x%lx\n",
				(unsigned long)ctx);

	dbus_libusb_exit((unsigned long)ctx);
}//end fp_libusb_exit(struct libusb_context *ctx)

ssize_t 
fp_libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_list() ctx := 0x%lx, list := 0x%lx.\n", 
			(unsigned long)ctx, (unsigned long)list);

	unsigned long **devs;
	size_t rc = dbus_libusb_get_device_list((unsigned long)ctx, &devs);
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_list() rc := %d, devs := 0x%lx.\n",
			(int)rc, (unsigned long) devs );
	*list = (libusb_device **)devs;
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_list() rc := %d, list := 0x%lx.\n", 
			(int)rc, (unsigned long)list);

	return(rc);
}//end fp_libusb_get_device_list(libusb_context *ctx, libusb_device ***list)

void
fp_libusb_free_device_list(libusb_device **list, int unref_devices)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_free_device_list() list := 0x%lx, unref := %d.\n", 
			(unsigned long)list, unref_devices);

	dbus_libusb_free_device_list((unsigned long)list, unref_devices);

	return;
}//end fp_libusb_free_device_list(libusb_device **list, int unref_devices)

int
fp_check_enviroment(void)
{
	static int fp_running = 1; // force work with portal.
	char buf[1024] = "";

	if(!fp_running){
		int fp_check_pid_1 = readlink("/proc/1/exe", buf, 1024);
		//fprintf(stderr,
				//"libusb::flatpak-portal-libusb.c::fp_check_enviroment() fp_check_pid_1 := %d, %s %s\n",
				 //fp_check_pid_1, buf, strstr(buf, "bwrap"));

		int fp_check_info_exist = open("/.flatpak-info", O_RDONLY);
		//fprintf(stderr,
				//"libusb::flatpak-portal-libusb.c::fp_check_enviroment() fp_check_info_exist := %d\n", 
				//fp_check_info_exist);
		if(fp_check_info_exist > 0 && strstr(buf, "bwrap"))
			fp_running = 1;
		if(fp_check_info_exist > 0) close(fp_check_info_exist);
	}//endif true

	//fprintf(stderr, "libusb::flatpak-portal-libusb.c::fp_check_enviroment() fp_running := %d\n", 
						 //fp_running);

	return(fp_running);
}//end fp_check_enviroment(void)

int //API_EXPORTED 
fp_libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_descriptor() dev := 0x%lx, desc := 0x%lx.\n", 
			(unsigned long)dev, (unsigned long)desc);

	int rc = dbus_libusb_get_device_descriptor((unsigned long *)dev, desc);

	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_descriptor() dev := 0x%lx, desc := 0x%lx, rc := %d.\n", 
			(unsigned long)dev, (unsigned long)desc, rc);

	return rc;
}

int //API_EXPORTED 
fp_libusb_open(libusb_device *dev,
	libusb_device_handle **dev_handle)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_open() dev := 0x%lx, dev_handle := 0x%lx.\n", 
			(unsigned long)dev, (unsigned long)dev_handle);
	int rc = dbus_libusb_open((unsigned long *)dev, (unsigned long *)&dev_handle);

	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_open() dev := 0x%lx, dev_handle := 0x%lx, rc := %d.\n", 
			(unsigned long)dev, (unsigned long)dev_handle, rc);
	return(rc);
}

uint8_t //API_EXPORTED 
fp_libusb_get_bus_number(libusb_device *dev)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_bus_number() dev := 0x%lx.\n", 
			(unsigned long)dev);
	uint8_t rc = dbus_libusb_get_bus_number((unsigned long *)dev);

	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_bus_number() dev := 0x%lx, rc := %d.\n", 
			(unsigned long)dev, rc);
	return(rc);
}//end libusb_get_bus_number(libusb_device *dev)

uint8_t //API_EXPORTED 
fp_libusb_get_device_address(libusb_device *dev)
{
	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_get_device_address() dev := 0x%lx.\n", 
			(unsigned long)dev);
	int rc = dbus_libusb_get_device_address((unsigned long *)dev);

	fprintf(stderr, 
			"libusb::flatpak-portal-libusb.c::fp_libusb_device_address() dev := 0x%lx, rc := %d.\n", 
			(unsigned long)dev, rc);
	return(rc);
}//end libusb_get_device_address(libusb_device *dev)
