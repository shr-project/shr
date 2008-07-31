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
#define DBUS_CALL_ERROR_NO_CARRIER "org.freesmartphone.GSM.Call.NoCarrier"
#define DBUS_CALL_ERROR_NOT_FOUND "org.freesmartphone.GSM.Call.NotFound"
#define IS_CALL_ERROR(error, code) g_error_matches(error, CALL_ERROR, code)

#define DBUS_CALL_TYPE_VOICE "voice"
#define DBUS_CALL_TYPE_DATA "data"

#define DBUS_CALL_STATUS_INCOMING "incoming"
#define DBUS_CALL_STATUS_OUTGOING "outgoing"
#define DBUS_CALL_STATUS_ACTIVE "active"
#define DBUS_CALL_STATUS_HELD "held"
#define DBUS_CALL_STATUS_RELEASED "released"

#define DBUS_CALL_PROPERTIES_REASON "reason"
#define DBUS_CALL_PROPERTIES_NUMBER "number"

typedef enum {
	CALL_ERROR_NO_CARRIER = -1,
	CALL_ERROR_NOT_FOUND = -2,
} CallErrors;

typedef enum {
    CALL_TYPE_VOICE,
    CALL_TYPE_DATA
} CallTypes;

typedef enum {
    CALL_STATUS_INCOMING,
    CALL_STATUS_OUTGOING,
    CALL_STATUS_ACTIVE,
    CALL_STATUS_HELD,
    CALL_STATUS_RELEASED
} CallStatus;

typedef enum {
    CALL_PROPERTIES_REASON,
    CALL_PROPERTIES_NUMBER
} CallProperties;

void call_status_handler (DBusGProxy *proxy, const int id, const char *status, GHashTable * properties, gpointer user_data);

void call_initiate(const char *number, const char* call_type, void (*callback)(GError *, int id_call, gpointer userdata), gpointer userdata);

void call_release(const char *message, const int id_call, void (*callback)(GError *, gpointer userdata), gpointer userdata);

void call_activate(const int id_call, void (*callback)(GError *, gpointer userdata), gpointer userdata);

extern DBusGProxy *callBus;

#endif
