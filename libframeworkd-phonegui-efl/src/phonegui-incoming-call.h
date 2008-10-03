#ifndef PHONEGUI_INCOMING_CALL_H
#define PHONEGUI_INCOMING_CALL_H

#include "phonegui-init.h"
#include <pthread.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

extern PhoneguiMode phonegui_mode;

void phonegui_incoming_call_ui_show(const int id_call, const int status, const char *number);
void phonegui_incoming_call_ui_hide(const int id_call);

void incoming_call_ui_input(void *data, Evas_Object *o, const char *emission, const char *source);
int incoming_call_ui_event(void *data, Ecore_Fd_Handler *fdh);

#endif
