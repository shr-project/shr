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
#include "frameworkd-glib-call.h"
#include "frameworkd-glib-dbus.h"
#include "dbus/call.h"

DBusGProxy *callBus = NULL;

typedef struct
{
    void (*callback)(GError *, int, gpointer);
    gpointer userdata;
} call_initiate_data_t;

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_release_data_t;

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_activate_data_t;

void call_status_handler (DBusGProxy *proxy, const int id, const char *status, GHashTable * properties, gpointer user_data)
{
    int st;
    void (*callback)(const int, const int, GHashTable*) = NULL;
    if(!strcmp(status, DBUS_CALL_STATUS_INCOMING)) {
        st = CALL_STATUS_INCOMING;
        // Display incoming UI
    } else if(!strcmp(status, DBUS_CALL_STATUS_OUTGOING)) {
        // Display outgoing UI
        st = CALL_STATUS_OUTGOING;
    } else if(!strcmp(status, DBUS_CALL_STATUS_ACTIVE)) {
        // TODO
        st = CALL_STATUS_ACTIVE;
    } else if(!strcmp(status, DBUS_CALL_STATUS_RELEASED)) {
        // Close UI
        st = CALL_STATUS_RELEASED;
    } else { // HELD
        st = CALL_STATUS_HELD;
    }

    callback = user_data;

    if(callback != NULL)
        (*callback)(id,st, properties);
}

GError* call_handle_errors(GError *dbus_error) {
    const char *error_name = dbus_g_error_get_name(dbus_error);
    int callError = 0;

    if(!strcmp(error_name, DBUS_CALL_ERROR_NO_CARRIER)) {
        callError = CALL_ERROR_NO_CARRIER;
    } else if(!strcmp(error_name, DBUS_CALL_ERROR_NOT_FOUND)) {
        callError = CALL_ERROR_NOT_FOUND;
    } else {
        lose_gerror ("Unknown call error", dbus_error);
    }

    return g_error_new (CALL_ERROR, callError, "TODO %s", error_name);
}

static void call_initiate_callback(DBusGProxy* proxy, int id_call, GError *dbus_error, gpointer userdata) {
    call_initiate_data_t *data = userdata;
    GError *error = NULL;
    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, id_call, data->userdata);
        if(error != NULL) g_error_free(error);
    }
    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (userdata);
}

void call_initiate(const char *number, const char* call_type, void (*callback)(GError *, int id_call, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_initiate_data_t *data = g_malloc (sizeof (call_initiate_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_initiate_async(callBus,number, call_type, call_initiate_callback, data);
}

static void call_release_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_release_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }
    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_release(const char *message, const int id_call, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_release_data_t *data = g_malloc (sizeof (call_release_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_release_async(callBus, message, id_call, call_release_callback, data);
}

static void call_activate_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_activate_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_activate(const int id_call, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_activate_data_t *data = g_malloc (sizeof (call_activate_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_activate_async(callBus, id_call, call_activate_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_release_held_data_t;

static void call_release_held_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_release_held_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_release_held(const char *message, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_release_held_data_t *data = g_malloc (sizeof (call_release_held_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_release_held_async(callBus, message, call_release_held_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_release_all_data_t;

static void call_release_all_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_release_all_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_release_all(const char *message, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_release_all_data_t *data = g_malloc (sizeof (call_release_all_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_release_all_async(callBus, message, call_release_all_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_activate_conference_data_t;

static void call_activate_conference_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_activate_conference_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_activate_conference(const int id_call, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_activate_conference_data_t *data = g_malloc (sizeof (call_activate_conference_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_activate_conference_async(callBus, id_call, call_activate_conference_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_hold_active_data_t;

static void call_hold_active_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_hold_active_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_hold_active(void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_hold_active_data_t *data = g_malloc (sizeof (call_hold_active_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_hold_active_async(callBus, call_hold_active_callback, data);
}


typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_join_data_t;

static void call_join_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_join_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_join(void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_join_data_t *data = g_malloc (sizeof (call_join_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_join_async(callBus, call_join_callback, data);
}


typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_transfer_data_t;

static void call_transfer_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_transfer_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_transfer(const char* number, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_transfer_data_t *data = g_malloc (sizeof (call_transfer_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_transfer_async(callBus, number, call_transfer_callback, data);
}


typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} call_send_dtmf_data_t;

static void call_send_dtmf_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
    call_send_dtmf_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);

    g_free (data);
}

void call_send_dtmf(const char* tones, void (*callback)(GError *, gpointer userdata), gpointer userdata) {
    dbus_connect_to_gsm_call();
    call_send_dtmf_data_t *data = g_malloc (sizeof (call_send_dtmf_data_t));
    data->callback = callback;
    data->userdata = userdata;
    org_freesmartphone_GSM_Call_send_dtmf_async(callBus, tones, call_send_dtmf_callback, data);
}

