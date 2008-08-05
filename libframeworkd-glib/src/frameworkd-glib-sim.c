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

#include "frameworkd-glib-dbus.h"
#include "dbus/sim.h"
#include "frameworkd-glib-sim.h"
#include "frameworkd-glib-phonegui.h"

DBusGProxy *simBus = NULL;

void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
    int st;
    void (*callback)(const int) = NULL;

    st = sim_handle_authentication_state(status);
    callback = user_data;

    if(callback != NULL)
        (*callback)(st);

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

typedef struct
{
    void (*callback)(GError *, int, gpointer);
    gpointer userdata;
} sim_get_authentication_state_data_t;

void sim_get_authentication_state(void (*callback)(GError*, int, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_get_authentication_state_data_t *data = g_malloc (sizeof (sim_get_authentication_state_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_SIM_get_auth_status_async(simBus, sim_get_authentication_state_callback, userdata);
}

void sim_get_authentication_state_callback(DBusGProxy *bus, char* status, GError* dbus_error, gpointer userdata) {
    sim_get_authentication_state_data_t *data = userdata;
    GError *error = NULL;
    int st = 0;


    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);
        else
            st = sim_handle_authentication_state(status);

        data->callback (error, st, data->userdata);
        if(error != NULL) g_error_free(error);
    } 

    free(status);
    g_free(data);
    if(dbus_error != NULL) g_error_free(dbus_error);

}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} sim_send_pin_code_data_t;

void sim_send_pin_code(const char* pin, void (*callback)(GError*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_send_pin_code_data_t *data = g_malloc (sizeof (sim_send_pin_code_data_t));
    data->callback = callback;
    data->userdata = userdata;

    if(pin != NULL)
        org_freesmartphone_GSM_SIM_send_auth_code_async(simBus, pin, sim_send_pin_code_callback, data);
}

void sim_send_pin_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
    sim_send_pin_code_data_t *data = userdata;
    GError *error = NULL;


    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    } 

    if(dbus_error != NULL) g_error_free(dbus_error);        
    g_free(data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} sim_send_puk_code_data_t;

void sim_send_puk_code(const char* puk, const char* pin, void (*callback)(GError*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_send_puk_code_data_t *data = g_malloc (sizeof (sim_send_puk_code_data_t));
    data->callback = callback;
    data->userdata = userdata;


    if(puk != NULL && pin != NULL)
        org_freesmartphone_GSM_SIM_unlock_async(simBus, puk, pin, sim_send_puk_code_callback, data);
}

void sim_send_puk_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
    sim_send_puk_code_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} sim_change_pin_code_data_t;

void sim_change_pin_code(const char* old, const char* new, void (*callback)(GError*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_change_pin_code_data_t *data = g_malloc (sizeof (sim_change_pin_code_data_t));
    data->callback = callback;
    data->userdata = userdata;

    if(old != NULL && new != NULL)
        org_freesmartphone_GSM_SIM_change_auth_code_async(simBus, old, new, sim_change_pin_code_callback, data);
}

void sim_change_pin_code_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
    sim_change_pin_code_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
}


typedef struct
{
    void (*callback)(GError *, char*, char*, gpointer);
    gpointer userdata;
} sim_retrieve_phonebook_entry_data_t;

void sim_retrieve_phonebook_entry(const int index, void (*callback)(GError*, char*, char*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_retrieve_phonebook_entry_data_t *data = g_malloc (sizeof (sim_retrieve_phonebook_entry_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_SIM_retrieve_entry_async(simBus, index, sim_retrieve_phonebook_entry_callback, data);
}

void sim_retrieve_phonebook_entry_callback(DBusGProxy* bus, char*name, char* number, GError *dbus_error, gpointer userdata) {
    sim_retrieve_phonebook_entry_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, name, number, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    free(name);
    free(number);
    g_free(data);
}


typedef struct
{
    void (*callback)(GError *, char*, char*, gpointer);
    gpointer userdata;
} sim_retrieve_message_data_t;

void sim_retrieve_message(const int index, void (*callback)(GError*, char*, char*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sim();

    sim_retrieve_message_data_t *data = g_malloc (sizeof (sim_retrieve_message_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_SIM_retrieve_message_async(simBus, index, sim_retrieve_message_callback, data);
}

void sim_retrieve_message_callback(DBusGProxy* bus, char*number, char* content, GError *dbus_error, gpointer userdata) {
    sim_retrieve_message_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, number, content, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    free(number);
    free(content);
    g_free(data);
}


void sim_display_code_UI () {
    sim_get_authentication_state (sim_display_code_UI_callback, NULL);
}

void sim_display_code_UI_callback(GError* error, int status, gpointer userdata) {
    if(error != NULL) {
        /* TODO */
    } else if (status != SIM_READY) {
        phonegui_display_pin_UI(status);
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
