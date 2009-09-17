/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Klaus 'mrmoku' Kurzmann <mok@fluxnetz.de>
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

#ifndef _OPHONEKITD_MAIN_H
#define _OPHONEKITD_MAIN_H

#include <dbus/dbus-glib.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>

int main(int argc, char ** argv);

void ophonekitd_call_status_handler(const int id_call, const int status, GHashTable *properties);
void ophonekitd_sim_auth_status_handler(const int status);
void ophonekitd_sim_ready_status_handler(gboolean status);
void ophonekitd_sim_incoming_stored_message_handler(const int id);
void ophonekitd_device_idle_notifier_state_handler(const int state);
void ophonekitd_incoming_ussd_handler(int mode, const char* message);
void ophonekitd_resource_changed_handler(const char *name, gboolean state, GHashTable *attributes);

gboolean list_resources();
void list_resources_callback(GError *error, char** resources, gpointer userdata);
void request_resource_callback(GError *error, gpointer userdata);
void power_up_antenna_callback(GError *error, gpointer userdata);
void sim_auth_status_callback(GError *error, int status, gpointer userdata);
void sim_ready_status_callback(GError *error, gboolean status, gpointer userdata);
void register_to_network_callback(GError *error, gpointer userdata);
void get_messagebook_info_callback(GError *error, GHashTable *info, gpointer userdata);
int exit_callback(void *data, int type, void *event);
gboolean power_up_antenna();

void cache_phonebook_callback(GError *error, GPtrArray *contacts, gpointer userdata);
void cache_phonebook_entry(GValueArray *entry, void *data);
char *cache_phonebook_lookup(char *number);

#endif
