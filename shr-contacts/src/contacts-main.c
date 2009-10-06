/*
 *  Copyright (C) 2007 Openmoko, Inc.
 *
 *  Authored by:
 *    quickdev
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

#include "contacts-main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#define CONFIG_FILE "/etc/ophonekitd/gui.conf"


int
main(int argc, char **argv)
{
	/* Connect to frameworkd */
	frameworkd_handler_connect(frameworkd_handler_new());

	/* Load, connect and initiate phonegui */
	phonegui_load(CONFIG_FILE);
	phonegui_init(argc, argv, exit_callback);

	/* Initialize glib main loop */
	GMainLoop *mainloop = NULL;
	g_type_init();
	mainloop = g_main_loop_new(NULL, FALSE);

	/* Enter glib main loop and start ui */
	if (argc == 2) {
		g_timeout_add(0, start_contact_new, argv[1]);
	}
	else {
		g_timeout_add(0, start, NULL);
	}

	g_main_loop_run(mainloop);

	return EXIT_SUCCESS;
}

gboolean
start()
{
	phonegui_contacts_show();
	return FALSE;
}

gboolean
start_contact_new(const char *number)
{
	phonegui_contacts_new_show(NULL, number);
	return FALSE;
}

void
exit_callback()
{
	g_debug("exit_callback()");
	phonegui_contacts_hide();
	exit(EXIT_SUCCESS);
}
