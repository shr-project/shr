/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Julien "Ainulindalë" Cassignol
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

#include "frameworkd-phonegui.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <glib.h>
#include <stdlib.h>

#include <phone-utils.h>

static void (*_phonegui_init) (int argc, char **argv, void (*exit_cb) ()) =
	NULL;

/* Calls */
static void (*_phonegui_incoming_call_show) (const int id, const int status,
					     const char *number) = NULL;
static void (*_phonegui_incoming_call_hide) (const int id) = NULL;
static void (*_phonegui_outgoing_call_show) (const int id, const int status,
					     const char *number) = NULL;
static void (*_phonegui_outgoing_call_hide) (const int id) = NULL;

/* Contacts */
static void (*_phonegui_contacts_show) () = NULL;
static void (*_phonegui_contacts_new_show) () = NULL;
static void (*_phonegui_contacts_hide) () = NULL;

/* Dialer */
static void (*_phonegui_dialer_show) () = NULL;
static void (*_phonegui_dialer_hide) () = NULL;

/* Dialog */
static void (*_phonegui_dialog_show) (int type) = NULL;
static void (*_phonegui_dialog_hide) () = NULL;

/* Messages */
static void (*_phonegui_messages_show) () = NULL;
static void (*_phonegui_messages_hide) () = NULL;
static void (*_phonegui_message_show) (const int id) = NULL;
static void (*_phonegui_message_hide) () = NULL;

/* Sim auth */
static void (*_phonegui_sim_auth_show) (const int status) = NULL;
static void (*_phonegui_sim_auth_hide) (const int status) = NULL;

/* USSD */
static void (*_phonegui_ussd_show) (int mode, const char *message) = NULL;
static void (*_phonegui_ussd_hide) () = NULL;


static void *phonegui_library = NULL;
Settings *conf = NULL;

static void phonegui_connect();

void
phonegui_load(const char *application_name)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	if (!g_key_file_load_from_file
	    (keyfile, FRAMEWORKD_PHONEGUI_CONFIG, flags, &error)) {
		g_error(error->message);
		return;
	}

	conf = g_slice_new(Settings);
	conf->library =
		g_key_file_get_string(keyfile, "phonegui", "library", NULL);
	/* Load library */
	if (conf->library != NULL) {
		phonegui_library =
			dlopen(conf->library, RTLD_LOCAL | RTLD_LAZY);
		if (!phonegui_library) {
			g_error("Loading %s failed: %s", conf->library,
				dlerror());
		}
	}
	else {
		g_error("Loading failed. Conf->library not set.");
	}
	phonegui_connect();
	/* init phone utils */
	/* FIXME: should deinit somewhere! */
	phone_utils_init();

}


void *
phonegui_get_function(const char *name)
{
	if (phonegui_library == NULL) {
		g_error("phonegui library not loaded");
	}

	void *pointer = dlsym(phonegui_library, name);
	char *error;
	if ((error = dlerror()) != NULL) {
		g_debug("Symbol not found: %s", error);
	}
	return pointer;
}

static void
phonegui_connect()
{
	_phonegui_init = phonegui_get_function("phonegui_backend_init");

	_phonegui_incoming_call_show =
		phonegui_get_function("phonegui_backend_incoming_call_show");
	_phonegui_incoming_call_hide =
		phonegui_get_function("phonegui_backend_incoming_call_hide");
	_phonegui_outgoing_call_show =
		phonegui_get_function("phonegui_backend_outgoing_call_show");
	_phonegui_outgoing_call_hide =
		phonegui_get_function("phonegui_backend_outgoing_call_hide");

	_phonegui_contacts_show =
		phonegui_get_function("phonegui_backend_contacts_show");
	_phonegui_contacts_new_show =
		phonegui_get_function("phonegui_backend_contacts_new_show");
	_phonegui_contacts_hide =
		phonegui_get_function("phonegui_backend_contacts_hide");

	_phonegui_dialer_show =
		phonegui_get_function("phonegui_backend_dialer_show");
	_phonegui_dialer_hide =
		phonegui_get_function("phonegui_backend_dialer_hide");

	_phonegui_dialog_show =
		phonegui_get_function("phonegui_backend_dialog_show");
	_phonegui_dialog_hide =
		phonegui_get_function("phonegui_backend_dialog_hide");

	_phonegui_message_show =
		phonegui_get_function("phonegui_backend_message_show");
	_phonegui_message_hide =
		phonegui_get_function("phonegui_backend_message_hide");
	_phonegui_messages_show =
		phonegui_get_function("phonegui_backend_messages_show");
	_phonegui_messages_hide =
		phonegui_get_function("phonegui_backend_messages_hide");

	_phonegui_sim_auth_show =
		phonegui_get_function("phonegui_backend_sim_auth_show");
	_phonegui_sim_auth_hide =
		phonegui_get_function("phonegui_backend_sim_auth_hide");

	_phonegui_ussd_show =
		phonegui_get_function("phonegui_backend_ussd_show");
	_phonegui_ussd_hide =
		phonegui_get_function("phonegui_backend_ussd_hide");
}

/* Implementation prototypes */
void
phonegui_init(int argc, char **argv, void (*exit_cb) ())
{
	if (_phonegui_init)
		_phonegui_init(argc, argv, exit_cb);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Calls */
void
phonegui_incoming_call_show(const int id, const int status, const char *number)
{
	if (_phonegui_incoming_call_show)
		_phonegui_incoming_call_show(id, status, number);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_incoming_call_hide(const int id)
{
	if (_phonegui_incoming_call_hide)
		_phonegui_incoming_call_hide(id);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_outgoing_call_show(const int id, const int status, const char *number)
{
	if (_phonegui_outgoing_call_show)
		_phonegui_outgoing_call_show(id, status, number);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_outgoing_call_hide(const int id)
{
	if (_phonegui_outgoing_call_hide)
		_phonegui_outgoing_call_hide(id);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Contacts */
void
phonegui_contacts_show()
{
	if (_phonegui_contacts_show)
		_phonegui_contacts_show();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_contacts_new_show()
{
	if (_phonegui_contacts_new_show)
		_phonegui_contacts_new_show();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_contacts_hide()
{
	if (_phonegui_contacts_hide)
		_phonegui_contacts_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Dialer */
void
phonegui_dialer_show()
{
	if (_phonegui_dialer_show)
		_phonegui_dialer_show();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_dialer_hide()
{
	if (_phonegui_dialer_hide)
		_phonegui_dialer_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Dialog */
void
phonegui_dialog_show(int type)
{
	if (_phonegui_dialog_show)
		_phonegui_dialog_show(type);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_dialog_hide()
{
	if (_phonegui_dialog_hide)
		_phonegui_dialog_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Messages */
void
phonegui_messages_show()
{
	if (_phonegui_messages_show)
		_phonegui_messages_show();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_messages_hide()
{
	if (_phonegui_messages_hide)
		_phonegui_messages_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_message_show(const int id)
{
	if (_phonegui_message_show)
		_phonegui_message_show(id);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_message_hide()
{
	if (_phonegui_message_hide)
		_phonegui_message_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* Sim auth */
void
phonegui_sim_auth_show(const int status)
{
	if (_phonegui_sim_auth_show)
		_phonegui_sim_auth_show(status);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_sim_auth_hide(const int status)
{
	if (_phonegui_sim_auth_hide)
		_phonegui_sim_auth_hide(status);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

/* USSD */
void
phonegui_ussd_show(int mode, const char *message)
{
	if (_phonegui_ussd_show)
		_phonegui_ussd_show(mode, message);
	else
		g_debug("can't find function %s", __FUNCTION__);
}

void
phonegui_ussd_hide()
{
	if (_phonegui_ussd_hide)
		_phonegui_ussd_hide();
	else
		g_debug("can't find function %s", __FUNCTION__);
}
