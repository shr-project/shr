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
void call_delete(Ecore_Evas *ee);
void call_event(int event);

void call_button_keypad_clicked(void *data, Evas_Object *obj, void *event_info);
void call_button_accept_clicked();
void call_button_release_clicked();
void call_button_speaker_clicked();
void call_button_dtmf_clicked();

void call_incoming_show();
void call_incoming_hide();
void call_active_show();
void call_active_hide();

void call_dtmf_enable();
void call_dtmf_disable();
void call_speaker_enable();
void call_speaker_disable();

#endif
