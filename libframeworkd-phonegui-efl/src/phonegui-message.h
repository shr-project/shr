#ifndef PHONEGUI_MESSAGE_H
#define PHONEGUI_MESSAGE_H

#include <glib-2.0/glib.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include "pipe.h"

extern PipeHandler pipe_handler;

void phonegui_message_show(const int id);
void phonegui_message_hide();

void message_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void message_event(int event);

void retrieve_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer userdata);

#endif
