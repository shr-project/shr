#ifndef PHONEGUI_OUTGOING_CALL_H
#define PHONEGUI_OUTGOING_CALL_H

#include "phonegui-init.h"
#include <pthread.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

extern PhoneguiMode phonegui_mode;

void phonegui_outgoing_call_ui_show(const int id, const int status, const char *number);
void phonegui_outgoing_call_ui_hide(const int id);
void outgoing_call_ui_input(void *data, Evas_Object *o, const char *emission, const char *source);
int outgoing_call_ui_event(void *data, Ecore_Fd_Handler *fdh);

#endif
