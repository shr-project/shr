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

#ifndef _DEVICE_H
#define _DEVICE_H

#define DBUS_DEVICE_ERROR_NOT_PRESENT "org.freesmartphone.GSM.Device.NotPresent"
#define DBUS_DEVICE_ERROR_TIMEOUT "org.freesmartphone.GSM.Device.Timeout"
#define DBUS_DEVICE_ERROR_FAILED "org.freesmartphone.GSM.Device.Failed"

#define DEVICE_ERROR g_quark_from_static_string(DEVICE_INTERFACE)

typedef enum {
	DEVICE_ERROR_NOT_PRESENT = -1,
	DEVICE_ERROR_TIMEOUT = -2,
	DEVICE_ERROR_FAILED = -3
} DeviceErrors;

GError* device_handle_errors(GError *dbus_error);
gboolean device_set_antenna_power(GError** error, gboolean power);

static DBusGProxy *deviceBus = NULL;

#endif
