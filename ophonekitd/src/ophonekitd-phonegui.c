/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
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

#include "ophonekitd-phonegui.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include "ophonekitd-main.h"

void *phonegui_load() {
    char name[64];
    void *library;

    /* Reading gui configuraton file */
    FILE *f;
    f = fopen(CONFIG_FILE, "r");
    if(f == NULL) {
        g_error("Could not open %s", CONFIG_FILE);
    }
    char *r = fgets(name, 63, f);
    if(r == NULL) {
        g_error("Reading failed");
    }
    name[strlen(name) - 1] = '\0';
    fclose(f);
    

    /* Load library */
    library = dlopen(name, RTLD_LOCAL | RTLD_LAZY);
    if(!library) {
        g_error("Loading %s failed: %s", name, dlerror());
    }

    return library;
}

void phonegui_connect(void *library) {
    phonegui_init = phonegui_get_function(library, "phonegui_init");
    phonegui_sim_auth_ui_show = phonegui_get_function(library, "phonegui_sim_auth_ui_show");
    phonegui_sim_auth_ui_hide = phonegui_get_function(library, "phonegui_sim_auth_ui_hide");
    phonegui_incoming_call_ui_show = phonegui_get_function(library, "phonegui_incoming_call_ui_show");
    phonegui_incoming_call_ui_hide = phonegui_get_function(library, "phonegui_incoming_call_ui_hide");
    phonegui_outgoing_call_ui_show = phonegui_get_function(library, "phonegui_outgoing_call_ui_show");
    phonegui_outgoing_call_ui_hide = phonegui_get_function(library, "phonegui_outgoing_call_ui_hide");
    phonegui_message_ui_show = phonegui_get_function(library, "phonegui_message_ui_show");
    phonegui_message_ui_hide = phonegui_get_function(library, "phonegui_message_ui_hide");
}

void *phonegui_get_function(void *library, char *name) {
    void *pointer = dlsym(library, name);
    char *error;
    if((error = dlerror()) != NULL)  {
        g_error("Symbol not found: %s", error);
    }
    return pointer;
}

