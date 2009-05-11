/*
 *  Copyright (C) 2009
 *      Authors (alphabetical) :
 *              Klaus 'mrmoku' Kurzmann (mok@fluxnetz.de)
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


#include <glib.h>
#include <glib/gthread.h>
#include <stdlib.h>
#include <string.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "ophonekitd-dbus-contact-cache.h"
#include "ophonekitd-main.h"

G_DEFINE_TYPE(OphonekitdContactCacheService, ophonekitd_contact_cache_service, G_TYPE_OBJECT)

GHashTable *contact_cache = NULL;

static void ophonekitd_contact_cache_service_class_init (OphonekitdContactCacheServiceClass *klass)
{
}

static void ophonekitd_contact_cache_service_init (OphonekitdContactCacheService *object)
{
    g_debug("initializing contact cache");
}


OphonekitdContactCacheService *ophonekitd_contact_cache_service_new (void)
{
    return (g_object_new (OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE, NULL));
}



void cache_phonebook_entry(void *_entry, void *data) {
    GValueArray *entry = (GValueArray *)_entry;
    char *number = strdup(g_value_get_string(g_value_array_get_nth(entry, 2)));
    char *name = strdup(g_value_get_string(g_value_array_get_nth(entry, 1)));
    g_hash_table_insert(contact_cache, number, name);
}



void ophonekitd_contact_cache_load_callback(GError *error, GPtrArray *contacts, gpointer userdata)
{
    g_debug("ophonekitd_contact_cache_load_callback called");
    if(error == NULL && contacts != NULL) {
        g_debug("creating contact_cache");
        contact_cache = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
        if (!contact_cache) {
            g_warning("could not allocate contact cache");
            return;
        }
        g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
    }
    else
        g_debug("caching phonebook failed: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);
}


void ophonekitd_contact_cache_load(void)
{
    ogsmd_sim_retrieve_phonebook("contacts", ophonekitd_contact_cache_load_callback, NULL);
}
    



void ophonekitd_contact_cache_unload(void)
{
    if( contact_cache ) {
        g_hash_table_destroy( contact_cache );
        contact_cache = NULL;
    }
}




void ophonekitd_contact_cache_service_lookup (OphonekitdContactCacheService *object, const char *number, DBusGMethodInvocation *context)
{
    if (contact_cache == NULL) {
        g_debug("contact cache not yet initialized");
        dbus_g_method_return(context, "");
        return;
    }

    g_debug("looking for '%s' in contact_cache", number);
    if (!number || !*number || !strcmp(number, "*****")) {
        g_debug("contact_cache: got unknown number");
        dbus_g_method_return(context, "");
        return;
    }

    /* strip eventual " from number */
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

    char *name = g_hash_table_lookup(contact_cache, number);
    if (name && *name) {
        g_debug("found name '%s'", name);
        dbus_g_method_return(context, strdup(name));
    }
    else
        dbus_g_method_return(context, "");
}



