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
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>

int main(int argc, char **argv) {
    /* Connect to frameworkd */
    frameworkd_handler_connect(frameworkd_handler_new());

    /* Load, connect and initiate phonegui */
    phonegui_load("shr-dialer");
    phonegui_init(argc, argv, exit_callback);

    /* Initiate glib main loop */
    GMainLoop *mainloop = NULL;
    g_type_init();
    mainloop = g_main_loop_new (NULL, FALSE);

    /* Run glib main loop and start ui */
    g_timeout_add(0, start, NULL);
    g_main_loop_run(mainloop);

    return EXIT_SUCCESS;
}

gboolean start() {
    phonegui_dialer_show();    
    return FALSE;
}

void exit_callback() {
    exit(EXIT_SUCCESS);
}
