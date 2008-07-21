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
#include "dbus.h"
#include "call.h"
#include "sim.h"
#include "network.h"
#include "device.h"
#include "ophonekitd-main.h"	
#include "moko-pin.h"

int main(int argc, char ** argv) {
	GMainLoop *mainloop = NULL;
#ifdef DEBUG
	printf("Starting...\n");
#endif
	mainloop = g_main_loop_new (NULL, FALSE);
	dbus_connect_to_bus();
#ifdef DEBUG
	printf("Connected to the buses\n");
#endif
	g_timeout_add(2000, register_to_network, NULL);
#ifdef DEBUG
	printf("Entering mainloop.\n");
#endif
	g_main_loop_run (mainloop);

	exit(EXIT_SUCCESS);

}

gboolean register_to_network() {

	GError* error = NULL;
	if(!device_set_antenna_power(&error, TRUE)) {
		if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
#ifdef DEBUG
	printf("Calling the UI on an AUTH_FAILED exception\n");
#endif
			sim_display_code_UI();
			do {
				sleep(5);
			} while(!device_set_antenna_power(&error, TRUE));
		}
	}

	return FALSE; // End timeout
}
