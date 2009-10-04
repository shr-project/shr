/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Julien "Ainulindalë" Cassignol
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

#include "frameworkd-phonegui.h"
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <glib.h>
#include <stdlib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>

#include <phone-utils.h>

void (*phonegui_init)(int argc, char **argv, void (*exit_cb)());

/* Calls */
void (*phonegui_incoming_call_show)(const int id, const int status, const char *number);
void (*phonegui_incoming_call_hide)(const int id);
void (*phonegui_outgoing_call_show)(const int id, const int status, const char *number);
void (*phonegui_outgoing_call_hide)(const int id);

/* Contacts */
void (*phonegui_contacts_hide)();
void (*phonegui_contacts_hide)();

/* Dialer */
void (*phonegui_dialer_show)();
void (*phonegui_dialer_hide)();

/* Dialog */
void (*phonegui_dialog_show)(int type);
void (*phonegui_dialog_hide)();

/* Messages */
void (*phonegui_messages_show)();
void (*phonegui_messages_hide)();
void (*phonegui_message_show)(const int id);
void (*phonegui_message_hide)();

/* Sim auth */
void (*phonegui_sim_auth_show)(const int status);
void (*phonegui_sim_auth_hide)(const int status);

/* USSD */
void (*phonegui_ussd_show)(int mode, const char *message);
void (*phonegui_ussd_hide)();


typedef struct
{
    gchar *library;
    GHashTable *contact_cache;
} Settings;

DBusGProxy *GQuery = NULL;
static void *phonegui_library = NULL;
static Settings *conf = NULL;

static void phonegui_connect();

void phonegui_load(const char *application_name) {
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    if (!g_key_file_load_from_file (keyfile, FRAMEWORKD_PHONEGUI_CONFIG, flags, &error))
    {
        g_error (error->message);
        return;
    }

    conf = g_slice_new (Settings);
    conf->library = g_key_file_get_string(keyfile,"phonegui","library",NULL);
    /* Load library */
    if(conf->library != NULL) {
        phonegui_library = dlopen(conf->library, RTLD_LOCAL | RTLD_LAZY);
        if(!phonegui_library) {
            g_error("Loading %s failed: %s", conf->library, dlerror());
        } 
    } else {
        g_error("Loading failed. Conf->library not set.");
    }
    phonegui_connect();
    /* init phone utils */
    /* FIXME: should deinit somewhere!*/
    phone_utils_init();
    
}


void *phonegui_get_function(const char *name) {
    if(phonegui_library == NULL) {
        g_error("phonegui library not loaded");
    }

    void *pointer = dlsym(phonegui_library, name);
    char *error;
    if((error = dlerror()) != NULL)  {
        g_error("Symbol not found: %s", error);
    }
    return pointer;
}

guint phone_number_hash(gconstpointer v)
{
    gchar *n = phone_utils_normalize_number((char *)v);
    guint ret = g_str_hash(n);
    g_free(n);
    return (ret);
}

void cache_phonebook_entry(GValueArray *entry, void *data) {
    char *number = strdup(g_value_get_string(g_value_array_get_nth(entry, 2)));
    char *name = strdup(g_value_get_string(g_value_array_get_nth(entry, 1)));
    g_hash_table_insert(conf->contact_cache, number, name);
}


void cache_phonebook_callback(GError *error, GPtrArray *contacts, gpointer userdata) {
    g_debug("cache_phonebook_callback called");
    if(error == NULL && contacts != NULL) {
        g_debug("creating contact_cache");
        conf->contact_cache = g_hash_table_new_full(phone_number_hash, phone_utils_numbers_equal, free, free);
        if (!conf->contact_cache) {
            g_warning("could not allocate contact cache");
            return;
        }
        g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
    }
    else
        g_debug("caching phonebook failed: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);

}

static char *
_lookup_add_prefix(const char *_number)
{
	char *number = NULL;
	if (strncmp(_number, "tel:", 4)) {
		number = malloc(strlen(_number) + 5); /* 5 is for "tel:" and the null */
		if (!number) {
			return NULL;
		}
		strcpy(number, "tel:");
		strcat(number, _number);
	}


	return number;
}

void
phonegui_contact_lookup(const char *_number, void (*name_callback)(GError *, char *, gpointer), void *data)
{
	GHashTable *query = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
	char *number = _lookup_add_prefix(_number);
		
	g_debug("number = %s --> querying name...", _number);

	
	GValue *value = g_slice_alloc0(sizeof(GValue));
	g_value_init(value, G_TYPE_STRING);
	g_value_set_string(value, (number) ? number : _number); /*  we prefer using number */
	g_hash_table_insert(query, "Phone", value);


	opimd_contacts_get_single_entry_single_field(query, "Name", name_callback, data);

	if (number)
		free(number);

}

char *phonegui_contact_cache_lookup(char *number) {
    if(conf->contact_cache == NULL) return (number);
    g_debug("looking for '%s' in contacts_cache", number);
    if (!number || !*number || !strcmp(number, "*****")) {
        g_debug("contact_cache: got unknown number");
        return ("");
    }
    if (*number == '"') {
        number++;
        char *s = number;
        while (*s) {
            if (*s == '"') {
                *s = '\0';
                break;
            }
            s++;
        }
    }

    char *name = g_hash_table_lookup(conf->contact_cache, number);
    if (name && *name) {
        g_debug("found name '%s'", name);
        return (name);
    }
    return (number);
}

void phonegui_init_contacts_cache() {
    ogsmd_sim_retrieve_phonebook("contacts", cache_phonebook_callback, NULL);
}

void phonegui_destroy_contacts_cache() {
    g_hash_table_destroy(conf->contact_cache);
}

static void
phonegui_connect()
{
	phonegui_init = phonegui_get_function("phonegui_init");

	phonegui_incoming_call_show = phonegui_get_function("phonegui_incoming_call_show");
	phonegui_incoming_call_hide = phonegui_get_function("phonegui_incoming_call_hide");
	phonegui_outgoing_call_show = phonegui_get_function("phonegui_outgoing_call_show");
	phonegui_outgoing_call_hide = phonegui_get_function("phonegui_outgoing_call_hide");

	phonegui_contacts_hide = phonegui_get_function("phonegui_contacts_hide");
	phonegui_contacts_hide = phonegui_get_function("phonegui_contacts_hide");

	phonegui_dialer_show = phonegui_get_function("phonegui_dialer_show");
	phonegui_dialer_hide = phonegui_get_function("phonegui_dialer_hide");

	phonegui_dialog_show = phonegui_get_function("phonegui_dialog_show");
	phonegui_dialog_hide = phonegui_get_function("phonegui_dialog_hide");

	phonegui_message_show = phonegui_get_function("phonegui_message_show");
	phonegui_message_hide = phonegui_get_function("phonegui_message_hide");
	phonegui_messages_show = phonegui_get_function("phonegui_messages_show");
	phonegui_messages_hide = phonegui_get_function("phonegui_messages_hide");

	phonegui_sim_auth_show = phonegui_get_function("phonegui_sim_auth_show");
	phonegui_sim_auth_hide = phonegui_get_function("phonegui_sim_auth_hide");

	phonegui_ussd_show = phonegui_get_function("phonegui_ussd_show");
	phonegui_ussd_hide = phonegui_get_function("phonegui_ussd_hide");
}