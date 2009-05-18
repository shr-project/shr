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
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <glib.h>
#include <stdlib.h>

typedef struct
{
    gchar *library;
    gchar *international_prefix;
    gint international_prefix_len;
    gchar *national_prefix;
    gint national_prefix_len;
    gchar *country_code;
    gchar *home_code;
    GHashTable *contact_cache;
} Settings;

static void *phonegui_library = NULL;
static Settings *conf = NULL;

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
    conf->international_prefix = g_key_file_get_string(keyfile,"local","international_prefix",NULL);
    conf->international_prefix_len = strlen(conf->international_prefix);
    conf->national_prefix = g_key_file_get_string(keyfile,"local","national_prefix",NULL);
    conf->national_prefix_len = strlen(conf->national_prefix);
    conf->country_code = g_key_file_get_string(keyfile,"local","country_code",NULL);
    conf->home_code = g_key_file_get_string(keyfile,"local","home_code",NULL);


    /* Load library */
    if(conf->library != NULL) {
        phonegui_library = dlopen(conf->library, RTLD_LOCAL | RTLD_LAZY);
        if(!phonegui_library) {
            g_error("Loading %s failed: %s", conf->library, dlerror());
        } 
    } else {
        g_error("Loading failed. Conf->library not set.");
    }
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

gchar *phonegui_get_user_international_prefix() {
    return conf->international_prefix;
}

gchar *phonegui_get_user_national_prefix() {
    return conf->national_prefix;
}

gchar* phonegui_get_user_country_code() {
    return conf->country_code;
}

gchar* phonegui_get_user_home_code() {
    return conf->home_code;
}


/*

17:43 < DocScrutinizer> so: 
17:43 < DocScrutinizer> Nr1 := Nr s/<IP>(.*)/\+$1/   #00 49 911 12345 -> + 49 911 12345
17:43 < DocScrutinizer> Nr2 = Nr1 s/<NP>/\+<CC>/  # 0 911 12345 ->  + 49 911 12345
17:43 < DocScrutinizer> Nr3 = Nr2 s/[^\+](.*)/\+<CC><HC>$1/  # 12345 ->  + 49 911 12345
17:43 < DocScrutinizer> step3 above isn't considered very practical for cellphones
17:43 < DocScrutinizer> <IP>, <CC>, <NP>, and <HC> are user definable config-values
17:47 < DocScrutinizer> oops, for correctness:    non-normalized my be: <NUMBER> | <NationalPrefix><anyHC><NUMBER> | 
<InterntlPrefix><anyCC><anyHC><NUMBER>
*/

gchar *normalize_phone_number(gchar *_number) {

    gchar *number;

    /* step 1: normalize 00 to + */
    if (conf->international_prefix_len > 0 && strncmp(_number, conf->international_prefix, conf->international_prefix_len) == 0) {
	number = g_strdup(_number[conf->international_prefix_len - 1]);
        *number = '+';
    }
    /* step 2: normalize national prefix to +<CC>
     * if national_prefix = "" assume it's a match */
    else if (conf->national_prefix_len >= 0 && strncmp(_number, conf->national_prefix, conf->national_prefix_len) == 0) {
        number = g_strconcat("+", conf->country_code, _number + conf->national_prefix_len, NULL);
    }
    else {
	number = g_strdup(_number);
    }
    g_debug("Normalized %s to %s\n", _number, number);
    return (number);
}

gboolean phone_number_equal(gconstpointer _a, gconstpointer _b)
{
    gboolean ret = FALSE;
    gchar *a = normalize_phone_number((gchar *)_a);
    gchar *b = normalize_phone_number((gchar *)_b);

    if (strcmp(a, b) == 0)
        ret = TRUE;

    g_free(a);
    g_free(b);

    return ret;
}

guint phone_number_hash(gconstpointer v)
{
    gchar *n = normalize_phone_number((gchar *)v);
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
        conf->contact_cache = g_hash_table_new_full(phone_number_hash, phone_number_equal, free, free);
        if (!conf->contact_cache) {
            g_warning("could not allocate contact cache");
            return;
        }
        g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
    }
    else
        g_debug("caching phonebook failed: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);

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
