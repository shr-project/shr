/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
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

#ifndef _OPHONEKITD_PHONEGUI_H
#define _OPHONEKITD_PHONEGUI_H

#include <frameworkd-glib/frameworkd-glib-phonegui.h>

void (*phonegui_init)();
void (*phonegui_sim_auth_show)();
void (*phonegui_sim_auth_hide)();
void (*phonegui_incoming_call_show)();
void (*phonegui_incoming_call_hide)();
void (*phonegui_outgoing_call_show)();
void (*phonegui_outgoing_call_hide)();
void (*phonegui_message_show)();
void (*phonegui_message_hide)();

void phonegui_connect();

#endif
