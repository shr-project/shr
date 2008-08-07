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

void network_register(void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_register_data_t *data = g_malloc (sizeof (network_register_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_register_async(networkBus, network_register_callback, data);
}


typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_register_with_provider_data_t;

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
    g_free(data);
}

void network_register_with_provider(int provider_id, void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_register_with_provider_data_t *data = g_malloc (sizeof (network_register_with_provider_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_register_with_provider_async(networkBus, provider_id, network_register_with_provider_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_unregister_data_t;

void network_unregister_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_unregister_data_t *data = userdata;
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

void network_unregister(void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_unregister_data_t *data = g_malloc (sizeof (network_unregister_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_unregister_async(networkBus, network_unregister_callback, data);
}

typedef struct
{
    void (*callback)(GError *, GHashTable* status, gpointer);
    gpointer userdata;
} network_get_status_data_t;

void network_get_status_callback(DBusGProxy *bus, GHashTable *status, GError *dbus_error, gpointer userdata) {
    network_get_status_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, status, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    g_hash_table_destroy(status);
}

void network_get_status(void (*callback)(GError *, GHashTable* status, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_get_status_data_t *data = g_malloc (sizeof (network_get_status_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_get_status_async(networkBus, network_get_status_callback, data);
}

typedef struct
{
    void (*callback)(GError *, int strength, gpointer);
    gpointer userdata;
} network_get_signal_strength_data_t;

void network_get_signal_strength_callback(DBusGProxy *bus, int strength, GError *dbus_error, gpointer userdata) {
    network_get_signal_strength_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, strength, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);

}

void network_get_signal_strength(void (*callback)(GError *, int strength, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_get_signal_strength_data_t *data = g_malloc (sizeof (network_get_signal_strength_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_get_signal_strength_async(networkBus, network_get_signal_strength_callback, data);
}

typedef struct
{
    void (*callback)(GError *, GPtrArray* providers, gpointer);
    gpointer userdata;
} network_list_providers_data_t;

void network_list_providers_callback(DBusGProxy *bus, GPtrArray* providers, GError *dbus_error, gpointer userdata) {
    network_list_providers_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, providers, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    dbus_free_data(dbus_get_type_int_g_string_g_string_g_string_array(), providers);
}

void network_list_providers(void (*callback)(GError *, GPtrArray* providers, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_list_providers_data_t *data = g_malloc (sizeof (network_list_providers_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_list_providers_async(networkBus, network_list_providers_callback, data);
}

typedef struct
{
    void (*callback)(GError *, char* country_code, gpointer);
    gpointer userdata;
} network_get_country_code_data_t;

void network_get_country_code_callback(DBusGProxy *bus, char * country_code, GError *dbus_error, gpointer userdata) {
    network_get_country_code_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, country_code, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    free(country_code);
}

void network_get_country_code(void (*callback)(GError *, char* country_code, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_get_country_code_data_t *data = g_malloc (sizeof (network_get_country_code_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_get_country_code_async(networkBus, network_get_country_code_callback, data);
}

typedef struct
{
    void (*callback)(GError *, GHashTable* forwards, gpointer);
    gpointer userdata;
} network_get_call_forwarding_data_t;

void network_get_call_forwarding_callback(DBusGProxy *bus, GHashTable* forwards, GError *dbus_error, gpointer userdata) {
    network_get_call_forwarding_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, forwards, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    g_hash_table_destroy(forwards);
}

void network_get_call_forwarding(void (*callback)(GError *, GHashTable* forwards, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_get_call_forwarding_data_t *data = g_malloc (sizeof (network_get_call_forwarding_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_get_call_forwarding_async(networkBus, network_get_call_forwarding_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_enable_call_forwarding_data_t;

void network_enable_call_forwarding_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_enable_call_forwarding_data_t *data = userdata;
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

void network_enable_call_forwarding(int reason, int forward_class, char* number, int timeout, void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_enable_call_forwarding_data_t *data = g_malloc (sizeof (network_enable_call_forwarding_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_enable_call_forwarding_async(networkBus, network_get_reason_for_dbus(reason), network_get_forward_class_for_dbus(forward_class), number, timeout, network_enable_call_forwarding_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_disable_call_forwarding_data_t;

void network_disable_call_forwarding_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_disable_call_forwarding_data_t *data = userdata;
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

void network_disable_call_forwarding(int reason, int forward_class, void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_disable_call_forwarding_data_t *data = g_malloc (sizeof (network_disable_call_forwarding_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_disable_call_forwarding_async(networkBus, network_get_reason_for_dbus(reason), network_get_forward_class_for_dbus(forward_class), network_disable_call_forwarding_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} network_set_calling_identification_data_t;

void network_set_calling_identification_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata) {
    network_set_calling_identification_data_t *data = userdata;
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

void network_set_calling_identification(int visible, void (*callback)(GError *, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_set_calling_identification_data_t *data = g_malloc (sizeof (network_set_calling_identification_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_set_calling_identification_async(networkBus, network_get_visible_for_dbus(visible), network_set_calling_identification_callback, data);
}

typedef struct
{
    void (*callback)(GError *, int visible, gpointer);
    gpointer userdata;
} network_get_calling_identification_data_t;

void network_get_calling_identification_callback(DBusGProxy *bus, char * visible, GError *dbus_error, gpointer userdata) {
    network_get_calling_identification_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, network_get_visible_from_dbus(visible), data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    free(visible);
}

void network_get_calling_identification(void (*callback)(GError *, int visible, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_network();

    network_get_calling_identification_data_t *data = g_malloc (sizeof (network_get_calling_identification_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_Network_get_calling_identification_async(networkBus, network_get_calling_identification_callback, data);
}

int network_get_registration_status_from_dbus(GHashTable *properties) {
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

const char* network_get_location_area_from_dbus(GHashTable *properties) {
    GValue* lac;

    if(properties != NULL) {
        lac = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_LOCATION_AREA);
        return lac == NULL ? NULL : g_value_get_string(lac);
    }

    return NULL;
}

const char* network_get_provider_from_dbus(GHashTable *properties) {
    GValue* provider;

    if(properties != NULL) {
        provider = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_PROVIDER);
        return provider == NULL ?  NULL : g_value_get_string(provider);
    }
    return NULL;
}

const char* network_get_cell_id_from_dbus(GHashTable *properties) {
    GValue* cid;

    if(properties != NULL) {
        cid = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_CELL_ID);
        return cid == NULL ? NULL : g_value_get_string(cid);
    }

    return NULL;
}

int network_get_signal_strength_from_dbus(GHashTable *properties) {
    GValue* strength = NULL;

    if(properties == NULL)
        return 0;

    strength = g_hash_table_lookup(properties, DBUS_NETWORK_PROPERTY_STRENGTH);
    return strength == NULL ? 0 : g_value_get_int(strength);
}

char* network_get_reason_for_dbus(int reason) {
    switch(reason) {
        case NETWORK_FORWARD_REASON_UNCONDITIONAL:
            return DBUS_NETWORK_FORWARD_REASON_UNCONDITIONAL;
        case NETWORK_FORWARD_REASON_MOBILE_BUSY:
            return DBUS_NETWORK_FORWARD_REASON_MOBILE_BUSY;
        case NETWORK_FORWARD_REASON_NO_REPLY:
            return DBUS_NETWORK_FORWARD_REASON_NO_REPLY;
        case NETWORK_FORWARD_REASON_NOT_REACHABLE:
            return DBUS_NETWORK_FORWARD_REASON_NOT_REACHABLE;
        case NETWORK_FORWARD_REASON_ALL:
            return DBUS_NETWORK_FORWARD_REASON_ALL;
        case NETWORK_FORWARD_REASON_ALL_CONDITIONAL:
            return DBUS_NETWORK_FORWARD_REASON_ALL_CONDITIONAL;

    }
    return NULL;
}

int network_get_reason_from_dbus(char *reason) {
    if(!strcmp(DBUS_NETWORK_FORWARD_REASON_UNCONDITIONAL, reason)) 
        return NETWORK_FORWARD_REASON_UNCONDITIONAL;
    else if(!strcmp(DBUS_NETWORK_FORWARD_REASON_MOBILE_BUSY, reason))
        return NETWORK_FORWARD_REASON_MOBILE_BUSY;
    else if(!strcmp( DBUS_NETWORK_FORWARD_REASON_NO_REPLY, reason)) 
        return  NETWORK_FORWARD_REASON_NO_REPLY;
    else if(!strcmp(DBUS_NETWORK_FORWARD_REASON_NOT_REACHABLE, reason))
        return NETWORK_FORWARD_REASON_NOT_REACHABLE;
    else if(!strcmp(DBUS_NETWORK_FORWARD_REASON_ALL, reason))
        return  NETWORK_FORWARD_REASON_ALL;
    else 
        return NETWORK_FORWARD_REASON_ALL_CONDITIONAL;

    return -1;
}

char * network_get_forward_class_for_dbus(int forward_class) {
    switch(forward_class) {
        case NETWORK_FORWARD_CLASS_VOICE:
            return DBUS_NETWORK_FORWARD_CLASS_VOICE;
        case NETWORK_FORWARD_CLASS_DATA:
            return DBUS_NETWORK_FORWARD_CLASS_DATA;
        case NETWORK_FORWARD_CLASS_VOICE_DATA:
            return DBUS_NETWORK_FORWARD_CLASS_VOICE_DATA;
        case NETWORK_FORWARD_CLASS_FAX:
            return DBUS_NETWORK_FORWARD_CLASS_FAX;
        case NETWORK_FORWARD_CLASS_VOICE_DATA_FAX:
            return DBUS_NETWORK_FORWARD_CLASS_VOICE_DATA_FAX;
        case NETWORK_FORWARD_CLASS_SMS:
            return DBUS_NETWORK_FORWARD_CLASS_SMS;
        case NETWORK_FORWARD_CLASS_DCS:
            return DBUS_NETWORK_FORWARD_CLASS_DCS;
        case NETWORK_FORWARD_CLASS_DCA:
            return DBUS_NETWORK_FORWARD_CLASS_DCA;
        case NETWORK_FORWARD_CLASS_DPA:
            return DBUS_NETWORK_FORWARD_CLASS_DPA;
        case NETWORK_FORWARD_CLASS_PAD:
            return DBUS_NETWORK_FORWARD_CLASS_PAD;
    }

    return NULL;
}

int network_get_forward_class_from_dbus(char *forward_class) {
    if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_VOICE, forward_class))
        return NETWORK_FORWARD_CLASS_VOICE;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_DATA, forward_class))
        return NETWORK_FORWARD_CLASS_DATA;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_VOICE_DATA, forward_class))
        return NETWORK_FORWARD_CLASS_VOICE_DATA;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_FAX, forward_class))
        return NETWORK_FORWARD_CLASS_FAX;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_VOICE_DATA_FAX, forward_class))
        return NETWORK_FORWARD_CLASS_VOICE_DATA_FAX;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_SMS, forward_class))
        return NETWORK_FORWARD_CLASS_SMS;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_DCS, forward_class))
        return NETWORK_FORWARD_CLASS_DCS;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_DCA, forward_class))
        return NETWORK_FORWARD_CLASS_DCA;
    else if(!strcmp(DBUS_NETWORK_FORWARD_CLASS_DPA, forward_class))
        return NETWORK_FORWARD_CLASS_DPA;
    else
        return NETWORK_FORWARD_CLASS_PAD;

    return -1;
}

char * network_get_visible_for_dbus(int visible) {
    switch(visible) {
        case NETWORK_IDENTIFICATION_STATUS_ON:
            return DBUS_NETWORK_IDENTIFICATION_STATUS_ON;
        case NETWORK_IDENTIFICATION_STATUS_OFF:
            return DBUS_NETWORK_IDENTIFICATION_STATUS_OFF;
        case NETWORK_IDENTIFICATION_STATUS_NETWORK:
            return DBUS_NETWORK_IDENTIFICATION_STATUS_NETWORK;
    }
    return NULL;
}

int network_get_visible_from_dbus(char *visible) {
    if(!strcmp(DBUS_NETWORK_IDENTIFICATION_STATUS_ON, visible))
        return NETWORK_IDENTIFICATION_STATUS_ON;
    else if(!strcmp(DBUS_NETWORK_IDENTIFICATION_STATUS_OFF, visible))
        return NETWORK_IDENTIFICATION_STATUS_OFF;
    else
        return NETWORK_IDENTIFICATION_STATUS_NETWORK;
    return -1;
}
