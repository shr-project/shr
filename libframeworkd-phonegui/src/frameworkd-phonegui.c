/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
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

static void *phonegui_library = NULL;

void phonegui_load(const char *application_name) {

    /*
     * application_name will be used later
     * in order to define different phoneguis
     * for different applications
     */

    char name[64];
    FILE *f;

    /* Reading gui configuraton file */
    f = fopen(FRAMEWORKD_PHONEGUI_CONFIG, "r");
    if(f == NULL) {
        g_error("Could not open %s", FRAMEWORKD_PHONEGUI_CONFIG);
    }
    if(fgets(name, 63, f) == NULL) {
        g_error("Reading failed");
    }
    name[strlen(name) - 1] = '\0';
    fclose(f);
    

    /* Load library */
    phonegui_library = dlopen(name, RTLD_LOCAL | RTLD_LAZY);
    if(!phonegui_library) {
        g_error("Loading %s failed: %s", name, dlerror());
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

