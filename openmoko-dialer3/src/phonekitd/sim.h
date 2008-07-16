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

#define DBUS_SIM_ERROR_NOT_PRESENT "org.freesmartphone.GSM.SIM.NotPresent"
#define DBUS_SIM_ERROR_AUTH_FAILED "org.freesmartphone.GSM.SIM.AuthFailed"
#define DBUS_SIM_ERROR_BLOCKED "org.freesmartphone.GSM.SIM.Blocked"
#define DBUS_SIM_ERROR_NOT_FOUND "org.freesmartphone.GSM.SIM.NotFound"
#define DBUS_SIM_ERROR_MEMORY_FULL "org.freesmartphone.GSM.SIM.MemoryFull"
#define DBUS_SIM_ERROR_INVALID_INDEX "org.freesmartphone.GSM.SIM.InvalidIndex"

typedef enum {
	SIM_READY,
	SIM_PIN_REQUIRED,
	SIM_PUK_REQUIRED,
	SIM_PIN2_REQUIRED,
	SIM_PUK2_REQUIRED
} SimStatus;

typedef enum {
	SIM_ERROR_NOT_PRESENT = -1,
	SIM_ERROR_AUTH_FAILED = -2,
	SIM_ERROR_BLOCKED = -3,
	SIM_ERROR_NOT_FOUND = -4,
	SIM_ERROR_MEMORY_FULL = -5,
	SIM_ERROR_INVALID_INDEX = -6
} SimErrors;


int get_authentication_state();
int send_pin_code(int codeToSet, const char* pin);
int send_puk_code(int codeToSet, const char* puk, const char* pin);
int sim_handle_errors(GError *error);
int handleSimAuth(int result, int codeToSet);
void displayPinUI(int codeToSet);
void displayPukUI(int codeToSet);

#endif
