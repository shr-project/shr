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

#ifndef _OPHONEKITD_PHONEGUI_H
#define _OPHONEKITD_PHONEGUI_H

void *phonegui_load();
void phonegui_connect(void *library);
void *phonegui_get_function(void *library, char *name);

void (*phonegui_init)();
void (*phonegui_sim_auth_ui_show)();
void (*phonegui_sim_auth_ui_hide)();
void (*phonegui_incoming_call_ui_show)();
void (*phonegui_incoming_call_ui_hide)();
void (*phonegui_outgoing_call_ui_show)();
void (*phonegui_outgoing_call_ui_hide)();
void (*phonegui_message_ui_show)();
void (*phonegui_message_ui_hide)();

#endif
