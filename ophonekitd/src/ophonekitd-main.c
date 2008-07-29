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
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/frameworkd-glib-call.h>
#include <frameworkd-glib/frameworkd-glib-sim.h>
#include <frameworkd-glib/frameworkd-glib-network.h>
#include <frameworkd-glib/frameworkd-glib-device.h>
#include "ophonekitd-main.h"
#include <frameworkd-glib/frameworkd-glib-phonegui.h>

static gboolean connected_to_network = FALSE;

int main(int argc, char ** argv) {
        GMainLoop *mainloop = NULL;
        FrameworkdHandlers fwHandler;

        g_type_init();

#ifdef DEBUG
        printf("Starting...\n");
#endif
        mainloop = g_main_loop_new (NULL, FALSE);

        fwHandler.networkStatus = ophonekitd_network_status_handler;
        fwHandler.simAuthStatus = ophonekitd_sim_auth_status_handler;
        fwHandler.callCallStatus = ophonekitd_call_status_handler;
        fwHandler.smsMessageSent = ophonekitd_sms_message_sent_handler;
        fwHandler.smsIncomingMessage = ophonekitd_sms_incoming_message_handler;
        fwHandler.networkSignalStrength = NULL;

        dbus_connect_to_bus(&fwHandler);
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
            if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
                sim_display_code_UI();
            }
                /* TODO */
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

void ophonekitd_call_status_handler(const int id_call, const int status, GHashTable *properties) {
}

void ophonekitd_network_status_handler(GHashTable *status) {
}

void ophonekitd_sim_auth_status_handler(const int status) {
        if(status == SIM_READY) {
                phonegui_destroy_pin_UI();
        }
        else {
                phonegui_display_pin_UI(status);
        }
#ifdef DEBUG
        printf ("Auth status handler calling the UI on a %i signal", status);
#endif

}

void ophonekitd_sms_message_sent_handler(const int id, gboolean success, const char* reason) {
}

void ophonekitd_sms_incoming_message_handler(const int id) {
}
