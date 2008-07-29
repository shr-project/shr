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

void sms_message_sent_handler (DBusGProxy *proxy, const int id, const gboolean success, const char* reason, gpointer user_data) {
        printf ("Received message sent status");
}

void sms_incoming_message_handler (DBusGProxy *proxy, const int id, gpointer user_data) {
        printf ("Received incoming message status");
}

        void sms_send_message(const char* number, const char* content, const gboolean report, void (*callback)(GError*, int)) {
                if(number != NULL && content != NULL && callback != NULL)
                        org_freesmartphone_GSM_SMS_send_message_async(smsBus, number, content, report, sms_send_message_callback, callback);
        }

void sms_send_message_callback(DBusGProxy* bus, gint transaction_index, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*, int) = NULL;
        GError *error = NULL;

        callback = userdata;

        if(dbus_error != NULL)
                error = dbus_handle_errors(dbus_error);

        (*(callback)) (error, transaction_index);
}
