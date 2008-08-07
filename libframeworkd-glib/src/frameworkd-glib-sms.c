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
#include "frameworkd-glib-sms.h"
#include "frameworkd-glib-dbus.h"
#include "dbus/sms.h"

DBusGProxy *smsBus = NULL;

void sms_message_sent_handler (DBusGProxy *proxy, const int id, const gboolean success, const char* reason, gpointer userdata) {
    void (*callback)(const int, const gboolean, const char*) = NULL;

    callback = userdata;

    if(callback != NULL)
        (*callback)(id, success, reason);

}

void sms_incoming_message_handler (DBusGProxy *proxy, const int id, gpointer userdata) {
    void (*callback)(const int) = NULL;

    callback = userdata;

    if(callback != NULL)
        (*callback)(id);

}

typedef struct
{
    void (*callback)(GError *, int, gpointer);
    gpointer userdata;
} sms_send_message_data_t;

void sms_send_message_callback(DBusGProxy* bus, int transaction_index, GError *dbus_error, gpointer userdata) {
    sms_send_message_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, transaction_index, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
}

void sms_send_message(const char*number, const char* content, gboolean report, void (*callback)(GError*, int transaction_index, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sms();

    sms_send_message_data_t *data = g_malloc (sizeof (sms_send_message_data_t));
    data->callback = callback;
    data->userdata = userdata;

    if(number != NULL && content != NULL)
        org_freesmartphone_GSM_SMS_send_message_async(smsBus, number, content, report, sms_send_message_callback, data);
}

typedef struct
{
    void (*callback)(GError *, char *mode, gpointer);
    gpointer userdata;
} sms_get_service_bearer_data_t;

void sms_get_service_bearer_callback(DBusGProxy* bus, char* mode, GError *dbus_error, gpointer userdata) {
    sms_get_service_bearer_data_t *data = userdata;
    GError *error = NULL;

    if(data->callback != NULL) {
        if(dbus_error != NULL)
            error = dbus_handle_errors(dbus_error);

        data->callback (error, mode, data->userdata);
        if(error != NULL) g_error_free(error);
    }

    if(dbus_error != NULL) g_error_free(dbus_error);
    g_free(data);
    free(mode);
}

void sms_get_service_bearer(void (*callback)(GError*, char* mode, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sms();

    sms_get_service_bearer_data_t *data = g_malloc (sizeof (sms_get_service_bearer_data_t));
    data->callback = callback;
    data->userdata = userdata;

    org_freesmartphone_GSM_SMS_get_service_bearer_async(smsBus, sms_get_service_bearer_callback, data);
}

typedef struct
{
    void (*callback)(GError *, gpointer);
    gpointer userdata;
} sms_set_service_bearer_data_t;

void sms_set_service_bearer_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
    sms_set_service_bearer_data_t *data = userdata;
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

void sms_set_service_bearer(const char* mode, void (*callback)(GError*, gpointer), gpointer userdata) {
    dbus_connect_to_gsm_sms();

    sms_set_service_bearer_data_t *data = g_malloc (sizeof (sms_set_service_bearer_data_t));
    data->callback = callback;
    data->userdata = userdata;

    if(mode != NULL)
        org_freesmartphone_GSM_SMS_set_service_bearer_async(smsBus, mode, sms_set_service_bearer_callback, data);
}
