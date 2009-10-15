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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <glib.h>
#include <glib/gthread.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include "ophonekitd-main.h"
#include "ophonekitd-dbus.h"
#include "ophonekitd-fso.h"
#include "ophonekitd-globals.h"


static int logfile = -1;

static int notify;

static void
_log_handler(const gchar *domain, GLogLevelFlags level, const gchar *message,
		gpointer userdata)
{
	struct timeval tv;
	struct tm ptime;
	gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &ptime);

	char *msg = g_strdup_printf("%04d.%02d.%02d %02d:%02d:%02d.%06d %s\n",
			ptime.tm_year+1900, ptime.tm_mon, ptime.tm_mday,
			ptime.tm_hour, ptime.tm_min, ptime.tm_sec, tv.tv_usec,
			message);
	write(logfile, msg, strlen(msg));
	free (msg);
}

int
main(int argc, char **argv)
{
	GError *error = NULL;
	fd_set fdst;
	FrameworkdHandler *fwHandler;

	/* --- daemonize --- */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	umask(0077);

	if (fork())
		return (0);

	chdir("/tmp");
	setsid();
	setpgrp();

	logfile = open("/var/log/ophonekitd.log", O_WRONLY|O_CREAT|O_APPEND);
	if (logfile == -1) {
		printf("failed creating the logfile");
		return (-3);
	}

	g_log_set_default_handler(_log_handler, NULL);

	g_type_init();

	/* init threading before accessing any glib functions ! */
	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	/* --- set up signal handling --- */
	set_signals();

	load_config();

	/* setup dbus server part */
	ophonekitd_dbus_setup();

	/* initialize libframeworkd-glib */
	g_debug("connect to frameworkd");
	fwHandler = frameworkd_handler_new();
	fwHandler->simAuthStatus = ophonekitd_sim_auth_status_handler;
	fwHandler->simReadyStatus = ophonekitd_sim_ready_status_handler;
	fwHandler->simIncomingStoredMessage =
		ophonekitd_sim_incoming_stored_message_handler;
	fwHandler->callCallStatus = ophonekitd_call_status_handler;
	fwHandler->deviceIdleNotifierState =
		ophonekitd_device_idle_notifier_state_handler;
	fwHandler->incomingUssd = ophonekitd_incoming_ussd_handler;

	fwHandler->usageResourceAvailable =
		ophonekitd_resource_available_handler;
	fwHandler->usageResourceChanged = ophonekitd_resource_changed_handler;

	frameworkd_handler_connect(fwHandler);
	g_debug("connected to the buses");

	//notify = inotify_init();
	//inotify_add_watch(notify, FRAMEWORKD_PHONEGUI_CONFIG, IN_MODIFY);

	/* Start glib main loop and run list_resources() */
	g_debug("entering glib main loop");
	g_timeout_add(0, list_resources, NULL);

	g_main_loop_run(loop);

	g_debug("finished mainloop");
//	if (incoming_calls)
//		free(incoming_calls);
//	if (outgoing_calls)
//		free(outgoing_calls);

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
		gsm_reregister_timeout =
			g_key_file_get_integer(keyfile, "ophonekitd",
					"reregister_timeout", NULL);
		/* ensure a sane value for the timeout... minimum is 60s */
		if (gsm_reregister_timeout < MINIMUM_GSM_REREGISTER_TIMEOUT) {
			g_message("invalid reregister_timeout - setting to %ds",
					MINIMUM_GSM_REREGISTER_TIMEOUT);
			gsm_reregister_timeout = MINIMUM_GSM_REREGISTER_TIMEOUT;
		}
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
//	running = 0;
}


