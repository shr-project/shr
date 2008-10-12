#ifndef _PHONEGUI_MESSAGES_H
#define _PHONEGUI_MESSAGES_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_messages_show();
void phonegui_messages_hide();

void messages_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void messages_event(int event);

void retrieve_messagebook_callback(GError*error, GPtrArray*messages, gpointer userdata);
void retrieve_message_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer data);
void process_message(GValueArray *message);

void messages_folders_show();
void messages_folders_hide();
void messages_message_show();
void messages_loading_show();
void messages_new1_show();
void messages_new1_hide();
void messages_new2_show();
void messages_new2_hide();

#endif

