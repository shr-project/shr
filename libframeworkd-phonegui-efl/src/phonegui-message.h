#ifndef PHONEGUI_MESSAGE_H
#define PHONEGUI_MESSAGE_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include "pipe.h"

extern PipeHandler pipe_handler;

void phonegui_message_show(const int id);
void phonegui_message_hide();
void message_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void message_event(int event);

#endif
