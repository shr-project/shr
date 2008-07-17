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

#include "dbus.h"
#include "dbus/sim.h"
#include "sim.h"

void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
	if(strcmp(status,DBUS_SIM_READY)) {
		// TODO
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
#ifdef DEBUG
	printf ("Received sim auth status %s", status);
#endif
}


gboolean sim_get_authentication_state(GError **error, int *code) {

	char *status = NULL;
	GError *dbus_error = NULL, *tmperror = NULL;
	gboolean result = FALSE;
	if (!(result = org_freesmartphone_GSM_SIM_get_auth_status(simBus, &status, &dbus_error))) {
		 tmperror = dbus_handle_errors(dbus_error);
		 g_propagate_error(error, tmperror);
	} else {	
		if(strcmp(status,DBUS_SIM_READY))
			*code = SIM_READY;
		else if(strcmp(status,DBUS_SIM_PIN_REQUIRED))
			*code = SIM_PIN_REQUIRED;
		else if(strcmp(status,DBUS_SIM_PUK_REQUIRED))
			*code = SIM_PUK_REQUIRED;
		else if(strcmp(status,DBUS_SIM_PIN2_REQUIRED))
			*code = SIM_PIN2_REQUIRED;
		else
			*code = SIM_PUK2_REQUIRED;

	}

	free(status);	
	return result;
}

void sim_display_code_UI() {
	int needed_code;
	GError *error = NULL;
	if(sim_get_authentication_state(&error, &needed_code)) {
		switch(needed_code) {
			case SIM_PIN_REQUIRED:
			case SIM_PIN2_REQUIRED:
				sim_display_pin_UI(needed_code);
				break;
			default:
				sim_display_puk_UI(needed_code);
		}
	} else {
		// What can we do ?
	}
}
void sim_display_pin_UI(int codeToSet) {
#ifdef DEBUG
	printf("Displaying PIN UI for %i",codeToSet);
#endif
	/* TODO.
	 * Launch the UI.
	 * Don't forget the "cancel" button */
}

void sim_display_puk_UI(int codeToSet) {
#ifdef DEBUG
	printf("Displaying PUK UI for %i",codeToSet);
#endif

	/* TODO */
}

gboolean sim_send_pin_code(GError** error, int *codeToSet, const char* pin) {
        
	GError *dbus_error = NULL, *tmperror = NULL;

	if(!org_freesmartphone_GSM_SIM_send_auth_code (simBus, pin, &dbus_error)) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error,tmperror);
	}

	return sim_handle_sim_auth(error, codeToSet);
}

gboolean sim_set_puk_code(GError** error, int* codeToSet, const char* puk, const char* pin) {
        
	GError *dbus_error = NULL, *tmperror = NULL;

	if(!org_freesmartphone_GSM_SIM_unlock (simBus, puk, pin, &dbus_error)) {
		tmperror = dbus_handle_errors(dbus_error);
		g_propagate_error(error,tmperror);
	}


	return sim_handle_sim_auth(error, codeToSet);

}

gboolean sim_handle_sim_auth(GError** error, int *codeToSet) {
	gboolean result = TRUE;
	if(error  != NULL && *error != NULL) {
		if(IS_SIM_ERROR(*error, SIM_ERROR_AUTH_FAILED))
			result = FALSE;
		else if(IS_SIM_ERROR(*error, SIM_ERROR_BLOCKED)) {
			/* We know there's a signal for that but we thought
			 * it was worth the round trip to dbus in order to
			 * prevent an UI rebuild */
			int needed_code;
			if((result = sim_get_authentication_state(error, &needed_code)))
				*codeToSet = needed_code;			
		
		}
	}
	return result;

}


GError* sim_handle_errors(GError *error) {
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
        	lose_gerror ("Unknown SIM error", error);
	}
	return g_error_new (SIM_ERROR, simError, "TODO");
}
