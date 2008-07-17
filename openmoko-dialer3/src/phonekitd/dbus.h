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

#ifndef _DBUS_H
#define _DBUS_H

#define NETWORKD_BUS		"org.freesmartphone.onetworkd"
#define GSMD_BUS		"org.freesmartphone.ogsmd"
#define BUS_PATH		"/org/freesmartphone/GSM/Device"
#define CALL_INTERFACE		"org.freesmartphone.GSM.Call"
#define SIM_INTERFACE		"org.freesmartphone.GSM.SIM"
#define NETWORK_INTERFACE 	"org.freesmartphone.GSM.Network"
#define	DEVICE_INTERFACE	"org.freesmartphone.GSM.Device"

#define DBUS_TYPE_G_STRING_VARIANT_HASHTABLE (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE))

static DBusGProxy *networkBus = NULL, *simBus = NULL, *callBus = NULL, *deviceBus = NULL;
static void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data);
static void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data);
static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data);
static void call_status_handler (DBusGProxy *proxy, const int id, const char *status, const GHashTable ** properties, gpointer user_data);
static void lose (const char *str, ...);
static void lose_gerror (const char *prefix, GError *error);

#endif
