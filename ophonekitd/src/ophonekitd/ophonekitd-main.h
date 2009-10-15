/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
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

#ifndef _OPHONEKITD_MAIN_H
#define _OPHONEKITD_MAIN_H

#include <dbus/dbus-glib.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>

#define MINIMUM_GSM_REREGISTER_TIMEOUT 60

int main(int argc, char **argv);

static void
load_config();
static void
reload_config();

static int
set_signals();
static void
sig_term(int i);

#endif
