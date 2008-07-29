/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "frameworkd-glib-dbus.h"
#include "frameworkd-glib-call.h"
#include "frameworkd-glib-sim.h"
#include "frameworkd-glib-network.h"
#include "frameworkd-glib-device.h"
#include "ophonekitd-main.h"	
#include "../libphonegui/frameworkd-glib-phonegui.h"

static gboolean connected_to_network = FALSE;

int main(int argc, char ** argv) {
        GMainLoop *mainloop = NULL;

        g_type_init();

#ifdef DEBUG
        printf("Starting...\n");
#endif
        mainloop = g_main_loop_new (NULL, FALSE);
        dbus_connect_to_bus();
        phonegui_init(argc, argv);
#ifdef DEBUG
        printf("Connected to the buses\n");
#endif
        g_timeout_add(5000, power_up_antenna, NULL);
#ifdef DEBUG
        printf("Entering mainloop.\n");
#endif
        g_main_loop_run (mainloop);

        exit(EXIT_SUCCESS);

}

gboolean power_up_antenna() {
        device_set_antenna_power(TRUE, power_up_antenna_callback);

        return connected_to_network; // End timeout
}

void power_up_antenna_callback(GError *error) {
        if(error != NULL) {
                /* TODO */
                g_error_free(error);
        }
        else {
                network_register(register_to_network_callback);
        }
}

void register_to_network_callback(GError *error) {
        if(error != NULL) {
                /* TODO */
        } else {
                /* Antenna works, registered to network */
                connected_to_network = TRUE;
        }
}
