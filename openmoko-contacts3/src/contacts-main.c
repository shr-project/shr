/*
 *  Copyright (C) 2007 Openmoko, Inc.
 *
 *  Authored by:
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

#include "contacts-main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include "contacts-phonegui.h"

#define CONFIG_FILE "/etc/ophonekitd/gui.conf"


int main(int argc, char **argv) {
    /* Connect to frameworkd */
    connect_to_frameworkd();

    /* Load, connect and initiate phonegui */
    phonegui_load(CONFIG_FILE);
    phonegui_connect();
    phonegui_init(argc, argv, NULL);
    phonegui_contacts_show();

    /* Initiate and start glib main loop */
    GMainLoop *mainloop = NULL;
    g_type_init();
    mainloop = g_main_loop_new(NULL, FALSE);
    g_debug("Entering glib main loop");
    g_main_loop_run(mainloop);

    return EXIT_SUCCESS;
}

void connect_to_frameworkd() {
    FrameworkdHandlers fwHandler;
    fwHandler.networkStatus = NULL;
    fwHandler.networkSignalStrength = NULL;
    fwHandler.simAuthStatus = NULL;
    fwHandler.simIncomingStoredMessage = NULL;
    fwHandler.callCallStatus = NULL;
    fwHandler.deviceIdleNotifierState = NULL;
    dbus_connect_to_bus(&fwHandler);
}

