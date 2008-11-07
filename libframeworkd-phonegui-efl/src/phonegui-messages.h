#ifndef _PHONEGUI_MESSAGES_H
#define _PHONEGUI_MESSAGES_H

#include <time.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <glib-2.0/glib-object.h>
#include <glib-2.0/glib.h>

void phonegui_messages_show();
void phonegui_messages_hide();

void messages_event(int event);

void retrieve_messagebook_callback(GError*error, GPtrArray*messages, gpointer userdata);
void process_message(GValueArray *message);
void delete_message_callback(GError *error, gpointer userdata);
void send_message_callback(GError *error, int transaction_index, gpointer userdata);

void messages_folders_show();
void messages_folders_hide();
void messages_list_show();
void messages_list_hide();
void messages_message_show();
void messages_message_hide();
void messages_loading_show();
void messages_new1_show();
void messages_new1_hide();
void messages_new2_show();
void messages_new2_hide();
void messages_delete_show();
void messages_delete_hide();

#endif

