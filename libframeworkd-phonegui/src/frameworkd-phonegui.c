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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <glib.h>
typedef struct
{
    gchar *library;
    gchar *home_code;
    gchar *home_prefix;
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
    conf->home_code = g_key_file_get_string(keyfile,"local","home_code",NULL);
    conf->home_prefix = g_key_file_get_string(keyfile,"local","home_prefix",NULL);



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
        if(phonegui_library == NULL)
        {
            g_error("phonegui library not loaded");
        }

        void *pointer = dlsym(phonegui_library, name);
        char *error;
        if((error = dlerror()) != NULL)  {
            g_error("Symbol not found: %s", error);
        }
        return pointer;
    }

gchar* phonegui_get_user_home_code() {
    return conf->home_code;
}

gchar* phonegui_get_user_home_prefix() {
    return conf->home_prefix;
}
