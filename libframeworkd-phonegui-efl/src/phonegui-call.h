#ifndef PHONEGUI_CALL_H
#define PHONEGUI_CALL_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>

void phonegui_incoming_call_show(const int id, const int status, const char *number);
void phonegui_incoming_call_hide(const int id);
void phonegui_outgoing_call_show(const int id, const int status, const char *number);
void phonegui_outgoing_call_hide(const int id);

void call_show(const int id, const int status, const char *number, const char *type);
void call_hide(const int id);
void call_input(void *data, Evas_Object *o, const char *emission, const char *source);
void call_delete(Ecore_Evas *ee);
void call_event(int event);

void call_incoming_show();
void call_active_show();
void call_active_hide();

void dtmf_enable();
void dtmf_disable();
void speaker_enable();
void speaker_disable();

#endif
