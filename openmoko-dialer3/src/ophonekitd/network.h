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


typedef enum {
	NETWORK_ERROR_NOT_PRESENT = -1,
	NETWORK_ERROR_UNAUTHORIZED = -2,
	NETWORK_ERROR_NOT_FOUND = -3,
	NETWORK_ERROR_NOT_SUPPORTED = -4,
} NetworkErrors;



extern DBusGProxy *networkBus;

void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data);
void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data);
gboolean network_register(GError** error);
gboolean network_register_with_provider(GError** error, int provider_id);
#endif
