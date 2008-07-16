/*
 *  Copyright (C) 2008 
 *	Authors (alphabetical) : 
 *		Marc-Olivier Barre <marco@marcochapeau.org>
 *		Julien Cassignol <ainulindale@gmail.com>
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
#include "sim.h"
#include "dbus.h"

static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
  printf ("Received sim auth status");
}


int get_authentication_state() {

	char *status = NULL;
	GError *error = NULL;
	int result = -1;
	if (!dbus_g_proxy_call (simBus, "GetAuthStatus", &error,
				G_TYPE_INVALID,
				G_TYPE_STRING, &status, G_TYPE_INVALID))
		lose_gerror ("Failed to complete GetAuthStatus", error);
	
	if(strcmp(status,DBUS_SIM_READY))
		result = SIM_READY;
	else if(strcmp(status,DBUS_SIM_PIN_REQUIRED))
		result = SIM_PIN_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PUK_REQUIRED))
		result = SIM_PUK_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PIN2_REQUIRED))
		result = SIM_PIN2_REQUIRED;
	else
		result = SIM_PUK2_REQUIRED;
	free(status);
	return result;
}


