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

static DBusGProxy *networkBus = NULL;

static void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data);
static void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data);

#endif
