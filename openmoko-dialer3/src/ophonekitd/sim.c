/*
 *  Copyright (C) 2008 
 *	Authors (alphabetical) : 
 *		Marc-Olivier Barre <marco@marcochapeau.org>
 *		Julien Cassignol <ainulindale@gmail.com>
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

#include "dbus.h"
#include "dbus/sim.h"
#include "sim.h"
#include "moko-pin-ophonekitd.h"

DBusGProxy *simBus = NULL;

void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
    int st = sim_handle_authentication_state(status);
    if(st == SIM_READY) {
          // TODO
    }
    else {
          get_sim_code_from_user(st);
    }
#ifdef DEBUG
	printf ("Auth status handler calling the UI on a %s signal", status);
#endif

}

int sim_handle_authentication_state(const char*status) {
        if(!strcmp(status,DBUS_SIM_READY)) {
                return SIM_READY;
        }
        else if(!strcmp(status,DBUS_SIM_PIN_REQUIRED)) {
                return SIM_PIN_REQUIRED;
        }
        else if(!strcmp(status,DBUS_SIM_PUK_REQUIRED)) {
                return SIM_PUK_REQUIRED;
        }
        else if(!strcmp(status,DBUS_SIM_PIN2_REQUIRED)) {
                return SIM_PIN2_REQUIRED;
        }
        else {
                return SIM_PUK2_REQUIRED;
        }

}

void sim_get_authentication_state(void (*callback)(GError*, int)) {
    if(callback != NULL)
        org_freesmartphone_GSM_SIM_get_auth_status_async(simBus, sim_get_authentication_state_callback, callback);
}

void sim_get_authentication_state_callback(DBusGProxy *bus, char* status, GError* dbus_error, gpointer userdata) {
    void (*callback)(GError*, int) = NULL;
    GError *error = NULL;
    int st = 0;

    callback = userdata;
    
    if(dbus_error != NULL)
        error = dbus_handle_errors(dbus_error);
    else
        st = sim_handle_authentication_state(status);

    (*(callback)) (error, st);

}

void sim_send_pin_code(const char* pin, void (*callback)(GError*)) {
        if(pin != NULL && callback != NULL)
            org_freesmartphone_GSM_SIM_send_auth_code_async(simBus, pin, sim_send_pin_code_callback, callback);
}

void sim_send_pin_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*) = NULL;
        GError *error = NULL;

        callback = userdata;

        if(dbus_error != NULL)
                error = dbus_handle_errors(dbus_error);

        (*(callback)) (error);

}

void sim_send_puk_code(const char* puk, const char* pin, void (*callback)(GError*)) {
        if(puk != NULL && pin != NULL && callback != NULL)
            org_freesmartphone_GSM_SIM_unlock_async(simBus, puk, pin, sim_send_puk_code_callback, callback);
}

void sim_send_puk_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*) = NULL;
        GError *error = NULL;

        callback = userdata;

        if(dbus_error != NULL)
                error = dbus_handle_errors(dbus_error);

        (*(callback)) (error);

}

void sim_display_code_UI () {
        sim_get_authentication_state (sim_display_code_UI_callback);
}

void sim_display_code_UI_callback(GError* error, int status) {
    if(error != NULL) {
        /* TODO */
    } else if (status != SIM_READY) {
        get_sim_code_from_user(status);
    }
}

GError* sim_handle_errors(GError *error) {
	const char *error_name = dbus_g_error_get_name(error);
	int simError = 0;

	if(!strcmp(error_name, DBUS_SIM_ERROR_NOT_PRESENT)) {
		simError = SIM_ERROR_NOT_PRESENT;
	} else if(!strcmp(error_name, DBUS_SIM_ERROR_AUTH_FAILED)) {
		simError = SIM_ERROR_AUTH_FAILED;
	} else if(!strcmp(error_name, DBUS_SIM_ERROR_BLOCKED)) {
		simError = SIM_ERROR_BLOCKED;
	} else if(!strcmp(error_name, DBUS_SIM_ERROR_NOT_FOUND)) {
		simError = SIM_ERROR_NOT_FOUND;
	} else if(!strcmp(error_name, DBUS_SIM_ERROR_MEMORY_FULL)) {
		simError = SIM_ERROR_MEMORY_FULL;
	} else if(!strcmp(error_name, DBUS_SIM_ERROR_INVALID_INDEX)) {
		simError = SIM_ERROR_INVALID_INDEX;
	} else {
        	lose_gerror ("Unknown SIM error", error);
	}
	return g_error_new (SIM_ERROR, simError, "TODO");
}
