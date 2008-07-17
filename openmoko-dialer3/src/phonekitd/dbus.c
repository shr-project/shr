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
#include "dbus.h"
#include "call.h"
#include "sim.h"
#include "network.h"
#include "device.h"
	
void lose (const char *str, ...)
{
	va_list args;

	va_start (args, str);

	vfprintf (stderr, str, args);
	fputc ('\n', stderr);

	va_end (args);

	exit (1);
}

void lose_gerror (const char *prefix, GError *error)
{
	lose ("%s: %s", prefix, error->message);
}


GError* dbus_handle_errors(GError *dbus_error) {
	const char *error_name = dbus_g_error_get_name(dbus_error);
	GError *error = NULL;
	if(strncmp(error_name, SIM_INTERFACE, strlen(SIM_INTERFACE))) {
		error = sim_handle_errors(dbus_error);
	} else if(strncmp(error_name, CALL_INTERFACE, strlen(CALL_INTERFACE))) {
		error = call_handle_errors(dbus_error);
	} else if(strncmp(error_name, NETWORK_INTERFACE, strlen(NETWORK_INTERFACE))) {
		error =  network_handle_errors(dbus_error);
	} else if(strncmp(error_name, DEVICE_INTERFACE, strlen(DEVICE_INTERFACE))) {
		error = device_handle_errors(dbus_error);
	} else {
		lose_gerror ("Failed to handle device error", dbus_error);
	}
	g_error_free(dbus_error);
	return error;
}

void dbus_connect_to_bus() {
	DBusGConnection *bus;

	GError *error = NULL;
	g_type_init ();

	{
		GLogLevelFlags fatal_mask;

		fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
		fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
		g_log_set_always_fatal (fatal_mask);
	}

	bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);


	if (!bus)
		lose_gerror ("Couldn't connect to system bus", error);

	networkBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, NETWORK_INTERFACE);
	simBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, SIM_INTERFACE);
	callBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, CALL_INTERFACE);
	deviceBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, DEVICE_INTERFACE);


	dbus_g_proxy_add_signal (networkBus, "Status", DBUS_TYPE_G_STRING_VARIANT_HASHTABLE, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (networkBus, "Status", G_CALLBACK (network_status_handler),
			NULL, NULL);

	dbus_g_proxy_add_signal (networkBus, "Status", DBUS_TYPE_G_STRING_VARIANT_HASHTABLE, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (networkBus, "Status", G_CALLBACK (network_status_handler),
			NULL, NULL);

	dbus_g_proxy_add_signal (networkBus, "SignalStrength", G_TYPE_UINT , G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (networkBus, "SignalStrength", G_CALLBACK (network_signal_strength_handler),
			NULL, NULL);

	dbus_g_proxy_add_signal (simBus, "AuthStatus", G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (simBus, "AuthStatus", G_CALLBACK (sim_auth_status_handler),
			NULL, NULL);

	dbus_g_proxy_add_signal (callBus, "CallStatus", G_TYPE_UINT, G_TYPE_STRING, DBUS_TYPE_G_STRING_VARIANT_HASHTABLE, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (callBus, "CallStatus", G_CALLBACK (network_status_handler),
			NULL, NULL);
}
