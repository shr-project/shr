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

#ifndef _SMS_H
#define _SMS_H

//#define SMS_ERROR g_quark_from_static_string(CALL_INTERFACE

void sms_message_sent_handler (DBusGProxy *proxy, const int id, const gboolean success, const char* reason, gpointer userdata);
void sms_incoming_message_handler (DBusGProxy *proxy, const int id, gpointer userdata);

void sms_send_message(const char* number, const char* content, const gboolean report, void (*callback)(GError*, int transaction_index, gpointer), gpointer userdata);

void sms_get_service_bearer(void (*callback)(GError*, char*mode, gpointer), gpointer userdata);

void sms_set_service_bearer(const char* mode, void (*callback)(GError*, gpointer), gpointer userdata);

extern DBusGProxy *smsBus;

#endif
