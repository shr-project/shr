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

#ifndef _SIM_H
#define _SIM_H

#include <dbus/dbus-glib.h>

#define DBUS_SIM_READY "READY"
#define DBUS_SIM_PIN_REQUIRED "SIM PIN"
#define DBUS_SIM_PUK_REQUIRED "SIM PUK"
#define DBUS_SIM_PIN2_REQUIRED "SIM PIN2"
#define DBUS_SIM_PUK2_REQUIRED "SIM PUK2"

#define DBUS_SIM_ERROR_NOT_PRESENT "org.freesmartphone.GSM.SIM.NotPresent"
#define DBUS_SIM_ERROR_AUTH_FAILED "org.freesmartphone.GSM.SIM.AuthFailed"
#define DBUS_SIM_ERROR_BLOCKED "org.freesmartphone.GSM.SIM.Blocked"
#define DBUS_SIM_ERROR_NOT_FOUND "org.freesmartphone.GSM.SIM.NotFound"
#define DBUS_SIM_ERROR_MEMORY_FULL "org.freesmartphone.GSM.SIM.MemoryFull"
#define DBUS_SIM_ERROR_INVALID_INDEX "org.freesmartphone.GSM.SIM.InvalidIndex"

#define SIM_ERROR g_quark_from_static_string(SIM_INTERFACE)
#define IS_SIM_ERROR(error, code) g_error_matches(error, SIM_ERROR, code)
typedef enum {
	SIM_READY,
	SIM_PIN_REQUIRED,
	SIM_PUK_REQUIRED,
	SIM_PIN2_REQUIRED,
	SIM_PUK2_REQUIRED
} SimStatus;

typedef enum {
	SIM_ERROR_NOT_PRESENT = -1,
	SIM_ERROR_AUTH_FAILED = -2,
	SIM_ERROR_BLOCKED = -3,
	SIM_ERROR_NOT_FOUND = -4,
	SIM_ERROR_MEMORY_FULL = -5,
	SIM_ERROR_INVALID_INDEX = -6
} SimErrors;

void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data);
int sim_handle_authentication_state(const char*status);

void sim_get_authentication_state(void (*callback)(GError*, int));
void sim_get_authentication_state_callback(DBusGProxy *bus, char* status, GError* dbus_error, gpointer userdata);

void sim_send_pin_code(const char* pin, void (*callback)(GError*));
void sim_send_pin_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata);

void sim_send_puk_code(const char* puk, const char* pin, void (*callback)(GError*));
void sim_send_puk_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata);

void sim_display_code_UI ();
void sim_display_code_UI_callback(GError* error, int status);

GError* sim_handle_errors(GError *dbus_error);

extern DBusGProxy *simBus;

#endif
