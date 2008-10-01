/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
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

#include "ophonekitd-main.h"
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-network.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-device.h>
#include <frameworkd-glib/ousaged/frameworkd-glib-ousaged.h>
#include "ophonekitd-phonegui.h"

gboolean sim_auth_ui_active = FALSE;
gboolean incoming_call_ui_active = FALSE;
gboolean outgoing_call_ui_active = FALSE;


int main(int argc, char ** argv) {
    GMainLoop *mainloop = NULL;
    FrameworkdHandlers fwHandler;

    /* Load, connect and initiate phonegui library */
    phonegui_load(CONFIG_FILE);
    phonegui_connect();
    phonegui_init(argc, argv);

    /* Initiate glib main loop for dbus */
    g_type_init();
    mainloop = g_main_loop_new (NULL, FALSE);

    /* Register dbus handlers */
    fwHandler.networkStatus = NULL;
    fwHandler.networkSignalStrength = NULL;
    fwHandler.simAuthStatus = ophonekitd_sim_auth_status_handler;
    fwHandler.simIncomingMessage = ophonekitd_sim_incoming_message_handler;
    fwHandler.callCallStatus = ophonekitd_call_status_handler;
    dbus_connect_to_bus(&fwHandler);
    g_debug("Connected to the buses");

    /* It's asynchronous and will detach immediately.
     * Every 5 seconds, we fetch the resource list in
     * order to know wether GSM is available.
     * This is needed for frameworkd startup. GSM is
     * not available immediately!
     */
    list_resources();

    /* Start glib main loop */
    g_debug("Entering glib main loop");
    g_main_loop_run (mainloop);

    exit(EXIT_SUCCESS);
}

void ophonekitd_call_status_handler(const int call_id, const int status, GHashTable *properties) {
    g_debug("call status handler called, id: %d, status: %d", call_id, status);
	gchar *number = g_hash_table_lookup(properties, "peer");

    switch(status) {
        case CALL_STATUS_INCOMING:
            g_debug("incoming call");
            phonegui_incoming_call_ui_show(call_id, status, number);
            incoming_call_ui_active = TRUE;
            break;
        case CALL_STATUS_OUTGOING:
            g_debug("outgoing call");
            phonegui_outgoing_call_ui_show(call_id, status, number);
            outgoing_call_ui_active = TRUE;
            break;
        case CALL_STATUS_RELEASE:
            g_debug("release call");

            /* TODO: Add call_id handling */

            if(incoming_call_ui_active == TRUE)
            {
                phonegui_incoming_call_ui_hide(call_id);
                incoming_call_ui_active = FALSE;
            }

            if(outgoing_call_ui_active == TRUE)
            {
                phonegui_outgoing_call_ui_hide(call_id);
                outgoing_call_ui_active = FALSE;
            }
            break;
        case CALL_STATUS_HELD:
        case CALL_STATUS_ACTIVE:
        	break;
        default:
        	g_error("Unknown CallStatus");
    }
}


void ophonekitd_sim_auth_status_handler(const int status) {
    g_debug("ophonekitd_sim_auth_status_handler()");
    if(status == SIM_READY) {
        g_debug("sim ready");
        
        if(sim_auth_ui_active)
        {
            phonegui_sim_auth_ui_hide(status);
        }
        ogsmd_network_register(register_to_network_callback, NULL);
    } else {
        g_debug("sim not ready");
        phonegui_sim_auth_ui_show(status);
    }
}


void ophonekitd_sim_incoming_message_handler(const int id) {
    g_error("ophonekitd_sim_incoming_message_handler()");
    phonegui_message_ui_show(id);
}


gboolean list_resources() {
    g_debug("list_resources()");
    ousaged_list_resources(list_resources_callback, NULL);
    return FALSE;
}

void list_resources_callback(GError *error, char** resources, gpointer userdata) {
    g_debug("list_resources_callback()");
    if(error == NULL)
    {
        gboolean available = FALSE;
        int i = 0;
        while(resources[i] != NULL)
        {
            g_debug("Resource %s available", resources[i]);
            if(!strcmp(resources[i], "GSM"))
            {
                available = TRUE;
                break;
            }
            i++;
        }

        if(available)
        {
            /* Sleep 10 seconds to avoid dbus error. It's a
             * workaround for a frameworkd bug.
             * FIXME
             */
            sleep(10);

            g_debug("Request GSM resource");
            ousaged_request_resource("GSM", request_resource_callback, NULL);
        }
        else
        {
            g_debug("gsm not available, try again in 5s");
            g_timeout_add(5000, list_resources, NULL);
        }
    }
    else if(IS_DBUS_ERROR(error, DBUS_ERROR_SERVICE_NOT_AVAILABLE) || IS_DBUS_ERROR(error, DBUS_ERROR_NO_REPLY))
    {
        g_debug("dbus not available, try again in 5s");
        g_timeout_add(5000, list_resources, NULL);
    }
    else
    {
        g_error("Unknown error");
    }
}

void request_resource_callback(GError *error, gpointer userdata) {
    g_debug("request_resource_callback()");

    if(error == NULL)
    {
        g_debug("ok");
        ogsmd_device_set_antenna_power(TRUE, power_up_antenna_callback, NULL);
    }
    else
    {   
        /* FIXME: Remove this when frameworkd code is ready */
        g_debug("request resource error, try again in 5s");
        g_timeout_add(5000, list_resources, NULL);
    }
}


void power_up_antenna_callback(GError *error, gpointer userdata) {
    g_debug("power_up_antenna_callback()");
    if(error != NULL)
    {
        if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED))
        {
            /* This auth status query is needed for startup when
             * there's no auth status signal emitted
             */
            ogsmd_sim_get_auth_status(sim_auth_status_callback, NULL);
        }
        else 
        {
            g_error("Unknown error");
        }
    }
}

void sim_auth_status_callback(GError *error, int status, gpointer userdata) {
    g_debug("sim_auth_status_callback()");

    if(sim_auth_ui_active == TRUE) {
        phonegui_sim_auth_ui_hide(status);
    }

    if(status == SIM_READY)
    {
        ogsmd_network_register(register_to_network_callback, NULL);
    }
    else
    {
        if(sim_auth_ui_active == FALSE)
        {
            phonegui_sim_auth_ui_show(status);
            sim_auth_ui_active = TRUE;
        }
    }
}

void register_to_network_callback(GError *error, gpointer userdata) {
    g_debug("register_to_network_callback()");
    if(error == NULL) {
        /* Antenna works, registered to network */
    } else {
        /* TODO */
    }
}
