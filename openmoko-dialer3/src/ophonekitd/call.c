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
#include "call.h"
#include "dbus.h"
#include "dbus/call.h"

DBusGProxy *callBus = NULL;

void call_status_handler (DBusGProxy *proxy, const int id, const char *status, GHashTable ** properties, gpointer user_data)
{
  if(!strcmp(status, CALL_STATUS_INCOMING)) {
	char *number = NULL;
	if(!(properties && *properties && (number = (char*)g_hash_table_lookup(*properties,(gconstpointer)CALL_PROPERTIES_NUMBER)))) {
		// No properties ?!
	}

  	// Display incoming UI
  } else if(!strcmp(status, CALL_STATUS_OUTGOING)) {
  	// Display outgoing UI
  } else if(!strcmp(status, CALL_STATUS_ACTIVE)) {
  	// TODO
  } else if(!strcmp(status, CALL_STATUS_RELEASED)) {
  	// Close UI
  } else { // HELD
  }
  printf ("Received call status");
}

GError* call_handle_errors(GError *dbus_error) {
	const char *error_name = dbus_g_error_get_name(dbus_error);
	int callError = 0;

	lose_gerror ("Unknown call error", dbus_error);
	
	return g_error_new (CALL_ERROR, callError, "TODO %s", error_name);
}

gboolean call_initiate(GError** error, const char *number, const char* call_type, int*id_call) {
	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;

	if(!(result = org_freesmartphone_GSM_Call_initiate (callBus, number, call_type, id_call, &dbus_error))) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error, tmperror);
	}

	return result;
}

gboolean call_release(GError** error, const char *message, const int id_call) {
	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;

	if(!(result = org_freesmartphone_GSM_Call_release (callBus, message, id_call, &dbus_error))) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error, tmperror);
	}

	return result;
}

gboolean call_activate(GError** error, int id_call) {
	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;

	if(!(result = org_freesmartphone_GSM_Call_activate (callBus, id_call, &dbus_error))) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error, tmperror);
	}

	return result;
}


