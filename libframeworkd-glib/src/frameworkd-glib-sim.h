/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Andreas Dalsgaard <andreas.dalsgaard@gmail.com>
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

#ifndef _FRAMEWORKD_GLIB_SIM_H
#define _FRAMEWORKD_GLIB_SIM_H

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

void sim_incoming_message_handler (DBusGProxy *proxy, const int index, gpointer user_data);

void sim_get_auth_status(void (*callback)(GError*, int status, gpointer), gpointer userdata);

void sim_send_auth_code(const char* pin, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_unlock(const char* puk, const char* pin, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_change_auth_code(const char* old, const char* new, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_retrieve_message(const int index, void (*callback)(GError*, char* number, char* content, gpointer), gpointer userdata);

void sim_retrieve_entry(const int index, void (*callback)(GError*, char* name, char* number, gpointer), gpointer userdata);

void sim_set_auth_code_required(const gboolean check, const char* pin, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_get_auth_code_required(void (*callback)(GError*, gboolean check, gpointer), gpointer userdata);

void sim_get_sim_info(void (*callback)(GError *,GHashTable* sim_info, gpointer), gpointer userdata);

void sim_send_generic_sim_command(char *command, void (*callback)(GError*, char* result, gpointer), gpointer userdata);

void sim_send_restricted_sim_command(const int command, const int fileid, const int p1, const int p2, const int p3, char *data, void (*callback)(GError*, char* result, gpointer), gpointer userdata);

void sim_get_home_zones(void (*callback)(GError*, GPtrArray*home_zones, gpointer), gpointer userdata);

void sim_get_phonebook_info(void (*callback)(GError*, GHashTable*phonebook_info, gpointer), gpointer userdata);

void sim_delete_entry(const int index, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_store_entry(const int index, char *name, char*number, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_get_messagebook_info( void (*callback)(GError*, GHashTable*messagebook_info, gpointer), gpointer userdata);

void sim_retrieve_messagebook( const char* category, void (*callback)(GError*, GPtrArray* messages, gpointer), gpointer userdata);

void sim_get_service_center_number(void (*callback)(GError*, char*number, gpointer), gpointer userdata);

void sim_set_service_center_number(char *number,  void (*callback)(GError*, gpointer), gpointer userdata);

void sim_delete_message(const int index, void (*callback)(GError*, gpointer), gpointer userdata);

void sim_store_message(char *number, char* content, void (*callback)(GError*, int index, gpointer), gpointer userdata);

void sim_send_stored_message(const int index, void (*callback)(GError*, int transaction_index, gpointer), gpointer userdata);

GError* sim_handle_errors(GError *dbus_error);

extern DBusGProxy *simBus;

#endif
