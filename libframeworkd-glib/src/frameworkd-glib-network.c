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
#include "frameworkd-glib-network.h"
#include "frameworkd-glib-dbus.h"
#include "dbus/network.h"

DBusGProxy *networkBus = NULL;

void network_status_handler (DBusGProxy *proxy, const  GHashTable * status, gpointer user_data)
{
    void (*callback)(const GHashTable *) = NULL;

    callback = user_data;

    if(callback != NULL)
        (*callback)(status);
}

void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data)
{
    void (*callback)(const int) = NULL;

    callback = user_data;

    if(callback != NULL)
        (*callback)(signal_strength);
}

GError* network_handle_errors(GError *dbus_error) {
    const char *error_name = dbus_g_error_get_name(dbus_error);
    int networkError = 0;

    lose_gerror ("Unknown network error", dbus_error);

    return g_error_new (NETWORK_ERROR, networkError, "TODO: %s", error_name);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_register_data_t;


void network_register(void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_register_data_t *data = g_malloc (sizeof (network_register_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_register_async(networkBus, network_register_callback, data);
}

void network_register_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_register_data_t *data = userdata;
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
} network_register_with_provider_data_t;

void network_register_with_provider(int provider_id, void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_register_with_provider_data_t *data = g_malloc (sizeof (network_register_with_provider_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_register_with_provider_async(networkBus, provider_id, network_register_callback, data);
}

void network_register_with_provider_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_register_with_provider_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

}

int network_get_registration_status(GHashTable *properties) {
    GValue* reg = NULL;
    const char *registration = NULL;

    if(properties == NULL || ((reg = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_REGISTRATION)) == NULL))
        return NETWORK_PROPERTY_REGISTRATION_UNKNOWN;

    registration = g_value_get_string(reg);

    if(!strcmp(registration, DBUS_NETWORK_PROPERTY_REGISTRATION_UNREGISTERED)) {
        return NETWORK_PROPERTY_REGISTRATION_UNREGISTERED;
    } else if(!strcmp(registration, DBUS_NETWORK_PROPERTY_REGISTRATION_HOME)) {
        return NETWORK_PROPERTY_REGISTRATION_HOME;
    } else if(!strcmp(registration, DBUS_NETWORK_PROPERTY_REGISTRATION_BUSY)) {
        return NETWORK_PROPERTY_REGISTRATION_BUSY;
    } else if(!strcmp(registration, DBUS_NETWORK_PROPERTY_REGISTRATION_DENIED)) {
        return NETWORK_PROPERTY_REGISTRATION_DENIED;
    } else if(!strcmp(registration, DBUS_NETWORK_PROPERTY_REGISTRATION_ROAMING)) {
        return NETWORK_PROPERTY_REGISTRATION_ROAMING;
    } 

    return NETWORK_PROPERTY_REGISTRATION_UNKNOWN;
}

const char* network_get_location_area(GHashTable *properties) {
    GValue* lac;

    if(properties != NULL) {
        lac = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_LOCATION_AREA);
        return lac == NULL ? NULL : g_value_get_string(lac);
    }

    return NULL;
}

const char* network_get_provider(GHashTable *properties) {
    GValue* provider;

    if(properties != NULL) {
        provider = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_PROVIDER);
        return provider == NULL ?  NULL : g_value_get_string(provider);
    }
    return NULL;
}

const char* network_get_cell_id(GHashTable *properties) {
    GValue* cid;

    if(properties != NULL) {
        cid = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_CELL_ID);
        return cid == NULL ? NULL : g_value_get_string(cid);
    }

    return NULL;
}

int network_get_signal_strength(GHashTable *properties) {
    GValue* strength = NULL;

    if(properties == NULL)
        return 0;

    strength = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_STRENGTH);
    return strength == NULL ? 0 : g_value_get_int(strength);
}

