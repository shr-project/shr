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
#include "frameworkd-glib-device.h"
#include "frameworkd-glib-dbus.h"
#include "dbus/device.h"

DBusGProxy *deviceBus = NULL;

GError* device_handle_errors(GError *dbus_error) {
        const char *error_name = dbus_g_error_get_name(dbus_error);
        int deviceError = 0;

        if(!strcmp(error_name, DBUS_DEVICE_ERROR_TIMEOUT)) {
                deviceError = DEVICE_ERROR_TIMEOUT;
        } else if(!strcmp(error_name, DBUS_DEVICE_ERROR_NOT_PRESENT)) {
                deviceError = DEVICE_ERROR_NOT_PRESENT;
        } else if(!strcmp(error_name, DBUS_DEVICE_ERROR_FAILED)) {
                deviceError = DEVICE_ERROR_FAILED;
        } else {
                lose_gerror ("Unknown device error", dbus_error);
        }
        return g_error_new (DEVICE_ERROR, deviceError, "TODO");
}

        void device_set_antenna_power(gboolean power, void (*callback)(GError *)) {
                if(callback != NULL)
                        org_freesmartphone_GSM_Device_set_antenna_power_async (deviceBus, power, device_set_antenna_power_callback, callback);
        }

void device_set_antenna_power_callback(DBusGProxy* bus, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*) = NULL;
        GError *error = NULL;

        callback = userdata;

        if(callback != NULL) {

                if(dbus_error != NULL)
                        error = dbus_handle_errors(dbus_error);


                (*(callback)) (error);
        } else {
                g_error_free(dbus_error);
        }
}

void device_get_antenna_power(void (*callback)(GError *, gboolean)) {
    if(callback != NULL)
        org_freesmartphone_GSM_Device_get_antenna_power_async(deviceBus, device_get_antenna_power_callback, callback);
}

void device_get_antenna_power_callback(DBusGProxy* bus, gboolean power, GError *dbus_error, gpointer userdata) {
        void (*callback)(GError*, gboolean) = NULL;
        GError *error = NULL;

        callback = userdata;
        if(callback != NULL) {
                if(dbus_error != NULL)
                        error = dbus_handle_errors(dbus_error);

                (*(callback)) (error, power);
        } else {
            g_error_free(dbus_error);
        }
}

