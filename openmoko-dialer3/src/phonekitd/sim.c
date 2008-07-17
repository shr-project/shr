/*
 *  Copyright (C) 2008 
 *	Authors (alphabetical) : 
 *		Marc-Olivier Barre <marco@marcochapeau.org>
 *		Julien Cassignol <ainulindale@gmail.com>
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
#include "sim.h"
#include "dbus.h"
#include "dbus/sim.h"

static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
	if(strcmp(status,DBUS_SIM_READY)) {
		// TODO Set Antenna Power 
	}
	else if(strcmp(status,DBUS_SIM_PIN_REQUIRED)) {
		sim_display_pin_UI(SIM_PIN_REQUIRED);
	}
	else if(strcmp(status,DBUS_SIM_PUK_REQUIRED)) {
		sim_display_puk_UI(SIM_PUK_REQUIRED);
	}
	else if(strcmp(status,DBUS_SIM_PIN2_REQUIRED)) {
		sim_display_pin_UI(SIM_PIN2_REQUIRED);
	}
	else {
		sim_display_puk_UI(SIM_PUK2_REQUIRED);
	}

	printf ("Received sim auth status");
}


int sim_get_authentication_state() {

	char *status = NULL;
	GError *error = NULL;
	int result = 0;
	if (!org_freesmartphone_GSM_SIM_get_auth_status(simBus, &status, &error))
		result = sim_handle_errors(error);
	
	// There's been an error.
	if(result < 0) {
		return -1;
	}

	if(strcmp(status,DBUS_SIM_READY))
		result = SIM_READY;
	else if(strcmp(status,DBUS_SIM_PIN_REQUIRED))
		result = SIM_PIN_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PUK_REQUIRED))
		result = SIM_PUK_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PIN2_REQUIRED))
		result = SIM_PIN2_REQUIRED;
	else
		result = SIM_PUK2_REQUIRED;
	free(status);
	return result;
}

void sim_display_pin_UI(int codeToSet) {
	int result = -1;

	while(result < 0) {
	/* TODO.
	 * Launch the UI.
	 * Don't forget the "cancel" button */
	
	}

}

void sim_display_puk_UI(int codeToSet) {

	/* TODO */
}

int sim_send_pin_code(int codeToSet, const char* pin) {
        
	char *status = NULL;
	GError *error = NULL;
	int result = 0;

	if(!org_freesmartphone_GSM_SIM_set_auth_code (simBus, pin, &error)) {
		result = sim_handle_errors(error);
	}

	return sim_handle_sim_auth(result, codeToSet);
}

int sim_set_puk_code(int codeToSet, const char* puk, const char* pin) {
        
	char *status = NULL;
	GError *error = NULL;
	int result = 0;

	if(!org_freesmartphone_GSM_SIM_unlock (simBus, puk, pin, &error)) {
		result = sim_handle_errors(error);
	}


	return sim_handle_sim_auth(result, codeToSet);

}

int sim_handle_sim_auth(int result, int codeToSet) {
	if(result  < 0) {
		switch(result) {
			case SIM_ERROR_AUTH_FAILED:
				result = codeToSet;
				break;
			case SIM_ERROR_BLOCKED:
				/* We know there's a signal for that but we thought
				 * it was worth the round trip to dbus in order to
				 * prevent an UI rebuild */
				result = sim_get_authentication_state();
				if(result < 0) {
					return -1;
				}
				break;
			default:
				/* Unhandled or SIM_BLOCKED. */
				result = -1;
		}
	}

	return result;

}


int sim_handle_errors(GError *error) {
	const char *error_name = dbus_g_error_get_name(error);
	int simError = 0;

	if(strcmp(error_name, DBUS_SIM_ERROR_NOT_PRESENT)) {
		simError = SIM_ERROR_NOT_PRESENT;
	} else if(strcmp(error_name, DBUS_SIM_ERROR_AUTH_FAILED)) {
		simError = SIM_ERROR_AUTH_FAILED;
	} else if(strcmp(error_name, DBUS_SIM_ERROR_BLOCKED)) {
		simError = SIM_ERROR_BLOCKED;
	} else if(strcmp(error_name, DBUS_SIM_ERROR_NOT_FOUND)) {
		simError = SIM_ERROR_NOT_FOUND;
	} else if(strcmp(error_name, DBUS_SIM_ERROR_MEMORY_FULL)) {
		simError = SIM_ERROR_MEMORY_FULL;
	} else if(strcmp(error_name, DBUS_SIM_ERROR_INVALID_INDEX)) {
		simError = SIM_ERROR_INVALID_INDEX;
	} else {
        	lose_gerror ("Failed to complete SendAuthCode", error);
	}
	g_error_free(error);
	free(error_name);
	return simError;	
}
