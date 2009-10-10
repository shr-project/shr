/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
 *              Klaus 'mrmoku' Kurzmann <mok@fluxnetz.de>
 *              quickdev
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <glib.h>
#include <glib/gthread.h>
#include "ophonekitd-main.h"
#include "ophonekitd-dbus.h"
#include "ophonekitd-frameworkd.h"
#include "ophonekitd-phonegui.h"
#include "ophonekitd-globals.h"



static int running = TRUE;
static int notify;

int
main(int argc, char **argv)
{
	GError *error = NULL;
	fd_set fdst;

	/* --- daemonize --- */
	//close(STDIN_FILENO);
	//close(STDOUT_FILENO);
	//close(STDERR_FILENO);
	//umask(0077);

	//if (fork())
	//	return (0);

	//chdir("/tmp");
	//setsid();
	//setpgrp();

	ophonekitd_argc = argc;
	ophonekitd_argv = argv;

	g_type_init();

	/* init threading before accessing any glib functions ! */
	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	/* --- set up signal handling --- */
	set_signals();

	load_config();

	notify = inotify_init();
	inotify_add_watch(notify, FRAMEWORKD_PHONEGUI_CONFIG, IN_MODIFY);

	phonegui_queue = g_async_queue_new();

	//if (!g_thread_create(ophonekitd_frameworkd_main, NULL,
	//			FALSE, &error)) {
	//	g_error("failed to create frameworkd thread");
	//}

	//if (!g_thread_create(ophonekitd_dbus_main, NULL,
	//			FALSE, &error)) {
	//	g_error("failed to create dbus thread");
	//}

	if (!g_thread_create(ophonekitd_phonegui_main, NULL,
				FALSE, &error)) {
		g_error("failed to create phonegui thread");
	}

	while (running) {
		FD_ZERO(&fdst);
		FD_SET(notify, &fdst);
		switch (select(notify+1, &fdst, 0, 0, 0)) {
		case -1:
			g_debug("select failed with %s", strerror(errno));
			break;
		case 0:
			g_debug("select gave 0");
			break;
		default:
			if (FD_ISSET(notify, &fdst))
				reload_config();
			break;
		}
	}

	/* TODO: cleanup threads */

	exit(EXIT_SUCCESS);
}


static void
load_config()
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;

	/* Read the ophonekitd preferences */
	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	if (g_key_file_load_from_file
	    (keyfile, FRAMEWORKD_PHONEGUI_CONFIG, flags, &error)) {
		show_incoming_sms =
			g_key_file_get_boolean(keyfile, "phonegui",
					       "show_incoming_sms", NULL);
		g_debug("Configuration file read");
	}
	else {
		g_error(error->message);
		g_debug("Reading configuration file error, skipping");
	}
}

static void
reload_config()
{
	g_debug("reloading configuration");
	load_config();
}

static int
set_signals()
{
	sigset_t sig;
	struct sigaction sh_term;

	sigemptyset(&sig);
	sigaddset(&sig, SIGTERM);

	// --- TERM Signal ---
	sh_term.sa_handler = sig_term;
	sh_term.sa_mask = sig;
	sh_term.sa_flags = SA_RESTART;
	sh_term.sa_restorer = FALSE;
	if ((sigaction(SIGTERM, &sh_term, 0)) == -1)
	{
		g_debug("failed installing sigterm handler!!!");
		return (0);
	}

	return (1);
}

static void
sig_term(int i)
{
	running = 0;
}


