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

#ifndef _NETWORK_H
#define _NETWORK_H

#define NETWORK_ERROR g_quark_from_static_string(NETWORK_INTERFACE)
#define DBUS_NETWORK_ERROR_NOT_PRESENT "org.freesmartphone.GSM.Network.NotPresent"
#define DBUS_NETWORK_ERROR_UNAUTHORIZED "org.freesmartphone.GSM.Network.Unauthorized"
#define DBUS_NETWORK_ERROR_NOT_SUPPORTED "org.freesmartphone.GSM.Network.NotSupported"
#define DBUS_NETWORK_ERROR_NOT_FOUND "org.freesmartphone.GSM.Network.NotFound"

#define DBUS_NETWORK_PROPERTY_REGISTRATION "registration"
#define DBUS_NETWORK_PROPERTY_PROVIDER "provider"
#define DBUS_NETWORK_PROPERTY_STRENGTH "strength"
#define DBUS_NETWORK_PROPERTY_LOCATION_AREA "lac"
#define DBUS_NETWORK_PROPERTY_CELL_ID "cid"

#define DBUS_NETWORK_PROPERTY_REGISTRATION_UNREGISTERED "unregistered"
#define DBUS_NETWORK_PROPERTY_REGISTRATION_HOME "home"
#define DBUS_NETWORK_PROPERTY_REGISTRATION_BUSY "busy"
#define DBUS_NETWORK_PROPERTY_REGISTRATION_DENIED "denied"
#define DBUS_NETWORK_PROPERTY_REGISTRATION_UNKNOWN "unknown"
#define DBUS_NETWORK_PROPERTY_REGISTRATION_ROAMING "roaming"

typedef enum {
	NETWORK_ERROR_NOT_PRESENT = -1,
	NETWORK_ERROR_UNAUTHORIZED = -2,
	NETWORK_ERROR_NOT_FOUND = -3,
	NETWORK_ERROR_NOT_SUPPORTED = -4,
} NetworkErrors;

typedef enum {
    NETWORK_PROPERTY_REGISTRATION_UNREGISTERED,
    NETWORK_PROPERTY_REGISTRATION_HOME,
    NETWORK_PROPERTY_REGISTRATION_BUSY,
    NETWORK_PROPERTY_REGISTRATION_DENIED,
    NETWORK_PROPERTY_REGISTRATION_UNKNOWN,
    NETWORK_PROPERTY_REGISTRATION_ROAMING    
} NetworkRegistrationProperties;

extern DBusGProxy *networkBus;

void network_status_handler (DBusGProxy *proxy, const  GHashTable * status, gpointer user_data);
void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data);

void network_register(void (*callback)(GError *));
void network_register_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata);

void network_register_with_provider(int provider_id, void (*callback)(GError *));
void network_register_with_provider_callback(DBusGProxy *bus, GError *dbus_error, gpointer userdata);

int network_get_registration_status(GHashTable *properties);
char* network_get_location_area(GHashTable *properties);
char* network_get_cell_id(GHashTable *properties);
int network_get_signal_strength(GHashTable *properties);
#endif
