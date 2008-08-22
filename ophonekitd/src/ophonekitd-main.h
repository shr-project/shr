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

#ifndef _OPHONEKITD_MAIN_H
#define _OPHONEKITD_MAIN_H
gboolean power_up_antenna();
void power_up_antenna_callback(GError *error, gpointer userdata);
void register_to_network_callback(GError *error, gpointer userdata);
void ophonekitd_sms_incoming_message_handler(const int id);
void ophonekitd_sms_message_sent_handler(const int id, gboolean success, const char* reason);
void ophonekitd_sim_incoming_message_handler(const int id);
void ophonekitd_sim_auth_status_handler(const int status);
void ophonekitd_network_status_handler(GHashTable *status);
void ophonekitd_call_status_handler(const int id_call, const int status, GHashTable *properties);
void ophonekitd_display_code_UI ();
#endif
