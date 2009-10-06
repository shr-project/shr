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

#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "ophonekitd-dbus.h"
#include "ophonekitd-dbus-common.h"
#include "ophonekitd-dbus-usage.h"
#include "ophonekitd-usage-service-glue.h"

static gpointer
dbus_register_object(DBusGConnection * connection,
		     DBusGProxy * proxy,
		     GType object_type,
		     const DBusGObjectInfo * info, const gchar * path)
{
	g_debug(path);
	GObject *object = g_object_new(object_type, NULL);
	dbus_g_object_type_install_info(object_type, info);
	dbus_g_connection_register_g_object(connection, path, object);
	return object;
}

void
ophonekitd_dbus_start()
{
	GError *error = NULL;
	guint result;
	DBusGConnection *connection;
	DBusGProxy *proxy;
	g_type_init();
	connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	proxy = dbus_g_proxy_new_for_name(connection,
					  DBUS_SERVICE_DBUS,
					  DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);


	if (!org_freedesktop_DBus_request_name(proxy,
					       OPHONEKITD_USAGE_SERVICE_NAME,
					       DBUS_NAME_FLAG_DO_NOT_QUEUE,
					       &result, &error)) {
		g_debug("Error requesting name! %s", error->message);
	}


	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_debug("Got result code %u from requesting name", result);
	}

	dbus_register_object(connection,
			     proxy,
			     OPHONEKITD_TYPE_USAGE_SERVICE,
			     &dbus_glib_ophonekitd_usage_service_object_info,
			     OPHONEKITD_USAGE_SERVICE_PATH);
}
