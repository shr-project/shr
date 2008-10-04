#ifndef PHONEGUI_INCOMING_CALL_H
#define PHONEGUI_INCOMING_CALL_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>

void phonegui_incoming_call_show(const int id_call, const int status, const char *number);
void phonegui_incoming_call_hide(const int id_call);

void incoming_call_event(int event);
void incoming_call_input(void *data, Evas_Object *o, const char *emission, const char *source);

#endif
