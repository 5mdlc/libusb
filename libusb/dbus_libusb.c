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

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <libusb.h>

#include "dbus_libusb.h"

typedef struct _item
{
	char type;
	void *var;
} item;

void
free_g_list(GList *l)
{
	GList *tmp = l;

	while(tmp){
		item *it = tmp->data;
		free(it->var);
		free(it);

		GList *d = NULL;
	  	d = g_list_remove(tmp, it);
		fprintf(stderr, "g_list_remove := 0x%lx.\n", (unsigned long)d);

		tmp = tmp->next;
	}//endwhile
}//end free_g_list(GList *l)

unsigned long defaultCtx = 1;
GList *
callMethod(const char *method, GList *listParams)
{
	DBusMessage* msg;
	DBusPendingCall* pending;
	DBusConnection* conn;
	DBusError err;
	DBusMessageIter args;

	// initialiset the errors
	dbus_error_init(&err);

	// connect to the system bus and check for errors
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err)) { 
		fprintf(stderr, "Connection Error (%s)\n", err.message); 
		dbus_error_free(&err);
	}
	if (NULL == conn) { 
		exit(1); 
	}


	msg = dbus_message_new_method_call( "org.freedesktop.portal.Desktop", // target for the method call
												  "/org/freedesktop/portal/desktop", // object to call on
													"org.freedesktop.portal.LibUsb", // interface to call on
												  method); // method name
												  //"libusb_init"); // method name

	if (NULL == msg) { 
		fprintf(stderr, "Message Null\n");
		exit(1);
	}

	if(listParams && g_list_length(listParams)){
		dbus_message_iter_init_append(msg, &args);
		GList *tmp = g_list_first(listParams);

		while(tmp){
			item *p = tmp->data;

			fprintf(stderr, "type := %c, var := %ld\n", p->type, (*(unsigned long *)p->var));
			if (!dbus_message_iter_append_basic(&args, p->type, p->var)) { 
				fprintf(stderr, "Out Of Memory!\n"); 
				exit(1);
			}//endif true
			tmp = tmp->next;
		}//endwhile


		// clean list?

	}//endif true

	// send message and get a handle for a reply
	if (!dbus_connection_send_with_reply (conn, msg, &pending, 100)) { // -1 is default timeout
		fprintf(stderr, "Out Of Memory!\n"); 
		exit(1);
	}

	if (NULL == pending) { 
		fprintf(stderr, "Pending Call Null\n"); 
		exit(1); 
	}
	dbus_connection_flush(conn);

	// free message
	dbus_message_unref(msg);

	// block until we receive a reply
	dbus_pending_call_block(pending);
	
	// get the reply message
	msg = dbus_pending_call_steal_reply(pending);
	if (NULL == msg) {
		fprintf(stderr, "Reply Null\n"); 
		exit(1); 
	}

	GList *listResults = NULL;
	// read the parameters
	if (dbus_message_iter_init(msg, &args)){
		fprintf(stderr, "Got Reply: ");

		do {
			item *it = malloc(sizeof(item));
			it->type = dbus_message_iter_get_arg_type(&args); 
			fprintf(stderr, "(%c) ", it->type); 

			switch(it->type){
			case 'i':
				it->var = malloc(sizeof(dbus_uint32_t));
				break;
			case 't':
				it->var = malloc(sizeof(dbus_uint64_t));
				break;
			default :
				fprintf(stderr, "type error\n");
				continue;
			}//endswitch
		  	dbus_message_iter_get_basic(&args, it->var);
			switch(it->type){
			case 'i':
				fprintf(stderr, "%d, ", (*(int *)it->var));
				break;
			case 't':
				fprintf(stderr, "%ld, ", (*(unsigned long *)it->var));
				break;
			case 's':
				fprintf(stderr, "%s, ", (char *)it->var);
				break;
			}//endswitch

			listResults = g_list_append(listResults, it);
		}while(dbus_message_iter_next(&args));

		fprintf(stderr, "\n");
	}//endif true
	else
		fprintf(stderr, "Message has no arguments!\n"); 

	// free reply and close connection
	dbus_message_unref(msg); 

	return(listResults);
}//end callMethod(char *args)


int
dbus_libusb_init(unsigned long **context)
{
	GList *listResults = NULL;
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_init() context := 0x%lx\n", 
						 (unsigned long)context);
	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	if(context)
		(*(dbus_uint64_t *)it->var) = **context;
	else
		(*(dbus_uint64_t *)it->var) = defaultCtx = 0;

	listResults = callMethod("libusb_init", g_list_append(NULL, it));
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_init() listResults := 0x%lx\n", 
						 (unsigned long)listResults);

	item *p = g_list_first(listResults)->data;
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_init() p := 0x%lx\n", 
						 (unsigned long)p);
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_init() p->var := 0x%lx\n", 
							(*(dbus_uint64_t*)p->var));
	if(context)
		**context = (*(dbus_uint64_t*)p->var);
	else 
		defaultCtx = (*(dbus_uint64_t*)p->var);
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_init() context := 0x%lx\n",
						 (unsigned long)context);

	p = g_list_first(listResults)->next->data;
	int rc = (*(dbus_uint32_t*)p->var);

	return(rc);
}//end dbus_libusb_init(unsigned long context)

void
dbus_libusb_exit(unsigned long context)
{
	GList *listResults = NULL;
	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	if(context)
		(*(dbus_uint64_t *)it->var) = context;
	else
		(*(dbus_uint64_t *)it->var) = defaultCtx;

	listResults = callMethod("libusb_exit", g_list_append(NULL, it));

	free_g_list(listResults);

}//end dbus_libusb_exit(unsigned long context)

int
dbus_libusb_get_device_list(unsigned long context, unsigned long ***devs)
{
	GList *listResults = NULL, *listParams = NULL;
	fprintf(stderr,
			"libusb::dbus_libusb.c::dbus_libusb_get_device_list() context := 0x%lx (0x%lx), devs := 0x%lx.\n", 
						 (unsigned long)context, (unsigned long) defaultCtx, (unsigned long) devs);
	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	if(context)
		(*(dbus_uint64_t *)it->var) = context;
	else
		(*(dbus_uint64_t *)it->var) = defaultCtx;
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_get_device_list() it->var := 0x%lx.\n", 
			(*(dbus_uint64_t *)it->var));
	listParams = g_list_append(NULL, it);

	it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = 0;
	listParams = g_list_append(listParams, it);

	listResults = callMethod("libusb_get_device_list", listParams);
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_get_device_list() listResults := 0x%lx\n", 
						 (unsigned long)listResults);

	item *p = g_list_first(listResults)->data;
	unsigned long devsList = (*(unsigned long *)p->var);
	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_get_device_list() *devsList := 0x%lx.\n", 
						 (unsigned long)devsList);

	p = g_list_first(listResults)->next->data;
	int cnt = (*(dbus_uint32_t*)p->var);

	//unsigned long *devsTmp = *devs = (unsigned long *)malloc(sizeof(dbus_uint64_t) * (cnt + 1));
  	*devs = (unsigned long **)malloc(sizeof(dbus_uint64_t) * (cnt + 1));
	unsigned long *devsTmp = (unsigned long *)*devs;
	memset(devsTmp, 0, sizeof(dbus_uint64_t) * (cnt + 1));

	fprintf(stderr, "libusb::dbus_libusb.c::dbus_libusb_get_device_list() *devs := 0x%lx, devsTmp := 0x%lx.\n", 
						 (unsigned long)*devs, (unsigned long)devsTmp);

	for(int i = 0; i < cnt; i++)
		devsTmp[i] = (unsigned long) i + devsList;

	for(int i = 0; i < cnt; i++)
		fprintf(stderr, "devsTmp[%d] := 0x%lx\n", i, (unsigned long)devsTmp[i]);

	free_g_list(listResults);
	free_g_list(listParams);

	return(cnt);
}//end dbus_libusb_get_device_list(unsigned long context, unsigned long devs)

void
dbus_libusb_free_device_list(unsigned long devs, int unref_devices)
{
	GList *listResults = NULL, *listParams = NULL;
	void *tmp = (void *)devs;
	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = (*(dbus_uint64_t *)tmp); //devs;
	listParams = g_list_append(NULL, it);

	it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint32_t *)it->var) = unref_devices;
	listParams = g_list_append(listParams, it);

	listResults = callMethod("libusb_free_device_list", listParams);

	free_g_list(listResults);
	free_g_list(listParams);

	free((void *)devs);

	return;
}//end dbus_libusb_free_device_list(unsigned long context, unsigned long devs)

int
dbus_libusb_get_device_descriptor(unsigned long *dev, struct libusb_device_descriptor *desc)
{
	GList *listResults = NULL, *listParams = NULL;
	int rc = -1;
	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = (unsigned long)dev;
	listParams = g_list_append(NULL, it);

	listResults = callMethod("libusb_get_device_descriptor", listParams);

	GList *tmp = g_list_first(listResults);

	//it = ((item *)(tmp->data)) ;
	//desc->bLength = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);
	fprintf(stderr, "list size := %u\n", g_list_length(tmp));

	if(g_list_length(tmp)){
		desc->bLength = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bDescriptorType = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bcdUSB = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bDeviceClass = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bDeviceSubClass = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bDeviceProtocol = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bMaxPacketSize0 = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->idVendor = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->idProduct = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bcdDevice = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->iManufacturer = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->iProduct = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->iSerialNumber = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		desc->bNumConfigurations = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);; tmp = tmp->next;

		/*

		item *p = tmp->data;

		fprintf(stderr, "type := %c, var := %ld\n", p->type, (*(unsigned long *)p->var));

		tmp = tmp->next;
		*/

		//it = g_list_last(listResults)->data;
		rc = (*(dbus_uint32_t*)(((item *)(tmp->data)))->var);
	}//endif true

	free_g_list(listResults);
	free_g_list(listParams);

	return(rc);
}//end dbus_libusb_get_device_descriptor(unsigned long dev)

int
dbus_libusb_open(unsigned long *dev, unsigned long *handle)
{
	GList *listResults = NULL, *listParams = NULL;
	int rc = -1;

	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = (unsigned long)dev;
	listParams = g_list_append(NULL, it);

	listResults = callMethod("libusb_open", listParams);

	GList *tmp = g_list_first(listResults);
	*handle = (*(dbus_uint64_t*)((item *)tmp->data)->var);
	tmp = tmp->next;
	rc = (*(dbus_uint32_t*)((item *)tmp->data)->var);
	fprintf(stderr, "rc = %d\n", rc);

	free_g_list(listResults);
	free_g_list(listParams);

	return(rc);
}//end dbus_libusb_open(unsigned long dev)

int
dbus_libusb_get_bus_number(unsigned long *dev)
{
	GList *listResults = NULL, *listParams = NULL;
	int rc = -1;

	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = (unsigned long)dev;
	listParams = g_list_append(NULL, it);

	listResults = callMethod("libusb_get_bus_number", listParams);

	GList *tmp = g_list_first(listResults);
	rc = (*(dbus_uint32_t*)((item *)tmp->data)->var);
	fprintf(stderr, "rc = %d\n", rc);

	free_g_list(listResults);
	free_g_list(listParams);

	return(rc);
}//end dbus_libusb_get_bus_number(unsigned long dev)

int
dbus_libusb_get_device_address(unsigned long *dev)
{
	GList *listResults = NULL, *listParams = NULL;
	int rc = -1;

	item *it = malloc(sizeof(item));
	it->type = 't';
	it->var = malloc(sizeof(dbus_uint64_t));
	(*(dbus_uint64_t *)it->var) = (unsigned long)dev;
	listParams = g_list_append(NULL, it);

	listResults = callMethod("libusb_get_device_address", listParams);

	GList *tmp = g_list_first(listResults);
	rc = (*(dbus_uint32_t*)((item *)tmp->data)->var);
	fprintf(stderr, "rc = %d\n", rc);

	free_g_list(listResults);
	free_g_list(listParams);

	return(rc);
}//end dbus_libusb_get_device_address(unsigned long dev)

#ifdef TEST_DBUS_LIBUSB 
int main(int argc, char** argv)
{
	char* typeParams = "no param";
	GList *listParams = NULL, *listResults = NULL;

	//fprintf(stderr, "press key: "); getchar();

	//////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "dbus_libusb_init(0) ... \n");
	dbus_uint64_t *context = malloc(sizeof(dbus_uint64_t));
	*context = 0;
	//int rc = dbus_libusb_init(&context);
	int rc = dbus_libusb_init(NULL);

	fprintf(stderr, "context := %ld, rc := %d.\n", context, rc);
	fprintf(stderr, "\n");

	//////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "dbus_libusb_get_device_list(%ld, 0) ... \n", context);
	unsigned long **devs;
	int cnt = dbus_libusb_get_device_list(*context, &devs);

	fprintf(stderr, "cnt := %d, devs := 0x%lx.\n", cnt, devs);

	//for(int i = 0; i < cnt; i++)
	for(int i = 0; devs[i]; i++)
		fprintf(stderr, "devs[%d] := 0x%lx.\n", i, devs[i]);

	fprintf(stderr, "\n");

	//////////////////////////////////////////////////////////////////////////
	struct libusb_device_descriptor *desc = malloc(sizeof(struct libusb_device_descriptor));
	libusb_device_handle *handle = NULL;

	for(int i = 0; i < cnt; i++){
		memset(desc, 0, sizeof(struct libusb_device_descriptor));
		handle = NULL;

		fprintf(stderr, "\tdbus_libusb_get_device_descriptor(0x%lx, desc) ... \n", devs[i]);
		rc = dbus_libusb_get_device_descriptor(devs[i], desc);
		fprintf(stderr, "\t\trc := %d\n", rc);

		fprintf(stderr, "\tdbus_libusb_open(0x%lx, handle) ... \n", devs[i]);
		rc = dbus_libusb_open(devs[i], (unsigned long *)&handle);
		fprintf(stderr, "\t\trc := %d, handle := 0x%lx\n", rc, handle);

		fprintf(stderr, "\tdbus_libusb_get_bus_number(0x%lx) ... \n", devs[i]);
		rc = dbus_libusb_get_bus_number(devs[i]);
		fprintf(stderr, "\t\trc := %d\n", rc);

		fprintf(stderr, "\tdbus_libusb_get_device_address(0x%lx) ... \n", devs[i]);
		rc = dbus_libusb_get_device_address(devs[i]);
		fprintf(stderr, "\t\trc := %d\n", rc);

	}//endfor

	//////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "dbus_libusb_free_device_list(0x%lx (0x%lx), 0) ... \n", (unsigned long)*devs, (unsigned long)devs);
	dbus_libusb_free_device_list((unsigned long)devs, 0);
	fprintf(stderr, "\n");

	//////////////////////////////////////////////////////////////////////////
	fprintf(stderr, "dbus_libusb_exit(%ld) ... \n", *context);
	dbus_libusb_exit(*context);
	fprintf(stderr, "\n");

	//fprintf(stderr, "press key: "); getchar();
	usleep(100);
	return 0;
}
#endif//TEST_DBUS_LIBUSB 
