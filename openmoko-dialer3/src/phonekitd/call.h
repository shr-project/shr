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

#ifndef _CALL_H
#define _CALL_H

#define CALL_ERROR g_quark_from_static_string(CALL_INTERFACE)
#define DBUS_CALL_ERROR_NOT_PRESENT "org.freesmartphone.GSM.Call.NoCarrier"
#define DBUS_CALL_ERROR_NOT_PRESENT "org.freesmartphone.GSM.Call.NotFound"

#define CALL_TYPE_VOICE "voice"
#define CALL_TYPE_DATA "data"

typedef enum {
	CALL_ERROR_NO_CARRIER = -1,
	CALL_ERROR_NOT_FOUND = -2,
} CallErrors;


void call_status_handler (DBusGProxy *proxy, const int id, const char *status, const GHashTable ** properties, gpointer user_data);
gboolean initiate_call(GError** error, const char *number, const char* call_type, int*id_call);
gboolean release_call(GError** error, const char *message, int id_call);
gboolean activate_call(GError** error, int id_call);
extern DBusGProxy *callBus;

#endif
