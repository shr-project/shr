/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Didier "Ptitjes" <ptitjes@free.fr>
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

#ifndef _OPHONEKITD_PHONELOG_H
#define _OPHONEKITD_PHONELOG_H

#include <dbus/dbus-glib.h>

void phonelog_init_database();
void phonelog_close_database();

int phonelog_add_new_call(const gchar* number);
void phonelog_log_call_event(const int unique_id, const int status);

#endif
