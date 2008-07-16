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

#ifndef _SIM_H
#define _SIM_H

#define DBUS_SIM_READY "READY"
#define DBUS_SIM_PIN_REQUIRED "SIM PIN"
#define DBUS_SIM_PUK_REQUIRED "SIM PUK"
#define DBUS_SIM_PIN2_REQUIRED "SIM PIN2"
#define DBUS_SIM_PUK2_REQUIRED "SIM PUK2"

typedef enum {
	SIM_READY,
	SIM_PIN_REQUIRED,
	SIM_PUK_REQUIRED,
	SIM_PIN2_REQUIRED,
	SIM_PUK2_REQUIRED
} SimStatus;

int get_authentication_state();

#endif
