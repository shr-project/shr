/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "network.h"
#include "dbus.h"
#include "dbus/network.h"

DBusGProxy *networkBus = NULL;

void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data)
{
	printf ("Received network status");
}

void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data)
{
	printf ("Received network signal strength");
}

GError* network_handle_errors(GError *dbus_error) {
	const char *error_name = dbus_g_error_get_name(dbus_error);
	int networkError = 0;

	lose_gerror ("Unknown network error", dbus_error);

	return g_error_new (NETWORK_ERROR, networkError, "TODO: %s", error_name);
}

gboolean network_register(GError** error) {

	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;
	if(!(result = org_freesmartphone_GSM_Network_register(networkBus, &dbus_error))) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error,tmperror);
	}

	return result;
}

gboolean network_register_with_provider(GError** error, int provider_id) {
	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;
	if(!(result = org_freesmartphone_GSM_Network_register_with_provider(networkBus, provider_id, &dbus_error))) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error,tmperror);
	}

	return result;

}
