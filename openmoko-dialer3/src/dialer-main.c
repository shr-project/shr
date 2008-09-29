/*
 *  Copyright (C) 2007 Openmoko, Inc.
 *
 *  Authored by:
 *    Marc-Olivier Barre <marco@marcochapeau.org>
 *    OpenedHand Ltd <info@openedhand.com>
 *    quickdev
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

#include "dialer-main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>

#define CONFIG_FILE "/etc/ophonekitd/gui.conf"

void (*func_phonegui_dialer_launch)();


void connect_to_frameworkd() {
    FrameworkdHandlers fwHandler;
    fwHandler.networkStatus = NULL;
    fwHandler.networkSignalStrength = NULL;
    fwHandler.simAuthStatus = NULL;
    fwHandler.simIncomingMessage = NULL;
    fwHandler.callCallStatus = NULL;
    dbus_connect_to_bus(&fwHandler);
}

void* load_gui_library() {
    char name[32];
    void *library;

    /* Reading gui configuraton file */
    FILE *f;
    f = fopen(CONFIG_FILE, "r");
    if(f == NULL) {
        g_error("Could not open %s", CONFIG_FILE);
    }
    char *r = fgets(name, 31, f);
    if(r == NULL) {
        g_error("Reading failed");
    }
    name[strlen(name) - 1] = '\0';
    fclose(f);
    

    /* Load gui library */
    library = dlopen(name, RTLD_LOCAL | RTLD_LAZY);
    if(!library) {
        g_error("Loading %s failed: %s", name, dlerror());
    }

    return library;
}

void connect_library_functions(void *library) {
    char *error;
    func_phonegui_dialer_launch = dlsym(library, "phonegui_dialer_launch");
    if((error = dlerror()) != NULL)  {
        g_error("Symbol not found: %s", error);
    }
}


int main (int argc, char **argv) {
    void *library;

    g_debug("Load gui library");
    library = load_gui_library();

    g_debug("Connect library functions");
    connect_library_functions(library);

    g_debug("Connect to frameworkd");
    connect_to_frameworkd();

    g_debug("Launch dialer");
    func_phonegui_dialer_launch();

    return EXIT_SUCCESS;
}

