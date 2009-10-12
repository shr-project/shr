/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
 *              quickdev
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#include <glib.h>
#include <glib/gthread.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/ousaged/frameworkd-glib-ousaged.h>
#include "ophonekitd-dbus-usage.h"
#include "ophonekitd-main.h"
//#include "ophonekitd-usage-service-glue.h"

G_DEFINE_TYPE(OphonekitdUsageService, ophonekitd_usage_service, G_TYPE_OBJECT)


int resources[OUSAGED_RESOURCE_COUNT];

static void
ophonekitd_usage_service_class_init(OphonekitdUsageServiceClass * klass)
{
//    dbus_g_object_type_install_info (G_OBJECT_TYPE (klass), &dbus_glib_ophonekitd_usage_service_object_info);
}

static void
ophonekitd_usage_service_init(OphonekitdUsageService * object)
{
	int f;
	for (f = 0; f < OUSAGED_RESOURCE_COUNT; f++)
		resources[f] = 0;
}


OphonekitdUsageService *
ophonekitd_usage_service_new(void)
{
	return g_object_new(OPHONEKITD_TYPE_USAGE_SERVICE, NULL);
}

void
ophonekitd_usage_get_resource_state_callback(GError * error, gboolean state,
					     gpointer userdata)
{
	DBusGMethodInvocation *context = (DBusGMethodInvocation *) userdata;
	if (error != NULL)
		dbus_g_method_return_error(context, error);
	else
		dbus_g_method_return(context, state);
}

void
ophonekitd_usage_service_get_resource_state(OphonekitdUsageService * object,
					    const char *resource,
					    DBusGMethodInvocation * context)
{
	if (resource != NULL)
		ousaged_get_resource_state(resource,
					   ophonekitd_usage_get_resource_state_callback,
					   context);
}

typedef struct {
	DBusGMethodInvocation *context;
	char *resource;
	int res;
} ophonekitd_usage_request_resource_data_t;

void
ophonekitd_usage_request_resource_callback(GError * error, gpointer userdata)
{
	ophonekitd_usage_request_resource_data_t *data = userdata;

	if (error != NULL) {
		g_debug("error: %s", error->message);
	}
	else {
		g_debug("requested resource %s", data->resource,
			resources[data->res]);
		resources[data->res] = 1;
	}
	g_free(data->resource);
	dbus_g_method_return(data->context);
}

void
ophonekitd_usage_service_request_resource(OphonekitdUsageService * object,
					  const char *resource,
					  DBusGMethodInvocation * context)
{
	if (resource != NULL) {
		int res = ousaged_resource_name_to_int(resource);
		resources[res]++;
		if (resources[res] > 1) {
			dbus_g_method_return(context);
			return;
		}
		ophonekitd_usage_request_resource_data_t *data =
			g_malloc(sizeof
				 (ophonekitd_usage_request_resource_data_t));
		data->context = context;
		data->resource = g_strdup(resource);
		data->res = res;
		ousaged_request_resource(resource,
					 ophonekitd_usage_request_resource_callback,
					 data);
	}
}

void
ophonekitd_usage_release_resource_callback(GError * error, gpointer userdata)
{
	ophonekitd_usage_request_resource_data_t *data = userdata;
	if (error != NULL) {
		g_debug("error: %s", error->message);
	}
	else {
		g_debug("released resource %s", data->resource);
	}
	resources[data->res] = 0;
	g_free(data->resource);
	dbus_g_method_return(data->context);
}

void
ophonekitd_usage_service_release_resource(OphonekitdUsageService * object,
					  const char *resource,
					  DBusGMethodInvocation * context)
{
	if (resource != NULL) {
		int res = ousaged_resource_name_to_int(resource);
		resources[res]--;
		if (resources[res] > 0) {
			dbus_g_method_return(context);
			return;
		}
		ophonekitd_usage_request_resource_data_t *data =
			g_malloc(sizeof
				 (ophonekitd_usage_request_resource_data_t));
		data->context = context;
		data->resource = g_strdup(resource);
		data->res = res;
		ousaged_release_resource(resource,
					 ophonekitd_usage_release_resource_callback,
					 data);
	}
}
