#ifndef PHONEGUI_OUTGOING_CALL_H
#define PHONEGUI_OUTGOING_CALL_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>

void phonegui_outgoing_call_show(const int id, const int status, const char *number);
void phonegui_outgoing_call_hide(const int id);

void outgoing_call_input(void *data, Evas_Object *o, const char *emission, const char *source);
void outgoing_call_event(int event);

#endif
