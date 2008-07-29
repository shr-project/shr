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

void call_status_handler (DBusGProxy *proxy, const int id, const char *status, GHashTable ** properties, gpointer user_data)
{
        int st;
        void (*callback)(const int, const int, GHashTable **) = NULL;
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

void call_initiate(const char *number, const char* call_type, void (*callback)(GError *, int)) {
    org_freesmartphone_GSM_Call_initiate_async(callBus,number, call_type, call_initiate_callback, callback);
}

void call_initiate_callback(DBusGProxy* proxy, int id_call, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*, int) = NULL;
        GError *error = NULL;

        callback = userdata;

        if(callback != NULL) {
            if(dbus_error != NULL)
                    error = dbus_handle_errors(dbus_error);

            (*(callback)) (error, id_call);
            if(error != NULL) g_error_free(error);
        } 
        
        if(dbus_error != NULL) g_error_free(dbus_error);
}

void call_release(const char *message, const int id_call, void (*callback)(GError *)) {
        org_freesmartphone_GSM_Call_release_async(callBus, message, id_call, call_release_callback, callback);
}

void call_release_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*) = NULL;
        GError *error = NULL;

        callback = userdata;
        
        if(callback != NULL) {
            if(dbus_error != NULL)
                    error = dbus_handle_errors(dbus_error);

            (*(callback)) (error);
            if(error != NULL) g_error_free(error);
        } 
        
        if(dbus_error != NULL) g_error_free(dbus_error);
}


void call_activate(const int id_call, void (*callback)(GError *)) {
    org_freesmartphone_GSM_Call_activate_async(callBus, id_call, call_activate_callback, callback);
}

void call_activate_callback(DBusGProxy* proxy, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*) = NULL;
        GError *error = NULL;

        callback = userdata;
        
        if(callback != NULL) {
            if(dbus_error != NULL)
                    error = dbus_handle_errors(dbus_error);

            (*(callback)) (error);
            if(error != NULL) g_error_free(error);
        } 
        
        if(dbus_error != NULL) g_error_free(dbus_error);
}
